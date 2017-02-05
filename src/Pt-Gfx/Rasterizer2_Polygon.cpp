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

void Rasterizer2::strokePolygon(const Point* points, size_t pointCount)
{
    switch( _pen.style() ) {
        case Pen::Solid:
            if( _pen.size() == 1 && pointCount == 2 ) {
                rasterOnePixelLine(points[0], points[1]);
            }
            break;

        case Pen::Dash:
        case Pen::DoubleDash:
            break;
    }
}

void Rasterizer2::fillPolygon(const Point* points, const size_t pointCount, bool useSupersamplingForAA)
{
    std::vector<Point> clipped;

#if 1
    genClippedPolygonPoints(clipped, points, pointCount);
#else
    #define DIV_FAC 50
    clipped.push_back(Point(450 / DIV_FAC, 100 / DIV_FAC));
    clipped.push_back(Point(350 / DIV_FAC, 300 / DIV_FAC));
    clipped.push_back(Point(650 / DIV_FAC, 400 / DIV_FAC));
#endif

    if(useSupersamplingForAA) {
        rasterPolygonAreaSS(clipped.data(), clipped.size(), _brush.color());
    }
    else {
        rasterPolygonArea(clipped.data(), clipped.size(), _brush.color());
        rasterPolygonOutline(clipped.data(), clipped.size(), _brush.color());
    }
}


// ======================================================================================
// ===== Private Member Functions =======================================================
// ======================================================================================

void Rasterizer2::rasterPolygonOutline(const Point* points, size_t pointCount, const Color& color)
{
    // Convert the coordinates to fixed-points
    std::vector<Pt::int32_t> lineX(pointCount);
    std::vector<Pt::int32_t> lineY(pointCount);

    for(size_t i = 0; i < pointCount; ++i) {
        lineX[i] = FIXED_POINT_FROM_INT(points[i].x());
        lineY[i] = FIXED_POINT_FROM_INT(points[i].y());
    }

    // Raster the outlines as multiple one-pixel lines
    const size_t pc1 = pointCount - 1;

    for(size_t i = 0; i < pc1; ++i) {
        if(lineY[i] == lineY[i + 1])
            rasterOnePixelHLineSegment(lineX[i], lineX[i + 1], lineY[i], color, true);
        else if(lineX[i] == lineX[i + 1])
            rasterOnePixelVLineSegment(lineX[i], lineY[i], lineY[i + 1], color, true);
        else
            rasterOnePixelGLineSegment(lineX[i], lineY[i], lineX[i + 1], lineY[i + 1], color, true);
    }

    if(lineY[0] == lineY[pc1])
        rasterOnePixelHLineSegment(lineX[0], lineX[pc1], lineY[0], color, true);
    else if(lineX[0] == lineX[pc1])
        rasterOnePixelVLineSegment(lineX[0], lineY[0], lineY[pc1], color, true);
    else
        rasterOnePixelGLineSegment(lineX[0], lineY[0], lineX[pc1], lineY[pc1], color, true);
}

