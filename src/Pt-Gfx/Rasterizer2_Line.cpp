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


// ======================================================================================
// ===== Public Member Functions ========================================================
// ======================================================================================
void Rasterizer2::strokeOnePixelLine(const Point& a, const Point& b, DrawLineMask* maskInOut)
{
    // Clip the points
    Pt::int32_t x1 = a.x();
    Pt::int32_t y1 = a.y();
    Pt::int32_t x2 = b.x();
    Pt::int32_t y2 = b.y();

    if(!ClipShape::clipLine(x1, y1, x2, y2, _currentClip)) return;

    // Find the minimum and maximum coordinates
    Pt::int32_t minX, minY, maxX, maxY;

    if(x2 > x1) {
        minX = x1;
        maxX = x2;
    }
    else {
        minX = x2;
        maxX = x1;
    }

    if(y2 > y1) {
        minY = y1;
        maxY = y2;
    }
    else {
        minY = y2;
        maxY = y1;
    }

    // Calculate the size of the line
    const Pt::int32_t sizeX = maxX - minX + 1;
    const Pt::int32_t sizeY = maxY - minY + 1;
    if(!sizeX && !sizeY) return;

    // Check for horizontal line
    if(_pen.style() == Pen::Solid)
        rasterOnePixelSolidLine(x1, y1, x2, y2, minX, minY, maxX, maxY, sizeX, sizeY, maskInOut);
    else
        rasterOnePixelPatternedLine(x1, y1, x2, y2, minX, minY, maxX, maxY, sizeX, sizeY, maskInOut);
}


// ======================================================================================
// ===== Private Member Functions =======================================================
// ======================================================================================

void Rasterizer2::rasterOnePixelSolidLine(Pt::int32_t x1, Pt::int32_t y1, Pt::int32_t x2, Pt::int32_t y2, Pt::int32_t minX, Pt::int32_t minY, Pt::int32_t maxX, Pt::int32_t maxY, Pt::int32_t sizeX, Pt::int32_t sizeY, DrawLineMask* maskInOut)
{
    // Check for horizontal line
    if(minY == maxY) {
        rasterOnePixelSolidHLineSegment(minX, maxX, minY, _pen.color(), maskInOut);
        return;
    }

    // Check for vertical line
    if(minX == maxX) {
        rasterOnePixelSolidVLineSegment(minX, minY, maxY, _pen.color(), maskInOut);
        return;
    }

    // Check for 45-degree line
    if(sizeX == sizeY) {
        rasterOnePixelSolidXLineSegment(x1, y1, x2, y2, _pen.color(), maskInOut);
        return;
    }

    // Raster the line
    if(_aaMode != AntiAliasingMode::None) {
        // Raster the line using anti-aliasing
        rasterOnePixelSolidGLineSegmentXWAA(x1, y1, x2, y2, _pen.color(), maskInOut);
    }
    else {
        // Raster the line without using anti-aliasing
        rasterOnePixelSolidGLineSegmentNoAA(x1, y1, x2, y2, _pen.color(), maskInOut);
    }
}

void Rasterizer2::rasterOnePixelSolidHLineSegment(Pt::int32_t x1, Pt::int32_t x2, Pt::int32_t y, const Color& color, DrawLineMask* maskInOut)
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

void Rasterizer2::rasterOnePixelSolidVLineSegment(Pt::int32_t x, Pt::int32_t y1, Pt::int32_t y2, const Color& color, DrawLineMask* maskInOut)
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

void Rasterizer2::rasterOnePixelSolidXLineSegment(Pt::int32_t x1, Pt::int32_t y1, Pt::int32_t x2, Pt::int32_t y2, const Color& color, DrawLineMask* maskInOut)
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
void Rasterizer2::rasterOnePixelSolidGLineSegmentNoAA(Pt::int32_t x1, Pt::int32_t y1, Pt::int32_t x2, Pt::int32_t y2, const Color& color, DrawLineMask* maskInOut)
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
void Rasterizer2::rasterOnePixelSolidGLineSegmentXWAA(Pt::int32_t x1, Pt::int32_t y1, Pt::int32_t x2, Pt::int32_t y2, const Color& color, DrawLineMask* maskInOut)
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

