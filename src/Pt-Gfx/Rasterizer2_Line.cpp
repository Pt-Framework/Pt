/* Copyright (C) 2017-2017 Aloysius Indrayanto
   Copyright (C) 2006-2017 Marc Boris Duerner
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
#include "ClipShape.h"

namespace Pt {

namespace Gfx {

//
// Same as fillPolygon, just uses pen and temporarily turns off gradient
// and texture filling
//
void Rasterizer2::rasterWideLine(const PointF* ps, std::size_t n)
{
    Pt::int32_t minX =  MAXIMUM_COORD;
    Pt::int32_t minY =  MAXIMUM_COORD;
    Pt::int32_t maxX = -MAXIMUM_COORD;
    Pt::int32_t maxY = -MAXIMUM_COORD;

    std::vector<PointF> clippedPolygon(ps, ps + n);

    BasicClipShape<PointF::ValueT>::clipPolygon(clippedPolygon, _currentClip);
    if(clippedPolygon.empty()) return;

    for(size_t j = 0; j < clippedPolygon.size(); ++j)
    {
        const PointF::ValueT x = clippedPolygon[j].x();
        const PointF::ValueT y = clippedPolygon[j].y();

        if(x < minX) minX = x;
        if(y < minY) minY = y;
        if(x > maxX) maxX = x;
        if(y > maxY) maxY = y;
    }

    // Disable texture and gradient
    const bool isTexture  = _isTexture;
    const bool isGradient = _isGradient;

    _isTexture  = false;
    _isGradient = false;

    if( this->isAntiAliasing() )
    {
        rasterPolygonXWAA(&clippedPolygon[0], clippedPolygon.size(),
                           _pen.color(), minX, minY, maxX, maxY);
    }
    else
    {
        rasterPolygonNoAA(&clippedPolygon[0], clippedPolygon.size(),
                           _pen.color(), minX, minY, maxX, maxY);
    }

    // Restore texture and gradient
    _isTexture  = isTexture;
    _isGradient = isGradient;
}


//
// Same as fillPolygons, just uses pen and temporarily turns off gradient
// and texture filling
//
void Rasterizer2::rasterWidePolyline(const std::vector<Polygon>& polygons)
{
    Pt::int32_t minX =  MAXIMUM_COORD;
    Pt::int32_t minY =  MAXIMUM_COORD;
    Pt::int32_t maxX = -MAXIMUM_COORD;
    Pt::int32_t maxY = -MAXIMUM_COORD;

    std::vector<Polygon> clippedPolygons = polygons;
    if(clippedPolygons.empty()) return;

    for(size_t i = 0; i < clippedPolygons.size(); ++i)
    {
        Polygon& polygon = clippedPolygons[i];

        BasicClipShape<PointF::ValueT>::clipPolygon(polygon.points(), _currentClip);
        if(polygon.empty()) continue;

        for(size_t j = 0; j < polygon.size(); ++j)
        {
            const PointF::ValueT x = polygon.at(j).x();
            const PointF::ValueT y = polygon.at(j).y();

            if(x < minX) minX = x;
            if(y < minY) minY = y;
            if(x > maxX) maxX = x;
            if(y > maxY) maxY = y;
        }
    }

#if 0
    const std::vector<PointF>& p = polygons[0].points();
    double mX = 99999;
    double mY = 99999;
    for(size_t i = 0; i < p.size(); ++i)
    {
        const double x = p[i].x();
        const double y = p[i].y();
        if(x < mX) mX = x;
        if(y < mY) mY = y;
    }
    fprintf(stderr, "###\n");
    for(size_t i = 0; i < p.size(); ++i)
    {
        const double x = p[i].x() - mX;
        const double y = p[i].y() - mY;
        fprintf(stderr, "            points.push_back( Pt::Gfx::PointF( x + %18.14f, y + %17.14f ) );\n", x, y);
    }
#endif

    // Disable texture and gradient
    const bool isTexture  = _isTexture;
    const bool isGradient = _isGradient;

    _isTexture  = false;
    _isGradient = false;

    if( this->isAntiAliasing() )
    {
        rasterPolygonsXWAA(clippedPolygons, _pen.color(), minX, minY, maxX, maxY);
    }
    else
    {
        rasterPolygonsNoAA(clippedPolygons, _pen.color(), minX, minY, maxX, maxY);
    }

    // Restore texture and gradient
    _isTexture  = isTexture;
    _isGradient = isGradient;
}


void Rasterizer2::rasterNarrowSolidLine(Pt::int32_t x1, Pt::int32_t y1,
                                        Pt::int32_t x2, Pt::int32_t y2,
                                        const Color& color, DrawLineMask* maskInOut)
{
    // Check for horizontal line
    if(y1 == y2)
    {
        rasterNarrowSolidHLineSegment(x1, x2, y1, color, maskInOut);
        return;
    }

    // Check for vertical line
    if(x1 == x2)
    {
        rasterNarrowSolidVLineSegment(x1, y1, y2, color, maskInOut);
        return;
    }

    // Check for 45-degree line
    if(abs(x2 - x1) == abs(y2 - y1))
    {
        rasterNarrowSolidXLineSegment(x1, y1, x2, y2, color, maskInOut);
        return;
    }

    // Generic line
    if( ! _aaMode)
    {
        // Raster the line without using anti-aliasing
        rasterNarrowSolidGLineSegmentNoAA(x1, y1, x2, y2, color, maskInOut);
    }
    else
    {
        // Raster the line using anti-aliasing
        rasterNarrowSolidGLineSegmentXWAA(x1, y1, x2, y2, color, maskInOut);
    }
}


void Rasterizer2::rasterNarrowSolidLine_F(float x1, float y1,
                                          float x2, float y2,
                                          const Color& color, DrawLineMask* maskInOut)
{
    // TODO: without AA

    rasterNarrowSolidGLineSegmentXWAA_F(x1, y1, x2, y2, color, maskInOut);
}


void Rasterizer2::rasterNarrowPatternedLine(Pt::int32_t x1, Pt::int32_t y1,
                                            Pt::int32_t x2, Pt::int32_t y2,
                                            const Color& color, Pt::int32_t& fpiCtrInOut,
                                            DrawLineMask* maskInOut)
{
    // Check the size of the line
    const Pt::int32_t sizeX = abs(x2 - x1);
    const Pt::int32_t sizeY = abs(y2 - y1);
    const Pt::int32_t sizeS = sizeX + sizeY;
    if(!sizeS) return;

    const Pt::int32_t sizeL = sqrtf(sizeX * sizeX + sizeY * sizeY);

    // Calculate the incremental factor of the pattern indexing counter
    Pt::int32_t fpiCtrInc = FIXED_POINT_FROM_INT(PATTERN_BUFFER_1P_SCALE_FACTOR);

    if( ! ( (y1 == y2) || (x1 == x2) || (abs(x2 - x1) == abs(y2 - y1)) ) ) { // Check generic line (non horizontal, vertical, or for 45-degree line)
        fpiCtrInc = FIXED_POINT_CONSTANT_ISQRT2 * PATTERN_BUFFER_1P_SCALE_FACTOR * sizeS / sizeL;
    }

    // Check for 45-degree line
    if(abs(x2 - x1) == abs(y2 - y1)) {
        rasterNarrowPatternedXLineSegment(x1, y1, x2, y2, color, fpiCtrInc, fpiCtrInOut, maskInOut);
    }

    // Generic line
    else {
        // Without anti-aliasing
        if( ! _aaMode)
            rasterNarrowPatternedGLineSegmentNoAA(x1, y1, x2, y2, color, fpiCtrInc, fpiCtrInOut, maskInOut);
        // With anti-aliasing
        else
            rasterNarrowPatternedGLineSegmentXWAA(x1, y1, x2, y2, color, fpiCtrInc, fpiCtrInOut, maskInOut);
    }
}


void Rasterizer2::rasterNarrowPatternedLine_F(float x1, float y1,
                                              float x2, float y2,
                                              const Color& color,
                                              Pt::int32_t& fpiCtrInOut,
                                              DrawLineMask* maskInOut)
{
    // Check the size of the line
    const float sizeX = ::fabs(x2 - x1);
    const float sizeY = ::fabs(y2 - y1);
    const float sizeS = sizeX + sizeY;
    if(!sizeS) return;

    const float sizeL = sqrt(sizeX * sizeX + sizeY * sizeY);

    // Calculate the incremental factor of the pattern indexing counter
    Pt::int32_t fpiCtrInc = FIXED_POINT_FROM_INT(PATTERN_BUFFER_1P_SCALE_FACTOR);

    if( ! ( (y1 == y2) || (x1 == x2) || (abs(x2 - x1) == abs(y2 - y1)) ) ) { // Check generic line (non horizontal, vertical, or for 45-degree line)
        fpiCtrInc = FIXED_POINT_CONSTANT_ISQRT2 * PATTERN_BUFFER_1P_SCALE_FACTOR * sizeS / sizeL;
    }

    // Rasterize line
    rasterNarrowPatternedGLineSegmentXWAA_F(x1, y1, x2, y2, color, fpiCtrInc, fpiCtrInOut, maskInOut);
}


void Rasterizer2::rasterNarrowSolidHLineSegment(Pt::int32_t x1, Pt::int32_t x2, Pt::int32_t y, const Color& color, DrawLineMask* maskInOut)
{
    // A flag that indicates if the line direction will need to be swapped
    const bool swapDir = (x1 > x2);

    // Swap the coordinates as needed
    if(swapDir) std::swap(x1, x2);

    // Adjust the start and end coordinates as needed
    if(maskInOut) {
        for(Pt::int32_t i = 0; i < 4; ++i) {
            if( x1 == (*maskInOut)[i].x() && y == (*maskInOut)[i].y() ) ++x1;
            if( x2 == (*maskInOut)[i].x() && y == (*maskInOut)[i].y() ) --x2;
        }
        if(x1 > x2) return;
    }

    // Store back the start and end coordinates to the mask as needed
    if(maskInOut) {
        (*maskInOut)[0].set(swapDir ? x2 : x1, y);
        (*maskInOut)[1] = maxPoint();
        (*maskInOut)[2].set(swapDir ? x1 : x2, y);
        (*maskInOut)[3] = maxPoint();
    }

    // Calculate the length of the line
    const Pt::int32_t sizeL = x2 - x1 + 1;

    // Draw the line
    Pixel pixel(_image->view(), x1, y);
    _image->format().setPixels(pixel, color, sizeL, _compositionMode);
}


void Rasterizer2::rasterNarrowSolidVLineSegment(Pt::int32_t x, Pt::int32_t y1, Pt::int32_t y2, const Color& color, DrawLineMask* maskInOut)
{
    // A flag that indicates if the line direction will need to be swapped
    const bool swapDir = (y1 > y2);

    // Swap the coordinates as needed
    if(swapDir) std::swap(y1, y2);

    // Adjust the start and end coordinates as needed
    if(maskInOut) {
        for(Pt::int32_t i = 0; i < 4; ++i) {
            if( x == (*maskInOut)[i].x() && y1 == (*maskInOut)[i].y() ) ++y1;
            if( x == (*maskInOut)[i].x() && y2 == (*maskInOut)[i].y() ) --y2;
        }
        if(y1 > y2) return;
    }

    // Store back the start and end coordinates to the mask as needed
    if(maskInOut) {
        (*maskInOut)[0].set(x, swapDir ? y2 : y1);
        (*maskInOut)[1] = maxPoint();
        (*maskInOut)[2].set(x, swapDir ? y1 : y2);
        (*maskInOut)[3] = maxPoint();
    }

    // Calculate the length of the line
    const Pt::int32_t sizeL = y2 - y1 + 1;

    // Draw the line
    for(Pt::int32_t i = 0; i < sizeL; ++i) {
        Pixel pixel(_image->view(), x, y1++);
        _image->format().setPixel(pixel, color, _compositionMode);
    }
}


void Rasterizer2::rasterNarrowSolidXLineSegment(Pt::int32_t x1, Pt::int32_t y1, Pt::int32_t x2, Pt::int32_t y2, const Color& color, DrawLineMask* maskInOut)
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
            if( _aaMode && ((x * y) & 1) ) {
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
        (*maskInOut)[1] = maxPoint();
        (*maskInOut)[2].set(x2, y2);
        (*maskInOut)[3] = maxPoint();
    }
}

// Using algorithm from: Bresenham's Line Algorithm
//                       https://en.wikipedia.org/wiki/Bresenham's_line_algorithm
//                       Last modified on February 21, 2017
void Rasterizer2::rasterNarrowSolidGLineSegmentNoAA(Pt::int32_t x1, Pt::int32_t y1,
                                                    Pt::int32_t x2, Pt::int32_t y2,
                                                    const Color& color,
                                                    DrawLineMask* maskInOut)
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

    // Check if the start and end coordinates are the same
    if(x1 == x2 && y1 == y2) {
        // Check if we should skip drawing the pixel
        bool skipDrawing = false;
        for(Pt::int32_t i = 0; i < 4; ++i) {
            if(x1 != mx[i] || y1 != my[i]) continue;
            skipDrawing = true;
            break;
        }
        // Draw the pixel as needed
        if(!skipDrawing) {
            Pixel pixel(_image->view(), x1, y1);
            _image->format().setPixel(pixel, color, _compositionMode);
        }
        // Store back the start and end coordinates to the mask as needed
        if(maskInOut) {
            (*maskInOut)[0].set(x1, y1);
            (*maskInOut)[1] = maxPoint();
            (*maskInOut)[2].set(x1, y1);
            (*maskInOut)[3] = maxPoint();
        }
        // Exit here
        return;
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
        (*maskInOut)[1] = maxPoint();
        (*maskInOut)[2].set(x2, y2);
        (*maskInOut)[3] = maxPoint();
    }
}

// Using algorithm from: Xiaolin Wu's Line Algorithm
//                       https://en.wikipedia.org/wiki/Xiaolin_Wu's_line_algorithm
//                       Last modified on January 19, 2017
void Rasterizer2::rasterNarrowSolidGLineSegmentXWAA(Pt::int32_t x1, Pt::int32_t y1,
                                                    Pt::int32_t x2, Pt::int32_t y2,
                                                    const Color& color,
                                                    DrawLineMask* maskInOut)
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

    // Used for storing back the mask's coordinates
    Pt::int32_t pCnt  = 0;
    Pt::int32_t lx[4] = { MAXIMUM_COORD, MAXIMUM_COORD, MAXIMUM_COORD, MAXIMUM_COORD };
    Pt::int32_t ly[4] = { MAXIMUM_COORD, MAXIMUM_COORD, MAXIMUM_COORD, MAXIMUM_COORD };

    // A helper macro to set pixel
    #define XW_SET_PIXEL(X, Y, A)                                                 \
        do {                                                                      \
            /* Clip the point */                                                  \
            if( !ClipShapeI::insideXYRange(X, Y, _currentClip) ) break;           \
            /* Check if we should skip drawing the pixel */                       \
            bool skipDrawing = false;                                             \
            for(Pt::int32_t j = 0; j < 4; ++j) {                                  \
                if( (X) != mx[j] || (Y) != my[j] ) continue;                      \
                skipDrawing = true;                                               \
                break;                                                            \
            }                                                                     \
            if(skipDrawing || !(A)) break;                                        \
            /* Store back the mask's coordinates */                               \
            lx[2] = lx[3]; lx[3] = X;                                             \
            ly[2] = ly[3]; ly[3] = Y;                                             \
            if(pCnt < 2) {                                                        \
                lx[pCnt] = X;                                                     \
                ly[pCnt] = Y;                                                     \
                ++pCnt;                                                           \
            }                                                                     \
            /* Set the pixel */                                                   \
            Pixel PIX(_image->view(), X, Y);                                      \
            _image->format().setPixel(PIX, color, _compositionMode, A);           \
        } while(false)

    // Check if the start and end coordinates are the same
    if(x1 == x2 && y1 == y2) {
        // Draw the pixel
        XW_SET_PIXEL(x1, y1, 255);
        // Store back the start and end coordinates to the mask as needed
        if(maskInOut) {
            (*maskInOut)[0].set(lx[0], ly[0]);
            (*maskInOut)[1].set(lx[0], ly[0]);
            (*maskInOut)[2].set(lx[0], ly[0]);
            (*maskInOut)[3].set(lx[0], ly[0]);
        }
        // Exit here
        return;
    }

    // Convert the coordinates to fixed-points
    Pt::int32_t fx1 = FIXED_POINT_FROM_INT(x1);
    Pt::int32_t fy1 = FIXED_POINT_FROM_INT(y1);
    Pt::int32_t fx2 = FIXED_POINT_FROM_INT(x2);
    Pt::int32_t fy2 = FIXED_POINT_FROM_INT(y2);

    // Swap the values as needed
    const Pt::int32_t deltaX = (fx2 >= fx1) ? (fx2 - fx1) : (fx1 - fx2);
    const Pt::int32_t deltaY = (fy2 >= fy1) ? (fy2 - fy1) : (fy1 - fy2);
    const bool        steep  = deltaY > deltaX;

    if(steep) {
        std::swap(fx1, fy1);
        std::swap(fx2, fy2);
    }

    const bool swapDir = (fx1 > fx2);

    if(swapDir) {
        std::swap(fx1, fx2);
        std::swap(fy1, fy2);
    }

    // Handle the gradient, starting point, and ending point
    const Pt::int32_t grad  = (fy2 - fy1) / FIXED_POINT_TO_INT(fx2 - fx1);
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
            XW_SET_PIXEL(FIXED_POINT_TO_INT(FIXED_POINT_IPART(ypxli)                           ), i, a1);
            XW_SET_PIXEL(FIXED_POINT_TO_INT(FIXED_POINT_IPART(ypxli) + FIXED_POINT_CONSTANT_ONE), i, a2);
            ypxli += grad;
        }
    }
    else {
        // Draw the pixels
        for(Pt::int32_t i = from; i <= to; ++i) {
            const Pt::uint8_t a1 = Rasterizer2::XWAA_WFILTER[ FIXED_POINT_FPART_TO_A8 (ypxli) ];
            const Pt::uint8_t a2 = Rasterizer2::XWAA_WFILTER[ FIXED_POINT_RFPART_TO_A8(ypxli) ];
            XW_SET_PIXEL(i, FIXED_POINT_TO_INT(FIXED_POINT_IPART(ypxli)                           ), a1);
            XW_SET_PIXEL(i, FIXED_POINT_TO_INT(FIXED_POINT_IPART(ypxli) + FIXED_POINT_CONSTANT_ONE), a2);
            ypxli += grad;
        }
    }

    // Store back the start and end coordinates to the mask as needed
    if(maskInOut) {
        if(swapDir) {
            (*maskInOut)[2].set(lx[0], ly[0]);
            (*maskInOut)[3].set(lx[1], ly[1]);
            (*maskInOut)[0].set(lx[2], ly[2]);
            (*maskInOut)[1].set(lx[3], ly[3]);
        }
        else {
            (*maskInOut)[0].set(lx[0], ly[0]);
            (*maskInOut)[1].set(lx[1], ly[1]);
            (*maskInOut)[2].set(lx[2], ly[2]);
            (*maskInOut)[3].set(lx[3], ly[3]);
        }
    }

    // Undefine the helper macro
    #undef XW_SET_PIXEL
}

