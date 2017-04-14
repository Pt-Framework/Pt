/* Copyright (C) 2010-2016 Marc Boris Duerner
   Copyright (C) 2017-2017 by Aloysius Indrayanto

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  As a special exception, you may use this file as part of a free
  software library without restriction. Specifically, if other files
  instantiate templates or use macros or inline functions from this
  file, or you compile this file and link it with other files to
  produce an executable, this file does not by itself cause the
  resulting executable to be covered by the GNU General Public
  License. This exception does not however invalidate any other
  reasons why the executable file might be covered by the GNU Library
  General Public License.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
  02110-1301 USA
*/

#ifndef PT_GFX_BILINEARSCALE_H
#define PT_GFX_BILINEARSCALE_H

#include <Pt/Gfx/Math.h>
#include <Pt/Gfx/SIMDConfig.h>


namespace Pt {
namespace Gfx {


#if defined(PT_GFX_USE_SSE4P1) || defined(PT_GFX_USE_SSE2)

// SSE constants
static const __m128 sseFour001 = _mm_set1_ps(  1);
static const __m128 sseFour256 = _mm_set1_ps(256);

#endif

#if defined(PT_GFX_USE_NEON)

// NEON constants
static const float32x4_t neonFour001 = NEON_SET_FLT32X4(  1,   1,   1,   1);
static const float32x4_t neonFour256 = NEON_SET_FLT32X4(256, 256, 256, 256);

#endif


// ======================================================================================
// ===== Internal Implementation ========================================================
// ======================================================================================

// Based on: FastC++: Coding Cpp Efficiently
//           Bilinear Pixel Interpolation using SSE
//           http://fastcpp.blogspot.co.id/2011/06/bilinear-pixel-interpolation-using-sse.html
//           Blog by theowl84, 2011

// x86_64
//     Image scaling 4 (block    - plain C   )          =     13
//     Image scaling 4 (bilinear - fixed C   )          =    136 (10.462)
//     Image scaling 4 (bilinear - SSE 2/4.1 )          =    114 ( 8.769)

// Arm
//     Image scaling 4 (block    - plain C   )          =    333
//     Image scaling 4 (bilinear - fixed C   )          =   1503 ( 4.514)

#if defined(PT_GFX_USE_SSE4P1)

inline Pt::int32_t bsGetPixel(const Pt::int32_t* img, Pt::ssize_t imgW, float x, float y)
{

    // Floor the coordinate
    const Pt::int32_t px = Pt::Gfx::Math::zfint(x);
    const Pt::int32_t py = Pt::Gfx::Math::zfint(y);

    // Pointer to the first pixel
    const Pt::int32_t* p0 = img + py * imgW + px;

    // Load the four neighboring pixels
    const __m128i p12      = _mm_loadl_epi64   ( (const __m128i*) &p0[0 * imgW]              ); // ? ? ? ? C C C C
    const __m128i p34      = _mm_loadl_epi64   ( (const __m128i*) &p0[1 * imgW]              ); // ? ? ? ? C C C C

    // Convert ARGB ARGB ARGB ARGB to AAAA RRRR GGGG BBBB
    const __m128i p1234aos = _mm_unpacklo_epi8 (p12,        p34                              );
    const __m128i p34xx    = _mm_unpackhi_epi64(p1234aos,   _mm_setzero_si128()              );
    const __m128i p1234soa = _mm_unpacklo_epi8 (p1234aos,   p34xx                            );

    // Extend to 16-bit integer
    const __m128i prgex    = _mm_unpacklo_epi8 (p1234soa,   _mm_setzero_si128()              );
    const __m128i pbaex    = _mm_unpackhi_epi8 (p1234soa,   _mm_setzero_si128()              );

    // Calculate weights
    const __m128  ssx      = _mm_set_ss        (x                                            ); // 0  0      0      X
    const __m128  ssy      = _mm_set_ss        (y                                            ); // 0  0      0      Y
    const __m128  zzyx     = _mm_unpacklo_ps   (ssx,        ssy                              ); // 0  0      Y      X
    const __m128  zzyxflor = _mm_floor_ps      (zzyx                                         ); // 0  0      Yi     Xi
    const __m128  zzyxfrac = _mm_sub_ps        (zzyx,       zzyxflor                         ); // 0  0      Yr     Xr
    const __m128  ooyxfrac = _mm_sub_ps        (sseFour001, zzyxfrac                         ); // ?  ?      (1-Yr) (1-Xr)
    const __m128  wxh      = _mm_unpacklo_ps   (ooyxfrac,   zzyxfrac                         ); // ?  ?      Xr     (1-Xr)
    const __m128  wx       = _mm_movelh_ps     (wxh,        wxh                              ); // Xr (1-Xr) Xr     (1-Xr)
    const __m128  wy       = _mm_shuffle_ps    (ooyxfrac,   zzyxfrac, _MM_SHUFFLE(1, 1, 1, 1)); // Yr Yr     (1-Yr) (1-Yr)
    const __m128  weight   = _mm_mul_ps        (wx,         wy                               );

    // Convert the weights to 16-bit integer
    const __m128  weights = _mm_mul_ps         (weight,     sseFour256                       ); // W4  .   W3  .   W2  .   W1  .
    const __m128i weighti = _mm_cvttps_epi32   (weights                                      ); // W4i .   W3i .   W2i .   W1i .
    const __m128i weighth = _mm_packs_epi32    (weighti,    weighti                          ); // W4i W3i W2i W1i W4i W3i W2i W1i

    // Multiply each pixel with the corresponding weight
    const __m128i resrg   = _mm_madd_epi16     (prgex,      weighth                          ); // [W1*R1 + W2*R2 | W3*R3 + W4*R4 | W1*G1 + W2*G2 | W3*G3 + W4*G4]
    const __m128i resba   = _mm_madd_epi16     (pbaex,      weighth                          ); // [W1*B1 + W2*B2 | W3*B3 + W4*B4 | W1*A1 + W2*A2 | W3*A3 + W4*A4]

    // Horizontal add the results and produce output values in 32-bit
    const __m128i r       = _mm_hadd_epi32     (resrg,      resba                            );

    // Divide the results by 256
    const __m128i rdiv256 = _mm_srli_epi32     (r,          8                                );

    // Convert back the results to 8-bit integer and pack
    const __m128i r16     = _mm_packus_epi32   (rdiv256,    _mm_setzero_si128()              );
    const __m128i rfin    = _mm_packus_epi16   (r16,        _mm_setzero_si128()              );

    // Return the result as a 32-bit integer
    return _mm_cvtsi128_si32(rfin);
}

#elif defined(PT_GFX_USE_SSE2)

inline Pt::int32_t bsGetPixel(const Pt::int32_t* img, Pt::ssize_t imgW, float x, float y)
{
    // Floor the coordinate
    const Pt::int32_t px = Pt::Gfx::Math::zfint(x);
    const Pt::int32_t py = Pt::Gfx::Math::zfint(y);

    // Pointer to the first pixel
    const Pt::int32_t* p0 = img + py * imgW + px;

    // Load the four neighboring pixels
    const __m128i p12      = _mm_loadl_epi64    ( (const __m128i*) &p0[0 * imgW]              ); // ? ? ? ? C C C C
    const __m128i p34      = _mm_loadl_epi64    ( (const __m128i*) &p0[1 * imgW]              ); // ? ? ? ? C C C C

    // Extend to 16-bit integer
    const __m128i p12ex    = _mm_unpacklo_epi8  (p12,        _mm_setzero_si128()              ); // 0 C 0 C 0 C 0 C
    const __m128i p34ex    = _mm_unpacklo_epi8  (p34,        _mm_setzero_si128()              ); // 0 C 0 C 0 C 0 C

    // Calculate weights
    const __m128  ssx      = _mm_set_ss         (x                                            ); // 0  0      0      X
    const __m128  ssy      = _mm_set_ss         (y                                            ); // 0  0      0      Y
    const __m128  zzyx     = _mm_unpacklo_ps    (ssx,        ssy                              ); // 0  0      Y      X
    const __m128i zzyxi    = _mm_cvttps_epi32   (zzyx                                         ); // 0  0      Yi     Xi
    const __m128  zzyxflor = _mm_cvtepi32_ps    (zzyxi                                        ); // 0  0      Yi     Xi
    const __m128  zzyxfrac = _mm_sub_ps         (zzyx,       zzyxflor                         ); // 0  0      Yr     Xr
    const __m128  ooyxfrac = _mm_sub_ps         (sseFour001, zzyxfrac                         ); // ?  ?      (1-Yr) (1-Xr)
    const __m128  wxh      = _mm_unpacklo_ps    (ooyxfrac,   zzyxfrac                         ); // ?  ?      Xr     (1-Xr)
    const __m128  wx       = _mm_movelh_ps      (wxh,        wxh                              ); // Xr (1-Xr) Xr     (1-Xr)
    const __m128  wy       = _mm_shuffle_ps     (ooyxfrac,   zzyxfrac, _MM_SHUFFLE(1, 1, 1, 1)); // Yr Yr     (1-Yr) (1-Yr)
    const __m128  weight   = _mm_mul_ps         (wx,         wy                               );

    // Convert the weights to 16-bit integer
    const __m128  sweight  = _mm_mul_ps         (weight,     sseFour256                       ); // W4  .   W3  .   W2  .   W1  .
    const __m128i sweighti = _mm_cvttps_epi32   (sweight                                      ); // W4i .   W3i .   W2i .   W1i .
    const __m128i weighth  = _mm_packs_epi32    (sweighti,   _mm_setzero_si128()              ); // 0   0   0   0   W4i W3i W2i W1i

    // Shuffle the weights
    const __m128i w12h     = _mm_shufflelo_epi16(weighth,    _MM_SHUFFLE(1, 1, 0, 0)          );
    const __m128i w12      = _mm_unpacklo_epi16 (w12h,       w12h                             ); // W2i W2i W2i W2i W1i W1i W1i W1i
    const __m128i w34h     = _mm_shufflelo_epi16(weighth,    _MM_SHUFFLE(3, 3, 2, 2)          );
    const __m128i w34      = _mm_unpacklo_epi16 (w34h,       w34h                             ); // W4i W4i W4i W4i W3i W3i W3i W3i

    // Multiply each pixel with the corresponding weight
    const __m128i r1r      = _mm_mullo_epi16    (p12ex,      w12                              );
    const __m128i r34      = _mm_mullo_epi16    (p34ex,      w34                              );

    // Add the results
    const __m128i r1234    = _mm_add_epi16      (r1r,        r34                              );
    const __m128i r1234h   = _mm_shuffle_epi32  (r1234,      _MM_SHUFFLE(3, 2, 3, 2)          );
    const __m128i r        = _mm_add_epi16      (r1234,      r1234h                           );

    // Divide the results by 256
    const __m128i rdiv256  = _mm_srli_epi16     (r,          8                                );

    // Convert back the results to 8-bit integer and pack
    const __m128i rfin     = _mm_packus_epi16   (rdiv256,    _mm_setzero_si128()              );

    // Return the result as a 32-bit integer
    return _mm_cvtsi128_si32(rfin);
}

#elif defined(PT_GFX_USEA_NEON)

inline Pt::int32_t bsGetPixel(const Pt::int32_t* img, Pt::ssize_t imgW, float x, float y)
{
    // Floor the coordinate
    const Pt::int32_t px = Pt::Gfx::Math::zfint(x);
    const Pt::int32_t py = Pt::Gfx::Math::zfint(y);

    // Pointer to the first pixel
    const Pt::int32_t* p0 = img + py * imgW + px;


    // Load the four neighboring pixels
    const uint8x8_t p12      = vld1_u8    ( (const uint8_t*) &p0[0 * imgW]              ); // ? ? ? ? C C C C
    const uint8x8_t p34      = vld1_u8    ( (const uint8_t*) &p0[1 * imgW]              ); // ? ? ? ? C C C C

    /*

    // Extend to 16-bit integer
    const __m128i p12ex    = _mm_unpacklo_epi8  (p12,        _mm_setzero_si128()              ); // 0 C 0 C 0 C 0 C
    const __m128i p34ex    = _mm_unpacklo_epi8  (p34,        _mm_setzero_si128()              ); // 0 C 0 C 0 C 0 C

    // Calculate weights
    const __m128  ssx      = _mm_set_ss         (x                                            ); // 0  0      0      X
    const __m128  ssy      = _mm_set_ss         (y                                            ); // 0  0      0      Y
    const __m128  zzyx     = _mm_unpacklo_ps    (ssx,        ssy                              ); // 0  0      Y      X
    const __m128i zzyxi    = _mm_cvttps_epi32   (zzyx                                         ); // 0  0      Yi     Xi
    const __m128  zzyxflor = _mm_cvtepi32_ps    (zzyxi                                        ); // 0  0      Yi     Xi
    const __m128  zzyxfrac = _mm_sub_ps         (zzyx,       zzyxflor                         ); // 0  0      Yr     Xr
    const __m128  ooyxfrac = _mm_sub_ps         (sseFour001, zzyxfrac                         ); // ?  ?      (1-Yr) (1-Xr)
    const __m128  wxh      = _mm_unpacklo_ps    (ooyxfrac,   zzyxfrac                         ); // ?  ?      Xr     (1-Xr)
    const __m128  wx       = _mm_movelh_ps      (wxh,        wxh                              ); // Xr (1-Xr) Xr     (1-Xr)
    const __m128  wy       = _mm_shuffle_ps     (ooyxfrac,   zzyxfrac, _MM_SHUFFLE(1, 1, 1, 1)); // Yr Yr     (1-Yr) (1-Yr)
    const __m128  weight   = _mm_mul_ps         (wx,         wy                               );

    // Convert the weights to 16-bit integer
    const __m128  sweight  = _mm_mul_ps         (weight,     sseFour256                       ); // W4  .   W3  .   W2  .   W1  .
    const __m128i sweighti = _mm_cvttps_epi32   (sweight                                      ); // W4i .   W3i .   W2i .   W1i .
    const __m128i weighth  = _mm_packs_epi32    (sweighti,   _mm_setzero_si128()              ); // 0   0   0   0   W4i W3i W2i W1i

    // Shuffle the weights
    const __m128i w12h     = _mm_shufflelo_epi16(weighth,    _MM_SHUFFLE(1, 1, 0, 0)          );
    const __m128i w12      = _mm_unpacklo_epi16 (w12h,       w12h                             ); // W2i W2i W2i W2i W1i W1i W1i W1i
    const __m128i w34h     = _mm_shufflelo_epi16(weighth,    _MM_SHUFFLE(3, 3, 2, 2)          );
    const __m128i w34      = _mm_unpacklo_epi16 (w34h,       w34h                             ); // W4i W4i W4i W4i W3i W3i W3i W3i

    // Multiply each pixel with the corresponding weight
    const __m128i r1r      = _mm_mullo_epi16    (p12ex,      w12                              );
    const __m128i r34      = _mm_mullo_epi16    (p34ex,      w34                              );

    // Add the results
    const __m128i r1234    = _mm_add_epi16      (r1r,        r34                              );
    const __m128i r1234h   = _mm_shuffle_epi32  (r1234,      _MM_SHUFFLE(3, 2, 3, 2)          );
    const __m128i r        = _mm_add_epi16      (r1234,      r1234h                           );

    // Divide the results by 256
    const __m128i rdiv256  = _mm_srli_epi16     (r,          8                                );

    // Convert back the results to 8-bit integer and pack
    const __m128i rfin     = _mm_packus_epi16   (rdiv256,    _mm_setzero_si128()              );

    // Return the result as a 32-bit integer
    return _mm_cvtsi128_si32(rfin);
    */

    return 0;
}

#else

inline Pt::int32_t bsGetPixel(const Pt::int32_t* img, Pt::ssize_t imgW, float x, float y)
{
    // Used for processing the pixels
    union Pixel4 {
        Pt::uint8_t c[4];
        Pt::int32_t i;
    };

    // Convert to fixed-points
    const Pt::int32_t Fx = Pt::Gfx::Math::zrint(x * 256);
    const Pt::int32_t Fy = Pt::Gfx::Math::zrint(y * 256);

    // Floor the coordinate
    const Pt::int32_t px = Fx & 0x0000FF00;
    const Pt::int32_t py = Fy & 0x0000FF00;

    // Pointer to the first pixel
    const Pixel4* p0 = (const Pixel4*) img + (py >> 8) * imgW + (px >> 8);

    // Load the four neighboring pixels
    const Pixel4& p1 = p0[0 + 0 * imgW];
    const Pixel4& p2 = p0[1 + 0 * imgW];
    const Pixel4& p3 = p0[0 + 1 * imgW];
    const Pixel4& p4 = p0[1 + 1 * imgW];

    // Calculate the weights for each pixel
    const Pt::int32_t fx  = Fx & 0x000000FF;
    const Pt::int32_t fy  = Fy & 0x000000FF;
    const Pt::int32_t fx1 = 255 - fx;
    const Pt::int32_t fy1 = 255 - fy;

    const Pt::int32_t w1 = fx1 * fy1;
    const Pt::int32_t w2 = fx  * fy1;
    const Pt::int32_t w3 = fx1 * fy;
    const Pt::int32_t w4 = fx  * fy;

    // Calculate the weighted sum of pixels
    Pixel4 r;
    r.c[0] = (p1.c[0] * w1 + p2.c[0] * w2 + p3.c[0] * w3 + p4.c[0] * w4) >> 16;
    r.c[1] = (p1.c[1] * w1 + p2.c[1] * w2 + p3.c[1] * w3 + p4.c[1] * w4) >> 16;
    r.c[2] = (p1.c[2] * w1 + p2.c[2] * w2 + p3.c[2] * w3 + p4.c[2] * w4) >> 16;
    r.c[3] = (p1.c[3] * w1 + p2.c[3] * w2 + p3.c[3] * w3 + p4.c[3] * w4) >> 16;

    // Return the resulting pixel
    return r.i;
}

#endif


// ======================================================================================
// ===== Public API =====================================================================
// ======================================================================================

template <typename InIterT, typename OutIterT>
void blockScale4(
    InIterT  from, Pt::ssize_t fromWidth, Pt::ssize_t fromHeight,
    OutIterT to,   Pt::ssize_t toWidth,   Pt::ssize_t toHeight
)
{
    const Pt::int32_t* src = (const Pt::int32_t*) from->base();
          Pt::int32_t* dst = (      Pt::int32_t*) to  ->base();

    const float incX = (float) fromWidth  / (float) toWidth;
    const float incY = (float) fromHeight / (float) toHeight;

    float itrY = 0;
    for(Pt::ssize_t y = 0; y < toHeight; ++y) {
        const Pt::int32_t srcY   = Pt::Gfx::Math::zrint(itrY)  * fromWidth;
              float       itrX = 0;
        for(Pt::ssize_t x = 0; x < toWidth; ++x) {
            *dst++ = src[srcY + Pt::Gfx::Math::zrint(itrX)];
            itrX += incX;
        }
        itrY += incY;
    }
}

template <typename InIterT, typename OutIterT>
void bilinearScale4(
    InIterT  from, Pt::ssize_t fromWidth, Pt::ssize_t fromHeight,
    OutIterT to,   Pt::ssize_t toWidth,   Pt::ssize_t toHeight
)
{
    const Pt::int32_t* src = (const Pt::int32_t*) from->base();
          Pt::int32_t* dst = (      Pt::int32_t*) to  ->base();

    const float incX = (float) fromWidth  / (float) toWidth;
    const float incY = (float) fromHeight / (float) toHeight;

    float itrY = 0;
    for(Pt::ssize_t y = 0; y < toHeight; ++y) {
        float itrX = 0;
        for(Pt::ssize_t x = 0; x < toWidth; ++x) {
            *dst++ = bsGetPixel(src, fromWidth, itrX, itrY);
            itrX += incX;
        }
        itrY += incY;
    }
}


} // namespace
} // namespace


#endif
