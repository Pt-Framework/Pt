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

void Rasterizer2::fillPolygon(const Point* points, size_t pointCount, bool useSupersamplingForAA)
{
    // Clip the coordinates
    std::vector<Point> clipped;
    genClippedPolygonPoints(clipped, points, pointCount);

#if 0
    #define DIV_FAC 5
    clipped.clear();
    clipped.push_back(Point(450 / DIV_FAC, 100 / DIV_FAC));
    clipped.push_back(Point(350 / DIV_FAC, 300 / DIV_FAC));
    clipped.push_back(Point(650 / DIV_FAC, 400 / DIV_FAC));
    pointCount = 3;
#endif

    // Get the minimum and maximum coordinate values
    Pt::int32_t minX, minY, maxX, maxY;
    getPolygonRectMinMax(clipped.data(), clipped.size(), minX, minY, maxX, maxY);

    // Update gradient as needed
    if(_isGradient)
        updateGradientBrush(maxX - minX + 1, maxY - minY + 1);

    // Draw the polygon
    if(useSupersamplingForAA)
        //rasterPolygonAreaSS(clipped.data(), clipped.size(), _brush.color(), minX, minY, maxX, maxY);
        rasterPolygonAreaMAA(clipped.data(), clipped.size(), _brush.color(), minX, minY, maxX, maxY);
    else {
        rasterPolygonAreaSS(clipped.data(), clipped.size(), _brush.color(), minX, minY, maxX, maxY);
        //rasterPolygonArea(clipped.data(), clipped.size(), _brush.color(), minX, minY, maxX, maxY);
        //rasterPolygonOutline(clipped.data(), clipped.size(), _brush.color());
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
void Rasterizer2::rasterPolygonArea(const Point* points, size_t pointCount, const Color& color, Pt::int32_t minX, Pt::int32_t minY, Pt::int32_t maxX, Pt::int32_t maxY)
{
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
                if((size_t) nodes >= nodeXf.size()) return;
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
            // Draw the span using texture
            if(_isTexture) {
                Pt::int32_t iterX     = from;
                Pt::int32_t spanWidth = to - from + 1;
                while(spanWidth > 0) {
                    const Pt::int32_t tX = (iterX  - minX) % _brushImage->width ();
                    const Pt::int32_t tY = (pixelY - minY) % _brushImage->height();
                    const Pt::int32_t n  = std::min<Pt::int32_t>(spanWidth, _brushImage->width() - tX);
                    if(n) {
                        ConstPixel srcPixel(_brushImage->view(), tX, tY);
                        Pixel      dstPixel(_image->view(), iterX, pixelY);
                        _image->format().copy(dstPixel, srcPixel,  n, _compositionMode);
                    }
                    spanWidth -= n;
                    iterX     += n;
                }
                continue;
            }
            // Draw the span using gradient
            if(_isGradient) {
                Pt::int32_t iterX     = from;
                Pt::int32_t spanWidth = to - from + 1;
                // Fill the span - vertical gradient
                if(_brush.fillStyle() == Pt::Gfx::Brush::VerticalGradient) {
                    const Pt::int32_t textureY = (pixelY - minY) % _brushImage->height();
                    ConstPixel        srcPixel(_brushImage->view(), 0, textureY);
                    Pixel             dstPixel(_image->view(), iterX, pixelY);
                    _image->format().setPixels(dstPixel, srcPixel, spanWidth, _compositionMode);
                }
                // Fill the span - horizontal gradient
                else {
                    while(spanWidth > 0) {
                        const Pt::int32_t tX = (iterX  - minX) % _brushImage->width ();
                        const Pt::int32_t tY = (pixelY - minY) % _brushImage->height();
                        const Pt::int32_t n  = std::min<Pt::int32_t>(spanWidth, _brushImage->width() - tX);
                        if(n) {
                            ConstPixel srcPixel(_brushImage->view(), tX, tY);
                            Pixel      dstPixel(_image->view(), iterX, pixelY);
                            _image->format().copy(dstPixel, srcPixel,  n, _compositionMode);
                        }
                        spanWidth -= n;
                        iterX     += n;
                    }
                }
                continue;
            }
            // Draw the span using solid color
            Pt::int32_t iterX     = from;
            Pt::int32_t spanWidth = to - from + 1;
            while(spanWidth > 0) {
                const Pt::int32_t n = std::min<Pt::int32_t>(_brushBuffer.width(), spanWidth);
                if(n) {
                    Pixel pixel(_image->view(), iterX, pixelY);
                    _image->format().copy(pixel, _brushPixel, n, _compositionMode);
                }
                spanWidth -= n;
                iterX     += n;
            }
        }
    }
}

