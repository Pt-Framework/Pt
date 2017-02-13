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
    // Minimum and maximum coordinate values for all the polygons
    Pt::int32_t minX =  COORDINATE_LIMIT;
    Pt::int32_t minY =  COORDINATE_LIMIT;
    Pt::int32_t maxX = -COORDINATE_LIMIT;
    Pt::int32_t maxY = -COORDINATE_LIMIT;

    // Separate the polygons and clip their coordinates
    std::vector<Point > clippedPoints;
    std::vector<size_t> clippedCounts;
    size_t              startIndex = 0;
    for(size_t i = 0; i < pointCount; ++i) {
        // Search for the separator point
        if(points[i].x() > COORDINATE_LIMIT && points[i].y() > COORDINATE_LIMIT) {
            // Calculate the number of points for this polygon
            const size_t curPC = i - startIndex;
            // Clip the coordinates
            std::vector<Point> clipped;
            genClippedPolygonPoints(clipped, points + startIndex, curPC);
            // Increment the start index
            startIndex += curPC + 1;
            // Calculate the minimum and maximum coordinate values
            Pt::int32_t curMinX, curMinY, curMaxX, curMaxY;
            getPolygonRectMinMax(clipped.data(), clipped.size(), curMinX, curMinY, curMaxX, curMaxY);
            if(curMinX < minX) minX = curMinX;
            if(curMinY < minY) minY = curMinY;
            if(curMaxX > maxX) maxX = curMaxX;
            if(curMaxY > maxY) maxY = curMaxY;
            // Store the clipped points
            clippedPoints.insert(clippedPoints.end(), clipped.begin(), clipped.end());
            // Store the number of points
            clippedCounts.push_back(curPC);
        }
    }

    // Update gradient as needed
    if(_isGradient)
        updateGradientBrush(maxX - minX + 1, maxY - minY + 1);

    // Draw the polygon
    if(antiAliasingLevel == 0) {
        rasterPolygonAreaNOAA(
            clippedPoints.data(), clippedCounts.data(),
            clippedCounts.size(), clippedPoints.size(),
            _brush.color(), minX, minY, maxX, maxY
        );
    }
    else if(antiAliasingLevel == 1) {
        rasterPolygonAreaFSAA(
            clippedPoints.data(), clippedCounts.data(),
            clippedCounts.size(), clippedPoints.size(),
            _brush.color(), minX, minY, maxX, maxY
        );
    }
    else {
        rasterPolygonAreaSSAA(
            clippedPoints.data(), clippedCounts.data(),
            clippedCounts.size(), clippedPoints.size(),
            _brush.color(), minX, minY, maxX, maxY
        );
    }
}

void Rasterizer2::fillPolygonSeparate(const Point* points, size_t pointCount, Pt::uint8_t antiAliasingLevel)
{
    // Separate the polygons, clip their coordinates, and raster them
    size_t startIndex = 0;

    for(size_t i = 0; i < pointCount; ++i) {
        // Search for the separator point
        if(points[i].x() > COORDINATE_LIMIT && points[i].y() > COORDINATE_LIMIT) {
            // Calculate the number of points for this polygon
            const size_t curPC = i - startIndex;
            // Clip the coordinates
            std::vector<Point> clipped;
            genClippedPolygonPoints(clipped, points + startIndex, curPC);
            // Increment the start index
            startIndex += curPC + 1;
            // Calculate the minimum and maximum coordinate values
            Pt::int32_t minX, minY, maxX, maxY;
            getPolygonRectMinMax(clipped.data(), clipped.size(), minX, minY, maxX, maxY);
            // Update gradient as needed
            if(_isGradient)
                updateGradientBrush(maxX - minX + 1, maxY - minY + 1);
            // Get the number of points for drawing this polygon
            const size_t numPoint[1] = { clipped.size() };
            // Draw the polygon
            if(antiAliasingLevel == 0)
                rasterPolygonAreaNOAA(clipped.data(), numPoint, 1, clipped.size(), _brush.color(), minX, minY, maxX, maxY);
            else if(antiAliasingLevel == 1) // Produces artifacts at almost every corner vertex
                rasterPolygonAreaFSAA(clipped.data(), numPoint, 1, clipped.size(), _brush.color(), minX, minY, maxX, maxY);
            else
                rasterPolygonAreaSSAA(clipped.data(), numPoint, 1, clipped.size(), _brush.color(), minX, minY, maxX, maxY);
        }
    }
}

