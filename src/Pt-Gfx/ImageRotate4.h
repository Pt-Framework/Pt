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

#ifndef PT_GFX_IMAGEROTATE4_H
#define PT_GFX_IMAGEROTATE4_H

#include <Pt/Gfx/Color.h>
#include <Pt/Gfx/Math.h>


namespace Pt {
namespace Gfx {


// ======================================================================================
// ===== Helper Functions ===============================================================
// ======================================================================================

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
// ===== Rotate Functions ===============================================================
// ======================================================================================

template <bool full, typename InIterT, typename OutIterT>
static inline void blockRotate4(
    InIterT      from, Pt::ssize_t  fromWidth, Pt::ssize_t fromHeight,
    OutIterT     to,   Pt::ssize_t  toWidth,   Pt::ssize_t toHeight,
    float        deg,  const Color& cfil
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
static inline void bilinearRotate4(
    InIterT      from, Pt::ssize_t  fromWidth, Pt::ssize_t fromHeight,
    OutIterT     to,   Pt::ssize_t  toWidth,   Pt::ssize_t toHeight,
    float        deg,  const Color& cfil
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