// Based on http://alienryderflex.com/polygon_fill
// Public-domain code by Darel Rex Finley, 2007
void Rasterizer2::rasterPolygonArea(const Point* points, size_t pointCount, const Color& color)
{
    // Find the minimum and maximum coordinates
    Pt::int32_t minX =  65535;
    Pt::int32_t minY =  65535;
    Pt::int32_t maxX = -65535;
    Pt::int32_t maxY = -65535;

    for(size_t i = 0; i < pointCount; ++i) {
        const Pt::int32_t x = points[i].x();
        const Pt::int32_t y = points[i].y();
        if(x < minX) minX = x;
        if(y < minY) minY = y;
        if(x > maxX) maxX = x;
        if(y > maxY) maxY = y;
    }

    // List of nodes that define the horizontal segments
    std::vector<Pt::int32_t> nodeXf(pointCount * 2, 0);

    //  Loop through the rows of the image
    for(Pt::int32_t pixelY = minY; pixelY <= maxY; ++pixelY) {
        // Build a list of nodes
        Pt::int32_t j     = pointCount - 1;
        Pt::int32_t nodes = 0;
        for(size_t i = 0; i < pointCount; ++i) {
            if( ( points[i].y() < pixelY && points[j].y() >= pixelY ) ||
                ( points[j].y() < pixelY && points[i].y() >= pixelY )
            ) {
                Pt::int32_t deltaYp = pixelY        - points[i].y();
                Pt::int32_t deltaYj = points[j].y() - points[i].y();
                Pt::int32_t deltaXj = points[j].x() - points[i].x();
                Pt::int32_t interXf = FIXED_POINT_FROM_INT(points[i].x()) + FIXED_POINT_FROM_INT(deltaYp) / deltaYj * deltaXj;
                nodeXf[nodes++] = interXf;
                // Bail out if we have produced too many nodes
                if((size_t)nodes >= nodeXf.size()) return;
            }
            j = i;
        }
        // Sort the nodes using bubble sort
        for(Pt::int32_t i = 0; i < nodes - 1;) {
            if(nodeXf[i] > nodeXf[i + 1]) {
                std::swap(nodeXf[i], nodeXf[i + 1]);
                if(i) --i;
            }
            else {
                ++i;
            }
        }
        // Fill the pixels between the node pairs
        for(Pt::int32_t i = 0; i < nodes; i += 2) {
            // Determine the X coordinates
            Pt::int32_t from;
            Pt::int32_t to;
            if(true) {
                Pt::int32_t x1 = FIXED_POINT_TO_INT(nodeXf[i]                           );
                Pt::int32_t x2 = FIXED_POINT_TO_INT(nodeXf[i] + FIXED_POINT_CONSTANT_ONE);
                from = (x1 == x2) ? x1 : x2;
            }
            if(true) {
                Pt::int32_t x1 = FIXED_POINT_TO_INT(nodeXf[i + 1] - FIXED_POINT_CONSTANT_ONE);
                Pt::int32_t x2 = FIXED_POINT_TO_INT(nodeXf[i + 1]                           );
                to = (x1 == x2) ? x1 : x2;
            }
            if(to < from) continue;
            // Draw the spans
            Pt::int32_t spanWidth = to - from + 1;
            Pixel       pixel(_image->view(), from, pixelY);
            while(spanWidth > 0) {
                const Pt::int32_t n = std::min<Pt::int32_t>(_brushBuffer.width(), spanWidth);
                _image->format().copy(pixel, _brushPixel, n, _compositionMode);
                pixel.advance(n);
                spanWidth -= n;
            }
        }
    }
}