// ======================================================================================
// ===== Private Member Functions =======================================================
// ======================================================================================

// Uncomment this to use Duff's device
// NOTE: enabling this one seems to only improve performance by 1.5% for SourceOver
//#define USE_DUFFS_DEVICE

// Uncomment this to use putPixels() for drawing solid colors
// NOTE: enabling this one seems to only improve performance by ~23% for SourceOver
#define USE_PUTPIXELS_FOR_SOLID_COLOR

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
void Rasterizer2::rasterPolygonAreaNOAA(const Point* points, const size_t* pointCount, size_t polyCount, size_t totalPointCount, const Color& color, Pt::int32_t minX, Pt::int32_t minY, Pt::int32_t maxX, Pt::int32_t maxY)
{
    // List of nodes that define the horizontal segments
    std::vector<Pt::int32_t> nodeX(totalPointCount * 2, 0);

    //  Loop through the rows of the image
    for(Pt::int32_t pixelY = minY; pixelY <= maxY; ++pixelY) {
        // Base pointer for the polygons
        const Point* curPointBase = points;
        // Build a list of nodes using all the polygons
        Pt::int32_t nodes = 0;
        for(size_t p = 0; p < polyCount; ++p) {
            // Get the current point count
            const size_t curPointCount = pointCount[p];
            // Loop through the points
            Pt::int32_t j = curPointCount - 1;
            for(size_t i = 0; i < curPointCount; ++i) {
                // Get the coordinates
                const Pt::int32_t curXi = (curPointBase + i)->x();
                const Pt::int32_t curYi = (curPointBase + i)->y();
                const Pt::int32_t curXj = (curPointBase + j)->x();
                const Pt::int32_t curYj = (curPointBase + j)->y();
                // Calculate the node's coordinate
                if( ( curYi < pixelY && curYj >= pixelY ) || ( curYj < pixelY && curYi >= pixelY ) ) {
                    // Bail out if we have produced too many nodes
                    if((size_t) nodes >= nodeX.size()) return;
                    // Calculate the node's coordinate
                    const Pt::int32_t deltaYp = pixelY - curYi;
                    const Pt::int32_t deltaYj = curYj  - curYi;
                    const Pt::int32_t deltaXj = curXj  - curXi;
                    const Pt::int32_t interXf = FIXED_POINT_FROM_INT(curXi)
                                              + FIXED_POINT_FROM_INT(deltaYp) / deltaYj * deltaXj;
                    nodeX[nodes++] = FIXED_POINT_TO_INT(interXf + FIXED_POINT_CONSTANT_HALF);
                }
                // Update the searching index
                j = i;
            }
            // Increment the base pointer
            curPointBase += curPointCount;
        }
        // Skip if there is no node
        if(!nodes) continue;
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
#ifdef USE_PUTPIXELS_FOR_SOLID_COLOR
            Pixel pixel(_image->view(), from, pixelY);
            _image->format().setPixels(pixel, _brush.color(), to - from + 1, _compositionMode);
#else
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
#endif
        }
    }
}