// Using algorithm from: Xiaolin Wu's Line Algorithm
//                       https://en.wikipedia.org/wiki/Xiaolin_Wu's_line_algorithm
//                       Last modified on January 19, 2017
void Rasterizer2::rasterNarrowSolidGLineSegmentXWAA_F(float x1, float y1,
                                                      float x2, float y2,
                                                      const Color& color,
                                                      DrawLineMask* maskInOut)
{
    // Get the mask's coordinates as needed
    float mx[4] = { MAXIMUM_COORD_F, MAXIMUM_COORD_F, MAXIMUM_COORD_F, MAXIMUM_COORD_F };
    float my[4] = { MAXIMUM_COORD_F, MAXIMUM_COORD_F, MAXIMUM_COORD_F, MAXIMUM_COORD_F };

    if(maskInOut) {
        for(Pt::int32_t i = 0; i < 4; ++i) {
            mx[i] = (*maskInOut)[i].x();
            my[i] = (*maskInOut)[i].y();
        }
    }

    // Used for storing back the mask's coordinates
    Pt::int32_t pCnt  = 0;
    Pt::int32_t lx[4] = { MAXIMUM_COORD, MAXIMUM_COORD, MAXIMUM_COORD, MAXIMUM_COORD };
    Pt::int32_t ly[4] = { MAXIMUM_COORD, MAXIMUM_COORD, MAXIMUM_COORD, MAXIMUM_COORD };

    // A helper macro to set pixel
    #define XW_SET_PIXEL(X, Y, A)                                                 \
        do {                                                                      \
            /* Clip the point */                                                  \
            if( !ClipShapeI::insideXYRange(X, Y, _currentClip) ) break;           \
            /* Check if we should skip drawing the pixel */                       \
            bool skipDrawing = false;                                             \
            for(Pt::int32_t j = 0; j < 4; ++j) {                                  \
                if( (X) != mx[j] || (Y) != my[j] ) continue;                      \
                skipDrawing = true;                                               \
                break;                                                            \
            }                                                                     \
            if(skipDrawing || !(A)) break;                                        \
            /* Store back the mask's coordinates */                               \
            lx[2] = lx[3]; lx[3] = X;                                             \
            ly[2] = ly[3]; ly[3] = Y;                                             \
            if(pCnt < 2) {                                                        \
                lx[pCnt] = X;                                                     \
                ly[pCnt] = Y;                                                     \
                ++pCnt;                                                           \
            }                                                                     \
            /* Set the pixel */                                                   \
            Pixel PIX(_image->view(), X, Y);                                      \
            _image->format().setPixel(PIX, color, _compositionMode, A);           \
        } while(false)

    // Check if the start and end coordinates are the same
    if(x1 == x2 && y1 == y2) {
        // Draw the pixel
        XW_SET_PIXEL( lround(x1), lround(y1), 255 );
        // Store back the start and end coordinates to the mask as needed
        if(maskInOut) {
            (*maskInOut)[0].set(lx[0], ly[0]);
            (*maskInOut)[1].set(lx[0], ly[0]);
            (*maskInOut)[2].set(lx[0], ly[0]);
            (*maskInOut)[3].set(lx[0], ly[0]);
        }
        // Exit here
        return;
    }

    // Copy the coordinates
    float fx1 = x1;
    float fy1 = y1;
    float fx2 = x2;
    float fy2 = y2;

    // Swap the values as needed
    const float deltaX = (fx2 >= fx1) ? (fx2 - fx1) : (fx1 - fx2);
    const float deltaY = (fy2 >= fy1) ? (fy2 - fy1) : (fy1 - fy2);
    const bool  steep  = deltaY > deltaX;

    if(steep) {
        std::swap(fx1, fy1);
        std::swap(fx2, fy2);
    }

    const bool swapDir = (fx1 > fx2);

    if(swapDir) {
        std::swap(fx1, fx2);
        std::swap(fy1, fy2);
    }

    // Handle the gradient, starting point, and ending point
    const float       grad  = (fy2 - fy1) / (fx2 - fx1);
    const Pt::int32_t xpxl1 = lround(fx1);
    const Pt::int32_t xpxl2 = lround(fx2);
    const float       ypxl  = fy1 + grad * (xpxl1 - fx1);

    // Draw the pixels
    Pt::int32_t from  = lround(fx1);
    Pt::int32_t to    = xpxl2;
    float       ypxli = ypxl;

    if(steep) {
        // Draw the pixels
        for(Pt::int32_t i = from; i <= to; ++i) {
            const Pt::int32_t fypxli = Pt::lround(floor(ypxli));
            const Pt::int32_t fpart  = Pt::lround( (ypxli - fypxli) * 255.0f );
            const Pt::int32_t rfpart = 255 - fpart;
            const Pt::uint8_t a1     = Rasterizer2::XWAA_WFILTER[ fpart];
            const Pt::uint8_t a2     = Rasterizer2::XWAA_WFILTER[rfpart];
            XW_SET_PIXEL(fypxli    , i, a1);
            XW_SET_PIXEL(fypxli + 1, i, a2);
            ypxli += grad;
        }
    }
    else {
        // Draw the pixels
        for(Pt::int32_t i = from; i <= to; ++i) {
            const Pt::int32_t fypxli = Pt::lround(floor(ypxli));
            const Pt::int32_t fpart  = Pt::lround( (ypxli - fypxli) * 255.0f );
            const Pt::int32_t rfpart = 255 - fpart;
            const Pt::uint8_t a1     = Rasterizer2::XWAA_WFILTER[ fpart];
            const Pt::uint8_t a2     = Rasterizer2::XWAA_WFILTER[rfpart];
            XW_SET_PIXEL(i, fypxli    , a1);
            XW_SET_PIXEL(i, fypxli + 1, a2);
            ypxli += grad;
        }
    }

    // Store back the start and end coordinates to the mask as needed
    if(maskInOut) {
        if(swapDir) {
            (*maskInOut)[2].set(lx[0], ly[0]);
            (*maskInOut)[3].set(lx[1], ly[1]);
            (*maskInOut)[0].set(lx[2], ly[2]);
            (*maskInOut)[1].set(lx[3], ly[3]);
        }
        else {
            (*maskInOut)[0].set(lx[0], ly[0]);
            (*maskInOut)[1].set(lx[1], ly[1]);
            (*maskInOut)[2].set(lx[2], ly[2]);
            (*maskInOut)[3].set(lx[3], ly[3]);
        }
    }

    // Undefine the helper macro
    #undef XW_SET_PIXEL
}


