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

void Rasterizer2::fillPolygon(const Point* points, size_t pointCount, Pt::uint8_t antiAliasingLevel)
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
    if(antiAliasingLevel == 0)
        rasterPolygonAreaNOAA(clipped.data(), clipped.size(), _brush.color(), minX, minY, maxX, maxY);
    else if(antiAliasingLevel == 1)
        rasterPolygonAreaFSAA(clipped.data(), clipped.size(), _brush.color(), minX, minY, maxX, maxY); // Produces artifacts at almost every corner vertex
    else
        rasterPolygonAreaSSAA(clipped.data(), clipped.size(), _brush.color(), minX, minY, maxX, maxY);
}


// ======================================================================================
// ===== Private Member Functions =======================================================
// ======================================================================================

// Uncomment this to use Duff's device
#define USE_DUFFS_DEVICE

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
void Rasterizer2::rasterPolygonAreaNOAA(const Point* points, size_t pointCount, const Color& color, Pt::int32_t minX, Pt::int32_t minY, Pt::int32_t maxX, Pt::int32_t maxY)
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
                // Bail out if we have produced too many nodes
                if((size_t) nodes >= nodeX.size()) return;
                // Calculate the node's coordinate
                Pt::int32_t deltaYp = pixelY        - points[i].y();
                Pt::int32_t deltaYj = points[j].y() - points[i].y();
                Pt::int32_t deltaXj = points[j].x() - points[i].x();
                Pt::int32_t interXf = FIXED_POINT_FROM_INT(points[i].x()) + FIXED_POINT_FROM_INT(deltaYp) / deltaYj * deltaXj;
                nodeX[nodes++] = FIXED_POINT_TO_INT(interXf + FIXED_POINT_CONSTANT_HALF);
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
void Rasterizer2::rasterPolygonAreaSSAA(const Point* points, size_t pointCount, const Color& color, Pt::int32_t minX, Pt::int32_t minY, Pt::int32_t maxX, Pt::int32_t maxY)
{
    // Calculate the size of the polygon
    Pt::int32_t sizeX = (maxX - minX + 1);
    Pt::int32_t sizeY = (maxY - minY + 1) * SUPERSAMPLING_SIZE;

    // Prepare a work buffer
    std::vector<Pt::uint8_t> alphas(sizeX, 0);

    // Scale the polygon to match the super sampling size and translate it to (0, 0)
    std::vector<Pt::int32_t> pointX(pointCount, 0);
    std::vector<Pt::int32_t> pointY(pointCount, 0);

#ifdef USE_DUFFS_DEVICE
    if(true) {
        register const Point* src  = points;
        register Pt::int32_t* dstX = &pointX[0];
        register Pt::int32_t* dstY = &pointY[0];
        register Pt::int32_t  rmnX = minX * SUPERSAMPLING_SIZE;
        register Pt::int32_t  rmnY = minY * SUPERSAMPLING_SIZE;
        register Pt::int32_t  cnt = pointCount;
        register Pt::int32_t  n   = (cnt + 7) / 8;
        switch(cnt % 8) {
                case 0 : do { *dstX = src->x() * SUPERSAMPLING_SIZE - rmnX; ++dstX;
                              *dstY = src->y() * SUPERSAMPLING_SIZE - rmnY; ++dstY; ++src;
                case 7 :      *dstX = src->x() * SUPERSAMPLING_SIZE - rmnX; ++dstX;
                              *dstY = src->y() * SUPERSAMPLING_SIZE - rmnY; ++dstY; ++src;
                case 6 :      *dstX = src->x() * SUPERSAMPLING_SIZE - rmnX; ++dstX;
                              *dstY = src->y() * SUPERSAMPLING_SIZE - rmnY; ++dstY; ++src;
                case 5 :      *dstX = src->x() * SUPERSAMPLING_SIZE - rmnX; ++dstX;
                              *dstY = src->y() * SUPERSAMPLING_SIZE - rmnY; ++dstY; ++src;
                case 4 :      *dstX = src->x() * SUPERSAMPLING_SIZE - rmnX; ++dstX;
                              *dstY = src->y() * SUPERSAMPLING_SIZE - rmnY; ++dstY; ++src;
                case 3 :      *dstX = src->x() * SUPERSAMPLING_SIZE - rmnX; ++dstX;
                              *dstY = src->y() * SUPERSAMPLING_SIZE - rmnY; ++dstY; ++src;
                case 2 :      *dstX = src->x() * SUPERSAMPLING_SIZE - rmnX; ++dstX;
                              *dstY = src->y() * SUPERSAMPLING_SIZE - rmnY; ++dstY; ++src;
                case 1 :      *dstX = src->x() * SUPERSAMPLING_SIZE - rmnX; ++dstX;
                              *dstY = src->y() * SUPERSAMPLING_SIZE - rmnY; ++dstY; ++src;
                         } while (--n > 0);
        }
    }
#else
    for(size_t i = 0; i < pointCount; ++i) {
        pointX[i] = points[i].x() * SUPERSAMPLING_SIZE - minX * SUPERSAMPLING_SIZE;
        pointY[i] = points[i].y() * SUPERSAMPLING_SIZE - minY * SUPERSAMPLING_SIZE;
    }
#endif

    // List of nodes that define the horizontal segments
    std::vector<Pt::int32_t> nodeX(pointCount * 2, 0);

    // A helper macro to scale the alpha
    #define SSAA_SCALE_ALPHA(A) ( Pt::uint16_t(A) * 17 / SUPERSAMPLING_SIZE / SUPERSAMPLING_SIZE )

    // The minimum and maximum values for alpha
    #define SSAA_MIN_ALPHA 15
    #define SSAA_MAX_ALPHA (SSAA_MIN_ALPHA * SUPERSAMPLING_SIZE * SUPERSAMPLING_SIZE)

    //  Loop through the rows of the image
    for(Pt::int32_t pixelY = 0; pixelY < sizeY; ++pixelY) {
        // Build a list of nodes
        Pt::int32_t j     = pointCount - 1;
        Pt::int32_t nodes = 0;
        for(size_t i = 0; i < pointCount; ++i) {
            if( ( pointY[i] < pixelY && pointY[j] >= pixelY ) ||
                ( pointY[j] < pixelY && pointY[i] >= pixelY )
            ) {
                // Bail out if we have produced too many nodes
                if((size_t) nodes >= nodeX.size()) return;
                // Calculate the node's coordinate
                const Pt::int32_t deltaYp = pixelY    - pointY[i];
                const Pt::int32_t deltaYj = pointY[j] - pointY[i];
                const Pt::int32_t deltaXj = pointX[j] - pointX[i];
                const Pt::int32_t interXf = FIXED_POINT_FROM_INT(pointX[i]) + FIXED_POINT_FROM_INT(deltaYp) / deltaYj * deltaXj;
                nodeX[nodes++] = FIXED_POINT_TO_INT(interXf + FIXED_POINT_CONSTANT_HALF);
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
#ifdef USE_DUFFS_DEVICE
            register Pt::uint8_t* dst = &alphas[0];
            register Pt::int32_t  cnt = to - from + 1;
            register Pt::int32_t  n   = (cnt + 7) / 8;
            register Pt::int32_t  k   = from;
            switch(cnt % 8) {
                    case 0 : do { dst[k / SUPERSAMPLING_SIZE] += SSAA_MIN_ALPHA; ++k;
                    case 7 :      dst[k / SUPERSAMPLING_SIZE] += SSAA_MIN_ALPHA; ++k;
                    case 6 :      dst[k / SUPERSAMPLING_SIZE] += SSAA_MIN_ALPHA; ++k;
                    case 5 :      dst[k / SUPERSAMPLING_SIZE] += SSAA_MIN_ALPHA; ++k;
                    case 4 :      dst[k / SUPERSAMPLING_SIZE] += SSAA_MIN_ALPHA; ++k;
                    case 3 :      dst[k / SUPERSAMPLING_SIZE] += SSAA_MIN_ALPHA; ++k;
                    case 2 :      dst[k / SUPERSAMPLING_SIZE] += SSAA_MIN_ALPHA; ++k;
                    case 1 :      dst[k / SUPERSAMPLING_SIZE] += SSAA_MIN_ALPHA; ++k;
                             } while (--n > 0);
            }
#else
            for(Pt::int32_t k = from; k <= to; ++k) {
                alphas[k / SUPERSAMPLING_SIZE] += SSAA_MIN_ALPHA;
            }
#endif
        }
        // Simply skip the next steps if we have not got all the needed samples
        if( ((pixelY + 1) % SUPERSAMPLING_SIZE) ) continue;
        // Fill the pixels between the node pairs
        for(Pt::int32_t i = 0; i < nodes; i += 2) {
            // Draw pixels that belongs to the left-part of the span to the image
            Pt::int32_t iterL = nodeX[i] / SUPERSAMPLING_SIZE - 2;
            if(iterL < 0) iterL = 0;
#ifdef USE_DUFFS_DEVICE
            if(true) { // Skip fully-transparent pixels
                register Pt::uint8_t* src  = &alphas[0];
                register Pt::int32_t  cnt  = sizeX - 1;
                register Pt::int32_t  n    = (cnt + 7) / 8;
                register Pt::int32_t  k    = iterL;
                switch(cnt % 8) {
                        case 0 : do { if(src[k]) {n = 0; break; } ++k;
                        case 7 :      if(src[k]) {n = 0; break; } ++k;
                        case 6 :      if(src[k]) {n = 0; break; } ++k;
                        case 5 :      if(src[k]) {n = 0; break; } ++k;
                        case 4 :      if(src[k]) {n = 0; break; } ++k;
                        case 3 :      if(src[k]) {n = 0; break; } ++k;
                        case 2 :      if(src[k]) {n = 0; break; } ++k;
                        case 1 :      if(src[k]) {n = 0; break; } ++k;
                                 } while (--n > 0);
                }
                iterL = k;
            }
#else
            for(; iterL < sizeX; ++iterL) { // Skip fully-transparent pixels
                if(alphas[iterL]) break;
            }
#endif
            if(_isTexture || _isGradient) { // Texture or gradient
                for(; iterL < sizeX; ++iterL) {
                    // Break if we have reached the non anti-aliased part of the span
                    if(alphas[iterL] >= SSAA_MAX_ALPHA) break;
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
                    // Break if we have reached the non anti-aliased part of the span
                    if(alphas[iterL] >= SSAA_MAX_ALPHA) break;
                    // Draw the pixel
                    Pixel pixel(_image->view(), minX + iterL, minY + pixelY / SUPERSAMPLING_SIZE);
                    _image->format().setPixel(pixel, color, _compositionMode, SSAA_SCALE_ALPHA(alphas[iterL]));
                }
            }
            // Draw pixels that belongs to the right-part of the span to the image
            Pt::int32_t iterR = nodeX[i + 1] / SUPERSAMPLING_SIZE + 2;
            if(iterR >= sizeX) iterR = sizeX - 1;
#ifdef USE_DUFFS_DEVICE
            if(true) { // Skip fully-transparent pixels
                register Pt::uint8_t* src  = &alphas[0];
                register Pt::int32_t  cnt  = sizeX - 1;
                register Pt::int32_t  n    = (cnt + 7) / 8;
                register Pt::int32_t  k    = iterR;
                switch(cnt % 8) {
                        case 0 : do { if(src[k]) {n = 0; break; } --k;
                        case 7 :      if(src[k]) {n = 0; break; } --k;
                        case 6 :      if(src[k]) {n = 0; break; } --k;
                        case 5 :      if(src[k]) {n = 0; break; } --k;
                        case 4 :      if(src[k]) {n = 0; break; } --k;
                        case 3 :      if(src[k]) {n = 0; break; } --k;
                        case 2 :      if(src[k]) {n = 0; break; } --k;
                        case 1 :      if(src[k]) {n = 0; break; } --k;
                                 } while (--n > 0);
                }
                iterR = k;
            }
#else
            for(; iterR >= 0; --iterR) { // Skip fully-transparent pixels
                if(alphas[iterR]) break;
            }
#endif
            if(_isTexture || _isGradient) { // Texture or gradient
                for(; iterR >= 0; --iterR) {
                    // Break if we have reached the non anti-aliased part of the span
                    if(alphas[iterR] >= SSAA_MAX_ALPHA) break;
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
                    // Break if we have reached the non anti-aliased part of the span
                    if(alphas[iterR] >= SSAA_MAX_ALPHA) break;
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
#if 1
                    Pixel pixel(_image->view(), minX + iterL, minY + pixelY / SUPERSAMPLING_SIZE);
                    _image->format().setPixels(pixel, _brush.color(), iterR - iterL + 1, _compositionMode);
#else
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
#endif
                }
            }
        }
        // Clear the work buffer
        memset(&alphas[0], 0, alphas.size());
    }

    // Undefine the helper macros
    #undef SSAA_SCALE_ALPHA
    #undef SSAA_MIN_ALPHA
    #undef SSAA_MAX_ALPHA
}

// Partially based on http://alienryderflex.com/polygon_fill
// Public-domain code by Darel Rex Finley, 2007
void Rasterizer2::rasterPolygonAreaFSAA(const Point* points, size_t pointCount, const Color& color, Pt::int32_t minX, Pt::int32_t minY, Pt::int32_t maxX, Pt::int32_t maxY)
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
    std::vector<Pt::int32_t> nodeX0(pointCount * 2, 0); // Row (Y    )
    std::vector<Pt::int32_t> nodeX1(pointCount * 2, 0); // Row (Y + 1)

    // A helper macro to scale the alpha
    #define FSAA_SCALE_ALPHA(A) ( Pt::uint16_t(A) * 17 / 2 / 2 )

    // The minimum, middle, and maximum values for alpha
    #define FSAA_MIN_ALPHA 15
    #define FSAA_MID_ALPHA (FSAA_MIN_ALPHA * 2    )
    #define FSAA_MAX_ALPHA (FSAA_MIN_ALPHA * 2 * 2)

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
                // Bail out if we have produced too many nodes
                if((size_t) nodes >= nodeX0.size()) return;
                // Row (Y)
                const Pt::int32_t deltaYp0 = iterY0    - pointY[i];
                const Pt::int32_t deltaYj  = pointY[j] - pointY[i];
                const Pt::int32_t deltaXj  = pointX[j] - pointX[i];
                const Pt::int32_t interXf0 = FIXED_POINT_FROM_INT(pointX[i]) + FIXED_POINT_FROM_INT(deltaYp0) / deltaYj * deltaXj;
                nodeX0[nodes] = FIXED_POINT_TO_INT(interXf0 + FIXED_POINT_CONSTANT_HALF);
                // Row (Y + 1) is valid
                if( ( pointY[i] < iterY1 && pointY[j] >= iterY1 ) ||
                    ( pointY[j] < iterY1 && pointY[i] >= iterY1 )
                ) {
                    const Pt::int32_t deltaYp1 = iterY1    - pointY[i];
                    const Pt::int32_t interXf1 = FIXED_POINT_FROM_INT(pointX[i]) + FIXED_POINT_FROM_INT(deltaYp1) / deltaYj * deltaXj;
                    nodeX1[nodes] = FIXED_POINT_TO_INT(interXf1 + FIXED_POINT_CONSTANT_HALF);
                }
                // Row (Y + 1) is not valid
                else {
                    nodeX1[nodes] = -1;
                }
                // Increment the number of nodes
                ++nodes;
            }
            j = i;
        }
        // Sort the nodes using bubble sort
        for(Pt::int32_t i = 0; i < nodes - 1;) {
            if(nodeX0[i] > nodeX0[i + 1]) {
                std::swap(nodeX0[i], nodeX0[i + 1]);
                std::swap(nodeX1[i], nodeX1[i + 1]);
                if(i) --i;
            }
            else {
                ++i;
            }
        }
        // Fill the pixels between the node pairs
        for(Pt::int32_t i = 0; i < nodes; i += 2) {
            // Get the from and to coordinates
            const Pt::int32_t from0   = nodeX0[i    ];
            const Pt::int32_t from1   = nodeX1[i    ];
            const Pt::int32_t to0     = nodeX0[i + 1];
            const Pt::int32_t to1     = nodeX1[i + 1];
            const Pt::int32_t fromMin = std::min(from0, from1);
            const Pt::int32_t fromMax = std::max(from0, from1);
            const Pt::int32_t toMin   = std::min(to0,   to1  );
            const Pt::int32_t toMax   = std::max(to0,   to1  );
            // Reset the alphas
            memset(&alphas[0], 0, alphas.size());
#if 1
            // Handle cases where the next row is not a valid row
            if(from1 < 0) {
                // Set the alphas of the left-part of the span
                alphas[from0 / 2    ] = FSAA_MID_ALPHA;
                // Set the alphas of the boundary edge of the middle-part of the span
                alphas[from0 / 2 + 1] = FSAA_MAX_ALPHA;
            }
            // Handle normal cases
            else {
                // Calculate the alphas of the left-part of the span
                for(Pt::int32_t iterX = fromMin; iterX <= fromMax; ++iterX) {
                    alphas[iterX / 2] += FSAA_MIN_ALPHA;
                }
                alphas[fromMax / 2] += FSAA_MIN_ALPHA;
                // Set the alphas of the boundary edge of the middle-part of the span
                alphas[(fromMax + 1) / 2] = FSAA_MAX_ALPHA;
            }
            // Handle cases where the next row is not a valid row
            if(to1 < 0) {
                // Set the alphas of the right-part of the span
                alphas[to0 / 2    ] = FSAA_MID_ALPHA;
                // Set the alphas of the boundary edge of the middle-part of the span
                alphas[to0 / 2 - 1] = FSAA_MAX_ALPHA;
            }
            // Handle normal cases
            else {
                // Calculate the alphas of the right-part of the span
                for(Pt::int32_t iterX = toMin; iterX <= toMax; ++iterX) {
                    alphas[iterX / 2] += FSAA_MIN_ALPHA;
                }
                alphas[toMin / 2] += FSAA_MIN_ALPHA;
                // Set the alphas of the boundary edge of the middle-part of the span
                alphas[(toMin   - 1) / 2] = FSAA_MAX_ALPHA;
            }
#else
            // Calculate the alphas of the left-part of the span
            for(Pt::int32_t iterX = fromMin; iterX <= fromMax; ++iterX) {
                alphas[iterX / 2] += FSAA_MIN_ALPHA;
            }
            alphas[fromMax / 2] += FSAA_MIN_ALPHA;
            // Calculate the alphas of the right-part of the span
            for(Pt::int32_t iterX = toMin; iterX <= toMax; ++iterX) {
                alphas[iterX / 2] += FSAA_MIN_ALPHA;
            }
            alphas[toMin / 2] += 15;
            // Set the alphas of the boundary edge of the middle-part of the span
            alphas[(fromMax + 1) / 2] = FSAA_MAX_ALPHA;
            alphas[(toMin   - 1) / 2] = FSAA_MAX_ALPHA;
#endif
            //lprintf("B: "); for(size_t k = 0; k < alphas.size(); ++k) lprintf("%d ", alphas[k] / 15); lprintf("\n");
            // Draw pixels that belongs to the left-part of the span to the image
            Pt::int32_t iterL = 0;
#ifdef USE_DUFFS_DEVICE
            if(true) { // Skip fully-transparent pixels
                register Pt::uint8_t* src  = &alphas[0];
                register Pt::int32_t  cnt  = sizeX - 1;
                register Pt::int32_t  n    = (cnt + 7) / 8;
                register Pt::int32_t  k    = iterL;
                switch(cnt % 8) {
                        case 0 : do { if(src[k]) {n = 0; break; } ++k;
                        case 7 :      if(src[k]) {n = 0; break; } ++k;
                        case 6 :      if(src[k]) {n = 0; break; } ++k;
                        case 5 :      if(src[k]) {n = 0; break; } ++k;
                        case 4 :      if(src[k]) {n = 0; break; } ++k;
                        case 3 :      if(src[k]) {n = 0; break; } ++k;
                        case 2 :      if(src[k]) {n = 0; break; } ++k;
                        case 1 :      if(src[k]) {n = 0; break; } ++k;
                                 } while (--n > 0);
                }
                iterL = k;
            }
#else
            for(; iterL < sizeX; ++iterL) { // Skip fully-transparent pixels
                if(alphas[iterL]) break;
            }
#endif
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
                    // Break if we have reached the non anti-aliased part of the span
                    if(alphas[iterL] >= FSAA_MAX_ALPHA) break;
                    // Draw the pixel
                    Pixel pixel(_image->view(), minX + iterL, minY + pixelY);
                    _image->format().setPixel(pixel, color, _compositionMode, FSAA_SCALE_ALPHA(alphas[iterL]));
                }
            }
            // Draw pixels that belongs to the right-part of the span to the image
            Pt::int32_t iterR = sizeX - 1;
