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

#include "Rasterizer2_Config.h"

namespace Pt {
namespace Gfx {


// ======================================================================================
// ===== Public Member Functions ========================================================
// ======================================================================================


// ======================================================================================
// ===== Private Member Functions =======================================================
// ======================================================================================

void Rasterizer2::rasterOnePixelLine(const Point& a, const Point& b)
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
    if(minY == maxY) {
        rasterOnePixelHLineSegment(minX, maxX, minY, _pen.color(), false);
        return;
    }

    // Check for vertical line
    if(minX == maxX) {
        rasterOnePixelVLineSegment(minX, minY, maxY, _pen.color(), false);
        return;
    }

    // Convert the coordinates to fixed-points
    const Pt::int32_t fx1 = FIXED_POINT_FROM_INT(x1);
    const Pt::int32_t fy1 = FIXED_POINT_FROM_INT(y1);
    const Pt::int32_t fx2 = FIXED_POINT_FROM_INT(x2);
    const Pt::int32_t fy2 = FIXED_POINT_FROM_INT(y2);

    // Raster the line
    if(_aaLevel)
        rasterOnePixelGLineSegmentXWAA(fx1, fy1, fx2, fy2, _pen.color(), false);
    else
        rasterOnePixelGLineSegmentNOAA(fx1, fy1, fx2, fy2, _pen.color(), false);
}

void Rasterizer2::rasterOnePixelHLineSegment(Pt::int32_t x1, Pt::int32_t x2, Pt::int32_t y, const Color& color, bool skipLastPoint)
{
    // Calculate the length of the line
    const Pt::int32_t sizeL = x2 - x1 + (skipLastPoint ? 0 : 1);

    // Draw the line
    Pixel pixel(_image->view(), x1, y);
    _image->format().setPixels(pixel, color, sizeL, _compositionMode);
}

void Rasterizer2::rasterOnePixelVLineSegment(Pt::int32_t x, Pt::int32_t y1, Pt::int32_t y2, const Color& color, bool skipLastPoint)
{
    // Calculate the length of the line
    const Pt::int32_t sizeL = y2 - y1 + (skipLastPoint ? 0 : 1);

    // Draw the line
    for(Pt::int32_t i = 0; i < sizeL; ++i) {
        Pixel pixel(_image->view(), x, y1++);
        _image->format().setPixel(pixel, color, _compositionMode);
    }
}

// Bresenham's Line Aalgorithm
// https://en.wikipedia.org/wiki/Bresenham's_line_algorithm
void Rasterizer2::rasterOnePixelGLineSegmentNOAA(Pt::int32_t fx1, Pt::int32_t fy1, Pt::int32_t fx2, Pt::int32_t fy2, const Color& color, bool skipLastPoint)
{
    Pt::int32_t x1 = FIXED_POINT_TO_INT(fx1);
    Pt::int32_t y1 = FIXED_POINT_TO_INT(fy1);
    Pt::int32_t x2 = FIXED_POINT_TO_INT(fx2);
    Pt::int32_t y2 = FIXED_POINT_TO_INT(fy2);

    Pt::int32_t dx = abs(x2 - x1);
    Pt::int32_t dy = abs(y2 - y1);

    Pt::int32_t p  = 2 * dy - dx;

    Pt::int32_t x, y, end;

    if(x1 > x2) {
        x   = x2;
        y   = y2;
        end = x1;
    }
    else {
        x = x1;
        y = y1;
        end = x2;
    }

    Pixel pixel(_image->view(), x, y);
    _image->format().setPixel(pixel, color, _compositionMode);

    while(x < end) {
        ++x;
        if(p < 0) {
            p = p + 2 * dy;
        }
        else {
            y = y + 1;
            p = p + 2 * (dy - dx);
        }
        
        Pixel pixel(_image->view(), x, y);
        _image->format().setPixel(pixel, color, _compositionMode);
    }
}

// Xiaolin Wu's Anti-Aliased Line Algorithm
// https://en.wikipedia.org/wiki/Xiaolin_Wu's_line_algorithm
void Rasterizer2::rasterOnePixelGLineSegmentXWAA(Pt::int32_t fx1, Pt::int32_t fy1, Pt::int32_t fx2, Pt::int32_t fy2, const Color& color, bool skipLastPoint)
{
    // A helper macro to set pixel
    #define XW_SET_PIXEL(IMG, COL, X, Y, A)                                        \
        do {                                                                       \
            if( X < 0 || X >= IMG->width() || Y < 0 || Y >= IMG->height() ) break; \
            Pixel PIX(IMG->view(), X, Y);                                          \
            IMG->format().setPixel(PIX, COL, _compositionMode, A);                 \
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
          Pt::int32_t ypxl  = fy1 + grad * FIXED_POINT_TO_INT(xpxl1 - fx1);

    // Loop through the rest of the pixels
    const Pt::int32_t from = FIXED_POINT_TO_INT(FIXED_POINT_ROUND(fx1));
    const Pt::int32_t to   = skipLastPoint ? ( FIXED_POINT_TO_INT(xpxl2) - 1) : FIXED_POINT_TO_INT(xpxl2);
    if(steep) {
        for(Pt::int32_t i = from; i <= to; ++i) {
            const Pt::uint8_t a1 = FIXED_POINT_RFPART_TO_A8(ypxl);
            const Pt::uint8_t a2 = FIXED_POINT_FPART_TO_A8 (ypxl);
            XW_SET_PIXEL(_image, color, FIXED_POINT_TO_INT(FIXED_POINT_IPART(ypxl)                           ), i, a1);
            XW_SET_PIXEL(_image, color, FIXED_POINT_TO_INT(FIXED_POINT_IPART(ypxl) + FIXED_POINT_CONSTANT_ONE), i, a2);
            ypxl += grad;
        }
    }
    else {
        for(Pt::int32_t i = from; i <= to; ++i) {
            const Pt::uint8_t a1 = FIXED_POINT_RFPART_TO_A8(ypxl);
            const Pt::uint8_t a2 = FIXED_POINT_FPART_TO_A8 (ypxl);
            XW_SET_PIXEL(_image, color, i, FIXED_POINT_TO_INT(FIXED_POINT_IPART(ypxl)                           ), a1);
            XW_SET_PIXEL(_image, color, i, FIXED_POINT_TO_INT(FIXED_POINT_IPART(ypxl) + FIXED_POINT_CONSTANT_ONE), a2);
            ypxl += grad;
        }
    }

    // Undefine the helper macro
    #undef XW_SET_PIXEL
}


} // namespace
} // namespace