void Rasterizer2::rasterNarrowPatternedXLineSegment(Pt::int32_t x1, Pt::int32_t y1,
                                                    Pt::int32_t x2, Pt::int32_t y2,
                                                    const Color& color,
                                                    Pt::int32_t fpiCtrInc,
                                                    Pt::int32_t& fpiCtrInOut,
                                                    DrawLineMask* maskInOut)
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
        // Get alpha from the pattern
        Pt::uint8_t patAlpha = _patternBuffer1PDyn[FIXED_POINT_TO_INT(fpiCtrInOut)];
        fpiCtrInOut += fpiCtrInc;
        if(fpiCtrInOut >= _patternBuffer1PDynCntMax) fpiCtrInOut -= _patternBuffer1PDynCntMax;
        /*
        Pt::uint8_t patAlpha = _patternBuffer1P[FIXED_POINT_TO_INT(fpiCtrInOut)];
        fpiCtrInOut += fpiCtrInc;
        if(fpiCtrInOut >= PATTERN_BUFFER_1P_COUNTER_MAX) fpiCtrInOut -= PATTERN_BUFFER_1P_COUNTER_MAX;
        */
        // Check if we should skip drawing the pixel
        bool skipDrawing = !patAlpha;
        for(Pt::int32_t i = 0; !skipDrawing && i < 4; ++i) {
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
            if( _aaMode && ((x * y) & 1) ) {
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
        (*maskInOut)[1] = maxPoint();
        (*maskInOut)[2].set(x2, y2);
        (*maskInOut)[3] = maxPoint();
    }
}

