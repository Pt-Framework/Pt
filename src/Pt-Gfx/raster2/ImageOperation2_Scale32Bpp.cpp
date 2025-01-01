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


#include "SIMDConfig.h"
#include "ImageOperation2.h"

namespace Pt {

namespace Gfx {


// ======================================================================================
// ===== Internal Implementation Functions===============================================
// ======================================================================================

#if defined(PT_GFX_USE_SSE4P1) || defined(PT_GFX_USE_SSE2)

// SSE constants
static const __m128 sseFour001 = _mm_set1_ps(  1);
static const __m128 sseFour256 = _mm_set1_ps(256);

// Based on: FastC++: Coding Cpp Efficiently
//           Bilinear Pixel Interpolation using SSE
//           http://fastcpp.blogspot.co.id/2011/06/bilinear-pixel-interpolation-using-sse.html
//           Blog by theowl84, 2011
static inline Pt::uint32_t bsGetPixel32Bpp_implSIMD(const Pt::uint8_t* img, Pt::ssize_t imgS, Pt::ssize_t imgW, Pt::ssize_t imgH, float x, float y)
{
    // Floor and limit the coordinates
    Pt::int32_t px = lround(floor(x));
    Pt::int32_t py = lround(floor(y));

    if(px + 1 >= imgW) {
        px = imgW - 2;
        x  = px;
    }

    if(py + 1 >= imgH) {
        py = imgH - 2;
        y  = py;
    }

    // Pointer to the rows
    const __m128i* r0 = reinterpret_cast<const __m128i*>(img + (py + 0) * imgS + px * 4);
    const __m128i* r1 = reinterpret_cast<const __m128i*>(img + (py + 1) * imgS + px * 4);

    // Load the four neighboring pixels
    const __m128i p12      = _mm_loadl_epi64   ( r0                                          );
    const __m128i p34      = _mm_loadl_epi64   ( r1                                          );

#if defined(PT_GFX_USE_SSE4P1)

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
    return static_cast<Pt::uint32_t>( _mm_cvtsi128_si32(rfin) );

#elif defined(PT_GFX_USE_SSE2)

    // Extend to 16-bit integer
    const __m128i p12ex    = _mm_unpacklo_epi8  (p12,        _mm_setzero_si128()              );
    const __m128i p34ex    = _mm_unpacklo_epi8  (p34,        _mm_setzero_si128()              );

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
    return static_cast<Pt::uint32_t>( _mm_cvtsi128_si32(rfin) );

#endif
}

#else

static inline Pt::uint32_t bsGetPixel32Bpp_implFP(const Pt::uint8_t* img, Pt::ssize_t imgS, Pt::ssize_t imgW, Pt::ssize_t imgH, Pt::uint32_t Fx, Pt::uint32_t Fy)
{
    // Used for processing the pixels
    union Pixel4 {
        Pt::uint8_t  c[4];
        Pt::uint32_t i;
    };

    // Floor the coordinates, convert them to normal integers, and limit them
    Pt::uint32_t px = (Fx & 0xFFFF0000) >> 16;
    Pt::uint32_t py = (Fy & 0xFFFF0000) >> 16;

    if(px + 1 >= (Pt::uint32_t) imgW) {
        px = imgW - 2;
        Fx = 65535;
    }

    if(py + 1 >= (Pt::uint32_t) imgH) {
        py = imgH - 2;
        Fy = 65535;
    }

    // Load the four neighboring pixels
    const Pixel4& p1 = *reinterpret_cast<const Pixel4*>( img + (py + 0) * imgS + (px + 0) * 4 );
    const Pixel4& p2 = *reinterpret_cast<const Pixel4*>( img + (py + 0) * imgS + (px + 1) * 4 );
    const Pixel4& p3 = *reinterpret_cast<const Pixel4*>( img + (py + 1) * imgS + (px + 0) * 4 );
    const Pixel4& p4 = *reinterpret_cast<const Pixel4*>( img + (py + 1) * imgS + (px + 1) * 4 );

    // Calculate the weights for each pixel
    const Pt::uint32_t fx  = Fx & 0x0000FFFF;
    const Pt::uint32_t fy  = Fy & 0x0000FFFF;
    const Pt::uint32_t fx1 = 65535 - fx;
    const Pt::uint32_t fy1 = 65535 - fy;

    const Pt::uint32_t w1 = (fx1 * fy1) >> 16;
    const Pt::uint32_t w2 = (fx  * fy1) >> 16;
    const Pt::uint32_t w3 = (fx1 * fy ) >> 16;
    const Pt::uint32_t w4 = (fx  * fy ) >> 16;

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

template <bool bilinear>
static inline void bblScale32Bpp_implFP(
    const Pt::uint8_t* src, Pt::ssize_t srcS, Pt::ssize_t srcW, Pt::ssize_t srcH,
          Pt::uint8_t* dst, Pt::ssize_t dstS, Pt::ssize_t dstW, Pt::ssize_t dstH
)
{
    // Calculate the increment factors
    const Pt::uint32_t FincX = 65536 * srcW / dstW;
    const Pt::uint32_t FincY = 65536 * srcH / dstH;

    // Walk through the row pixels
    Pt::uint32_t FitrY = 0;
    for(Pt::ssize_t y = 0; y < dstH; ++y) {
        // Calculate the source row address
        const Pt::int32_t FsrcY = ( (FitrY + 32768) >> 16 );
        // Walk through the column pixels
        Pt::uint32_t FitrX = 0;
        for(Pt::ssize_t x = 0; x < dstH; ++x) {
#if !defined(PT_GFX_USE_SSE4P1) && !defined(PT_GFX_USE_SSE2)
            // Bilinear scaling (non SIMD)
            if(bilinear) {
                *reinterpret_cast<Pt::uint32_t*>(dst) = bsGetPixel32Bpp_implFP(src, srcS, srcW, srcH, FitrX, FitrY);
                dst += 4;
            }
            // Block scaling
            else {
#endif
                const Pt::uint32_t offset = FsrcY * srcS + ( (FitrX + 32768) >> 16 ) * 4;
                *reinterpret_cast<Pt::uint32_t*>(dst) = *reinterpret_cast<const Pt::uint32_t*>(src + offset);
                dst += 4;
#if !defined(PT_GFX_USE_SSE4P1) && !defined(PT_GFX_USE_SSE2)
            }
#endif
            // Increment the iterator
            FitrX += FincX;
        }
        // Increment the iterator
        FitrY += FincY;
        // Adjust the destination pointer
        dst += (dstS - dstW * 4);
    }
}


// ======================================================================================
// ===== Public Member Functions ========================================================
// ======================================================================================

void ImageOperation2::blockScale32Bpp(
    const Pt::uint8_t* src, Pt::ssize_t srcS, Pt::ssize_t srcW, Pt::ssize_t srcH,
          Pt::uint8_t* dst, Pt::ssize_t dstS, Pt::ssize_t dstW, Pt::ssize_t dstH
)
{
    bblScale32Bpp_implFP<false>(
        src, srcS, srcW, srcH,
        dst, dstS, dstW, dstH
    );
}

void ImageOperation2::bilinearScale32Bpp(
    const Pt::uint8_t* src, Pt::ssize_t srcS, Pt::ssize_t srcW, Pt::ssize_t srcH,
          Pt::uint8_t* dst, Pt::ssize_t dstS, Pt::ssize_t dstW, Pt::ssize_t dstH
)
{
#if defined(PT_GFX_USE_SSE4P1) || defined(PT_GFX_USE_SSE2)

    // Calculate the increment factors
    const float incX = (float) srcW / dstW;
    const float incY = (float) srcH / dstH;;

    // Walk through the row pixels
    float itrY = 0;
    for(Pt::ssize_t y = 0; y < dstH; ++y) {
        // Walk through the column pixels
        float itrX = 0;
        for(Pt::ssize_t x = 0; x < dstW; ++x) {
            // Get the interpolated pixel (SIMD)
            *reinterpret_cast<Pt::uint32_t*>(dst) = bsGetPixel32Bpp_implSIMD(src, srcS, srcW, srcH, itrX, itrY);
            dst += 4;
            // Increment the iterator
            itrX += incX;
        }
        // Increment the iterator
        itrY += incY;
        // Adjust the destination pointer
        dst += (dstS - dstW * 4);
    }

#else

    bblScale32Bpp_implFP<true>(
        src, srcS, srcW, srcH,
        dst, dstS, dstW, dstH
    );

#endif
}


} // namespace
} // namespace