// Xiaolin Wu's Anti-Aliased Line Algorithm
// https://en.wikipedia.org/wiki/Xiaolin_Wu's_line_algorithm
void Rasterizer2::fillOnePixelSolidGLineSegmentXWAA(Pt::int32_t x1, Pt::int32_t y1, Pt::int32_t x2, Pt::int32_t y2, Pt::int32_t minX, Pt::int32_t minY, const PolygonScanline16s& exclusionZone, DrawLineMask& maskInOut)
{
    // Get the mask's coordinate
    Pt::int32_t mx[4] = { MAXIMUM_COORD, MAXIMUM_COORD, MAXIMUM_COORD, MAXIMUM_COORD };
    Pt::int32_t my[4] = { MAXIMUM_COORD, MAXIMUM_COORD, MAXIMUM_COORD, MAXIMUM_COORD };

    for(Pt::int32_t i = 0; i < 4; ++i) {
        mx[i] = maskInOut[i].x();
        my[i] = maskInOut[i].y();
    }

    // Convert the coordinates to fixed-points
    Pt::int32_t fx1 = FIXED_POINT_FROM_INT(x1);
    Pt::int32_t fy1 = FIXED_POINT_FROM_INT(y1);
    Pt::int32_t fx2 = FIXED_POINT_FROM_INT(x2);
    Pt::int32_t fy2 = FIXED_POINT_FROM_INT(y2);

    // A helper macro to fill pixel
    #define XW_FILL_PIXEL(IMG, X, Y, A)                                                     \
        do {                                                                                \
            /* Check the boundary limit, just in case */                                    \
            if( (X) < 0 || (X) >= IMG->width() || (Y) < 0 || (Y) >= IMG->height() ) break;  \
            /* Check if we should skip drawing the pixel */                                 \
            bool skipDrawing = false;                                                       \
            for(Pt::int32_t j = 0; j < 4; ++j) {                                            \
                if( (X) != mx[j] || (Y) != my[j] ) continue;                                \
                skipDrawing = true;                                                         \
                break;                                                                      \
            }                                                                               \
            if(skipDrawing) break;                                                          \
            /* Fill the pixel */                                                            \
            if(_isTexture || _isGradient) {                                                 \
                const Pt::int32_t bw = _brushImage->width();                                \
                const Pt::int32_t bh = _brushImage->height();                               \
                ConstPixel srcPixel(_brushImage->view(), (X - minX) % bw, (Y - minY) % bh); \
                Pixel      dstPixel(IMG->view(), X, Y);                                     \
                _image->format().setPixel(dstPixel, srcPixel, _compositionMode, A);         \
            }                                                                               \
            else {                                                                          \
                Pixel pixel(IMG->view(), X, Y);                                             \
                _image->format().setPixel(pixel, _brush.color(), _compositionMode, A);      \
            }                                                                               \
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
            // Calculate the alphas and coordinates
            const Pt::uint8_t a1 = Rasterizer2::XWAA_WFILTER[ FIXED_POINT_FPART_TO_A8 (ypxli) ];
            const Pt::uint8_t a2 = Rasterizer2::XWAA_WFILTER[ FIXED_POINT_RFPART_TO_A8(ypxli) ];
            const Pt::int32_t x1 = FIXED_POINT_TO_INT(FIXED_POINT_IPART(ypxli)                           );
            const Pt::int32_t x2 = FIXED_POINT_TO_INT(FIXED_POINT_IPART(ypxli) + FIXED_POINT_CONSTANT_ONE);
            const Pt::int32_t y  = i;
            ypxli += grad;
            // Draw the pixels as needed
            bool skipPixel1 = false;
            bool skipPixel2 = false;
            if(!exclusionZone.empty()) {
                for(std::vector<PolygonScanline16>::const_iterator it = exclusionZone[y - minY].begin(); it != exclusionZone[y - minY].end(); ++it) {
                    if(x1 >= it->from && x1 <= it->to) skipPixel1 = true;
                    if(x2 >= it->from && x2 <= it->to) skipPixel2 = true;
                    if(skipPixel1 && skipPixel2) break;
                }
            }
            if(!skipPixel1) XW_FILL_PIXEL(_image, x1, y, a1);
            if(!skipPixel2) XW_FILL_PIXEL(_image, x2, y, a2);
        }
        // Store back the start and end coordinates to the mask as needed
        maskInOut[0].set(FIXED_POINT_TO_INT(FIXED_POINT_IPART(ypxl )                           ), from);
        maskInOut[1].set(FIXED_POINT_TO_INT(FIXED_POINT_IPART(ypxl ) + FIXED_POINT_CONSTANT_ONE), from);
        maskInOut[2].set(FIXED_POINT_TO_INT(FIXED_POINT_IPART(ypxli)                           ), to  );
        maskInOut[3].set(FIXED_POINT_TO_INT(FIXED_POINT_IPART(ypxli) + FIXED_POINT_CONSTANT_ONE), to  );
    }
    else {
        // Draw the pixels
        for(Pt::int32_t i = from; i <= to; ++i) {
            // Calculate the alphas and coordinates
            const Pt::uint8_t a1 = Rasterizer2::XWAA_WFILTER[ FIXED_POINT_FPART_TO_A8 (ypxli) ];
            const Pt::uint8_t a2 = Rasterizer2::XWAA_WFILTER[ FIXED_POINT_RFPART_TO_A8(ypxli) ];
            const Pt::int32_t x  = i;
            const Pt::int32_t y1 = FIXED_POINT_TO_INT(FIXED_POINT_IPART(ypxli)                           );
            const Pt::int32_t y2 = FIXED_POINT_TO_INT(FIXED_POINT_IPART(ypxli) + FIXED_POINT_CONSTANT_ONE);
            ypxli += grad;
            // Draw the pixels as needed
            bool skipPixel = false;
            if(!exclusionZone.empty()) {
                for(std::vector<PolygonScanline16>::const_iterator it = exclusionZone[y1 - minY].begin(); it != exclusionZone[y1 - minY].end(); ++it) {
                    if (x < it->from || x > it->to) continue;
                    skipPixel = true;
                    break;
                }
            }
            if(!skipPixel) XW_FILL_PIXEL(_image, x, y1, a1);
            skipPixel = false;
            if(!exclusionZone.empty()) {
                for(std::vector<PolygonScanline16>::const_iterator it = exclusionZone[y2 - minY].begin(); it != exclusionZone[y2 - minY].end(); ++it) {
                    if (x < it->from || x > it->to) continue;
                    skipPixel = true;
                    break;
                }
            }
            if(!skipPixel) XW_FILL_PIXEL(_image, x, y2, a2);
        }
        // Store back the start and end coordinates to the mask as needed
        maskInOut[0].set(from, FIXED_POINT_TO_INT(FIXED_POINT_IPART(ypxl )                           ));
        maskInOut[1].set(from, FIXED_POINT_TO_INT(FIXED_POINT_IPART(ypxl ) + FIXED_POINT_CONSTANT_ONE));
        maskInOut[2].set(to,   FIXED_POINT_TO_INT(FIXED_POINT_IPART(ypxli)                           ));
        maskInOut[3].set(to,   FIXED_POINT_TO_INT(FIXED_POINT_IPART(ypxli) + FIXED_POINT_CONSTANT_ONE));
    }

    // Undefine the helper macro
    #undef XW_FILL_PIXEL
}


} // namespace
} // namespace
