/* Copyright (C) 2017-2017 Aloysius Indrayanto
   Copyright (C) 2006-2015 Marc Boris Duerner
   Copyright (C) 2006-2015 Laurentiu-Gheorghe Crisan

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

#include "Rasterizer2.h"


namespace Pt {
namespace Gfx {

#include <math.h>
// ======================================================================================
// ===== Private Member Functions =======================================================
// ======================================================================================
void Rasterizer2::rasterOnePixelPatternedLine(Pt::int32_t x1, Pt::int32_t y1, Pt::int32_t x2, Pt::int32_t y2, const Color& color, DrawLineMask* maskInOut)
{
    // Check the size of the line
    const Pt::int32_t sizeX = abs(x2 - x1) + 1;
    const Pt::int32_t sizeY = abs(y2 - y1) + 1;

    // Calculate the incremental factor of the pattern indexing counter
    const Pt::int32_t piCtrInc = PI_CTR_INC_MUL_FACTOR * (sizeX + sizeY) / sqrtf(sizeX * sizeX + sizeY * sizeY) - PI_CTR_INC_SUB_FACTOR;


    //
    Pt::int32_t piCtrInOut = 0;
    rasterOnePixelPatternedGLineSegmentNoAA(x1, y1, x2, y2, color, piCtrInc, piCtrInOut, maskInOut);

}

// Bresenham's Line Aalgorithm
// https://en.wikipedia.org/wiki/Bresenham's_line_algorithm
// https://rosettacode.org/wiki/Bitmap/Bresenham%27s_line_algorithm
void Rasterizer2::rasterOnePixelPatternedGLineSegmentNoAA(Pt::int32_t x1, Pt::int32_t y1, Pt::int32_t x2, Pt::int32_t y2, const Color& color, Pt::int32_t piCtrInc, Pt::int32_t& piCtrInOut, DrawLineMask* maskInOut)
{
    // Get the mask's coordinates as needed
    Pt::int32_t mx[4] = { MAXIMUM_COORD, MAXIMUM_COORD, MAXIMUM_COORD, MAXIMUM_COORD };
    Pt::int32_t my[4] = { MAXIMUM_COORD, MAXIMUM_COORD, MAXIMUM_COORD, MAXIMUM_COORD };

    if(maskInOut) {
        for(Pt::int32_t i = 0; i < 4; ++i) {
            mx[i] = (*maskInOut)[i].x();
            my[i] = (*maskInOut)[i].y();
        }
    }

    // Calculate the deltas
    const Pt::int32_t dx = abs(x2 - x1);
    const Pt::int32_t dy = abs(y2 - y1);

    // Determine the directions
    const Pt::int32_t sx = (x1 < x2) ? 1 : -1;
    const Pt::int32_t sy = (y1 < y2) ? 1 : -1;

    // Calculate the initial error
    Pt::int32_t err1 = (dx > dy ? dx : -dy) / 2;
    Pt::int32_t err2;

    // Draw the pixels
    Pt::int32_t x = x1;
    Pt::int32_t y = y1;
    for(;;) {

        Pt::uint8_t alpha = _patternBuffer[piCtrInOut];
        piCtrInOut += piCtrInc;
        if(piCtrInOut >= PI_PAT_TOTAL_BUF_SIZE) piCtrInOut = 0;

        // Check if we should skip drawing the pixel
        bool skipDrawing = false;
        for(Pt::int32_t i = 0; i < 4; ++i) {
            if(x != mx[i] || y != my[i]) continue;
            skipDrawing = true;
            break;
        }
        // Draw the pixel as needed
        if(!skipDrawing) {
            Pixel pixel(_image->view(), x, y);
            _image->format().setPixel(pixel, color, _compositionMode, alpha);
        }
        // Stop if we have reached the end
        if(x == x2 && y == y2) break;
        // Update the coordinates
        err2 = err1;
        if(err2 > -dx) {
            err1 -= dy;
            x    += sx;
        }
        if(err2 <  dy) {
            err1 += dx;
            y    += sy;
        }
    }

    // Store back the start and end coordinates to the mask as needed
    if(maskInOut) {
        (*maskInOut)[0].set(x1, y1);
        (*maskInOut)[1] = MAXIMUM_POINT;
        (*maskInOut)[2].set(x2, y2);
        (*maskInOut)[3] = MAXIMUM_POINT;
    }
}

#if 0
void Rasterizer2::rasterOnePixelHLineSegment(Pt::int32_t x1, Pt::int32_t x2, Pt::int32_t y, const Color& color, DrawLineMask* maskInOut)
{
    // Swap the coordinates as needed
    if(x1 > x2) std::swap(x1, x2);

    // Adjust the start and end coordinates as needed
    if(maskInOut) {
        if((*maskInOut)[0].x() == x1) ++x1;
        if((*maskInOut)[1].x() == x1) ++x1;
        if((*maskInOut)[2].x() == x2) --x2;
        if((*maskInOut)[3].x() == x2) --x2;
        if(x1 > x2) return;
    }

    // Store back the start and end coordinates to the mask as needed
    if(maskInOut) {
        (*maskInOut)[0].set(x1, y);
        (*maskInOut)[1] = MAXIMUM_POINT;
        (*maskInOut)[2].set(x2, y);
        (*maskInOut)[3] = MAXIMUM_POINT;
    }

    // Calculate the length of the line
    const Pt::int32_t sizeL = x2 - x1 + 1;

    // Draw the line
    Pixel pixel(_image->view(), x1, y);
    _image->format().setPixels(pixel, color, sizeL, _compositionMode);
}

void Rasterizer2::rasterOnePixelVLineSegment(Pt::int32_t x, Pt::int32_t y1, Pt::int32_t y2, const Color& color, DrawLineMask* maskInOut)
{
    // Swap the coordinates as needed
    if(y1 > y2) std::swap(y1, y2);

    // Adjust the start and end coordinates as needed
    if(maskInOut) {
        if((*maskInOut)[0].y() == y1) ++y1;
        if((*maskInOut)[1].y() == y1) ++y1;
        if((*maskInOut)[2].y() == y2) --y2;
        if((*maskInOut)[3].y() == y2) --y2;
        if(y1 > y2) return;
    }

    // Store back the start and end coordinates to the mask as needed
    if(maskInOut) {
        (*maskInOut)[0].set(x, y1);
        (*maskInOut)[1] = MAXIMUM_POINT;
        (*maskInOut)[2].set(x, y2);
        (*maskInOut)[3] = MAXIMUM_POINT;
    }

    // Calculate the length of the line
    const Pt::int32_t sizeL = y2 - y1 + 1;

    // Draw the line
    for(Pt::int32_t i = 0; i < sizeL; ++i) {
        Pixel pixel(_image->view(), x, y1++);
        _image->format().setPixel(pixel, color, _compositionMode);
    }
}

void Rasterizer2::rasterOnePixelXLineSegment(Pt::int32_t x1, Pt::int32_t y1, Pt::int32_t x2, Pt::int32_t y2, const Color& color, DrawLineMask* maskInOut)
{
    // Get the mask's coordinates as needed
    Pt::int32_t mx[4] = { MAXIMUM_COORD, MAXIMUM_COORD, MAXIMUM_COORD, MAXIMUM_COORD };
    Pt::int32_t my[4] = { MAXIMUM_COORD, MAXIMUM_COORD, MAXIMUM_COORD, MAXIMUM_COORD };

    if(maskInOut) {
        for(Pt::int32_t i = 0; i < 4; ++i) {
            mx[i] = (*maskInOut)[i].x();
            my[i] = (*maskInOut)[i].y();
        }
    }

    // Determine the directions
    const Pt::int32_t sx = (x1 < x2) ? 1 : -1;
    const Pt::int32_t sy = (y1 < y2) ? 1 : -1;

    // Draw the pixels
    Pt::int32_t x = x1;
    Pt::int32_t y = y1;
    for(;;) {
        // Check if we should skip drawing the pixel
        bool skipDrawing = false;
        for(Pt::int32_t i = 0; i < 4; ++i) {
            if(x != mx[i] || y != my[i]) continue;
            skipDrawing = true;
            break;
        }
        // Draw the pixel as needed
        if(!skipDrawing) {
            // Draw the primary pixel
            Pixel pixel(_image->view(), x, y);
            _image->format().setPixel(pixel, color, _compositionMode);
            // Draw the secondary pixels as needed
            if( _aaMode != AntiAliasingMode::None && ((x * y) & 1) ) {
                Pixel pixel1(_image->view(), x + 1, y);
                Pixel pixel2(_image->view(), x - 1, y);
                _image->format().setPixel(pixel1, color, _compositionMode, 63);
                _image->format().setPixel(pixel2, color, _compositionMode, 63);
            }
        }
        // Stop if we have reached the end
        if(x == x2 && y == y2) break;
        // Update the coordinates
        x += sx;
        y += sy;
    }

    // Store back the start and end coordinates to the mask as needed
    if(maskInOut) {
        (*maskInOut)[0].set(x1, y1);
        (*maskInOut)[1] = MAXIMUM_POINT;
        (*maskInOut)[2].set(x2, y2);
        (*maskInOut)[3] = MAXIMUM_POINT;
    }
}

// Bresenham's Line Aalgorithm
// https://en.wikipedia.org/wiki/Bresenham's_line_algorithm
// https://rosettacode.org/wiki/Bitmap/Bresenham%27s_line_algorithm
void Rasterizer2::rasterOnePixelGLineSegmentNoAA(Pt::int32_t x1, Pt::int32_t y1, Pt::int32_t x2, Pt::int32_t y2, const Color& color, DrawLineMask* maskInOut)
{
    // Get the mask's coordinates as needed
    Pt::int32_t mx[4] = { MAXIMUM_COORD, MAXIMUM_COORD, MAXIMUM_COORD, MAXIMUM_COORD };
    Pt::int32_t my[4] = { MAXIMUM_COORD, MAXIMUM_COORD, MAXIMUM_COORD, MAXIMUM_COORD };

    if(maskInOut) {
        for(Pt::int32_t i = 0; i < 4; ++i) {
            mx[i] = (*maskInOut)[i].x();
            my[i] = (*maskInOut)[i].y();
        }
    }

    // Calculate the deltas
    const Pt::int32_t dx = abs(x2 - x1);
    const Pt::int32_t dy = abs(y2 - y1);

    // Determine the directions
    const Pt::int32_t sx = (x1 < x2) ? 1 : -1;
    const Pt::int32_t sy = (y1 < y2) ? 1 : -1;

    // Calculate the initial error
    Pt::int32_t err1 = (dx > dy ? dx : -dy) / 2;
    Pt::int32_t err2;

    // Draw the pixels
    Pt::int32_t x = x1;
    Pt::int32_t y = y1;
    for(;;) {
        // Check if we should skip drawing the pixel
        bool skipDrawing = false;
        for(Pt::int32_t i = 0; i < 4; ++i) {
            if(x != mx[i] || y != my[i]) continue;
            skipDrawing = true;
            break;
        }
        // Draw the pixel as needed
        if(!skipDrawing) {
            Pixel pixel(_image->view(), x, y);
            _image->format().setPixel(pixel, color, _compositionMode);
        }
        // Stop if we have reached the end
        if(x == x2 && y == y2) break;
        // Update the coordinates
        err2 = err1;
        if(err2 > -dx) {
            err1 -= dy;
            x    += sx;
        }
        if(err2 <  dy) {
            err1 += dx;
            y    += sy;
        }
    }

    // Store back the start and end coordinates to the mask as needed
    if(maskInOut) {
        (*maskInOut)[0].set(x1, y1);
        (*maskInOut)[1] = MAXIMUM_POINT;
        (*maskInOut)[2].set(x2, y2);
        (*maskInOut)[3] = MAXIMUM_POINT;
    }
}

// Xiaolin Wu's Anti-Aliased Line Algorithm
// https://en.wikipedia.org/wiki/Xiaolin_Wu's_line_algorithm
void Rasterizer2::rasterOnePixelGLineSegmentXWAA(Pt::int32_t x1, Pt::int32_t y1, Pt::int32_t x2, Pt::int32_t y2, const Color& color, DrawLineMask* maskInOut)
{
    // Get the mask's coordinates as needed
    Pt::int32_t mx[4] = { MAXIMUM_COORD, MAXIMUM_COORD, MAXIMUM_COORD, MAXIMUM_COORD };
    Pt::int32_t my[4] = { MAXIMUM_COORD, MAXIMUM_COORD, MAXIMUM_COORD, MAXIMUM_COORD };

    if(maskInOut) {
        for(Pt::int32_t i = 0; i < 4; ++i) {
            mx[i] = (*maskInOut)[i].x();
            my[i] = (*maskInOut)[i].y();
        }
    }

    // Convert the coordinates to fixed-points
    Pt::int32_t fx1 = FIXED_POINT_FROM_INT(x1);
    Pt::int32_t fy1 = FIXED_POINT_FROM_INT(y1);
    Pt::int32_t fx2 = FIXED_POINT_FROM_INT(x2);
    Pt::int32_t fy2 = FIXED_POINT_FROM_INT(y2);

    // A helper macro to set pixel
    #define XW_SET_PIXEL(IMG, COL, X, Y, A)                                                \
        do {                                                                               \
            /* Check the boundary limit, just in case */                                   \
            if( (X) < 0 || (X) >= IMG->width() || (Y) < 0 || (Y) >= IMG->height() ) break; \
            /* Check if we should skip drawing the pixel */                                \
            bool skipDrawing = false;                                                      \
            for(Pt::int32_t j = 0; j < 4; ++j) {                                           \
                if( (X) != mx[j] || (Y) != my[j] ) continue;                               \
                skipDrawing = true;                                                        \
                break;                                                                     \
            }                                                                              \
            if(skipDrawing) break;                                                         \
            /* Set the pixel */                                                            \
            Pixel PIX(IMG->view(), X, Y);                                                  \
            IMG->format().setPixel(PIX, COL, _compositionMode, A);                         \
        } while(false)

    // Swap the values as needed
    const Pt::int32_t deltaX = (fx2 >= fx1) ? (fx2 - fx1) : (fx1 - fx2);
    const Pt::int32_t deltaY = (fy2 >= fy1) ? (fy2 - fy1) : (fy1 - fy2);
    const bool        steep  = deltaY > deltaX;

    if(steep) {
        std::swap(fx1, fy1);
        std::swap(fx2, fy2);
    }

    if(fx1 > fx2) {
        std::swap(fx1, fx2);
        std::swap(fy1, fy2);
    }

    // Handle the gradient, starting point, and ending point
    const Pt::int32_t grad = (fy2 - fy1) / FIXED_POINT_TO_INT(fx2 - fx1);
    const Pt::int32_t xpxl1 = FIXED_POINT_ROUND(fx1);
    const Pt::int32_t xpxl2 = FIXED_POINT_ROUND(fx2);
    const Pt::int32_t ypxl  = fy1 + grad * FIXED_POINT_TO_INT(xpxl1 - fx1);

    // Draw the pixels
    Pt::int32_t from  = FIXED_POINT_TO_INT(FIXED_POINT_ROUND(fx1));
    Pt::int32_t to    = FIXED_POINT_TO_INT(xpxl2);
    Pt::int32_t ypxli = ypxl;

    if(steep) {
        // Draw the pixels
        for(Pt::int32_t i = from; i <= to; ++i) {
            const Pt::uint8_t a1 = Rasterizer2::XWAA_WFILTER[ FIXED_POINT_FPART_TO_A8 (ypxli) ];
            const Pt::uint8_t a2 = Rasterizer2::XWAA_WFILTER[ FIXED_POINT_RFPART_TO_A8(ypxli) ];
            XW_SET_PIXEL(_image, color, FIXED_POINT_TO_INT(FIXED_POINT_IPART(ypxli)                           ), i, a1);
            XW_SET_PIXEL(_image, color, FIXED_POINT_TO_INT(FIXED_POINT_IPART(ypxli) + FIXED_POINT_CONSTANT_ONE), i, a2);
            ypxli += grad;
        }
        // Store back the start and end coordinates to the mask as needed
        if(maskInOut) {
            (*maskInOut)[0].set(FIXED_POINT_TO_INT(FIXED_POINT_IPART(ypxl )                           ), from);
            (*maskInOut)[1].set(FIXED_POINT_TO_INT(FIXED_POINT_IPART(ypxl ) + FIXED_POINT_CONSTANT_ONE), from);
            (*maskInOut)[2].set(FIXED_POINT_TO_INT(FIXED_POINT_IPART(ypxli)                           ), to  );
            (*maskInOut)[3].set(FIXED_POINT_TO_INT(FIXED_POINT_IPART(ypxli) + FIXED_POINT_CONSTANT_ONE), to  );
        }
    }
    else {
        // Draw the pixels
        for(Pt::int32_t i = from; i <= to; ++i) {
            const Pt::uint8_t a1 = Rasterizer2::XWAA_WFILTER[ FIXED_POINT_FPART_TO_A8 (ypxli) ];
            const Pt::uint8_t a2 = Rasterizer2::XWAA_WFILTER[ FIXED_POINT_RFPART_TO_A8(ypxli) ];
            XW_SET_PIXEL(_image, color, i, FIXED_POINT_TO_INT(FIXED_POINT_IPART(ypxli)                           ), a1);
            XW_SET_PIXEL(_image, color, i, FIXED_POINT_TO_INT(FIXED_POINT_IPART(ypxli) + FIXED_POINT_CONSTANT_ONE), a2);
            ypxli += grad;
        }
        // Store back the start and end coordinates to the mask as needed
        if(maskInOut) {
            (*maskInOut)[0].set(from, FIXED_POINT_TO_INT(FIXED_POINT_IPART(ypxl )                           ));
            (*maskInOut)[1].set(from, FIXED_POINT_TO_INT(FIXED_POINT_IPART(ypxl ) + FIXED_POINT_CONSTANT_ONE));
            (*maskInOut)[2].set(to,   FIXED_POINT_TO_INT(FIXED_POINT_IPART(ypxli)                           ));
            (*maskInOut)[3].set(to,   FIXED_POINT_TO_INT(FIXED_POINT_IPART(ypxli) + FIXED_POINT_CONSTANT_ONE));
        }
    }

    // Undefine the helper macro
    #undef XW_SET_PIXEL
}
#endif


} // namespace
} // namespace