// Using algorithm from: Bresenham's Line Algorithm
//                       https://en.wikipedia.org/wiki/Bresenham's_line_algorithm
//                       Last modified on February 21, 2017
void Rasterizer2::rasterNarrowPatternedGLineSegmentNoAA(Pt::int32_t x1, Pt::int32_t y1,
                                                        Pt::int32_t x2, Pt::int32_t y2,
                                                        const Color& color,
                                                        Pt::int32_t fpiCtrInc,
                                                        Pt::int32_t& fpiCtrInOut,
                                                        DrawLineMask* maskInOut)
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
        // Get alpha from the pattern
        Pt::uint8_t patAlpha = _patternBuffer1PDyn[FIXED_POINT_TO_INT(fpiCtrInOut)];
        fpiCtrInOut += fpiCtrInc;
        if(fpiCtrInOut >= _patternBuffer1PDynCntMax) fpiCtrInOut -= _patternBuffer1PDynCntMax;
        /*
        Pt::uint8_t patAlpha = _patternBuffer1P[FIXED_POINT_TO_INT(fpiCtrInOut)];
        fpiCtrInOut += fpiCtrInc;
        if(fpiCtrInOut >= PATTERN_BUFFER_1P_COUNTER_MAX) fpiCtrInOut -= PATTERN_BUFFER_1P_COUNTER_MAX;
        */
        // Check if we should skip drawing the pixel
        bool skipDrawing = !patAlpha;
        for(Pt::int32_t i = 0; !skipDrawing && i < 4; ++i) {
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
        (*maskInOut)[1] = maxPoint();
        (*maskInOut)[2].set(x2, y2);
        (*maskInOut)[3] = maxPoint();
    }
}