// Partially based on http://alienryderflex.com/polygon_fill
// Public-domain code by Darel Rex Finley, 2007
void Rasterizer2::rasterPolygonAreaSS(const Point* points, size_t pointCount, const Color& color, Pt::int32_t minX, Pt::int32_t minY, Pt::int32_t maxX, Pt::int32_t maxY)
{
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
    #define SCALE_ALPHA(A) ( Pt::uint8_t(A) * 17 / SUPERSAMPLING_SIZE / SUPERSAMPLING_SIZE )

    //  Loop through the rows of the image
    for(Pt::int32_t pixelY = 0; pixelY < sizeY; ++pixelY) {
        // Build a list of nodes
        Pt::int32_t j     = pointCount - 1;
        Pt::int32_t nodes = 0;
        for(size_t i = 0; i < pointCount; ++i) {
            if( ( pointY[i] < pixelY && pointY[j] >= pixelY ) ||
                ( pointY[j] < pixelY && pointY[i] >= pixelY )
            ) {
                const Pt::int32_t deltaYp = pixelY    - pointY[i];
                const Pt::int32_t deltaYj = pointY[j] - pointY[i];
                const Pt::int32_t deltaXj = pointX[j] - pointX[i];
                const Pt::int32_t interXf = FIXED_POINT_FROM_INT(pointX[i]) + FIXED_POINT_FROM_INT(deltaYp) / deltaYj * deltaXj;
                nodeXf[nodes++] = interXf + FIXED_POINT_CONSTANT_HALF;
                // Bail out if we have produced too many nodes
                if((size_t) nodes >= nodeXf.size()) return;
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
        // Accumulate the alphas of the samples between the node pairs
        for(Pt::int32_t i = 0; i < nodes; i += 2) {
            const Pt::int32_t from = FIXED_POINT_TO_INT(nodeXf[i    ]);
            const Pt::int32_t to   = FIXED_POINT_TO_INT(nodeXf[i + 1]);
            for(Pt::int32_t k = from; k <= to; ++k) {
                alphas[k / SUPERSAMPLING_SIZE] += 15;
            }
        }
        // Simply skip the next steps if we have not got all the needed samples
        if( ((pixelY + 1) % SUPERSAMPLING_SIZE) ) continue;
        // Draw pixels that belongs to the left-part of the span to the image
        Pt::int32_t iterL = 0;
        for(; iterL < sizeX; ++iterL) { // Skip fully-transparent pixels
            if(alphas[iterL]) break;
        }
        if(_isTexture || _isGradient) { // Texture or gradient
            for(; iterL < sizeX; ++iterL) {
                // Break if the pixel has become fully opaque
                if(alphas[iterL] >= (15 * SUPERSAMPLING_SIZE * SUPERSAMPLING_SIZE)) break;
                // Draw the pixel
                const Pt::int32_t iterX = minX + iterL;
                const Pt::int32_t iterY = minY + pixelY / SUPERSAMPLING_SIZE;
                const Pt::int32_t tX    = (iterL                      ) % _brushImage->width ();
                const Pt::int32_t tY    = (pixelY / SUPERSAMPLING_SIZE) % _brushImage->height();
                ConstPixel srcPixel(_brushImage->view(), tX, tY);
                Pixel      dstPixel(_image->view(), iterX, iterY);
                _image->format().setPixel(dstPixel, srcPixel, _compositionMode, SCALE_ALPHA(alphas[iterL]));
            }
        }
        else { // Solid color
            for(; iterL < sizeX; ++iterL) {
                // Break if the pixel has become fully opaque
                if(alphas[iterL] >= (15 * SUPERSAMPLING_SIZE * SUPERSAMPLING_SIZE)) break;
                // Draw the pixel
                Pixel pixel(_image->view(), minX + iterL, minY + pixelY / SUPERSAMPLING_SIZE);
                _image->format().setPixel(pixel, color, _compositionMode, SCALE_ALPHA(alphas[iterL]));
            }
        }
        // Draw pixels that belongs to the right-part of the span to the image
        Pt::int32_t iterR = sizeX - 1;
        for(; iterR >= 0; --iterR) { // Skip fully-transparent pixels
            if(alphas[iterR]) break;
        }
        if(_isTexture || _isGradient) { // Texture or gradient
            for(; iterR >= 0; --iterR) {
                // Break if the pixel has become fully opaque
                if(alphas[iterR] >= (15 * SUPERSAMPLING_SIZE * SUPERSAMPLING_SIZE)) break;
                // Draw the pixel
                const Pt::int32_t iterX = minX + iterR;
                const Pt::int32_t iterY = minY + pixelY / SUPERSAMPLING_SIZE;
                const Pt::int32_t tX    = (iterR                      ) % _brushImage->width ();
                const Pt::int32_t tY    = (pixelY / SUPERSAMPLING_SIZE) % _brushImage->height();
                ConstPixel srcPixel(_brushImage->view(), tX, tY);
                Pixel      dstPixel(_image->view(), iterX, iterY);
                _image->format().setPixel(dstPixel, srcPixel, _compositionMode, SCALE_ALPHA(alphas[iterR]));
            }
        }
        else { // Solid color
            for(; iterR >= 0; --iterR) {
                // Break if the pixel has become fully opaque
                if(alphas[iterR] >= (15 * SUPERSAMPLING_SIZE * SUPERSAMPLING_SIZE)) break;
                // Draw the pixel
                Pixel pixel(_image->view(), minX + iterR, minY + pixelY / SUPERSAMPLING_SIZE);
                _image->format().setPixel(pixel, color, _compositionMode, SCALE_ALPHA(alphas[iterR]));
            }
        }
        // Draw pixels that belongs to the middle-part of the span to the image
        if(iterR >= iterL) {
            // Draw the span using texture
            if(_isTexture) {
                Pt::int32_t iterX     = iterL;
                Pt::int32_t spanWidth = iterR - iterL + 1;
                while(spanWidth > 0) {
                    const Pt::int32_t tX = (iterX                      ) % _brushImage->width ();
                    const Pt::int32_t tY = (pixelY / SUPERSAMPLING_SIZE) % _brushImage->height();
                    const Pt::int32_t n  = std::min<Pt::int32_t>(spanWidth, _brushImage->width() - tX);
                    if(n) {
                        ConstPixel srcPixel(_brushImage->view(), tX, tY);
                        Pixel      dstPixel(_image->view(), minX + iterX, minY + pixelY / SUPERSAMPLING_SIZE);
                        _image->format().copy(dstPixel, srcPixel,  n, _compositionMode);
                    }
                    spanWidth -= n;
                    iterX     += n;
                }
            }
            // Draw the span using gradient
            else if(_isGradient) {
                Pt::int32_t iterX     = iterL;
                Pt::int32_t spanWidth = iterR - iterL + 1;
                // Fill the span - vertical gradient
                if(_brush.fillStyle() == Pt::Gfx::Brush::VerticalGradient) {
                    const Pt::int32_t textureY = (pixelY / SUPERSAMPLING_SIZE) % _brushImage->height();
                    ConstPixel        srcPixel(_brushImage->view(), 0, textureY);
                    Pixel             dstPixel(_image->view(), minX + iterX, minY + pixelY / SUPERSAMPLING_SIZE);
                    _image->format().setPixels(dstPixel, srcPixel, spanWidth, _compositionMode);
                }
                // Fill the span - horizontal gradient
                else {
                    while(spanWidth > 0) {
                        const Pt::int32_t tX = (iterX                      ) % _brushImage->width ();
                        const Pt::int32_t tY = (pixelY / SUPERSAMPLING_SIZE) % _brushImage->height();
                        const Pt::int32_t n  = std::min<Pt::int32_t>(spanWidth, _brushImage->width() - tX);
                        if(n) {
                            ConstPixel srcPixel(_brushImage->view(), tX, tY);
                            Pixel      dstPixel(_image->view(), minX + iterX, minY + pixelY / SUPERSAMPLING_SIZE);
                            _image->format().copy(dstPixel, srcPixel,  n, _compositionMode);
                        }
                        spanWidth -= n;
                        iterX     += n;
                    }
                }
            }
            // Draw the span using solid color
            else {
                Pt::int32_t iterX     = minX + iterL;
                Pt::int32_t spanWidth = iterR - iterL + 1;
                while(spanWidth > 0) {
                    const Pt::int32_t n = std::min<Pt::int32_t>(_brushBuffer.width(), spanWidth);
                    if(n) {
                        Pixel pixel(_image->view(), iterX, minY + pixelY / SUPERSAMPLING_SIZE);
                        _image->format().copy(pixel, _brushPixel, n, _compositionMode);
                    }
                    spanWidth -= n;
                    iterX     += n;
                }
            }
        }
        // Clear the work buffer
        memset(&alphas[0], 0, alphas.size());
    }
}

// Partially based on http://alienryderflex.com/polygon_fill
// Public-domain code by Darel Rex Finley, 2007
void Rasterizer2::rasterPolygonAreaMAA(const Point* points, size_t pointCount, const Color& color, Pt::int32_t minX, Pt::int32_t minY, Pt::int32_t maxX, Pt::int32_t maxY)
{
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
    #define SCALE_ALPHA(A) ( Pt::uint8_t(A) * 17 / SUPERSAMPLING_SIZE / SUPERSAMPLING_SIZE )

    //Solid-filled    polygon    @ ImagePainter2 =     71
    //Solid-filled    polygon SS @ ImagePainter2 =   1006

    //  Loop through the rows of the image
    for(Pt::int32_t pixelY = 0; pixelY < sizeY; ++pixelY) {
        // Build a list of nodes
        Pt::int32_t j     = pointCount - 1;
        Pt::int32_t nodes = 0;
        for(size_t i = 0; i < pointCount; ++i) {
            if( ( pointY[i] < pixelY && pointY[j] >= pixelY ) ||
                ( pointY[j] < pixelY && pointY[i] >= pixelY )
            ) {
                const Pt::int32_t deltaYp = pixelY    - pointY[i];
                const Pt::int32_t deltaYj = pointY[j] - pointY[i];
                const Pt::int32_t deltaXj = pointX[j] - pointX[i];
                const Pt::int32_t interXf = FIXED_POINT_FROM_INT(pointX[i]) + FIXED_POINT_FROM_INT(deltaYp) / deltaYj * deltaXj;
                nodeXf[nodes++] = interXf + FIXED_POINT_CONSTANT_HALF;
                // Bail out if we have produced too many nodes
                if((size_t) nodes >= nodeXf.size()) return;
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
        // Accumulate the alphas of the anti-aliased parts of the samples between the node pairs
        for(Pt::int32_t i = 0; i < nodes; i += 2) {
                  Pt::int32_t from    = FIXED_POINT_TO_INT(nodeXf[i    ]);
                  Pt::int32_t to      = FIXED_POINT_TO_INT(nodeXf[i + 1]);
            const Pt::int32_t fromMax = from + 4 * SUPERSAMPLING_SIZE;
            const Pt::int32_t toMin   = to   - 4 * SUPERSAMPLING_SIZE;
            for(; from < fromMax; ++from) {
                if(from >= to) break;
                alphas[from / SUPERSAMPLING_SIZE] += 15;
            }
            for(; to > toMin; --to) {
                if(to <= from) break;
                alphas[to / SUPERSAMPLING_SIZE] += 15;
            }
            if(to > from) {
                alphas[from / SUPERSAMPLING_SIZE] = 0;
                alphas[to / SUPERSAMPLING_SIZE] = 0;
            }

        }
        // Simply skip the next steps if we have not got all the needed samples
        if( ((pixelY + 1) % SUPERSAMPLING_SIZE) ) continue;
#if 1
        // Draw pixels that belongs to the left-part of the span to the image
        Pt::int32_t iterL = 0;
        for(; iterL < sizeX; ++iterL) { // Skip fully-transparent pixels
            if(alphas[iterL]) break;
        }
        if(_isTexture || _isGradient) { // Texture or gradient
            for(; iterL < sizeX; ++iterL) {
                // Break if we have reached the non anti-aliased part of the span
                if(!alphas[iterL]) break;
                // Draw the pixel
                const Pt::int32_t iterX = minX + iterL;
                const Pt::int32_t iterY = minY + pixelY / SUPERSAMPLING_SIZE;
                const Pt::int32_t tX    = (iterL                      ) % _brushImage->width ();
                const Pt::int32_t tY    = (pixelY / SUPERSAMPLING_SIZE) % _brushImage->height();
                ConstPixel srcPixel(_brushImage->view(), tX, tY);
                Pixel      dstPixel(_image->view(), iterX, iterY);
                _image->format().setPixel(dstPixel, srcPixel, _compositionMode, SCALE_ALPHA(alphas[iterL]));
            }
        }
        else { // Solid color
            for(; iterL < sizeX; ++iterL) {
                // Break if we have reached the non anti-aliased part of the span
                if(!alphas[iterL]) break;
                // Draw the pixel
                Pixel pixel(_image->view(), minX + iterL, minY + pixelY / SUPERSAMPLING_SIZE);
                _image->format().setPixel(pixel, color, _compositionMode, SCALE_ALPHA(alphas[iterL]));
            }
        }
        // Draw pixels that belongs to the right-part of the span to the image
        Pt::int32_t iterR = sizeX - 1;
        for(; iterR >= 0; --iterR) { // Skip fully-transparent pixels
            if(alphas[iterR]) break;
        }
        if(_isTexture || _isGradient) { // Texture or gradient
            for(; iterR >= 0; --iterR) {
                // Break if we have reached the non anti-aliased part of the span
                if(!alphas[iterR]) break;
                // Draw the pixel
                const Pt::int32_t iterX = minX + iterR;
                const Pt::int32_t iterY = minY + pixelY / SUPERSAMPLING_SIZE;
                const Pt::int32_t tX    = (iterR                      ) % _brushImage->width ();
                const Pt::int32_t tY    = (pixelY / SUPERSAMPLING_SIZE) % _brushImage->height();
                ConstPixel srcPixel(_brushImage->view(), tX, tY);
                Pixel      dstPixel(_image->view(), iterX, iterY);
                _image->format().setPixel(dstPixel, srcPixel, _compositionMode, SCALE_ALPHA(alphas[iterR]));
            }
        }
        else { // Solid color
            for(; iterR >= 0; --iterR) {
                // Break if we have reached the non anti-aliased part of the span
                if(!alphas[iterR]) break;
                // Draw the pixel
                Pixel pixel(_image->view(), minX + iterR, minY + pixelY / SUPERSAMPLING_SIZE);
                _image->format().setPixel(pixel, color, _compositionMode, SCALE_ALPHA(alphas[iterR]));
            }
        }
        // Draw pixels that belongs to the middle-part of the span to the image
        --iterL;
        ++iterR;
        if(iterR > iterL) {
            // Draw the span using texture
            if(_isTexture) {
                Pt::int32_t iterX     = iterL;
                Pt::int32_t spanWidth = iterR - iterL + 1;
                while(spanWidth > 0) {
                    const Pt::int32_t tX = (iterX                      ) % _brushImage->width ();
                    const Pt::int32_t tY = (pixelY / SUPERSAMPLING_SIZE) % _brushImage->height();
                    const Pt::int32_t n  = std::min<Pt::int32_t>(spanWidth, _brushImage->width() - tX);
                    if(n) {
                        ConstPixel srcPixel(_brushImage->view(), tX, tY);
                        Pixel      dstPixel(_image->view(), minX + iterX, minY + pixelY / SUPERSAMPLING_SIZE);
                        _image->format().copy(dstPixel, srcPixel,  n, _compositionMode);
                    }
                    spanWidth -= n;
                    iterX     += n;
                }
            }
            // Draw the span using gradient
            else if(_isGradient) {
                Pt::int32_t iterX     = iterL;
                Pt::int32_t spanWidth = iterR - iterL + 1;
                // Fill the span - vertical gradient
                if(_brush.fillStyle() == Pt::Gfx::Brush::VerticalGradient) {
                    const Pt::int32_t textureY = (pixelY / SUPERSAMPLING_SIZE) % _brushImage->height();
                    ConstPixel        srcPixel(_brushImage->view(), 0, textureY);
                    Pixel             dstPixel(_image->view(), minX + iterX, minY + pixelY / SUPERSAMPLING_SIZE);
                    _image->format().setPixels(dstPixel, srcPixel, spanWidth, _compositionMode);
                }
                // Fill the span - horizontal gradient
                else {
                    while(spanWidth > 0) {
                        const Pt::int32_t tX = (iterX                      ) % _brushImage->width ();
                        const Pt::int32_t tY = (pixelY / SUPERSAMPLING_SIZE) % _brushImage->height();
                        const Pt::int32_t n  = std::min<Pt::int32_t>(spanWidth, _brushImage->width() - tX);
                        if(n) {
                            ConstPixel srcPixel(_brushImage->view(), tX, tY);
                            Pixel      dstPixel(_image->view(), minX + iterX, minY + pixelY / SUPERSAMPLING_SIZE);
                            _image->format().copy(dstPixel, srcPixel,  n, _compositionMode);
                        }
                        spanWidth -= n;
                        iterX     += n;
                    }
                }
            }
            // Draw the span using solid color
            else {
                Pt::int32_t iterX     = minX + iterL;
                Pt::int32_t spanWidth = iterR - iterL + 1;
                while(spanWidth > 0) {
                    const Pt::int32_t n = std::min<Pt::int32_t>(_brushBuffer.width(), spanWidth);
                    if(n) {
                        Pixel pixel(_image->view(), iterX, minY + pixelY / SUPERSAMPLING_SIZE);
                        _image->format().copy(pixel, _brushPixel, n, _compositionMode);
                    }
                    spanWidth -= n;
                    iterX     += n;
                }
            }
        }
#endif
        // Clear the work buffer
        memset(&alphas[0], 0, alphas.size());
    }
}

// Partially based on http://alienryderflex.com/polygon_fill
// Public-domain code by Darel Rex Finley, 2007
void Rasterizer2::rasterPolygonAreaFAA(const Point* points, size_t pointCount, const Color& color, Pt::int32_t minX, Pt::int32_t minY, Pt::int32_t maxX, Pt::int32_t maxY)
{
    // Calculate the size of the polygon
    Pt::int32_t sizeX = (maxX - minX + 1);
    Pt::int32_t sizeY = (maxY - minY + 1);

    // Scale the polygon twice as big
    std::vector<Pt::int32_t> pointX(pointCount, 0);
    std::vector<Pt::int32_t> pointY(pointCount, 0);

    for(size_t i = 0; i < pointCount; ++i) {
        pointX[i] = points[i].x() * 2;
        pointY[i] = points[i].y() * 2;
    }

    // List of nodes that define the horizontal segments
    std::vector<Pt::int32_t> nodeXf0(pointCount * 2, 0); // Row (Y    )
    std::vector<Pt::int32_t> nodeXf1(pointCount * 2, 0); // Row (Y + 1)

    //  Loop through the rows of the image
    for(Pt::int32_t pixelY = minY; pixelY < maxY; ++pixelY) {
        // We examine two rows at a time
        const Pt::int32_t iterY0 = pixelY * 2;
        const Pt::int32_t iterY1 = iterY0 + 1;
        // Build a list of nodes
        Pt::int32_t j     = pointCount - 1;
        Pt::int32_t nodes = 0;
        for(size_t i = 0; i < pointCount; ++i) {
            if( ( pointY[i] < iterY0 && pointY[j] >= iterY0 ) ||
                ( pointY[j] < iterY0 && pointY[i] >= iterY0 )
            ) {
                const Pt::int32_t deltaYp0 = iterY0    - pointY[i];
                const Pt::int32_t deltaYp1 = iterY1    - pointY[i];
                const Pt::int32_t deltaYj  = pointY[j] - pointY[i];
                const Pt::int32_t deltaXj  = pointX[j] - pointX[i];
                const Pt::int32_t interXf0 = FIXED_POINT_FROM_INT(pointX[i]) + FIXED_POINT_FROM_INT(deltaYp0) / deltaYj * deltaXj;
                const Pt::int32_t interXf1 = FIXED_POINT_FROM_INT(pointX[i]) + FIXED_POINT_FROM_INT(deltaYp1) / deltaYj * deltaXj;
                nodeXf0[nodes] = interXf0 + FIXED_POINT_CONSTANT_HALF;
                nodeXf1[nodes] = interXf1 + FIXED_POINT_CONSTANT_HALF;
                ++nodes;
                // Bail out if we have produced too many nodes
                if((size_t) nodes >= nodeXf0.size()) return;
            }
            j = i;
        }
        // Sort the nodes using bubble sort
        for(Pt::int32_t i = 0; i < nodes - 1;) {
            if(nodeXf0[i] > nodeXf0[i + 1]) {
                std::swap(nodeXf0[i], nodeXf0[i + 1]);
                std::swap(nodeXf1[i], nodeXf1[i + 1]);
                if(i) --i;
            }
            else {
                ++i;
            }
        }
        // Fill the samples between the node pairs
        for(Pt::int32_t i = 0; i < nodes; i += 2) {
            // Get the from and to coordinates (X axis)
            //     Row (Y    ) : from0 --- to0
            //     Row (Y + 1) : from1 --- to1
            Pt::int32_t from0 = nodeXf0[i    ] / 2;
            Pt::int32_t from1 = nodeXf1[i    ] / 2;
            Pt::int32_t to0   = nodeXf0[i + 1] / 2;
            Pt::int32_t to1   = nodeXf1[i + 1] / 2;
            // Calculate the alphas
            //     f0l f0r --- t0l t0r
            //     f1l f1r --- t1l t1r
            Pt::int32_t from0AlphaL = FIXED_POINT_RFPART_TO_A8(from0);
            Pt::int32_t from0AlphaR = FIXED_POINT_FPART_TO_A8 (from0);
            Pt::int32_t from1AlphaL = FIXED_POINT_RFPART_TO_A8(from1);
            Pt::int32_t from1AlphaR = FIXED_POINT_FPART_TO_A8 (from1);
            Pt::int32_t to0AlphaL   = FIXED_POINT_RFPART_TO_A8(to0  );
            Pt::int32_t to0AlphaR   = FIXED_POINT_FPART_TO_A8 (to0  );
            Pt::int32_t to1AlphaL   = FIXED_POINT_RFPART_TO_A8(to1  );
            Pt::int32_t to1AlphaR   = FIXED_POINT_FPART_TO_A8 (to1  );
            //
            /*
             *     ┌────┬────┐        ┌────┬────┐
             *     │ 00 │ f0 ┝        │ t0 ┝ 00 │
             *     ├────┼────┤        ├────┼────┤
             *     │ f1 ┝ FF │        │ FF │ t1 ┝
             *     └────┴────┘        └────┴────┘
             *
             *     ┌────┬────┐        ┌────┬────┐
             *     │ 00 │ f0 ┝        │ t0 ┝ 00 │
             *     ├────┼────┤        ├────┼────┤
             *     │ 00 │ f1 ┝        │ t1 ┝ 00 │
             *     └────┴────┘        └────┴────┘
             *
             *     ┌────┬────┐        ┌────┬────┐
             *     │ f0 ┝ FF │        │ FF │ t0 ┝
             *     ├────┼────┤        ├────┼────┤
             *     │ 00 │ f1 ┝        │ t1 ┝ 00 │
             *     └────┴────┘        └────┴────┘
             */
            Pt::int32_t from = 0, to = 0, alpha;
            // Left side of the span
            if(from1 < from0) {
                from = FIXED_POINT_TO_INT(from1);
                Pixel pixel(_image->view(), from, pixelY);
                alpha = (0 + from0AlphaL + from1AlphaL + from1AlphaR) / 4;
                _image->format().setPixel(pixel, color, _compositionMode, alpha);
                pixel.advance();
                alpha = (from0AlphaR + 255 + 255 + 255) / 4;
                _image->format().setPixel(pixel, color, _compositionMode, alpha);
            }
            else if(from1 == from0) {
                from = FIXED_POINT_TO_INT(from0 - 1);
                Pixel pixel(_image->view(), from, pixelY);
                alpha = (0 + from0AlphaL + 0 + from1AlphaL) / 4;
                _image->format().setPixel(pixel, color, _compositionMode, alpha);
                pixel.advance();
                alpha = (from0AlphaR + 255 + from1AlphaR + 255) / 4;
                _image->format().setPixel(pixel, color, _compositionMode, alpha);
            }
            else { // from1 > from0
                from = FIXED_POINT_TO_INT(from0);
                Pixel pixel(_image->view(), from, pixelY);
                alpha = (from0AlphaL + from0AlphaR + 0 + from1AlphaL) / 4;
                _image->format().setPixel(pixel, color, _compositionMode, alpha);
                pixel.advance();
                alpha = (255 + 255 + from1AlphaR + 255) / 4;
                _image->format().setPixel(pixel, color, _compositionMode, alpha);
            }
            // Right side of the span
            if(to1 > to0) {
                to = FIXED_POINT_TO_INT(to0);
                Pixel pixel(_image->view(), to, pixelY);
                alpha = (to0AlphaL + to0AlphaR + 255 + to1AlphaL) / 4;
                _image->format().setPixel(pixel, color, _compositionMode, alpha);
                pixel.advance();
                alpha = (0 + 0 + to1AlphaR + 0) / 4;
                _image->format().setPixel(pixel, color, _compositionMode, alpha);
            }
            else if(to1 == to0) {
                to = FIXED_POINT_TO_INT(to0);
                Pixel pixel(_image->view(), to, pixelY);
                alpha = (to0AlphaL + to0AlphaR + to1AlphaL + to1AlphaR) / 4;
                _image->format().setPixel(pixel, color, _compositionMode, alpha);
            }
            else { // to1 < to0
                to  = FIXED_POINT_TO_INT(to1);
                Pixel pixel(_image->view(), to, pixelY);
                alpha = (255 + to0AlphaL + to1AlphaL + to1AlphaR) / 4;
                _image->format().setPixel(pixel, color, _compositionMode, alpha);
                pixel.advance();
                alpha = (to0AlphaR + 0 + 0 + 0) / 4;
                _image->format().setPixel(pixel, color, _compositionMode, alpha);
            }
            // Adjust coordinates
            from += 2;
            to   -= 1;
            // Middle side of the span
            Pt::int32_t iterX     = from;
            Pt::int32_t spanWidth = to - from + 1;
            while(spanWidth > 0) {
                const Pt::int32_t n = std::min<Pt::int32_t>(_brushBuffer.width(), spanWidth);
                if(n) {
                    Pixel pixel(_image->view(), iterX, pixelY);
                    _image->format().copy(pixel, _brushPixel, n, _compositionMode);
                }
                spanWidth -= n;
                iterX     += n;
            }
        }
    }
}


} // namespace
} // namespace
