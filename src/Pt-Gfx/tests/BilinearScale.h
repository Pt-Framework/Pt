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


#if defined(PT_GFX_USEA_SSE4P1)

#elif defined(PT_GFX_USEA_SSE2)

#elif defined(PT_GFX_USEA_NEON)

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



template <typename InIterT, typename OutIterT>
void bilinearScale(
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
