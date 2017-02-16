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

#include "Rasterizer2_Common.h"

namespace Pt {
namespace Gfx {


// ======================================================================================
// ===== Public Member Functions ========================================================
// ======================================================================================

void Rasterizer2::strokePolygon(const Point* points, size_t pointCount)
{
    switch( _pen.style() ) {
        case Pen::Solid:
            // Draw a simple, one-pixel line
            if( _pen.size() == 1 && pointCount == 2 ) {
                rasterOnePixelLine(points[0], points[1]);
            }
            break;

        case Pen::Dash:
        case Pen::DoubleDash:
            break;
    }
}

void Rasterizer2::fillPolygon(const Point* points, size_t pointCount)
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
    for(size_t i = 0; i <= pointCount; ++i) {
        // Search for the end and/or separator points
        if( i == pointCount || (points[i].x() > COORDINATE_LIMIT && points[i].y() > COORDINATE_LIMIT) ) {
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
    if(_aaLevel == 0) {
        rasterPolygonAreaJaggies(
            clippedPoints.data(), clippedCounts.data(),
            clippedCounts.size(), clippedPoints.size(),
            _brush.color(), minX, minY, maxX, maxY
        );
    }
    else if(_aaLevel == 1) {
        rasterPolygonAreaFSAA2x2(
            clippedPoints.data(), clippedCounts.data(),
            clippedCounts.size(), clippedPoints.size(),
            _brush.color(), minX, minY, maxX, maxY
        );
    }
    else {
        rasterPolygonAreaSSAA4x4(
            clippedPoints.data(), clippedCounts.data(),
            clippedCounts.size(), clippedPoints.size(),
            _brush.color(), minX, minY, maxX, maxY
        );
    }
}

void Rasterizer2::fillPolygonSeparate(const Point* points, size_t pointCount)
{
    // Separate the polygons, clip their coordinates, and raster them
    size_t startIndex = 0;

    for(size_t i = 0; i < pointCount; ++i) {
        // Search for the end and/or separator points
        if( i == pointCount || (points[i].x() > COORDINATE_LIMIT && points[i].y() > COORDINATE_LIMIT) ) {
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
            if(_aaLevel == 0)
                rasterPolygonAreaJaggies(clipped.data(), numPoint, 1, clipped.size(), _brush.color(), minX, minY, maxX, maxY);
            else if(_aaLevel == 1) // Produces artifacts at almost every corner vertex
                rasterPolygonAreaFSAA2x2(clipped.data(), numPoint, 1, clipped.size(), _brush.color(), minX, minY, maxX, maxY);
            else
                rasterPolygonAreaSSAA4x4(clipped.data(), numPoint, 1, clipped.size(), _brush.color(), minX, minY, maxX, maxY);
        }
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
        else {
            if(_aaLevel)
                rasterOnePixelGLineSegmentXWAA(lineX[i], lineY[i], lineX[i + 1], lineY[i + 1], color, true);
            else
                rasterOnePixelGLineSegmentNOAA(lineX[i], lineY[i], lineX[i + 1], lineY[i + 1], color, true);
        }
    }

    if(lineY[0] == lineY[pc1])
        rasterOnePixelHLineSegment(lineX[0], lineX[pc1], lineY[0], color, true);
    else if(lineX[0] == lineX[pc1])
        rasterOnePixelVLineSegment(lineX[0], lineY[0], lineY[pc1], color, true);
    else {
        if(_aaLevel)
            rasterOnePixelGLineSegmentXWAA(lineX[0], lineY[0], lineX[pc1], lineY[pc1], color, true);
        else
            rasterOnePixelGLineSegmentNOAA(lineX[0], lineY[0], lineX[pc1], lineY[pc1], color, true);
    }
}

// Inspired by http://alienryderflex.com/polygon_fill
// Public-domain code by Darel Rex Finley, 2007
void Rasterizer2::rasterPolygonAreaJaggies(const Point* points, const size_t* pointCount, size_t polyCount, size_t totalPointCount, const Color& color, Pt::int32_t minX, Pt::int32_t minY, Pt::int32_t maxX, Pt::int32_t maxY)
{
    // List of nodes that define the horizontal spans
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
                if( ( pixelY >= curYi && pixelY < curYj ) || ( pixelY >= curYj && pixelY < curYi ) ) {
                    // Bail out if we have produced too many nodes
                    if((size_t) nodes >= nodeX.size()) return;
                    // Calculate the node's coordinate
                    const Pt::int32_t deltaYp = pixelY - curYi;
                    const Pt::int32_t deltaYj = curYj  - curYi;
                    const Pt::int32_t deltaXj = curXj  - curXi;
                    const Pt::int32_t interXf = FIXED_POINT_FROM_INT(curXi)
                                              + ( (FIXED_POINT_FROM_INT(deltaYp) + FIXED_POINT_CONSTANT_HALF) /
                                                  deltaYj * deltaXj
                                                );
                    nodeX[nodes++] = FIXED_POINT_TO_INT(interXf);
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
            const Pt::int32_t from = nodeX[i    ];
            const Pt::int32_t to   = nodeX[i + 1];
            //lprintf("%03d %03d @ %03d\n", from - minX, to - minX, pixelY - minY);
            rasterScanline(from - minX, to - minX, pixelY - minY, minX, minY, color);
        }
    }
}

// Inspired by http://alienryderflex.com/polygon_fill
// Public-domain code by Darel Rex Finley, 2007
void Rasterizer2::rasterPolygonAreaFSAA2x2(const Point* points, const size_t* pointCount, size_t polyCount, size_t totalPointCount, const Color& color, Pt::int32_t minX, Pt::int32_t minY, Pt::int32_t maxX, Pt::int32_t maxY)
{
    // Internal macros
    #define FSAA2X2_SUPERSAMPLE_SIZE 2
    #define FSAA2X2_MUL_ALPHA        255
    #define FSAA2X2_MIN_ALPHA        1
    #define FSAA2X2_MID_ALPHA        (FSAA2X2_MIN_ALPHA * FSAA2X2_SUPERSAMPLE_SIZE                           )
    #define FSAA2X2_MAX_ALPHA        (FSAA2X2_MIN_ALPHA * FSAA2X2_SUPERSAMPLE_SIZE * FSAA2X2_SUPERSAMPLE_SIZE)

    // Calculate the size of the polygon
    Pt::int32_t sizeX = (maxX - minX + 1);
    Pt::int32_t sizeY = (maxY - minY + 1);

    // Prepare a work buffer
    std::vector<Pt::uint8_t> alphas(sizeX, 0);

    // Scale the polygon to be twice as large and translate its origin to (0, 0)
    std::vector<Pt::int32_t> pointX(totalPointCount, 0);
    std::vector<Pt::int32_t> pointY(totalPointCount, 0);

    for(size_t i = 0; i < totalPointCount; ++i) {
        pointX[i] = (points[i].x() - minX) * FSAA2X2_SUPERSAMPLE_SIZE;
        pointY[i] = (points[i].y() - minY) * FSAA2X2_SUPERSAMPLE_SIZE;
    }

    // List of nodes that define the horizontal spans
    std::vector<Pt::int32_t> nodeX0(totalPointCount * 2, 0); // Row (Y    )
    std::vector<Pt::int32_t> nodeX1(totalPointCount * 2, 0); // Row (Y + 1)

    //  Loop through the rows of the image
    for(Pt::int32_t pixelY = 0; pixelY < sizeY; ++pixelY) {
        // We examine two rows at a time
        const Pt::int32_t iterY0 = pixelY * FSAA2X2_SUPERSAMPLE_SIZE;
        const Pt::int32_t iterY1 = iterY0 + 1;
        // Base pointers for the polygons
        const Pt::int32_t* curPointBaseX = pointX.data();
        const Pt::int32_t* curPointBaseY = pointY.data();
        // Build a list of nodes using all the polygons
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
                if( ( iterY0 >= curYi && iterY0 < curYj ) || ( iterY0 >= curYj && iterY0 < curYi ) ) {
                    // Bail out if we have produced too many nodes
                    if((size_t) nodes0 >= nodeX0.size()) return;
                    // Calculate the nodes' coordinates
                    const Pt::int32_t deltaYp0 = iterY0 - curYi;
                    const Pt::int32_t deltaYj  = curYj  - curYi;
                    const Pt::int32_t deltaXj  = curXj  - curXi;
                    const Pt::int32_t interXf0 = FIXED_POINT_FROM_INT(curXi)
                                               + ( (FIXED_POINT_FROM_INT(deltaYp0) + FIXED_POINT_CONSTANT_QUARTER) /
                                                   deltaYj * deltaXj
                                                 );
                    nodeX0[nodes0++] = FIXED_POINT_TO_INT(interXf0);
                }
                // Row (Y + 1)
                if( ( iterY1 >= curYi && iterY1 < curYj ) || ( iterY1 >= curYj && iterY1 < curYi ) ) {
                    // Bail out if we have produced too many nodes
                    if((size_t) nodes1 >= nodeX1.size()) return;
                    // Calculate the nodes' coordinates
                    const Pt::int32_t deltaYp1 = iterY1 - curYi;
                    const Pt::int32_t deltaYj  = curYj  - curYi;
                    const Pt::int32_t deltaXj  = curXj  - curXi;
                    const Pt::int32_t interXf1 = FIXED_POINT_FROM_INT(curXi)
                                               + ( (FIXED_POINT_FROM_INT(deltaYp1) + FIXED_POINT_CONSTANT_QUARTER) /
                                                   deltaYj * deltaXj
                                                 );
                    nodeX1[nodes1++] = FIXED_POINT_TO_INT(interXf1);
                }
                // Update the searching index
                j = i;
            }
            // Increment the base pointers
            curPointBaseX += curPointCount;
            curPointBaseY += curPointCount;
        }
        // Skip if there is no node
        if(!nodes0 && !nodes1) continue;
        // Sort the nodes using bubble sort
        for(Pt::int32_t i = 0; i < nodes0 - 1;) {
            if(nodeX0[i] > nodeX0[i + 1]) {
                std::swap(nodeX0[i], nodeX0[i + 1]);
                if(i) --i;
            }
            else {
                ++i;
            }
        }
        for(Pt::int32_t i = 0; i < nodes1 - 1;) {
            if(nodeX1[i] > nodeX1[i + 1]) {
                std::swap(nodeX1[i], nodeX1[i + 1]);
                if(i) --i;
            }
            else {
                ++i;
            }
        }
        // Reset the alphas
        memset(&alphas[0], 0, alphas.size());
        // Accumulate the alphas of the samples between the node pairs
        // --- The number of nodes within the two rows are equal ---
        if(nodes0 == nodes1) {
            for(Pt::int32_t i = 0; i < nodes0; i += 2) {
                // Calculate the cells and coverage areas
                const Pt::int32_t from0      = std::min(nodeX0[i    ], nodeX1[i    ]);
                const Pt::int32_t from1      = std::max(nodeX0[i    ], nodeX1[i    ]);
                const Pt::int32_t to0        = std::min(nodeX0[i + 1], nodeX1[i + 1]);
                const Pt::int32_t to1        = std::max(nodeX0[i + 1], nodeX1[i + 1]);
                const Pt::int32_t from0_cell = from0 / FSAA2X2_SUPERSAMPLE_SIZE;
                const Pt::int32_t from1_cell = from1 / FSAA2X2_SUPERSAMPLE_SIZE;
                const Pt::int32_t to0_cell   = to0   / FSAA2X2_SUPERSAMPLE_SIZE;
                const Pt::int32_t to1_cell   = to1   / FSAA2X2_SUPERSAMPLE_SIZE;
                const Pt::int32_t from0_area = ( (from0_cell * FSAA2X2_SUPERSAMPLE_SIZE) < from0 ) ? FSAA2X2_MIN_ALPHA : FSAA2X2_MID_ALPHA;
                const Pt::int32_t from1_area = ( (from1_cell * FSAA2X2_SUPERSAMPLE_SIZE) < from1 ) ? FSAA2X2_MIN_ALPHA : FSAA2X2_MID_ALPHA;
                const Pt::int32_t to0_area   = ( (to0_cell   * FSAA2X2_SUPERSAMPLE_SIZE) < to0   ) ? FSAA2X2_MID_ALPHA : FSAA2X2_MIN_ALPHA;
                const Pt::int32_t to1_area   = ( (to1_cell   * FSAA2X2_SUPERSAMPLE_SIZE) < to1   ) ? FSAA2X2_MID_ALPHA : FSAA2X2_MIN_ALPHA;
                // Calculate alphas for the left side
                if(from0_cell == from1_cell) {
                    alphas[from0_cell] = from0_area + from1_area;
                }
                else {
                    alphas[from0_cell] = from0_area;
                    alphas[from1_cell] = from1_area + FSAA2X2_MID_ALPHA;
                    for(Pt::int32_t k = (from0_cell + 1); k <= (from1_cell - 1); ++k) {
                        alphas[k] = FSAA2X2_MID_ALPHA;
                    }
                }
                // Calculate alphas for the right side
                if(to0_cell == to1_cell) {
                    alphas[to0_cell] = to0_area + to1_area;
                }
                else {
                    alphas[to0_cell] = to0_area + FSAA2X2_MID_ALPHA;
                    alphas[to1_cell] = to1_area;
                    for(Pt::int32_t k = (to0_cell + 1); k <= (to1_cell - 1); ++k) {
                        alphas[k] = FSAA2X2_MID_ALPHA;
                    }
                }
                // Assign alphas for the middle side
                const Pt::int32_t len = (to0_cell - 1) - (from1_cell + 1) + 1;
                if(len > 0) memset(&alphas[from1_cell + 1], FSAA2X2_MAX_ALPHA, len);
            }
        }
        // Accumulate the alphas of the samples between the node pairs
        // --- The number of nodes within the two rows are not equal ---
        else {
            for(Pt::int32_t i = 0; i < nodes0; i += 2) {
                const Pt::int32_t from = nodeX0[i    ];
                const Pt::int32_t to   = nodeX0[i + 1];
                for(Pt::int32_t k = from; k <= to; ++k) {
                    alphas[k / FSAA2X2_SUPERSAMPLE_SIZE] += FSAA2X2_MIN_ALPHA;
                }
            }
            for(Pt::int32_t i = 0; i < nodes1; i += 2) {
                const Pt::int32_t from = nodeX1[i    ];
                const Pt::int32_t to   = nodeX1[i + 1];
                for(Pt::int32_t k = from; k <= to; ++k) {
                    alphas[k / FSAA2X2_SUPERSAMPLE_SIZE] += FSAA2X2_MIN_ALPHA;
                }
            }
        }
        //lprintf("%03d: ", pixelY); for(size_t k = 0; k < alphas.size(); ++k) lprintf("%02d ", alphas[k] / FSAA2X2_MIN_ALPHA); lprintf("\n");
        // Fill the pixels between the node pairs
        for(Pt::int32_t i = 0; i < nodes0; i += 2) {
            const Pt::int32_t iterL = nodeX0[i    ] / FSAA2X2_SUPERSAMPLE_SIZE - 1;
            const Pt::int32_t iterR = nodeX0[i + 1] / FSAA2X2_SUPERSAMPLE_SIZE + 1;
            rasterScanline<FSAA2X2_SUPERSAMPLE_SIZE, FSAA2X2_MIN_ALPHA, FSAA2X2_MUL_ALPHA>(
                iterL, iterR, pixelY, minX, minY, sizeX, color, alphas
            );
        }
    }