// Based on http://alienryderflex.com/polygon_fill
// Public-domain code by Darel Rex Finley, 2007
void Rasterizer2::rasterPolygonAreaSS(const Point* points, size_t pointCount, const Color& color)
{
    // Find the minimum and maximum coordinates
    Pt::int32_t minX =  65535;
    Pt::int32_t minY =  65535;
    Pt::int32_t maxX = -65535;
    Pt::int32_t maxY = -65535;

    for(size_t i = 0; i < pointCount; ++i) {
        const Pt::int32_t x = points[i].x();
        const Pt::int32_t y = points[i].y();
        if(x < minX) minX = x;
        if(y < minY) minY = y;
        if(x > maxX) maxX = x;
        if(y > maxY) maxY = y;
    }

    // Calculate the size of the polygon
    Pt::int32_t sizeX = (maxX - minX + 1);
    Pt::int32_t sizeY = (maxY - minY + 1) * SUPERSAMPLING_SIZE;

    // Prepare a work buffer
    std::vector<Pt::uint8_t> alphas(sizeX, 0);

    // Scale the polygon to match the super sampling size and translate it to (0, 0)
    std::vector<Pt::int32_t> pointX(pointCount, 0);
    std::vector<Pt::int32_t> pointY(pointCount, 0);

    for(size_t i = 0; i < pointCount; ++i) {
        pointX[i] = points[i].x() * SUPERSAMPLING_SIZE - minX * SUPERSAMPLING_SIZE;
        pointY[i] = points[i].y() * SUPERSAMPLING_SIZE - minY * SUPERSAMPLING_SIZE;
    }

    // List of nodes that define the horizontal segments
    std::vector<Pt::int32_t> nodeXf(pointCount * 2, 0);

    // A helper macro to scale the alpha
    #define SCALE_ALPHA(A) ( Pt::uint16_t(A) * 17 / SUPERSAMPLING_SIZE / SUPERSAMPLING_SIZE )

    //  Loop through the rows of the image
    for(Pt::int32_t pixelY = 0; pixelY < sizeY; ++pixelY) {
        // Build a list of nodes
        Pt::int32_t j     = pointCount - 1;
        Pt::int32_t nodes = 0;
        for(size_t i = 0; i < pointCount; ++i) {
            if( ( pointY[i] < pixelY && pointY[j] >= pixelY ) ||
                ( pointY[j] < pixelY && pointY[i] >= pixelY )
            ) {
                Pt::int32_t deltaYp = pixelY    - pointY[i];
                Pt::int32_t deltaYj = pointY[j] - pointY[i];
                Pt::int32_t deltaXj = pointX[j] - pointX[i];
                Pt::int32_t interXf = FIXED_POINT_FROM_INT(pointX[i]) + FIXED_POINT_FROM_INT(deltaYp) / deltaYj * deltaXj;
                nodeXf[nodes++] = interXf + FIXED_POINT_CONSTANT_HALF;
                // Bail out if we have produced too many nodes
                if((size_t)nodes >= nodeXf.size()) return;
            }
            j = i;
        }
        // Sort the nodes using bubble sort
        for(Pt::int32_t i = 0; i < nodes - 1;) {
            if(nodeXf[i] > nodeXf[i + 1]) {
                std::swap(nodeXf[i], nodeXf[i + 1]);
                if(i) --i;
            }
            else {
                ++i;
            }
        }
        // Fill the samples between the node pairs
        for(Pt::int32_t i = 0; i < nodes/2; i += 2) {
            Pt::int32_t from = FIXED_POINT_TO_INT(nodeXf[i    ]);
            Pt::int32_t to   = FIXED_POINT_TO_INT(nodeXf[i + 1]);
            for(Pt::int32_t k = from; k <= to; ++k) {
                alphas[k / SUPERSAMPLING_SIZE] += 15;
            }
        }
        // Simply skip the next steps if we have not got all the needed samples
        if( ((pixelY + 1) % SUPERSAMPLING_SIZE) ) continue;
        // Draw pixels that belongs to the left-part of the shape to the image
        Pt::int32_t iterL = 0;
        for(; iterL < sizeX; ++iterL) { // Skip fully-transparent pixels
            if(alphas[iterL]) break;
        }
        for(; iterL < sizeX; ++iterL) {
            // Break if the pixel has become fully opaque
            if(alphas[iterL] >= (15 * SUPERSAMPLING_SIZE * SUPERSAMPLING_SIZE)) break;
            // Draw the pixel
            Pixel pixel(_image->view(), minX + iterL, minY + pixelY / SUPERSAMPLING_SIZE);
            _image->format().setPixel(pixel, color, _compositionMode, SCALE_ALPHA(alphas[iterL]));
        }
        // Draw pixels that belongs to the right-part of the shape to the image
        Pt::int32_t iterR = sizeX - 1;
        for(; iterR >= 0; --iterR) { // Skip fully-transparent pixels
            if(alphas[iterR]) break;
        }
        for(; iterR >= 0; --iterR) {
            // Break if the pixel has become fully opaque
            if(alphas[iterR] >= (15 * SUPERSAMPLING_SIZE * SUPERSAMPLING_SIZE)) break;
            // Draw the pixel
            Pixel pixel(_image->view(), minX + iterR, minY + pixelY / SUPERSAMPLING_SIZE);
            _image->format().setPixel(pixel, color, _compositionMode, SCALE_ALPHA(alphas[iterR]));
        }
        // Draw pixels that belongs to the middle-part of the shape to the image
        if(iterR >= iterL) {
            Pt::int32_t spanWidth = iterR - iterL + 1;
            Pixel       pixel(_image->view(), minX + iterL, minY + pixelY / SUPERSAMPLING_SIZE);
            while(spanWidth > 0) {
                const Pt::int32_t n = std::min<Pt::int32_t>(_brushBuffer.width(), spanWidth);
                _image->format().copy(pixel, _brushPixel, n, _compositionMode);
                pixel.advance(n);
                spanWidth -= n;
            }
        }
        // Clear the work buffer
        memset(&alphas[0], 0, alphas.size());
    }
}


} // namespace
} // namespace
