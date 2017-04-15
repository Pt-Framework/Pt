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

#ifndef PT_GFX_IMAGEOPERATION4_H
#define PT_GFX_IMAGEOPERATION4_H

#include <Pt/Gfx/Math.h>
#include <Pt/Gfx/SIMDConfig.h>


//#undef PT_GFX_USE_SSE4P1
//#undef PT_GFX_USE_SSE2


namespace Pt {
namespace Gfx {


#if defined(PT_GFX_USE_SSE4P1) || defined(PT_GFX_USE_SSE2)

// SSE constants
static const __m128 sseFour001 = _mm_set1_ps(  1);
static const __m128 sseFour256 = _mm_set1_ps(256);

#endif


// ======================================================================================
// ===== Internal Implementation ========================================================
// ======================================================================================

// Based on: FastC++: Coding Cpp Efficiently
//           Bilinear Pixel Interpolation using SSE
//           http://fastcpp.blogspot.co.id/2011/06/bilinear-pixel-interpolation-using-sse.html
//           Blog by theowl84, 2011

#if defined(PT_GFX_USE_SSE4P1)

static inline Pt::uint32_t bsGetPixel(const Pt::uint32_t* img, Pt::ssize_t imgW, float x, float y)
{

    // Floor the coordinates
    const Pt::int32_t px = Pt::Gfx::Math::zfint(x);
    const Pt::int32_t py = Pt::Gfx::Math::zfint(y);

    // Pointer to the first pixel
    const Pt::uint32_t* p0 = img + py * imgW + px;

    // Load the four neighboring pixels
    const __m128i p12      = _mm_loadl_epi64   ( (const __m128i*) &p0[0 * imgW]              );
    const __m128i p34      = _mm_loadl_epi64   ( (const __m128i*) &p0[1 * imgW]              );

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
}

#elif defined(PT_GFX_USE_SSE2)

static inline Pt::uint32_t bsGetPixel(const Pt::uint32_t* img, Pt::ssize_t imgW, float x, float y)
{
    // Floor the coordinates
    const Pt::int32_t px = Pt::Gfx::Math::zfint(x);
    const Pt::int32_t py = Pt::Gfx::Math::zfint(y);

    // Pointer to the first pixel
    const Pt::uint32_t* p0 = img + py * imgW + px;

    // Load the four neighboring pixels
    const __m128i p12      = _mm_loadl_epi64    ( (const __m128i*) &p0[0 * imgW]              );
    const __m128i p34      = _mm_loadl_epi64    ( (const __m128i*) &p0[1 * imgW]              );

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
}

#else

static inline Pt::uint32_t bsGetPixel(const Pt::uint32_t* img, Pt::ssize_t imgW, Pt::uint32_t Fx, Pt::uint32_t Fy)
{
    // Used for processing the pixels
    union Pixel4 {
        Pt::uint8_t  c[4];
        Pt::uint32_t i;
    };

    // Floor the coordinates
    const Pt::uint32_t px = Fx & 0xFFFF0000;
    const Pt::uint32_t py = Fy & 0xFFFF0000;

    // Pointer to the first pixel
    const Pixel4* p0 = reinterpret_cast<const Pixel4*>( img + (py >> 16) * imgW + (px >> 16) );

    // Load the four neighboring pixels
    const Pixel4& p1 = p0[0 * imgW + 0];
    const Pixel4& p2 = p0[0 * imgW + 1];
    const Pixel4& p3 = p0[1 * imgW + 0];
    const Pixel4& p4 = p0[1 * imgW + 1];

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

static inline Pt::uint32_t bsMixPixel(const Pt::uint32_t* img, Pt::ssize_t imgW, Pt::ssize_t imgH, Pt::int32_t Fx, Pt::int32_t Fy, Pt::uint32_t fil)
{
    // Used for processing the pixels
    union Pixel4 {
        Pt::uint8_t  c[4];
        Pt::uint32_t i;
    };

    // Floor the coordinates
    const Pt::int32_t px = Fx & 0xFFFF0000;
    const Pt::int32_t py = Fy & 0xFFFF0000;

    // Pointer to the origin pixel
    const Pixel4* p = reinterpret_cast<const Pixel4*>(img);

    // Convert the coordinates from fixed-point to normal integer
    const Pt::int32_t pxi = px >> 16;
    const Pt::int32_t pyi = py >> 16;

    // Load the four neighboring pixels
    Pixel4 p1;
    if(pxi + 0 < 0 || pyi + 0 < 0 || pxi + 0 >= imgW || pyi + 0 >= imgH) p1.i = fil;
    else                                                                 p1   = p[ (pyi + 0) * imgW + pxi + 0 ];

    Pixel4 p2;
    if(pxi + 1 < 0 || pyi + 0 < 0 || pxi + 1 >= imgW || pyi + 0 >= imgH) p2.i = fil;
    else                                                                 p2   = p[ (pyi + 0) * imgW + pxi + 1 ];

    Pixel4 p3;
    if(pxi + 0 < 0 || pyi + 1 < 0 || pxi + 0 >= imgW || pyi + 1 >= imgH) p3.i = fil;
    else                                                                 p3   = p[ (pyi + 1) * imgW + pxi + 0 ];

    Pixel4 p4;
    if(pxi + 1 < 0 || pyi + 1 < 0 || pxi + 1 >= imgW || pyi + 1 >= imgH) p4.i = fil;
    else                                                                 p4   = p[ (pyi + 1) * imgW + pxi + 1 ];

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


// ======================================================================================
// ===== Public API =====================================================================
// ======================================================================================

template <typename InIterT, typename OutIterT>
inline void blockScale4(
    InIterT  from, Pt::ssize_t fromWidth, Pt::ssize_t fromHeight,
    OutIterT to,   Pt::ssize_t toWidth,   Pt::ssize_t toHeight
)
{
    // Get the source and destination pointers
    const Pt::uint32_t* src = reinterpret_cast<const Pt::uint32_t*>( from->base() );
          Pt::uint32_t* dst = reinterpret_cast<      Pt::uint32_t*>( to  ->base() );

    // Calculate the increment factors
    const Pt::uint32_t incX = 65536 * fromWidth  / toWidth;
    const Pt::uint32_t incY = 65536 * fromHeight / toHeight;

    // Walk through the pixels
    Pt::uint32_t itrY = 0;
    for(Pt::ssize_t y = 0; y < toHeight; ++y) {
        const Pt::int32_t  srcY = ( (itrY + 32768) >> 16 ) * fromWidth;
              Pt::uint32_t itrX = 0;
        for(Pt::ssize_t x = 0; x < toWidth; ++x) {
            *dst++ = src[ srcY + ( (itrX + 32768) >> 16 ) ];
            itrX += incX;
        }
        itrY += incY;
    }
}

template <typename InIterT, typename OutIterT>
inline void bilinearScale4(
    InIterT  from, Pt::ssize_t fromWidth, Pt::ssize_t fromHeight,
    OutIterT to,   Pt::ssize_t toWidth,   Pt::ssize_t toHeight
)
{
    // Get the source and destination pointers
    const Pt::uint32_t* src = reinterpret_cast<const Pt::uint32_t*>( from->base() );
          Pt::uint32_t* dst = reinterpret_cast<      Pt::uint32_t*>( to  ->base() );

    // Calculate the increment factors
#if defined(PT_GFX_USE_SSE4P1) || defined(PT_GFX_USE_SSE2)
    typedef float ValueT;
    const ValueT incX = (ValueT) fromWidth  / toWidth;
    const ValueT incY = (ValueT) fromHeight / toHeight;
#else
    typedef Pt::uint32_t ValueT;
    const ValueT incX = 65536 * fromWidth  / toWidth;
    const ValueT incY = 65536 * fromHeight / toHeight;
#endif

    // Walk through the pixels
    ValueT itrY = 0;
    for(Pt::ssize_t y = 0; y < toHeight; ++y) {
        ValueT itrX = 0;
        for(Pt::ssize_t x = 0; x < toWidth; ++x) {
            *dst++ = bsGetPixel(src, fromWidth, itrX, itrY);
            itrX += incX;
        }
        itrY += incY;
    }
}

template <bool full, typename InIterT, typename OutIterT>
inline void blockRotate4(
    InIterT      from, Pt::ssize_t fromWidth, Pt::ssize_t fromHeight,
    OutIterT     to,   Pt::ssize_t toWidth,   Pt::ssize_t toHeight,
    float        deg,
    const Color& cfil = Color::fromRgb8(0, 0, 0, 255)
)
{
    // Calculate the filler color
    const Pt::uint32_t fil = ( Pt::uint32_t(cfil.alpha() & 0xFF00) << 16 ) |
                             ( Pt::uint32_t(cfil.red  () & 0xFF00) <<  8 ) |
                               Pt::uint32_t(cfil.green() & 0xFF00)         |
                             ( Pt::uint32_t(cfil.blue ()         ) >>  8 );

    // Get the source and destination pointers
    const Pt::uint32_t* src = reinterpret_cast<const Pt::uint32_t*>( from->base() );
          Pt::uint32_t* dst = reinterpret_cast<      Pt::uint32_t*>( to  ->base() );

    // Calculate the increment factors
    const Pt::int32_t incX = 65536 * fromWidth  / toWidth;
    const Pt::int32_t incY = 65536 * fromHeight / toHeight;

    // Calculate the center positions
    const Pt::int32_t midX = 32768 * fromWidth;
    const Pt::int32_t midY = 32768 * fromHeight;

    // Calculate the sine and cosine values
    const double      r = -deg * (M_PI / 180);
    const Pt::int32_t s = Pt::Gfx::Math::zrint( 256 * ::sin(r) );
    const Pt::int32_t c = Pt::Gfx::Math::zrint( 256 * ::cos(r) );
    const Pt::int32_t f = ::abs(s) + ::abs(c);

    // Walk through the row pixels
    Pt::int32_t itrY = 0;
    for(Pt::ssize_t y = 0; y < toHeight; ++y) {
        // Walk through the column pixels
        Pt::int32_t itrX = 0;
        for(Pt::ssize_t x = 0; x < toWidth; ++x) {
            // Get the centered source coordinates
            const Pt::int32_t srcX = full ? ( (itrX - midX) / f ) : ( (itrX - midX) >> 8 );
            const Pt::int32_t srcY = full ? ( (itrY - midY) / f ) : ( (itrY - midY) >> 8 );
            // Rotate the coordinates and offset them back
            const Pt::int32_t rotX =  c * srcX + s * srcY + midX;
            const Pt::int32_t rotY = -s * srcX + c * srcY + midY;
            // Calculate the read coordinates
            const Pt::int32_t getX = (rotX + 32768) >> 16;
            const Pt::int32_t getY = (rotY + 32768) >> 16;
            // Check if the any of the coordinates is outside the image
            if(getX < 0 || getY < 0 || getX >= fromWidth || getY >= fromHeight) {
                *dst++ = fil;
            }
            // The coordinates are inside the image
            else {
                *dst++ = src[getY * fromWidth + getX];
            }
            // Increment the iterator
            itrX += incX;
        }
        // Increment the iterator
        itrY += incY;
    }
}

template <bool full, typename InIterT, typename OutIterT>
inline void bilinearRotate4(
    InIterT      from, Pt::ssize_t fromWidth, Pt::ssize_t fromHeight,
    OutIterT     to,   Pt::ssize_t toWidth,   Pt::ssize_t toHeight,
    float        deg,
    const Color& cfil = Color::fromRgb8(0, 0, 0, 255)
)
{
    // Get the filler color
    const Pt::uint32_t fil = ( Pt::uint32_t(cfil.alpha() & 0xFF00) << 16 ) |
                             ( Pt::uint32_t(cfil.red  () & 0xFF00) <<  8 ) |
                               Pt::uint32_t(cfil.green() & 0xFF00)         |
                             ( Pt::uint32_t(cfil.blue ()         ) >>  8 );

    // Get the source and destination pointers
    const Pt::uint32_t* src = reinterpret_cast<const Pt::uint32_t*>( from->base() );
          Pt::uint32_t* dst = reinterpret_cast<      Pt::uint32_t*>( to  ->base() );

    // Calculate the increment factors
    const Pt::int32_t incX = 65536 * fromWidth  / toWidth;
    const Pt::int32_t incY = 65536 * fromHeight / toHeight;

    // Calculate the center positions
    const Pt::int32_t midX = 32768 * fromWidth;
    const Pt::int32_t midY = 32768 * fromHeight;

    // Calculate the sine and cosine values
    const double      r = -deg * (M_PI / 180);
    const Pt::int32_t s = Pt::Gfx::Math::zrint( 256 * ::sin(r) );
    const Pt::int32_t c = Pt::Gfx::Math::zrint( 256 * ::cos(r) );
    const Pt::int32_t f = ::abs(s) + ::abs(c);

    // Walk through the row pixels
    Pt::int32_t itrY = 0;
    for(Pt::ssize_t y = 0; y < toHeight; ++y) {
        // Walk through the column pixels
        Pt::int32_t itrX = 0;
        for(Pt::ssize_t x = 0; x < toWidth; ++x) {
            // Get the centered source coordinates
            const Pt::int32_t srcX = full ? ( (itrX - midX) / f ) : ( (itrX - midX) >> 8 );
            const Pt::int32_t srcY = full ? ( (itrY - midY) / f ) : ( (itrY - midY) >> 8 );
            // Rotate the coordinates and offset them back
            const Pt::int32_t rotX =  c * srcX + s * srcY + midX;
            const Pt::int32_t rotY = -s * srcX + c * srcY + midY;
            // Get and mix the pixel with the filler color as needed
            *dst++ = bsMixPixel(src, fromWidth, fromHeight, rotX, rotY, fil);
            // Increment the iterator
            itrX += incX;
        }
        // Increment the iterator
        itrY += incY;
    }
}


} // namespace
} // namespace


#endif

/*
-----------------------------
x86_64 (i5-4460; 64-Bit Mode)
-----------------------------
                                                   (Time) (Factor)
                                                   ------ --------
Image scaling  4 (block               )          =     10
Image scaling  4 (bilinear            )          =    120 (12.000)

Image rotation 4 (block    - normal   )          =     36
Image rotation 4 (block    - fullscale)          =     73 ( 2.028)
Image rotation 4 (bilinear - normal   )          =    198 ( 5.500)
Image rotation 4 (bilinear - fullscale)          =    291 ( 8.083)
*/

/*
---------------------------------------------------
Arm (v7l; A53; BCM2709; RaspberryPi 3; 32-bit Mode)
---------------------------------------------------
Image scaling  4 (block               )          =    144
Image scaling  4 (bilinear            )          =   1425 ( 9.896)

Image rotation 4 (block    - normal   )          =    650
Image rotation 4 (block    - fullscale)          =   2148 ( 3.305)
Image rotation 4 (bilinear - normal   )          =   2887 ( 4.442)
Image rotation 4 (bilinear - fullscale)          =   3569 ( 5.491)
*/
