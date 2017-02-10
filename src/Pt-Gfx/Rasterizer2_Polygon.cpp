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

void Rasterizer2::fillPolygon(const Point* points, size_t pointCount, bool useAntiAliasing)
{
    // Clip the coordinates
    std::vector<Point> clipped;
    genClippedPolygonPoints(clipped, points, pointCount);

#if 0
    #define DIV_FAC 50
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
    if(useAntiAliasing) {
      //rasterPolygonAreaTrueSSAA(clipped.data(), clipped.size(), _brush.color(), minX, minY, maxX, maxY);
      //rasterPolygonAreaEdgeSSAA(clipped.data(), clipped.size(), _brush.color(), minX, minY, maxX, maxY);
      //rasterPolygonAreaFastSSAA(clipped.data(), clipped.size(), _brush.color(), minX, minY, maxX, maxY);
        rasterPolygonAreaFastAASC(clipped.data(), clipped.size(), _brush.color(), minX, minY, maxX, maxY);
    }

    else {
        rasterPolygonAreaFastNOAA(clipped.data(), clipped.size(), _brush.color(), minX, minY, maxX, maxY);
      //rasterPolygonAreaTrueSSAA(clipped.data(), clipped.size(), _brush.color(), minX, minY, maxX, maxY);
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
void Rasterizer2::rasterPolygonAreaFastNOAA(const Point* points, size_t pointCount, const Color& color, Pt::int32_t minX, Pt::int32_t minY, Pt::int32_t maxX, Pt::int32_t maxY)
{
    // List of nodes that define the horizontal segments
    std::vector<Pt::int32_t> nodeX(pointCount * 2, 0);

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
                nodeX[nodes++] = FIXED_POINT_TO_INT(interXf + FIXED_POINT_CONSTANT_HALF);
                // Bail out if we have produced too many nodes
                if((size_t) nodes >= nodeX.size()) return;
            }
            j = i;
        }
        // Sort the nodes using bubble sort
        for(Pt::int32_t i = 0; i < nodes - 1;) {
            if(nodeX[i] > nodeX[i + 1]) {
                std::swap(nodeX[i], nodeX[i + 1]);
                if(i) --i;
            }
            else {
                ++i;
            }
        }
        // Fill the pixels between the node pairs
        for(Pt::int32_t i = 0; i < nodes; i += 2) {
            // Determine the X coordinates
            Pt::int32_t from = nodeX[i    ];
            Pt::int32_t to   = nodeX[i + 1];
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
void Rasterizer2::rasterPolygonAreaTrueSSAA(const Point* points, size_t pointCount, const Color& color, Pt::int32_t minX, Pt::int32_t minY, Pt::int32_t maxX, Pt::int32_t maxY)
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
    std::vector<Pt::int32_t> nodeX(pointCount * 2, 0);

    // A helper macro to scale the alpha
    #define SSAA_SCALE_ALPHA(A) ( Pt::uint16_t(A) * 17 / SUPERSAMPLING_SIZE / SUPERSAMPLING_SIZE )

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
                nodeX[nodes++] = FIXED_POINT_TO_INT(interXf + FIXED_POINT_CONSTANT_HALF);
                // Bail out if we have produced too many nodes
                if((size_t) nodes >= nodeX.size()) return;
            }
            j = i;
        }
        // Sort the nodes using bubble sort
        for(Pt::int32_t i = 0; i < nodes - 1;) {
            if(nodeX[i] > nodeX[i + 1]) {
                std::swap(nodeX[i], nodeX[i + 1]);
                if(i) --i;
            }
            else {
                ++i;
            }
        }
        // Accumulate the alphas of the samples between the node pairs
        for(Pt::int32_t i = 0; i < nodes; i += 2) {
            const Pt::int32_t from = nodeX[i    ];
            const Pt::int32_t to   = nodeX[i + 1];
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
                _image->format().setPixel(dstPixel, srcPixel, _compositionMode, SSAA_SCALE_ALPHA(alphas[iterL]));
            }
        }
        else { // Solid color
            for(; iterL < sizeX; ++iterL) {
                // Break if the pixel has become fully opaque
                if(alphas[iterL] >= (15 * SUPERSAMPLING_SIZE * SUPERSAMPLING_SIZE)) break;
                // Draw the pixel
                Pixel pixel(_image->view(), minX + iterL, minY + pixelY / SUPERSAMPLING_SIZE);
                _image->format().setPixel(pixel, color, _compositionMode, SSAA_SCALE_ALPHA(alphas[iterL]));
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
                _image->format().setPixel(dstPixel, srcPixel, _compositionMode, SSAA_SCALE_ALPHA(alphas[iterR]));
            }
        }
        else { // Solid color
            for(; iterR >= 0; --iterR) {
                // Break if the pixel has become fully opaque
                if(alphas[iterR] >= (15 * SUPERSAMPLING_SIZE * SUPERSAMPLING_SIZE)) break;
                // Draw the pixel
                Pixel pixel(_image->view(), minX + iterR, minY + pixelY / SUPERSAMPLING_SIZE);
                _image->format().setPixel(pixel, color, _compositionMode, SSAA_SCALE_ALPHA(alphas[iterR]));
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

    // Undefine the helper macro
    #undef SSAA_SCALE_ALPHA
}

// Partially based on http://alienryderflex.com/polygon_fill
// Public-domain code by Darel Rex Finley, 2007
void Rasterizer2::rasterPolygonAreaEdgeSSAA(const Point* points, size_t pointCount, const Color& color, Pt::int32_t minX, Pt::int32_t minY, Pt::int32_t maxX, Pt::int32_t maxY)
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
    std::vector<Pt::int32_t> nodeX(pointCount * 2, 0);

    // A helper macro to scale the alpha
    #define ASAA_SCALE_ALPHA(A) ( Pt::uint16_t(A) * 17 / SUPERSAMPLING_SIZE / SUPERSAMPLING_SIZE )

    // How far from the edges shall the anti-aliasing be done
    #define EDGE_FACTOR (8 * SUPERSAMPLING_SIZE)

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
                nodeX[nodes++] = FIXED_POINT_TO_INT(interXf + FIXED_POINT_CONSTANT_HALF);
                // Bail out if we have produced too many nodes
                if((size_t) nodes >= nodeX.size()) return;
            }
            j = i;
        }
        // Sort the nodes using bubble sort
        for(Pt::int32_t i = 0; i < nodes - 1;) {
            if(nodeX[i] > nodeX[i + 1]) {
                std::swap(nodeX[i], nodeX[i + 1]);
                if(i) --i;
            }
            else {
                ++i;
            }
        }
        // Accumulate the alphas of the anti-aliased parts of the samples between the node pairs
        for(Pt::int32_t i = 0; i < nodes; i += 2) {
            Pt::int32_t from    = nodeX[i    ];
            Pt::int32_t to      = nodeX[i + 1];
            if( (to - from) <= (EDGE_FACTOR * 2 ) ) {
                for(Pt::int32_t k = from; k <= to; ++k) {
                    alphas[k / SUPERSAMPLING_SIZE] += 15;
                }
            }
            else {
                const Pt::int32_t fromMax = from + EDGE_FACTOR;
                const Pt::int32_t toMin   = to   - EDGE_FACTOR;
                for(; from < fromMax; ++from) {
                    if(from >= to) break;
                    alphas[from / SUPERSAMPLING_SIZE] += 15;
                }
                for(; to > toMin; --to) {
                    if(to <= from) break;
                    alphas[to / SUPERSAMPLING_SIZE] += 15;
                }
            }
        }
        // Simply skip the next steps if we have not got all the needed samples
        if( ((pixelY + 1) % SUPERSAMPLING_SIZE) ) continue;
        // Set the corresponding alphas of the edge of the middle-part of the span to zeroes
        for(Pt::int32_t i = 0; i < nodes; i += 2) {
            Pt::int32_t from = nodeX[i    ] + EDGE_FACTOR / 2;
            Pt::int32_t to   = nodeX[i + 1] - EDGE_FACTOR / 2;
            if(to < from) continue;
            for(Pt::int32_t k = from; k <= from + EDGE_FACTOR / 2; ++k) {
                if(k >= to) break;
                alphas[k / SUPERSAMPLING_SIZE] = 0;
            }
            for(Pt::int32_t k = to; k >= to - EDGE_FACTOR / 2; --k) {
                if(k <= from) break;
                alphas[k / SUPERSAMPLING_SIZE] = 0;
            }
        }
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
                _image->format().setPixel(dstPixel, srcPixel, _compositionMode, ASAA_SCALE_ALPHA(alphas[iterL]));
            }
        }
        else { // Solid color
            for(; iterL < sizeX; ++iterL) {
                // Break if we have reached the non anti-aliased part of the span
                if(!alphas[iterL]) break;
                // Draw the pixel
                Pixel pixel(_image->view(), minX + iterL, minY + pixelY / SUPERSAMPLING_SIZE);
                _image->format().setPixel(pixel, color, _compositionMode, ASAA_SCALE_ALPHA(alphas[iterL]));
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
                _image->format().setPixel(dstPixel, srcPixel, _compositionMode, ASAA_SCALE_ALPHA(alphas[iterR]));
            }
        }
        else { // Solid color
            for(; iterR >= 0; --iterR) {
                // Break if we have reached the non anti-aliased part of the span
                if(!alphas[iterR]) break;
                // Draw the pixel
                Pixel pixel(_image->view(), minX + iterR, minY + pixelY / SUPERSAMPLING_SIZE);
                _image->format().setPixel(pixel, color, _compositionMode, ASAA_SCALE_ALPHA(alphas[iterR]));
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

    // Undefine the helper macro
    #undef EDGE_FACTOR
    #undef ASAA_SCALE_ALPHA
}

// Partially based on http://alienryderflex.com/polygon_fill
// Public-domain code by Darel Rex Finley, 2007
void Rasterizer2::rasterPolygonAreaFastSSAA(const Point* points, size_t pointCount, const Color& color, Pt::int32_t minX, Pt::int32_t minY, Pt::int32_t maxX, Pt::int32_t maxY)
{
    // Calculate the size of the polygon
    Pt::int32_t sizeX = (maxX - minX + 1);
    Pt::int32_t sizeY = (maxY - minY + 1);

    // Prepare a work buffer
    std::vector<Pt::uint8_t> alphas(maxX - minX + 1, 0);

    // Scale the polygon twice as big
    std::vector<Pt::int32_t> pointX(pointCount, 0);
    std::vector<Pt::int32_t> pointY(pointCount, 0);

    for(size_t i = 0; i < pointCount; ++i) {
        pointX[i] = (points[i].x() - minX) * 2;
        pointY[i] = (points[i].y() - minY) * 2;
    }

    // List of nodes that define the horizontal segments
    std::vector<Pt::int32_t> nodeXf0(pointCount * 2, 0); // Row (Y    )
    std::vector<Pt::int32_t> nodeXf1(pointCount * 2, 0); // Row (Y + 1)

    // A helper macro to scale the alpha
    #define FSAA_SCALE_ALPHA(A) ( Pt::uint16_t(A) * 17 / 2 / 2 )

    // The maximum possible value for alpha
    #define FSAA_MAX_ALPHA ( 15 * 2 * 2 )

    //  Loop through the rows of the image
    for(Pt::int32_t pixelY = 0; pixelY < sizeY; ++pixelY) {
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
                nodeXf0[nodes] = FIXED_POINT_TO_INT(interXf0 + FIXED_POINT_CONSTANT_HALF);
                nodeXf1[nodes] = FIXED_POINT_TO_INT(interXf1 + FIXED_POINT_CONSTANT_HALF);
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
            // Get the from and to coordinates
            const Pt::int32_t from0   = nodeXf0[i    ];
            const Pt::int32_t from1   = nodeXf1[i    ];
            const Pt::int32_t to0     = nodeXf0[i + 1];
            const Pt::int32_t to1     = nodeXf1[i + 1];
            const Pt::int32_t fromMin = std::min(from0, from1);
            const Pt::int32_t fromMax = std::max(from0, from1);
            const Pt::int32_t toMin   = std::min(to0,   to1  );
            const Pt::int32_t toMax   = std::max(to0,   to1  );
            // Reset the alphas
            memset(&alphas[0], 0, alphas.size());
            // Calculate the alphas of the left-part of the span
            for(Pt::int32_t iterX = fromMin; iterX <= fromMax; ++iterX) {
                alphas[iterX / 2] += 15;
            }
            alphas[fromMax / 2] += 15;
            // Calculate the alphas of the right-part of the span
            for(Pt::int32_t iterX = toMin; iterX <= toMax; ++iterX) {
                alphas[iterX / 2] += 15;
            }
            alphas[toMin / 2] += 15;
            // Set the alphas of the boundary edge of the middle-part of the span
            alphas[(fromMax + 1) / 2] = FSAA_MAX_ALPHA;
            alphas[(toMin   - 1) / 2] = FSAA_MAX_ALPHA;
            //lprintf("B: "); for(size_t k = 0; k < alphas.size(); ++k) lprintf("%d ", alphas[k] / 15); lprintf("\n");
            // Draw pixels that belongs to the left-part of the span to the image
            Pt::int32_t iterL = 0;
            for(; iterL < sizeX; ++iterL) { // Skip fully-transparent pixels
                if(alphas[iterL]) break;
            }
            if(_isTexture || _isGradient) { // Texture or gradient
                for(; iterL < sizeX; ++iterL) {
                    // Break if we have reached the non anti-aliased part of the span
                    if(alphas[iterL] >= FSAA_MAX_ALPHA) break;
                    // Draw the pixel
                    const Pt::int32_t iterX = minX + iterL;
                    const Pt::int32_t iterY = minY + pixelY;
                    const Pt::int32_t tX    = iterL  % _brushImage->width ();
                    const Pt::int32_t tY    = pixelY % _brushImage->height();
                    ConstPixel srcPixel(_brushImage->view(), tX, tY);
                    Pixel      dstPixel(_image->view(), iterX, iterY);
                    _image->format().setPixel(dstPixel, srcPixel, _compositionMode, FSAA_SCALE_ALPHA(alphas[iterL]));
                }
            }
            else { // Solid color
                for(; iterL < sizeX; ++iterL) {
                    // Break if the pixel has become fully opaque
                    if(alphas[iterL] >= FSAA_MAX_ALPHA) break;
                    // Draw the pixel
                    Pixel pixel(_image->view(), minX + iterL, minY + pixelY);
                    _image->format().setPixel(pixel, color, _compositionMode, FSAA_SCALE_ALPHA(alphas[iterL]));
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
                    if(alphas[iterR] >= FSAA_MAX_ALPHA) break;
                    // Draw the pixel
                    const Pt::int32_t iterX = minX + iterR;
                    const Pt::int32_t iterY = minY + pixelY ;
                    const Pt::int32_t tX    = iterR  % _brushImage->width ();
                    const Pt::int32_t tY    = pixelY % _brushImage->height();
                    ConstPixel srcPixel(_brushImage->view(), tX, tY);
                    Pixel      dstPixel(_image->view(), iterX, iterY);
                    _image->format().setPixel(dstPixel, srcPixel, _compositionMode, FSAA_SCALE_ALPHA(alphas[iterR]));
                }
            }
            else { // Solid color
                for(; iterR >= 0; --iterR) {
                    // Break if we have reached the non anti-aliased part of the span
                    if(alphas[iterR] >= FSAA_MAX_ALPHA) break;
                    // Draw the pixel
                    Pixel pixel(_image->view(), minX + iterR, minY + pixelY);
                    _image->format().setPixel(pixel, color, _compositionMode, FSAA_SCALE_ALPHA(alphas[iterR]));
                }
            }
            // Draw pixels that belongs to the middle-part of the span to the image
            if(iterR >= iterL) {
                // Adjust the coordinate on the last row
                if(pixelY == sizeY - 1) {
                    iterL = from0 / 2;
                    iterR = to0   / 2;
                }
                // Draw the span using texture
                if(_isTexture) {
                    Pt::int32_t iterX     = iterL;
                    Pt::int32_t spanWidth = iterR - iterL + 1;
                    while(spanWidth > 0) {
                        const Pt::int32_t tX = iterX  % _brushImage->width ();
                        const Pt::int32_t tY = pixelY % _brushImage->height();
                        const Pt::int32_t n  = std::min<Pt::int32_t>(spanWidth, _brushImage->width() - tX);
                        if(n) {
                            ConstPixel srcPixel(_brushImage->view(), tX, tY);
                            Pixel      dstPixel(_image->view(), minX + iterX, minY + pixelY);
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
                        const Pt::int32_t textureY = pixelY % _brushImage->height();
                        ConstPixel        srcPixel(_brushImage->view(), 0, textureY);
                        Pixel             dstPixel(_image->view(), minX + iterX, minY + pixelY);
                        _image->format().setPixels(dstPixel, srcPixel, spanWidth, _compositionMode);
                    }
                    // Fill the span - horizontal gradient
                    else {
                        while(spanWidth > 0) {
                            const Pt::int32_t textureX = iterX  % _brushImage->width ();
                            const Pt::int32_t n        = std::min<Pt::int32_t>(spanWidth, _brushImage->width() - textureX);
                            if(n) {
                                ConstPixel srcPixel(_brushImage->view(), textureX, 0);
                                Pixel      dstPixel(_image->view(), minX + iterX, minY + pixelY);
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
                            Pixel pixel(_image->view(), iterX, minY + pixelY);
                            _image->format().copy(pixel, _brushPixel, n, _compositionMode);
                        }
                        spanWidth -= n;
                        iterX     += n;
                    }
                }
            }
        }
    }

    // Undefine the helper macro
    #undef FSAA_SCALE_ALPHA
    #undef FSAA_MAX_ALPHA
}

// Fast Anti-Aliasing Polygon Scan Conversion
// Jack Morrison, Graphics Gems, Academic Press, 1990
// Dan Field, Incremental Linear Interpolation, ACM Transactions on Graphics, 1985
// http://www.realtimerendering.com/resources/GraphicsGems
// http://www.realtimerendering.com/resources/GraphicsGems/category.html
// http://www.realtimerendering.com/resources/GraphicsGems/gems/AAPolyScan.c

#define SUBYRES 8       /* subpixel Y resolution per scanline */
#define SUBXRES 16      /* subpixel X resolution per pixel */
#define MAX_AREA    (SUBYRES*SUBXRES)
#define MODRES(y)   ((y) & 7)       /*subpixel Y modulo */
#define MAX_X   0x7FFF  /* subpixel X beyond right edge */

const Point *Vleft, *VnextLeft;      /* current left edge */
const Point *Vright, *VnextRight;    /* current right edge */

struct  SubPixel  {         /* subpixel extents for scanline */
    int xLeft, xRight;
    } sp[SUBYRES];

int xLmin, xLmax;       /* subpixel x extremes for scanline */
int xRmax, xRmin;       /* (for optimization shortcut) */

inline void vLerp(double alpha, const Point* Va, const Point* Vb, Point* Vout)
{
    double ialpha = 1.0 - alpha;

    Vout->setX(ialpha * Va->x() + alpha * Vb->x());
    Vout->setY(ialpha * Va->y() + alpha * Vb->y());
}

inline double LERP(double alpha, int a, int b)
{
    double ialpha = 1.0 - alpha;
    return ialpha * a + alpha * b;
}

/* Compute bitmask indicating which subpixels are covered by
 * polygon at current pixel. (Not all hidden-surface methods
 * need this mask. )
*/
void computePixelMask(
    int x,          /* left subpixel of pixel */
    unsigned mask[]    /* output bitmask */
)
{
    static unsigned leftMaskTable[] =
        { 0xFFFF, 0x7FFF, 0x3FFF, 0x1FFF, 0x0FFF, 0x07FF, 0x03FF,
          0x01FF, 0x00FF, 0x007F, 0x003F, 0x001F, 0x000F, 0x0007,
          0x0003, 0x0001  };
    static unsigned rightMaskTable[] =
        { 0x8000, 0xC000, 0xE000, 0xF000, 0xF800, 0xFC00,
          0xFE00, 0xFF00, 0xFF80, 0xFFC0, 0xFFE0, 0xFFF0,
          0xFFF8, 0xFFFC, 0xFFFE, 0xFFFF   };
    unsigned leftMask, rightMask;       /* partial masks */
    int xr = x+SUBXRES-1;           /* right subpixel of pixel */
    int y;

/* shortcut for common case of fully covered pixel */
    if (x>xLmax && x<xRmin)     {
        for (y=0; y<SUBYRES; y++)
            mask[y] = 0xFFFF;
    } else  {
        for (y=0; y<SUBYRES; y++)   {
            if (sp[y].xLeft < x)    /* completely left of pixel*/
                leftMask = 0xFFFF;
            else if (sp[y].xLeft > xr)  /* completely right */
                leftMask = 0;
            else
                leftMask = leftMaskTable[sp[y].xLeft -x];

            if (sp[y].xRight > xr)      /* completely  */
                            /* right of pixel*/
                rightMask = 0xFFFF;
            else if (sp[y].xRight < x)  /*completely left */
                rightMask = 0;
            else
                rightMask = rightMaskTable[sp[y].xRight -x];
            mask[y] = leftMask & rightMask;
        }
    }
}
/*
 * Compute number of subpixels covered by polygon at current pixel
*/
int Coverage(
    int x          /* left subpixel of pixel */
    )
{
    int  area;          /* total covered area */
    int partialArea;      /* covered area for current subpixel y */
    int xr = x+SUBXRES-1;   /*right subpixel of pixel */
    int y;

    /* shortcut for common case of fully covered pixel */
    if (x>xLmax && x<xRmin)
        return MAX_AREA;

    for (area=y=0; y<SUBYRES; y++) {
        partialArea = std::min(sp[y].xRight, xr)
             - std::max(sp[y].xLeft, x) + 1;
        if (partialArea > 0)
            area += partialArea;
    }
    return area;
}

/*
 * Render one scanline of polygon
 */

void Rasterizer2::renderScanline(const Point* Vl, const Point* Vr, int y)
{
    Point Vpixel;  /*object info interpolated at one pixel */
    unsigned mask[SUBYRES]; /*pixel coverage bitmask */
    int x;          /* leftmost subpixel of current pixel */

    for (x=SUBXRES*floor((double)(xLmin/SUBXRES)); x<=xRmax; x+=SUBXRES) {
        int cov = Coverage(x) * 255 / MAX_AREA;
        vLerp((double)(x-xLmin)/(xRmax-xLmin), Vl, Vr, &Vpixel);
        computePixelMask(x, mask);


        for(int yy = 0; yy < SUBYRES; ++yy) {
            Pixel      dstPixel(_image->view(), x/SUBXRES, y*SUBYRES +yy);
            _image->format().setPixel(dstPixel, _brush.color(), _compositionMode, cov);// * mask[yy] / 65535);
        }
    }
}



void Rasterizer2::rasterPolygonAreaFastAASC(const Point* points, size_t pointCount, const Color& color, Pt::int32_t minX, Pt::int32_t minY, Pt::int32_t maxX, Pt::int32_t maxY)
{
#define screenX(P) (P->x()*SUBXRES)

    const Point *endPoly;            /* end of polygon vertex list */
    Point VscanLeft, VscanRight;   /* interpolated vertices */                                 /* at scanline */
    double aLeft, aRight;           /* interpolation ratios */
    struct SubPixel *sp_ptr;        /* current subpixel info */
    int xLeft, xNextLeft;           /* subpixel coordinates for */
    int  xRight, xNextRight;        /* active polygon edges */
    int i,y;

/* find vertex with minimum y (display coordinate) */
Vleft = points;
for  (i=1; i<pointCount; i++)
    if  (points[i].y() < Vleft->y())
        Vleft = &points[i];
endPoly = &points[pointCount-1];

/* initialize scanning edges */
Vright = VnextRight = VnextLeft = Vleft;

/* prepare bottom of initial scanline - no coverage by polygon */
for (i=0; i<SUBYRES; i++)
    sp[i].xLeft = sp[i].xRight = -1;
xLmin = xRmin = MAX_X;
xLmax = xRmax = -1;

/* scan convert for each subpixel from bottom to top */
for (y=Vleft->y(); ; y++) {

    while (y == VnextLeft->y())   {   /* reached next left vertex */
        VnextLeft = (Vleft=VnextLeft) + 1;  /* advance */
        if (VnextLeft > endPoly)            /* (wraparound) */
            VnextLeft = points;
        if (VnextLeft == Vright)    /* all y's same?  */
            return;             /* (null polygon) */
        xLeft = screenX(Vleft);
        xNextLeft = screenX(VnextLeft);
    }

    while (y == VnextRight->y())  { /*reached next right vertex */
        VnextRight = (Vright=VnextRight) -1;
        if (VnextRight < points)           /* (wraparound) */
            VnextRight = endPoly;
        xRight = screenX(Vright);
        xNextRight = screenX(VnextRight);
    }

    if (y>VnextLeft->y() || y>VnextRight->y())  {
                /* done, mark uncovered part of last scanline */
        for (; MODRES(y); y++)
            sp[MODRES(y)].xLeft = sp[MODRES(y)].xRight = -1;
        renderScanline(Vleft, Vright, y/SUBYRES);
        return;
    }

/*
 * Interpolate sub-pixel x endpoints at this y,
 * and update extremes for pixel coherence optimization
 */

    sp_ptr = &sp[MODRES(y)];
    aLeft = (double)(y - Vleft->y()) / (VnextLeft->y() - Vleft->y());
    sp_ptr->xLeft = LERP(aLeft, xLeft, xNextLeft);
    if (sp_ptr->xLeft < xLmin)
        xLmin = sp_ptr->xLeft;
    if (sp_ptr->xLeft > xLmax)
        xLmax = sp_ptr->xLeft;

    aRight = (double)(y - Vright->y()) / (VnextRight->y()
                    - Vright->y());
    sp_ptr->xRight = LERP(aRight, xRight, xNextRight);
    if (sp_ptr->xRight < xRmin)
        xRmin = sp_ptr->xRight;
    if (sp_ptr->xRight > xRmax)
        xRmax = sp_ptr->xRight;

    if (MODRES(y) == SUBYRES-1) {   /* end of scanline */
            /* interpolate edges to this scanline */
        vLerp(aLeft, Vleft, VnextLeft, &VscanLeft);
        vLerp(aRight, Vright, VnextRight, &VscanRight);
        renderScanline(&VscanLeft, &VscanRight, y/SUBYRES);
        xLmin = xRmin = MAX_X;      /* reset extremes */
        xLmax = xRmax = -1;
    }
  }
}


} // namespace
} // namespace