    // Undefine the macros
    #undef FSAA2X2_SUPERSAMPLE_SIZE
    #undef FSAA2X2_MUL_ALPHA
    #undef FSAA2X2_MIN_ALPHA
    #undef FSAA2X2_MID_ALPHA
    #undef FSAA2X2_MAX_ALPHA
}

// Inspired by http://alienryderflex.com/polygon_fill
// Public-domain code by Darel Rex Finley, 2007
void Rasterizer2::rasterPolygonAreaFSAA4x4(const Point* points, const size_t* pointCount, size_t polyCount, size_t totalPointCount, const Color& color, Pt::int32_t minX, Pt::int32_t minY, Pt::int32_t maxX, Pt::int32_t maxY)
{
    // Internal macros
    #define FSAA4X4_SUPERSAMPLE_SIZE 4
    #define FSAA4X4_MUL_ALPHA        255
    #define FSAA4X4_MIN_ALPHA        1
    #define FSAA4X4_MID_ALPHA        (FSAA4X4_MIN_ALPHA * FSAA4X4_SUPERSAMPLE_SIZE                           )
    #define FSAA4X4_MAX_ALPHA        (FSAA4X4_MIN_ALPHA * FSAA4X4_SUPERSAMPLE_SIZE * FSAA4X4_SUPERSAMPLE_SIZE)

    // Calculate the size of the polygon
    Pt::int32_t sizeX = (maxX - minX + 1);
    Pt::int32_t sizeY = (maxY - minY + 1);

    // Prepare a work buffer
    std::vector<Pt::uint8_t> alphas(sizeX, 0);

    // Scale the polygon to be twice as large and translate its origin to (0, 0)
    std::vector<Pt::int32_t> pointX(totalPointCount, 0);
    std::vector<Pt::int32_t> pointY(totalPointCount, 0);

    for(size_t i = 0; i < totalPointCount; ++i) {
        pointX[i] = (points[i].x() - minX) * FSAA4X4_SUPERSAMPLE_SIZE;
        pointY[i] = (points[i].y() - minY) * FSAA4X4_SUPERSAMPLE_SIZE;
    }

    // List of nodes that define the horizontal spans
    // Row (Y) ... Row (Y + FSAA4X4_SUPERSAMPLE_SIZE - 1)
    std::vector< std::vector<Pt::int32_t> > nodeX(FSAA4X4_SUPERSAMPLE_SIZE);
    for(Pt::int32_t s = 0; s < FSAA4X4_SUPERSAMPLE_SIZE; ++s) {
        nodeX[s].resize(totalPointCount * 2);
    }

    //  Loop through the rows of the image
    for(Pt::int32_t pixelY = 0; pixelY < sizeY; ++pixelY) {
        // We examine multiple rows at a time
        Pt::int32_t iterY[FSAA4X4_SUPERSAMPLE_SIZE];
        iterY[0] = pixelY * FSAA4X4_SUPERSAMPLE_SIZE;
        for(Pt::int32_t s = 1; s < FSAA4X4_SUPERSAMPLE_SIZE; ++s) {
            iterY[s] = iterY[0] + s;
        }
        // Base pointers for the polygons
        const Pt::int32_t* curPointBaseX = pointX.data();
        const Pt::int32_t* curPointBaseY = pointY.data();
        // Build a list of nodes using all the polygons
        Pt::int32_t nodes[FSAA4X4_SUPERSAMPLE_SIZE] = { 0 };
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
                // Row (Y) ... Row (Y + FSAA4X4_SUPERSAMPLE_SIZE - 1)
                for(Pt::int32_t s = 0; s < FSAA4X4_SUPERSAMPLE_SIZE; ++s) {
                    if( ( iterY[s] >= curYi && iterY[s] < curYj ) || ( iterY[s] >= curYj && iterY[s] < curYi ) ) {
                        // Bail out if we have produced too many nodes
                        if((size_t) nodes[s] >= nodeX[s].size()) return;
                        // Calculate the nodes' coordinates
                        const Pt::int32_t deltaYp = iterY[s] - curYi;
                        const Pt::int32_t deltaYj = curYj    - curYi;
                        const Pt::int32_t deltaXj = curXj    - curXi;
                        const Pt::int32_t interXf = FIXED_POINT_FROM_INT(curXi)
                                                  + ( (FIXED_POINT_FROM_INT(deltaYp) + FIXED_POINT_CONSTANT_QUARTER) /
                                                      deltaYj * deltaXj
                                                    );
                        nodeX[s][nodes[s]++] = FIXED_POINT_TO_INT(interXf);
                    }
                }
                // Update the searching index
                j = i;
            }
            // Increment the base pointers
            curPointBaseX += curPointCount;
            curPointBaseY += curPointCount;
        }
        // Skip if there is no node
        bool gotNodes = false;
        for(Pt::int32_t s = 0; s < FSAA4X4_SUPERSAMPLE_SIZE; ++s) {
            if(nodes[s]) {
                gotNodes = true;
                break;
            }
        }
        if(!gotNodes) continue;
        // Sort the nodes using bubble sort
        for(Pt::int32_t s = 0; s < FSAA4X4_SUPERSAMPLE_SIZE; ++s) {
            for(Pt::int32_t i = 0; i < nodes[s] - 1;) {
                if(nodeX[s][i] > nodeX[s][i + 1]) {
                    std::swap(nodeX[s][i], nodeX[s][i + 1]);
                    if(i) --i;
                }
                else {
                    ++i;
                }
            }
        }
        // Reset the alphas
        memset(&alphas[0], 0, alphas.size());
        // Accumulate the alphas of the samples between the node pairs
        // --- Check if all the rows have the same number of nodes ---
        const Pt::int32_t nodes0            = nodes[0];
              bool        hasSameNumOfNodes = true;
        for(Pt::int32_t s = 1; s < FSAA4X4_SUPERSAMPLE_SIZE; ++s) {
            if(nodes[s] == nodes0) continue;
            hasSameNumOfNodes = false;
            break;
        }
        // --- The number of nodes within all the rows are equal ---
        if(true && hasSameNumOfNodes) {
            for(Pt::int32_t i = 0; i < nodes0; i += 2) {
                // Calculate the cells
                Pt::int32_t from     [FSAA4X4_SUPERSAMPLE_SIZE];
                Pt::int32_t to       [FSAA4X4_SUPERSAMPLE_SIZE];
                Pt::int32_t from_cell[FSAA4X4_SUPERSAMPLE_SIZE];
                Pt::int32_t to_cell  [FSAA4X4_SUPERSAMPLE_SIZE];
                for(Pt::int32_t s = 0; s < FSAA4X4_SUPERSAMPLE_SIZE; ++s) {
                    from     [s] = nodeX[s][i    ];
                    to       [s] = nodeX[s][i + 1];
                    from_cell[s] = from[s] / FSAA4X4_SUPERSAMPLE_SIZE;
                    to_cell  [s] = to  [s] / FSAA4X4_SUPERSAMPLE_SIZE;
                }
                // Sort the cells
                for(Pt::int32_t s = 0; s < FSAA4X4_SUPERSAMPLE_SIZE - 1;) {
                    if(from_cell[s] > from_cell[s + 1]) {
                        std::swap(from     [s], from     [s + 1]);
                        std::swap(from_cell[s], from_cell[s + 1]);
                        if(s) --s;
                    }
                    else {
                        ++s;
                    }
                }
                for(Pt::int32_t s = 0; s < FSAA4X4_SUPERSAMPLE_SIZE - 1;) {
                    if(to_cell[s] > to_cell[s + 1]) {
                        std::swap(to     [s], to     [s + 1]);
                        std::swap(to_cell[s], to_cell[s + 1]);
                        if(s) --s;
                    }
                    else {
                        ++s;
                    }
                }
                // Accumulate the alphas for the left side
                // --- Accumulate from individual cells ----
                bool sameL = true;
                for(Pt::int32_t s = 0; s < FSAA4X4_SUPERSAMPLE_SIZE; ++s) {
                    if(from_cell[s] == to_cell[s]) continue;
                    sameL = false;
                    break;
                }
                if(!sameL) {
                    for(Pt::int32_t s = 0; s < FSAA4X4_SUPERSAMPLE_SIZE; ++s) {
                        alphas[from_cell[s]] += FSAA4X4_SUPERSAMPLE_SIZE - (from[s] - from_cell[s] * FSAA4X4_SUPERSAMPLE_SIZE);
                    }
                }
                // Accumulate the alphas for the left side
                // --- Accumulate from cells on the left ----
                Pt::int32_t prevL = from_cell[0];
                for(Pt::int32_t s = 1; s < FSAA4X4_SUPERSAMPLE_SIZE; ++s) {
                    // We only want to process unique cells
                    if(from_cell[s] == prevL) continue;
                    prevL = from_cell[s];
                    // Accumulate the alphas
                    Pt::int32_t acc = 0;
                    for(Pt::int32_t c = 1; c < s; ++c) {
                        if(from_cell[c] < from_cell[s]) acc += FSAA4X4_MID_ALPHA;
                    }
                    //alphas[from_cell[s]] += acc;
                    //if(alphas[from_cell[s]] > FSAA4X4_MAX_ALPHA) alphas[from_cell[s]] = FSAA4X4_MAX_ALPHA;
                }
                // Accumulate the alphas for the left side
                // --- Accumulate from the intermediate cells ----
                for(Pt::int32_t s = 0; s < (FSAA4X4_SUPERSAMPLE_SIZE - 1); ++s) {
                    for(Pt::int32_t k = (from_cell[s] + 1); k <= (from_cell[s + 1] - 1); ++k) {
                        Pt::int32_t acc = 0;
                        for(Pt::int32_t c = 1; c < (s + 1); ++c) {
                            if(from_cell[c] < from_cell[s + 1]) acc += FSAA4X4_MID_ALPHA;
                        }
                        //alphas[k] += acc;
                        //if(alphas[k] > FSAA4X4_MAX_ALPHA) alphas[k] = FSAA4X4_MAX_ALPHA;
                    }
                }
                // Accumulate the alphas for the right side
                // --- Accumulate from individual cells ----
                if(!sameL) {
                    for(Pt::int32_t s = 0; s < FSAA4X4_SUPERSAMPLE_SIZE; ++s) {
                        alphas[to_cell[s]] += (to[s] - to_cell[s] * FSAA4X4_SUPERSAMPLE_SIZE) + 1;
                        //if(pixelY == 0) lprintf("from=%03d to=%03d k=%03d k4=%03d alpha=%02d\n", from[s], to[s], 99, to_cell[s], alphas[to_cell[s]]);
                    }
                }
                // Accumulate the alphas for the right side
                // --- Accumulate from cells on the right ----
                Pt::int32_t prevR = to_cell[FSAA4X4_SUPERSAMPLE_SIZE - 1];
                for(Pt::int32_t s = (FSAA4X4_SUPERSAMPLE_SIZE - 2); s >= 0; --s) {
                    // We only want to process unique cells
                    if(to_cell[s] == prevR) continue;
                    prevR = to_cell[s];
                    // Accumulate the alphas
                    Pt::int32_t acc = 0;
                    for(Pt::int32_t c = (s + 1); c < FSAA4X4_SUPERSAMPLE_SIZE; ++c) {
                        if(to_cell[c] > to_cell[s]) acc += FSAA4X4_MID_ALPHA;
                    }
                    //alphas[to_cell[s]] += acc;
                    //if(alphas[to_cell[s]] > FSAA4X4_MAX_ALPHA) alphas[to_cell[s]] = FSAA4X4_MAX_ALPHA;
                }
                // Accumulate the alphas for the right side
                // --- Accumulate from the intermediate cells ----
                for(Pt::int32_t s = 0; s < (FSAA4X4_SUPERSAMPLE_SIZE - 1); ++s) {
                    for(Pt::int32_t k = (to_cell[s] + 1); k <= (to_cell[s + 1] - 1); ++k) {
                        Pt::int32_t acc = 0;
                        for(Pt::int32_t c = (s + 1); c < FSAA4X4_SUPERSAMPLE_SIZE; ++c) {
                            if(to_cell[c] > to_cell[s]) acc += FSAA4X4_MID_ALPHA;
                        }
                        //alphas[k] += acc;
                        //if(alphas[k] > FSAA4X4_MAX_ALPHA) alphas[k] = FSAA4X4_MAX_ALPHA;
                    }
                }
                // Assign the alphas for the middle side
                const Pt::int32_t len = (to_cell[0] - 1) - (from_cell[FSAA4X4_SUPERSAMPLE_SIZE - 1] + 1) + 1;
                if(len > 0) memset(&alphas[from_cell[FSAA4X4_SUPERSAMPLE_SIZE - 1] + 1], FSAA4X4_MAX_ALPHA, len);
            }
        }
        // Accumulate the alphas of the samples between the node pairs
        // --- The number of nodes within all or some of the rows are not equal ---
        else {
            for(Pt::int32_t s = 0; s < FSAA4X4_SUPERSAMPLE_SIZE; ++s) {
                for(Pt::int32_t i = 0; i < nodes[s]; i += 2) {
                    const Pt::int32_t from = nodeX[s][i    ];
                    const Pt::int32_t to   = nodeX[s][i + 1];
                    for(Pt::int32_t k = from; k <= to; ++k) {
                        alphas[k / FSAA4X4_SUPERSAMPLE_SIZE] += FSAA4X4_MIN_ALPHA;
                        //if(pixelY == 0) lprintf("from=%03d to=%03d k=%03d k4=%03d alpha=%02d\n", from, to, k, k / FSAA4X4_SUPERSAMPLE_SIZE, alphas[k / FSAA4X4_SUPERSAMPLE_SIZE]);
                    }
                }
            }
        }
        lprintf("%03d: ", pixelY); for(size_t k = 0; k < alphas.size(); ++k) lprintf("%02d ", alphas[k] / FSAA4X4_MIN_ALPHA); lprintf("\n");
        // Fill the pixels between the node pairs
        for(Pt::int32_t i = 0; i < nodes[0]; i += 2) {
            const Pt::int32_t iterL = nodeX[0][i    ] / FSAA4X4_SUPERSAMPLE_SIZE - 1;
            const Pt::int32_t iterR = nodeX[0][i + 1] / FSAA4X4_SUPERSAMPLE_SIZE + 1;
            rasterScanline<FSAA4X4_SUPERSAMPLE_SIZE, FSAA4X4_MIN_ALPHA, FSAA4X4_MUL_ALPHA>(
                iterL, iterR, pixelY, minX, minY, sizeX, color, alphas
            );
        }
    }

    // Undefine the macros
    #undef FSAA4X4_SUPERSAMPLE_SIZE
    #undef FSAA4X4_MUL_ALPHA
    #undef FSAA4X4_MIN_ALPHA
    #undef FSAA4X4_MID_ALPHA
    #undef FSAA4X4_MAX_ALPHA
}