// Partially based on http://alienryderflex.com/polygon_fill
// Public-domain code by Darel Rex Finley, 2007
void Rasterizer2::rasterPolygonAreaFSAA(const Point* points, const size_t* pointCount, size_t polyCount, size_t totalPointCount, const Color& color, Pt::int32_t minX, Pt::int32_t minY, Pt::int32_t maxX, Pt::int32_t maxY)
{
    // Calculate the size of the polygon
    Pt::int32_t sizeX = (maxX - minX + 1);
    Pt::int32_t sizeY = (maxY - minY + 1);

    // Prepare a work buffer
    std::vector<Pt::uint8_t> alphas(sizeX, 0);

    // Scale the polygon twice as big
    std::vector<Pt::int32_t> pointX(totalPointCount, 0);
    std::vector<Pt::int32_t> pointY(totalPointCount, 0);

    for(size_t i = 0; i < totalPointCount; ++i) {
        pointX[i] = (points[i].x() - minX) * 2;
        pointY[i] = (points[i].y() - minY) * 2;
    }

    // List of nodes that define the horizontal segments
    std::vector<Pt::int32_t> nodeX0(totalPointCount * 2, 0); // Nodes' X coordinates for row (Y    )
    std::vector<Pt::int32_t> nodeX1(totalPointCount * 2, 0); // Nodes' X coordinates for row (Y + 1)
    //std::vector<Pt::uint8_t> nodeV0(totalPointCount * 2, 0); // Node's validity flag for row (Y    )
    //std::vector<Pt::uint8_t> nodeV1(totalPointCount * 2, 0); // Node's validity flag for row (Y + 1)

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
        // Base pointers for the polygons
        const Pt::int32_t* curPointBaseX = pointX.data();
        const Pt::int32_t* curPointBaseY = pointY.data();
        // Build a list of nodes using all the polygons
        //Pt::int32_t nodes = 0;
        Pt::int32_t nodes0 = 0;
        Pt::int32_t nodes1 = 0;

        for(size_t p = 0; p < polyCount; ++p) {
            // Get the current point count
            const size_t curPointCount = pointCount[p];
            // Loop through the points
            Pt::int32_t j = curPointCount - 1;
            for(size_t i = 0; i < curPointCount; ++i) {
                // Get the coordinates
                const Pt::int32_t curXi = *(curPointBaseX + i);
                const Pt::int32_t curYi = *(curPointBaseY + i);
                const Pt::int32_t curXj = *(curPointBaseX + j);
                const Pt::int32_t curYj = *(curPointBaseY + j);
                // Row (Y)
                if( ( curYi < iterY0 && curYj >= iterY0 ) || ( curYj < iterY0 && curYi >= iterY0 ) ) {
                    // Bail out if we have produced too many nodes
                    if((size_t) nodes0 >= nodeX0.size()) return;
                    // Calculate the nodes' coordinates
                    const Pt::int32_t deltaYp0 = iterY0 - curYi;
                    const Pt::int32_t deltaYj  = curYj  - curYi;
                    const Pt::int32_t deltaXj  = curXj  - curXi;
                    const Pt::int32_t interXf0 = FIXED_POINT_FROM_INT(curXi)
                                               + FIXED_POINT_FROM_INT(deltaYp0) / deltaYj * deltaXj;
                    nodeX0[nodes0++] = FIXED_POINT_TO_INT(interXf0 + FIXED_POINT_CONSTANT_HALF);
                }
                // Row (Y + 1)
                if( ( curYi < iterY1 && curYj >= iterY1 ) || ( curYj < iterY1 && curYi >= iterY1 ) ) {
                    // Bail out if we have produced too many nodes
                    if((size_t) nodes1 >= nodeX1.size()) return;
                    // Calculate the nodes' coordinates
                    const Pt::int32_t deltaYp1 = iterY1 - curYi;
                    const Pt::int32_t deltaYj  = curYj  - curYi;
                    const Pt::int32_t deltaXj  = curXj  - curXi;
                    const Pt::int32_t interXf1 = FIXED_POINT_FROM_INT(curXi)
                                               + FIXED_POINT_FROM_INT(deltaYp1) / deltaYj * deltaXj;
                    nodeX1[nodes1++] = FIXED_POINT_TO_INT(interXf1 + FIXED_POINT_CONSTANT_HALF);
                }
                /*
                // Check the nodes' validity
                nodeV0[nodes] = ( ( curYi < iterY0 && curYj >= iterY0 ) || ( curYj < iterY0 && curYi >= iterY0 ) );
                nodeV1[nodes] = ( ( curYi < iterY1 && curYj >= iterY1 ) || ( curYj < iterY1 && curYi >= iterY1 ) );
                // Calculate the nodes' coordinates
                if(nodeV0[nodes] || nodeV1[nodes]) {
                    // Row (Y)
                    const Pt::int32_t deltaYp0 = iterY0 - curYi;
                    const Pt::int32_t deltaYj  = curYj  - curYi;
                    const Pt::int32_t deltaXj  = curXj  - curXi;
                    const Pt::int32_t interXf0 = FIXED_POINT_FROM_INT(curXi)
                                               + FIXED_POINT_FROM_INT(deltaYp0) / deltaYj * deltaXj;
                    nodeX0[nodes] = FIXED_POINT_TO_INT(interXf0 + FIXED_POINT_CONSTANT_HALF);
                    // Row (Y + 1)
                    const Pt::int32_t deltaYp1 = iterY1 - curYi;
                    const Pt::int32_t interXf1 = FIXED_POINT_FROM_INT(curXi)
                                               + FIXED_POINT_FROM_INT(deltaYp1) / deltaYj * deltaXj;
                    nodeX1[nodes] = FIXED_POINT_TO_INT(interXf1 + FIXED_POINT_CONSTANT_HALF);
                    // Increment the number of nodes
                    ++nodes;
                }
                */
                // Update the searching index
                j = i;
            }
            // Increment the base pointers
            curPointBaseX += curPointCount;
            curPointBaseY += curPointCount;
        }
        // Skip if there is no node
        if(!nodes0 || !nodes1) continue;
        // Sort the nodes using bubble sort
        for(Pt::int32_t i = 0; i < nodes0 - 1;) {
            if(nodeX0[i] > nodeX0[i + 1]) {
                std::swap(nodeX0[i], nodeX0[i + 1]);
               // std::swap(nodeV0[i], nodeV0[i + 1]);
                if(i) --i;
            }
            else {
                ++i;
            }
        }

        for(Pt::int32_t i = 0; i < nodes1 - 1;) {
            if(nodeX1[i] > nodeX1[i + 1]) {
                std::swap(nodeX1[i], nodeX1[i + 1]);
               // std::swap(nodeV1[i], nodeV1[i + 1]);
                if(i) --i;
            }
            else {
                ++i;
            }
        }

        // Reset the alphas
        memset(&alphas[0], 0, alphas.size());

        // Accumulate the alphas of the samples between the node pairs
        //if(nodes0 != nodes1) lprintf("%d %d\n", nodes0, nodes1);

        // 00112233   00112233
        // #     #    0   1
        //  #     #   0    1
        // 01    01    1  0
        // 10    10   1    0

        //*
        for(Pt::int32_t i = 0; i < std::max(nodes0, nodes1); i += 2) {
            if(i < nodes0 && i < nodes1) {
                const Pt::int32_t from0 = std::min(nodeX0[i    ], nodeX1[i    ]);
                const Pt::int32_t from1 = std::max(nodeX0[i    ], nodeX1[i    ]);

                const Pt::int32_t to0   = std::min(nodeX0[i + 1], nodeX1[i + 1]);
                const Pt::int32_t to1   = std::max(nodeX0[i + 1], nodeX1[i + 1]);

                int from;
                int to;

                int from0_cell   = from0 / 2;
                int from1_cell   = from1 / 2;
                int from0_weight = ((from0_cell * 2) < from0) ? 1 : 2;
                int from1_weight = ((from1_cell * 2) < from1) ? 1 : 2;
                if(from0_cell == from1_cell) {
                    alphas[from0_cell] = (from0_weight + from1_weight) * FSAA_MIN_ALPHA;
                    from = from1_cell + 1;
                }
                else {
                    alphas[from0_cell] = (from0_weight               ) * FSAA_MIN_ALPHA;
                    alphas[from1_cell] = (               from1_weight) * FSAA_MIN_ALPHA;
                    for(Pt::int32_t k = from0_cell + 1; k < from1_cell - 1; ++k) {
                        alphas[k] = FSAA_MID_ALPHA;
                    }
                    from = from1_cell + 1;
                }

                int to0_cell   = to0 / 2;
                int to1_cell   = to1 / 2;
                int to0_weight = ((to0_cell * 2) < to0) ? 1 : 2;
                int to1_weight = ((to1_cell * 2) < to1) ? 1 : 2;
                if(to0_cell == to1_cell) {
                    alphas[to0_cell] = (to0_weight + to1_weight) * FSAA_MIN_ALPHA;
                    to = to0_cell - 1;
                }
                else {
                    alphas[to0_cell] = (to0_weight               ) * FSAA_MIN_ALPHA;
                    alphas[to1_cell] = (               to1_weight) * FSAA_MIN_ALPHA;
                    for(Pt::int32_t k = to0_cell + 1; k < to1_cell - 1; ++k) {
                        alphas[k] = FSAA_MID_ALPHA;
                    }
                    to = to0_cell - 1;
                }

                for(Pt::int32_t k = from; k <= to; ++k) {
                    alphas[k] = FSAA_MAX_ALPHA;
                }
            }
            else {
                if(i < nodes0) {
                    const Pt::int32_t from = nodeX0[i    ];
                    const Pt::int32_t to   = nodeX0[i + 1];
                    for(Pt::int32_t k = from; k <= to; ++k) {
                        alphas[k / 2] += FSAA_MIN_ALPHA;
                    }
                }
                if(i < nodes1) {
                    const Pt::int32_t from = nodeX1[i    ];
                    const Pt::int32_t to   = nodeX1[i + 1];
                    for(Pt::int32_t k = from; k <= to; ++k) {
                        alphas[k / 2] += FSAA_MIN_ALPHA;
                    }
                }
            }
        }
        //*/

        /*
        for(Pt::int32_t i = 0; i < nodes0; i += 2) {
            const Pt::int32_t from = nodeX0[i    ];
            const Pt::int32_t to   = nodeX0[i + 1];
            for(Pt::int32_t k = from; k <= to; ++k) {
                alphas[k / 2] += FSAA_MIN_ALPHA;
            }
        }

        for(Pt::int32_t i = 0; i < nodes1; i += 2) {
            const Pt::int32_t from = nodeX1[i    ];
            const Pt::int32_t to   = nodeX1[i + 1];
            for(Pt::int32_t k = from; k <= to; ++k) {
                alphas[k / 2] += FSAA_MIN_ALPHA;
            }
        }
        //*/

        // Fill the pixels between the node pairs
        for(Pt::int32_t i = 0; i < nodes1; i += 2) {
            // Draw pixels that belongs to the left-part of the span to the image
            Pt::int32_t iterL = nodeX1[i] / 2 - 1; // SUPERSAMPLING_SIZE * 2;
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
            Pt::int32_t iterR = nodeX1[i + 1] / 2 + 1; // 2 * 2;
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
                }
                // Draw the span using solid color
                else {
#ifdef USE_PUTPIXELS_FOR_SOLID_COLOR
                    Pixel pixel(_image->view(), minX + iterL, minY + pixelY);
                    _image->format().setPixels(pixel, _brush.color(), iterR - iterL + 1, _compositionMode);
#else
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
#endif
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

// Partially based on http://alienryderflex.com/polygon_fill
// Public-domain code by Darel Rex Finley, 2007
void Rasterizer2::rasterPolygonAreaSSAA(const Point* points, const size_t* pointCount, size_t polyCount, size_t totalPointCount, const Color& color, Pt::int32_t minX, Pt::int32_t minY, Pt::int32_t maxX, Pt::int32_t maxY)
{
    // Calculate the size of the polygon
    Pt::int32_t sizeX = (maxX - minX + 1);
    Pt::int32_t sizeY = (maxY - minY + 1) * SUPERSAMPLING_SIZE;

    // Prepare a work buffer
    std::vector<Pt::uint8_t> alphas(sizeX, 0);

    // Scale the polygon to match the super sampling size and translate it to (0, 0)
    std::vector<Pt::int32_t> pointX(totalPointCount, 0);
    std::vector<Pt::int32_t> pointY(totalPointCount, 0);

    for(size_t i = 0; i < totalPointCount; ++i) {
        pointX[i] = (points[i].x() - minX) * SUPERSAMPLING_SIZE;
        pointY[i] = (points[i].y() - minY) * SUPERSAMPLING_SIZE;
    }

    // List of nodes that define the horizontal segments
    std::vector<Pt::int32_t> nodeX(totalPointCount * 2, 0);

    // A helper macro to scale the alpha
    #define SSAA_SCALE_ALPHA(A) ( Pt::uint16_t(A) * 17 / SUPERSAMPLING_SIZE / SUPERSAMPLING_SIZE )

    // The minimum and maximum values for alpha
    #define SSAA_MIN_ALPHA 15
    #define SSAA_MAX_ALPHA (SSAA_MIN_ALPHA * SUPERSAMPLING_SIZE * SUPERSAMPLING_SIZE)

    //  Loop through the rows of the image
    for(Pt::int32_t pixelY = 0; pixelY < sizeY; ++pixelY) {
        // Base pointers for the polygons
        const Pt::int32_t* curPointBaseX = pointX.data();
        const Pt::int32_t* curPointBaseY = pointY.data();
        // Build a list of nodes using all the polygons
        Pt::int32_t nodes = 0;
        for(size_t p = 0; p < polyCount; ++p) {
            // Get the current point count
            const size_t curPointCount = pointCount[p];
            // Loop through the points
            Pt::int32_t j = curPointCount - 1;
            for(size_t i = 0; i < curPointCount; ++i) {
                // Get the coordinates
                const Pt::int32_t curXi = *(curPointBaseX + i);
                const Pt::int32_t curYi = *(curPointBaseY + i);
                const Pt::int32_t curXj = *(curPointBaseX + j);
                const Pt::int32_t curYj = *(curPointBaseY + j);
                if( ( curYi < pixelY && curYj >= pixelY ) || ( curYj < pixelY && curYi >= pixelY ) ) {
                    // Bail out if we have produced too many nodes
                    if((size_t) nodes >= nodeX.size()) return;
                    // Calculate the node's coordinate
                    const Pt::int32_t deltaYp = pixelY - curYi;
                    const Pt::int32_t deltaYj = curYj  - curYi;
                    const Pt::int32_t deltaXj = curXj  - curXi;
                    // Calculate the node's coordinate
                    const Pt::int32_t interXf = FIXED_POINT_FROM_INT(curXi)
                                              + FIXED_POINT_FROM_INT(deltaYp) / deltaYj * deltaXj;
                    nodeX[nodes++] = FIXED_POINT_TO_INT(interXf + FIXED_POINT_CONSTANT_HALF);
                }
                // Update the searching index
                j = i;
            }
            // Increment the base pointers
            curPointBaseX += curPointCount;
            curPointBaseY += curPointCount;
        }
        // Skip if there is no node
        if(!nodes) continue;
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
        //lprintf("%03d: ", pixelY / SUPERSAMPLING_SIZE); for(size_t k = 0; k < alphas.size(); ++k) lprintf("%d", alphas[k] / 15); lprintf("\n");
        // Fill the pixels between the node pairs
        for(Pt::int32_t i = 0; i < nodes; i += 2) {
            // Draw pixels that belongs to the left-part of the span to the image
            Pt::int32_t iterL = nodeX[i] / SUPERSAMPLING_SIZE - 1; // SUPERSAMPLING_SIZE * 2;
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
            Pt::int32_t iterR = nodeX[i + 1] / SUPERSAMPLING_SIZE + 1; // SUPERSAMPLING_SIZE * 2;
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
#ifdef USE_PUTPIXELS_FOR_SOLID_COLOR
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


#undef USE_DUFFS_DEVICE
#undef USE_PUTPIXELS_FOR_SOLID_COLOR


} // namespace
} // namespace