// Using algorithm from: Xiaolin Wu's Line Algorithm
//                       https://en.wikipedia.org/wiki/Xiaolin_Wu's_line_algorithm
//                       Last modified on January 19, 2017
void Rasterizer2::rasterNarrowPatternedGLineSegmentXWAA(Pt::int32_t x1, Pt::int32_t y1,
                                                        Pt::int32_t x2, Pt::int32_t y2,
                                                        const Color& color,
                                                        Pt::int32_t fpiCtrInc,
                                                        Pt::int32_t& fpiCtrInOut,
                                                        DrawLineMask* maskInOut)
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

    // A helper macro to set pixel
    #define XW_SET_PIXEL(X, Y, A, PA)                                        \
        do {                                                                 \
            /* Clip the point */                                             \
            if( !ClipShapeI::insideXYRange(X, Y, _currentClip) ) break;      \
            /* Check if we should skip drawing the pixel */                  \
            bool skipDrawing = false;                                        \
            for(Pt::int32_t j = 0; j < 4; ++j) {                             \
                if( (X) != mx[j] || (Y) != my[j] ) continue;                 \
                skipDrawing = true;                                          \
                break;                                                       \
            }                                                                \
            if(skipDrawing) break;                                           \
            /* Combine and check the alpha */                                \
            Pt::uint8_t calpha = (Pt::uint32_t) (A) * (PA) / 255;            \
            if(!calpha) break;                                               \
            /* Set the pixel */                                              \
            Pixel PIX(_image->view(), X, Y);                                 \
            _image->format().setPixel(PIX, color, _compositionMode, calpha); \
        } while(false)

    // Check if the start and end coordinates are the same
    if(x1 == x2 && y1 == y2) return;

    // Convert the coordinates to fixed-points
    Pt::int32_t fx1 = FIXED_POINT_FROM_INT(x1);
    Pt::int32_t fy1 = FIXED_POINT_FROM_INT(y1);
    Pt::int32_t fx2 = FIXED_POINT_FROM_INT(x2);
    Pt::int32_t fy2 = FIXED_POINT_FROM_INT(y2);

    // Swap the values as needed
    const Pt::int32_t deltaX = (fx2 >= fx1) ? (fx2 - fx1) : (fx1 - fx2);
    const Pt::int32_t deltaY = (fy2 >= fy1) ? (fy2 - fy1) : (fy1 - fy2);
    const bool        steep  = deltaY > deltaX;

    if(steep) {
        std::swap(fx1, fy1);
        std::swap(fx2, fy2);
    }

    const bool swapDir = (fx1 > fx2);

    if(swapDir) {
        std::swap(fx1, fx2);
        std::swap(fy1, fy2);
    }

    // Handle the gradient, starting point, and ending point
    const Pt::int32_t grad = (fy2 - fy1) / FIXED_POINT_TO_INT(fx2 - fx1);
    const Pt::int32_t xpxl1 = FIXED_POINT_ROUND(fx1);
    const Pt::int32_t xpxl2 = FIXED_POINT_ROUND(fx2);
    const Pt::int32_t ypxl  = fy1 + grad * FIXED_POINT_TO_INT(xpxl1 - fx1);

    // Calculate the starting coordinates
    Pt::int32_t from  = FIXED_POINT_TO_INT(FIXED_POINT_ROUND(fx1));
    Pt::int32_t to    = FIXED_POINT_TO_INT(xpxl2);
    Pt::int32_t ypxli = ypxl;

    // If the line direction is swapped, determine the ending and starting value of the pattern indexing counter
    Pt::int32_t fpiCtrNextOut = 0;
    if(swapDir) {
        const Pt::int32_t lineLen = to - from + 1;
        fpiCtrNextOut = (fpiCtrInOut + fpiCtrInc * lineLen) % _patternBuffer1PDynCntMax;
        fpiCtrInOut   = fpiCtrNextOut - fpiCtrInc;
        if(fpiCtrInOut < 0) fpiCtrInOut += _patternBuffer1PDynCntMax;
        /*
        const Pt::int32_t lineLen = to - from + 1;
        fpiCtrNextOut = (fpiCtrInOut + fpiCtrInc * lineLen) % PATTERN_BUFFER_1P_COUNTER_MAX;
        fpiCtrInOut   = fpiCtrNextOut - fpiCtrInc;
        if(fpiCtrInOut < 0) fpiCtrInOut += PATTERN_BUFFER_1P_COUNTER_MAX;
        */
    }

    // Draw the pixels
    if(steep) {
        // Draw the pixels
        for(Pt::int32_t i = from; i <= to; ++i) {
            // Get alpha from the pattern
            Pt::uint8_t pa = _patternBuffer1PDyn[FIXED_POINT_TO_INT(fpiCtrInOut)];
            if(swapDir) {
                fpiCtrInOut -= fpiCtrInc;
                if(fpiCtrInOut < 0) fpiCtrInOut += _patternBuffer1PDynCntMax;
            }
            else {
                fpiCtrInOut += fpiCtrInc;
                if(fpiCtrInOut >= _patternBuffer1PDynCntMax) fpiCtrInOut -= _patternBuffer1PDynCntMax;
            }
            /*
            Pt::uint8_t pa = _patternBuffer1P[FIXED_POINT_TO_INT(fpiCtrInOut)];
            if(swapDir) {
                fpiCtrInOut -= fpiCtrInc;
                if(fpiCtrInOut < 0) fpiCtrInOut += PATTERN_BUFFER_1P_COUNTER_MAX;
            }
            else {
                fpiCtrInOut += fpiCtrInc;
                if(fpiCtrInOut >= PATTERN_BUFFER_1P_COUNTER_MAX) fpiCtrInOut -= PATTERN_BUFFER_1P_COUNTER_MAX;
            }
            */
            // Draw the pixels
            const Pt::uint8_t a1 = Rasterizer2::XWAA_WFILTER[ FIXED_POINT_FPART_TO_A8 (ypxli) ];
            const Pt::uint8_t a2 = Rasterizer2::XWAA_WFILTER[ FIXED_POINT_RFPART_TO_A8(ypxli) ];
            XW_SET_PIXEL(FIXED_POINT_TO_INT(FIXED_POINT_IPART(ypxli)                           ), i, a1, pa);
            XW_SET_PIXEL(FIXED_POINT_TO_INT(FIXED_POINT_IPART(ypxli) + FIXED_POINT_CONSTANT_ONE), i, a2, pa);
            ypxli += grad;
        }
        // Store back the start and end coordinates to the mask as needed
        if(maskInOut) {
            (*maskInOut)[swapDir ? 2 : 0].set(FIXED_POINT_TO_INT(FIXED_POINT_IPART(ypxl )                           ), from);
            (*maskInOut)[swapDir ? 3 : 1].set(FIXED_POINT_TO_INT(FIXED_POINT_IPART(ypxl ) + FIXED_POINT_CONSTANT_ONE), from);
            (*maskInOut)[swapDir ? 0 : 2].set(FIXED_POINT_TO_INT(FIXED_POINT_IPART(ypxli)                           ), to  );
            (*maskInOut)[swapDir ? 1 : 3].set(FIXED_POINT_TO_INT(FIXED_POINT_IPART(ypxli) + FIXED_POINT_CONSTANT_ONE), to  );
        }
    }
    else {
        // Draw the pixels
        for(Pt::int32_t i = from; i <= to; ++i) {
            // Get alpha from the pattern
            Pt::uint8_t pa = _patternBuffer1PDyn[FIXED_POINT_TO_INT(fpiCtrInOut)];
            if(swapDir) {
                fpiCtrInOut -= fpiCtrInc;
                if(fpiCtrInOut < 0) fpiCtrInOut += _patternBuffer1PDynCntMax;
            }
            else {
                fpiCtrInOut += fpiCtrInc;
                if(fpiCtrInOut >= _patternBuffer1PDynCntMax) fpiCtrInOut -= _patternBuffer1PDynCntMax;
            }
            /*
            Pt::uint8_t pa = _patternBuffer1P[FIXED_POINT_TO_INT(fpiCtrInOut)];
            if(swapDir) {
                fpiCtrInOut -= fpiCtrInc;
                if(fpiCtrInOut < 0) fpiCtrInOut += PATTERN_BUFFER_1P_COUNTER_MAX;
            }
            else {
                fpiCtrInOut += fpiCtrInc;
                if(fpiCtrInOut >= PATTERN_BUFFER_1P_COUNTER_MAX) fpiCtrInOut -= PATTERN_BUFFER_1P_COUNTER_MAX;
            }
            */
            // Draw the pixels
            const Pt::uint8_t a1 = Rasterizer2::XWAA_WFILTER[ FIXED_POINT_FPART_TO_A8 (ypxli) ];
            const Pt::uint8_t a2 = Rasterizer2::XWAA_WFILTER[ FIXED_POINT_RFPART_TO_A8(ypxli) ];
            XW_SET_PIXEL(i, FIXED_POINT_TO_INT(FIXED_POINT_IPART(ypxli)                           ), a1, pa);
            XW_SET_PIXEL(i, FIXED_POINT_TO_INT(FIXED_POINT_IPART(ypxli) + FIXED_POINT_CONSTANT_ONE), a2, pa);
            ypxli += grad;
        }
        // Store back the start and end coordinates to the mask as needed
        if(maskInOut) {
            (*maskInOut)[swapDir ? 2 : 0].set(from, FIXED_POINT_TO_INT(FIXED_POINT_IPART(ypxl )                           ));
            (*maskInOut)[swapDir ? 3 : 1].set(from, FIXED_POINT_TO_INT(FIXED_POINT_IPART(ypxl ) + FIXED_POINT_CONSTANT_ONE));
            (*maskInOut)[swapDir ? 0 : 2].set(to,   FIXED_POINT_TO_INT(FIXED_POINT_IPART(ypxli)                           ));
            (*maskInOut)[swapDir ? 1 : 3].set(to,   FIXED_POINT_TO_INT(FIXED_POINT_IPART(ypxli) + FIXED_POINT_CONSTANT_ONE));
        }
    }

    // If the line direction is swapped, send out the ending value of the pattern indexing counter which was previously calculated
    if(swapDir) fpiCtrInOut = fpiCtrNextOut;

    // Undefine the helper macro
    #undef XW_SET_PIXEL
}