// Inspired by http://alienryderflex.com/polygon_fill
// Public-domain code by Darel Rex Finley, 2007
void Rasterizer2::rasterPolygonAreaSSAA4x4(const Point* points, const size_t* pointCount, size_t polyCount, size_t totalPointCount, const Color& color, Pt::int32_t minX, Pt::int32_t minY, Pt::int32_t maxX, Pt::int32_t maxY)
{
    // Internal macros
    #define SSAA4X4_SUPERSAMPLE_SIZE 4
    #define SSAA4X4_MUL_ALPHA        17
    #define SSAA4X4_MIN_ALPHA        15

    // Calculate the size of the polygon
    Pt::int32_t sizeX = (maxX - minX + 1);
    Pt::int32_t sizeY = (maxY - minY + 1) * SSAA4X4_SUPERSAMPLE_SIZE;

    // Prepare a work buffer
    std::vector<Pt::uint8_t> alphas(sizeX, 0);

    // Scale the polygon to be four times as large and translate its origin to (0, 0)
    std::vector<Pt::int32_t> pointX(totalPointCount, 0);
    std::vector<Pt::int32_t> pointY(totalPointCount, 0);

    for(size_t i = 0; i < totalPointCount; ++i) {
        pointX[i] = (points[i].x() - minX) * SSAA4X4_SUPERSAMPLE_SIZE;
        pointY[i] = (points[i].y() - minY) * SSAA4X4_SUPERSAMPLE_SIZE;
    }

    // List of nodes that define the horizontal spans
    std::vector<Pt::int32_t> nodeX(totalPointCount * 2, 0);

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
                if( ( pixelY >= curYi && pixelY < curYj ) || ( pixelY >= curYj && pixelY < curYi ) ) {
                    // Bail out if we have produced too many nodes
                    if((size_t) nodes >= nodeX.size()) return;
                    // Calculate the node's coordinate
                    const Pt::int32_t deltaYp = pixelY - curYi;
                    const Pt::int32_t deltaYj = curYj  - curYi;
                    const Pt::int32_t deltaXj = curXj  - curXi;
                    // Calculate the node's coordinate
                    const Pt::int32_t interXf = FIXED_POINT_FROM_INT(curXi)
                                              + ( (FIXED_POINT_FROM_INT(deltaYp) + FIXED_POINT_CONSTANT_QUARTER) /
                                                  deltaYj * deltaXj
                                                );
                    nodeX[nodes++] = FIXED_POINT_TO_INT(interXf);
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
            // Use Duff's device for:
            //     for(Pt::int32_t k = from; k <= to; ++k) {
            //         alphas[k / SSAA4X4_SUPERSAMPLE_SIZE] += SSAA4X4_MIN_ALPHA;
            //     }
            register Pt::uint8_t* dst = &alphas[0];
            register Pt::int32_t  cnt = to - from + 1;
            register Pt::int32_t  n   = (cnt + 7) / 8;
            register Pt::int32_t  k   = from;
            switch(cnt % 8) {
                    case 0 : do { dst[k / SSAA4X4_SUPERSAMPLE_SIZE] += SSAA4X4_MIN_ALPHA; ++k;
                    case 7 :      dst[k / SSAA4X4_SUPERSAMPLE_SIZE] += SSAA4X4_MIN_ALPHA; ++k;
                    case 6 :      dst[k / SSAA4X4_SUPERSAMPLE_SIZE] += SSAA4X4_MIN_ALPHA; ++k;
                    case 5 :      dst[k / SSAA4X4_SUPERSAMPLE_SIZE] += SSAA4X4_MIN_ALPHA; ++k;
                    case 4 :      dst[k / SSAA4X4_SUPERSAMPLE_SIZE] += SSAA4X4_MIN_ALPHA; ++k;
                    case 3 :      dst[k / SSAA4X4_SUPERSAMPLE_SIZE] += SSAA4X4_MIN_ALPHA; ++k;
                    case 2 :      dst[k / SSAA4X4_SUPERSAMPLE_SIZE] += SSAA4X4_MIN_ALPHA; ++k;
                    case 1 :      dst[k / SSAA4X4_SUPERSAMPLE_SIZE] += SSAA4X4_MIN_ALPHA; ++k;
                             } while (--n > 0);
            }
        }
        // Simply skip the next steps if we have not got all the needed samples
        if( ((pixelY + 1) % SSAA4X4_SUPERSAMPLE_SIZE) ) continue;
        //lprintf("%03d: ", pixelY / SSAA4X4_SUPERSAMPLE_SIZE); for(size_t k = 0; k < alphas.size(); ++k) lprintf("%02d ", alphas[k] / SSAA4X4_MIN_ALPHA); lprintf("\n");
        // Fill the pixels between the node pairs
        for(Pt::int32_t i = 0; i < nodes; i += 2) {
            const Pt::int32_t iterL = nodeX[i    ] / SSAA4X4_SUPERSAMPLE_SIZE - 1;
            const Pt::int32_t iterR = nodeX[i + 1] / SSAA4X4_SUPERSAMPLE_SIZE + 1;
            rasterScanline<SSAA4X4_SUPERSAMPLE_SIZE, SSAA4X4_MIN_ALPHA, SSAA4X4_MUL_ALPHA>(
                iterL, iterR, pixelY / SSAA4X4_SUPERSAMPLE_SIZE, minX, minY, sizeX, color, alphas
            );
        }
        // Clear the work buffer
        memset(&alphas[0], 0, alphas.size());
    }

    // Undefine the macros
    #undef SSAA4X4_SUPERSAMPLE_SIZE
    #undef SSAA4X4_MUL_ALPHA
    #undef SSAA4X4_MIN_ALPHA
}


} // namespace
} // namespace