#ifdef USE_DUFFS_DEVICE
            if(true) { // Skip fully-transparent pixels
                register Pt::uint8_t* src  = &alphas[0];
                register Pt::int32_t  cnt  = sizeX - 1;
                register Pt::int32_t  n    = (cnt + 7) / 8;
                register Pt::int32_t  k    = iterR;
                switch(cnt % 8) {
                        case 0 : do { if(src[k]) {n = 0; break; } --k;
                        case 7 :      if(src[k]) {n = 0; break; } --k;
                        case 6 :      if(src[k]) {n = 0; break; } --k;
                        case 5 :      if(src[k]) {n = 0; break; } --k;
                        case 4 :      if(src[k]) {n = 0; break; } --k;
                        case 3 :      if(src[k]) {n = 0; break; } --k;
                        case 2 :      if(src[k]) {n = 0; break; } --k;
                        case 1 :      if(src[k]) {n = 0; break; } --k;
                                 } while (--n > 0);
                }
                iterR = k;
            }
#else
            for(; iterR >= 0; --iterR) { // Skip fully-transparent pixels
                if(alphas[iterR]) break;
            }
#endif
            if(_isTexture || _isGradient) { // Texture or gradient
                for(; iterR >= 0; --iterR) {
                    // Break if we have reached the non anti-aliased part of the span
                    if(alphas[iterR] >= FSAA_MAX_ALPHA) break;
                    // Draw the pixel
                    const Pt::int32_t iterX = minX + iterR;
                    const Pt::int32_t iterY = minY + pixelY;
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

    // Undefine the helper macros
    #undef FSAA_SCALE_ALPHA
    #undef FSAA_MIN_ALPHA
    #undef FSAA_MID_ALPHA
    #undef FSAA_MAX_ALPHA
}

#undef USE_DUFFS_DEVICE


} // namespace
} // namespace