// Using algorithm from: Xiaolin Wu's Line Algorithm
//                       https://en.wikipedia.org/wiki/Xiaolin_Wu's_line_algorithm
//                       Last modified on January 19, 2017
void Rasterizer2::rasterNarrowPatternedGLineSegmentXWAA_F(float x1, float y1,
                                                          float x2, float y2,
                                                          const Color& color,
                                                          Pt::int32_t fpiCtrInc,
                                                          Pt::int32_t& fpiCtrInOut,
                                                          DrawLineMask* maskInOut)
{
    // Get the mask's coordinates as needed
    float mx[4] = { MAXIMUM_COORD_F, MAXIMUM_COORD_F, MAXIMUM_COORD_F, MAXIMUM_COORD_F };
    float my[4] = { MAXIMUM_COORD_F, MAXIMUM_COORD_F, MAXIMUM_COORD_F, MAXIMUM_COORD_F };

    if(maskInOut) {
        for(Pt::int32_t i = 0; i < 4; ++i) {
            mx[i] = (*maskInOut)[i].x();
            my[i] = (*maskInOut)[i].y();
        }
    }

    // A helper macro to set pixel
    #define XW_SET_PIXEL(X, Y, A, PA)                                        \
        do {                                                                 \
            /* Clip the point */                                             \
            if( !ClipShapeI::insideXYRange(X, Y, _currentClip) ) break;      \
            /* Check if we should skip drawing the pixel */                  \
            bool skipDrawing = false;                                        \
            for(Pt::int32_t j = 0; j < 4; ++j) {                             \
                if( (X) != mx[j] || (Y) != my[j] ) continue;                 \
                skipDrawing = true;                                          \
                break;                                                       \
            }                                                                \
            if(skipDrawing) break;                                           \
            /* Combine and check the alpha */                                \
            Pt::uint8_t calpha = (Pt::uint32_t) (A) * (PA) / 255;            \
            if(!calpha) break;                                               \
            /* Set the pixel */                                              \
            Pixel PIX(_image->view(), X, Y);                                 \
            _image->format().setPixel(PIX, color, _compositionMode, calpha); \
        } while(false)

    // Check if the start and end coordinates are the same
    if(x1 == x2 && y1 == y2) return;

    // Copy the coordinates
    float fx1 = x1;
    float fy1 = y1;
    float fx2 = x2;
    float fy2 = y2;

    // Swap the values as needed
    const float deltaX = (fx2 >= fx1) ? (fx2 - fx1) : (fx1 - fx2);
    const float deltaY = (fy2 >= fy1) ? (fy2 - fy1) : (fy1 - fy2);
    const bool  steep  = deltaY > deltaX;

    if(steep) {
        std::swap(fx1, fy1);
        std::swap(fx2, fy2);
    }

    const bool swapDir = (fx1 > fx2);

    if(swapDir) {
        std::swap(fx1, fx2);
        std::swap(fy1, fy2);
    }

    // Handle the gradient, starting point, and ending point
    const float       grad  = (fy2 - fy1) / (fx2 - fx1);
    const Pt::int32_t xpxl1 = lround(fx1);
    const Pt::int32_t xpxl2 = lround(fx2);
    const float       ypxl  = fy1 + grad * (xpxl1 - fx1);

    // Draw the pixels
    Pt::int32_t from  = lround(fx1);
    Pt::int32_t to    = xpxl2;
    float       ypxli = ypxl;

    // If the line direction is swapped, determine the ending and starting value of the pattern indexing counter
    Pt::int32_t fpiCtrNextOut = 0;
    if(swapDir) {
        const Pt::int32_t lineLen = to - from + 1;
        fpiCtrNextOut = (fpiCtrInOut + fpiCtrInc * lineLen) % _patternBuffer1PDynCntMax;
        fpiCtrInOut   = fpiCtrNextOut - fpiCtrInc;
        if(fpiCtrInOut < 0) fpiCtrInOut += _patternBuffer1PDynCntMax;
        /*
        const Pt::int32_t lineLen = to - from + 1;
        fpiCtrNextOut = (fpiCtrInOut + fpiCtrInc * lineLen) % PATTERN_BUFFER_1P_COUNTER_MAX;
        fpiCtrInOut   = fpiCtrNextOut - fpiCtrInc;
        if(fpiCtrInOut < 0) fpiCtrInOut += PATTERN_BUFFER_1P_COUNTER_MAX;
        */
    }

    // Draw the pixels
    if(steep) {
        // Draw the pixels
        for(Pt::int32_t i = from; i <= to; ++i) {
            // Get alpha from the pattern
            Pt::uint8_t pa = _patternBuffer1PDyn[FIXED_POINT_TO_INT(fpiCtrInOut)];
            if(swapDir) {
                fpiCtrInOut -= fpiCtrInc;
                if(fpiCtrInOut < 0) fpiCtrInOut += _patternBuffer1PDynCntMax;
            }
            else {
                fpiCtrInOut += fpiCtrInc;
                if(fpiCtrInOut >= _patternBuffer1PDynCntMax) fpiCtrInOut -= _patternBuffer1PDynCntMax;
            }
            /*
            Pt::uint8_t pa = _patternBuffer1P[FIXED_POINT_TO_INT(fpiCtrInOut)];
            if(swapDir) {
                fpiCtrInOut -= fpiCtrInc;
                if(fpiCtrInOut < 0) fpiCtrInOut += PATTERN_BUFFER_1P_COUNTER_MAX;
            }
            else {
                fpiCtrInOut += fpiCtrInc;
                if(fpiCtrInOut >= PATTERN_BUFFER_1P_COUNTER_MAX) fpiCtrInOut -= PATTERN_BUFFER_1P_COUNTER_MAX;
            }
            */
            // Draw the pixels
            const Pt::int32_t fypxli = Pt::lround(floor(ypxli));
            const Pt::int32_t fpart  = Pt::lround( (ypxli - fypxli) * 255.0f );
            const Pt::int32_t rfpart = 255 - fpart;
            const Pt::uint8_t a1     = Rasterizer2::XWAA_WFILTER[ fpart];
            const Pt::uint8_t a2     = Rasterizer2::XWAA_WFILTER[rfpart];
            XW_SET_PIXEL(fypxli    , i, a1, pa);
            XW_SET_PIXEL(fypxli + 1, i, a2, pa);
            ypxli += grad;
        }
        // Store back the start and end coordinates to the mask as needed
        if(maskInOut) {
            (*maskInOut)[swapDir ? 2 : 0].set(Pt::lround(floor(ypxl ))    , from);
            (*maskInOut)[swapDir ? 3 : 1].set(Pt::lround(floor(ypxl )) + 1, from);
            (*maskInOut)[swapDir ? 0 : 2].set(Pt::lround(floor(ypxli))    , to  );
            (*maskInOut)[swapDir ? 1 : 3].set(Pt::lround(floor(ypxli)) + 1, to  );
        }
    }
    else {
        // Draw the pixels
        for(Pt::int32_t i = from; i <= to; ++i) {
            // Get alpha from the pattern
            Pt::uint8_t pa = _patternBuffer1PDyn[FIXED_POINT_TO_INT(fpiCtrInOut)];
            if(swapDir) {
                fpiCtrInOut -= fpiCtrInc;
                if(fpiCtrInOut < 0) fpiCtrInOut += _patternBuffer1PDynCntMax;
            }
            else {
                fpiCtrInOut += fpiCtrInc;
                if(fpiCtrInOut >= _patternBuffer1PDynCntMax) fpiCtrInOut -= _patternBuffer1PDynCntMax;
            }
            /*
            Pt::uint8_t pa = _patternBuffer1P[FIXED_POINT_TO_INT(fpiCtrInOut)];
            if(swapDir) {
                fpiCtrInOut -= fpiCtrInc;
                if(fpiCtrInOut < 0) fpiCtrInOut += PATTERN_BUFFER_1P_COUNTER_MAX;
            }
            else {
                fpiCtrInOut += fpiCtrInc;
                if(fpiCtrInOut >= PATTERN_BUFFER_1P_COUNTER_MAX) fpiCtrInOut -= PATTERN_BUFFER_1P_COUNTER_MAX;
            }
            */
            // Draw the pixels
            const Pt::int32_t fypxli = Pt::lround(floor(ypxli));
            const Pt::int32_t fpart  = Pt::lround( (ypxli - fypxli) * 255.0f );
            const Pt::int32_t rfpart = 255 - fpart;
            const Pt::uint8_t a1     = Rasterizer2::XWAA_WFILTER[ fpart];
            const Pt::uint8_t a2     = Rasterizer2::XWAA_WFILTER[rfpart];
            XW_SET_PIXEL(i, fypxli    , a1, pa);
            XW_SET_PIXEL(i, fypxli + 1, a2, pa);
            ypxli += grad;
        }
        // Store back the start and end coordinates to the mask as needed
        if(maskInOut) {
            (*maskInOut)[swapDir ? 2 : 0].set(from, Pt::lround(floor(ypxl ))    );
            (*maskInOut)[swapDir ? 3 : 1].set(from, Pt::lround(floor(ypxl )) + 1);
            (*maskInOut)[swapDir ? 0 : 2].set(to,   Pt::lround(floor(ypxli))    );
            (*maskInOut)[swapDir ? 1 : 3].set(to,   Pt::lround(floor(ypxli)) + 1);
        }
    }

    // If the line direction is swapped, send out the ending value of the pattern indexing counter which was previously calculated
    if(swapDir) fpiCtrInOut = fpiCtrNextOut;

    // Undefine the helper macro
    #undef XW_SET_PIXEL
}

} // namespace

} // namespace
