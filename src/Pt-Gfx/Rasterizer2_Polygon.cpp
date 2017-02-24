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

void Rasterizer2::strokeOnePixelSolidPolygon(const Point* points, size_t pointCount)
{
    rasterOnePixelSolidPolygonOutline(points, pointCount, _pen.color());
}

void Rasterizer2::fillPolygon(const Point* points, size_t pointCount)
{
    // Minimum and maximum coordinate values for all the polygons
    Pt::int32_t minX =  MAXIMUM_COORD;
    Pt::int32_t minY =  MAXIMUM_COORD;
    Pt::int32_t maxX = -MAXIMUM_COORD;
    Pt::int32_t maxY = -MAXIMUM_COORD;

    // Separate the polygons and clip their coordinates
    std::vector<Point > clippedPoints;
    std::vector<size_t> clippedCounts;
    size_t              startIndex = 0;
    for(size_t i = 0; i <= pointCount; ++i) {
        // Search for the end and/or separator points
        if( i == pointCount || (points[i].x() > MAXIMUM_COORD && points[i].y() > MAXIMUM_COORD) ) {
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

    // Update the gradient as needed
    if(_isGradient)
        updateGradientBrush(maxX - minX + 1, maxY - minY + 1);


#if 0

    // Draw the polygon
    /*
    rasterPolygonAreaNoAA(
        clippedPoints.data(), clippedCounts.data(),
        clippedCounts.size(), clippedPoints.size(),
        _brush.color(), minX, minY, maxX, maxY
    );
    */

    if(_aaMode == AntiAliasingMode::None) return;

    // Raster the anti-aliased outline
    const Point* curPointBase = clippedPoints.data();
    for(size_t p = 0; p < clippedCounts.size(); ++p) {
        fillOnePixelSolidPolygonOutlineOutsideAAOnly(curPointBase, clippedCounts[p], minX, minY);
        curPointBase += clippedCounts[p];
    }

#else

    // Draw the polygon
    if(_aaMode == AntiAliasingMode::None) {
        rasterPolygonAreaNoAA(
            clippedPoints.data(), clippedCounts.data(),
            clippedCounts.size(), clippedPoints.size(),
            _brush.color(), minX, minY, maxX, maxY
        );
    }
    else if(_aaMode == AntiAliasingMode::Fastest) {
        rasterPolygonAreaFSAA2x2(
            clippedPoints.data(), clippedCounts.data(),
            clippedCounts.size(), clippedPoints.size(),
            _brush.color(), minX, minY, maxX, maxY
        );
    }
    else if(_aaMode == AntiAliasingMode::Medium) {
        rasterPolygonAreaFSAAGen<4>(
            clippedPoints.data(), clippedCounts.data(),
            clippedCounts.size(), clippedPoints.size(),
            _brush.color(), minX, minY, maxX, maxY
        );
    }
    else { // _aaMode == AntiAliasingMode::Maximum
        rasterPolygonAreaFSAAGen<8>(
            clippedPoints.data(), clippedCounts.data(),
            clippedCounts.size(), clippedPoints.size(),
            _brush.color(), minX, minY, maxX, maxY
        );
    }

#endif
}

void Rasterizer2::fillPolygonSeparate(const Point* points, size_t pointCount)
{
    // Separate the polygons, clip their coordinates, and raster them
    size_t startIndex = 0;

    for(size_t i = 0; i < pointCount; ++i) {
        // Search for the end and/or separator points
        if( i == pointCount || (points[i].x() > MAXIMUM_COORD && points[i].y() > MAXIMUM_COORD) ) {
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
            // Update the gradient as needed
            if(_isGradient)
                updateGradientBrush(maxX - minX + 1, maxY - minY + 1);
            // Get the number of points for drawing this polygon
            const size_t numPoint[1] = { clipped.size() };
            // Draw the polygon
            if(_aaMode == AntiAliasingMode::None)
                rasterPolygonAreaNoAA(clipped.data(), numPoint, 1, clipped.size(), _brush.color(), minX, minY, maxX, maxY);
            else if(_aaMode == AntiAliasingMode::Fastest)
                rasterPolygonAreaFSAA2x2(clipped.data(), numPoint, 1, clipped.size(), _brush.color(), minX, minY, maxX, maxY);
            else if(_aaMode == AntiAliasingMode::Medium)
                rasterPolygonAreaFSAAGen<4>(clipped.data(), numPoint, 1, clipped.size(), _brush.color(), minX, minY, maxX, maxY);
            else // _aaMode == AntiAliasingMode::Maximum
                rasterPolygonAreaFSAAGen<8>(clipped.data(), numPoint, 1, clipped.size(), _brush.color(), minX, minY, maxX, maxY);
        }
    }
}

// ======================================================================================
// ===== Private Member Functions =======================================================
// ======================================================================================

void Rasterizer2::rasterOnePixelSolidPolygonOutline(const Point* points, size_t pointCount, const Color& color)
{
    // Mask
    Rasterizer2::DrawLineMask mask_zero = Rasterizer2::NullLineMask;
    Rasterizer2::DrawLineMask mask_nnp1 = Rasterizer2::NullLineMask;

    // From point N to point (N + 1), successively
    const size_t pc1 = pointCount - 1;

    for(size_t i = 0; i < pc1; ++i) {
        if(points[i].y() == points[i + 1].y())
            rasterOnePixelHLineSegment(points[i].x(), points[i + 1].x(), points[i].y(), color, &mask_nnp1);
        else if(points[i].x() == points[i + 1].x())
            rasterOnePixelVLineSegment(points[i].x(), points[i].y(), points[i + 1].y(), color, &mask_nnp1);
        else {
            const bool xline = ( abs(points[i + 1].x() - points[i].x()) ==
                                 abs(points[i + 1].y() - points[i].y()) );
            if(xline || _aaMode == AntiAliasingMode::None)
                rasterOnePixelGLineSegmentNoAA(points[i].x(), points[i].y(), points[i + 1].x(), points[i + 1].y(), color, &mask_nnp1);
            else
                rasterOnePixelGLineSegmentXWAA(points[i].x(), points[i].y(), points[i + 1].x(), points[i + 1].y(), color, &mask_nnp1);
        }
        if(!i) memcpy(&mask_zero, &mask_nnp1, sizeof(mask_zero));
    }

    mask_zero[2] = mask_zero[0];
    mask_zero[3] = mask_zero[1];
    mask_zero[0] = mask_nnp1[2];
    mask_zero[1] = mask_nnp1[3];

    // From the last point to the first point
    if(points[pc1].y() == points[0].y())
        rasterOnePixelHLineSegment(points[pc1].x(), points[0].x(), points[pc1].y(), color, &mask_zero);
    else if(points[pc1].x() == points[0].x())
         rasterOnePixelVLineSegment(points[pc1].x(), points[pc1].y(), points[0].y(), color, &mask_zero);
    else {
        const bool xline = ( abs(points[pc1].x() - points[0].x()) ==
                             abs(points[pc1].y() - points[0].y()) );
        if(xline || _aaMode == AntiAliasingMode::None)
            rasterOnePixelGLineSegmentNoAA(points[pc1].x(), points[pc1].y(), points[0].x(), points[0].y(), color, &mask_zero);
        else
            rasterOnePixelGLineSegmentXWAA(points[pc1].x(), points[pc1].y(), points[0].x(), points[0].y(), color, &mask_zero);
    }
}

void Rasterizer2::fillOnePixelSolidPolygonOutlineOutsideAAOnly(const Point* points, size_t pointCount, Pt::int32_t minX, Pt::int32_t minY)
{
    // Mask
    Rasterizer2::DrawLineMask mask_zero = Rasterizer2::NullLineMask;
    Rasterizer2::DrawLineMask mask_nnp1 = Rasterizer2::NullLineMask;

    // From point N to point (N + 1), successively
    const size_t pc1 = pointCount - 1;

    for(size_t i = 0; i < pc1; ++i) {
        fillOnePixelGLineSegmentXWAAOutsideOnly(points[i].x(), points[i].y(), points[i + 1].x(), points[i + 1].y(), minX, minY, &mask_nnp1);
        if(!i) memcpy(&mask_zero, &mask_nnp1, sizeof(mask_zero));
    }

    mask_zero[2] = mask_zero[0];
    mask_zero[3] = mask_zero[1];
    mask_zero[0] = mask_nnp1[2];
    mask_zero[1] = mask_nnp1[3];

    // From the last point to the first point
    fillOnePixelGLineSegmentXWAAOutsideOnly(points[pc1].x(), points[pc1].y(), points[0].x(), points[0].y(), minX, minY, &mask_zero);
}

// Inspired by http://alienryderflex.com/polygon_fill
// Public-domain code by Darel Rex Finley, 2007
void Rasterizer2::rasterPolygonAreaNoAA(const Point* points, const size_t* pointCount, size_t polyCount, size_t totalPointCount, const Color& color, Pt::int32_t minX, Pt::int32_t minY, Pt::int32_t maxX, Pt::int32_t maxY)
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
        // Sort the nodes
        bubbleSortAscending(nodeX, nodes);
        // Fill the pixels between the node pairs
        for(Pt::int32_t i = 0; i < nodes; i += 2) {
            const Pt::int32_t from = nodeX[i    ];
            const Pt::int32_t to   = nodeX[i + 1];
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
    #define FSAA2X2_MAX_ALPHA        (FSAA2X2_MIN_ALPHA * FSAA2X2_SUPERSAMPLE_SIZE * FSAA2X2_SUPERSAMPLE_SIZE)
    #define FSAA2X2_MID_ALPHA        (FSAA2X2_MIN_ALPHA * FSAA2X2_SUPERSAMPLE_SIZE)

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
        pointY[i] = (points[i].y() - minY) * FSAA2X2_SUPERSAMPLE_SIZE * 2;
    }

    // List of nodes that define the horizontal spans
    std::vector<Pt::int32_t> nodeX0(totalPointCount * 2, 0); // Row (Y    )
    std::vector<Pt::int32_t> nodeX1(totalPointCount * 2, 0); // Row (Y + 1)

    //  Loop through the rows of the image
    for(Pt::int32_t pixelY = 0; pixelY <= sizeY; ++pixelY) {
        // We examine two rows at a time
        const Pt::int32_t iterY0 = pixelY * FSAA2X2_SUPERSAMPLE_SIZE * 2 - 1;
        const Pt::int32_t iterY1 = pixelY * FSAA2X2_SUPERSAMPLE_SIZE * 2 + 1;
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
        // Sort the nodes
        bubbleSortAscending(nodeX0, nodes0);
        bubbleSortAscending(nodeX1, nodes1);
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
                // If the span is short, accumulate alphas for the whole span directly
                // in order to avoid some alpha-related artifacts
                if( (to0 - from0 <= FSAA2X2_SUPERSAMPLE_SIZE) || (to1 - from1 <= FSAA2X2_SUPERSAMPLE_SIZE) ) {
                    for(Pt::int32_t k = from0; k <= to0; ++k) {
                        alphas[k / FSAA2X2_SUPERSAMPLE_SIZE] += FSAA2X2_MIN_ALPHA;
                    }
                    for(Pt::int32_t k = from1; k <= to1; ++k) {
                        alphas[k / FSAA2X2_SUPERSAMPLE_SIZE] += FSAA2X2_MIN_ALPHA;
                    }
                    continue;
                }
                // Calculate alphas for the left side of the span
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
                // Calculate alphas for the right side of the span
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
                // Assign alphas for the middle side of the span
                const Pt::int32_t len = (to0_cell - 1) - (from1_cell + 1) + 1;
                if(len > 0) memset(&alphas[from1_cell + 1], FSAA2X2_MAX_ALPHA, len);
            }
        }
        // Accumulate the alphas of the samples between the node pairs
        // --- The number of nodes within the two rows are not equal ---
        else {
            for(Pt::int32_t i = 0; i < nodes0; i += 2) {
                //const Pt::int32_t from  = nodeX0[i    ];
                //const Pt::int32_t to    = nodeX0[i + 1];
                //for(Pt::int32_t k = from; k <= to; ++k) {
                //    alphas[k / FSAA2X2_SUPERSAMPLE_SIZE] += FSAA2X2_MIN_ALPHA;
                //}
                // Calculate the cells and coverage areas
                const Pt::int32_t from      = nodeX0[i    ];
                const Pt::int32_t to        = nodeX0[i + 1];
                const Pt::int32_t from_cell = from / FSAA2X2_SUPERSAMPLE_SIZE;
                const Pt::int32_t to_cell   = to   / FSAA2X2_SUPERSAMPLE_SIZE;
                const Pt::int32_t from_area = ( (from_cell * FSAA2X2_SUPERSAMPLE_SIZE) < from ) ? FSAA2X2_MIN_ALPHA : FSAA2X2_MID_ALPHA;
                const Pt::int32_t to_area   = ( (to_cell   * FSAA2X2_SUPERSAMPLE_SIZE) < to   ) ? FSAA2X2_MID_ALPHA : FSAA2X2_MIN_ALPHA;
                // If the span is short, accumulate alphas for the whole span directly
                // in order to avoid some alpha-related artifacts
                if( to - from <= FSAA2X2_SUPERSAMPLE_SIZE ) {
                    for(Pt::int32_t k = from; k <= to; ++k) {
                        alphas[k / FSAA2X2_SUPERSAMPLE_SIZE] += FSAA2X2_MIN_ALPHA;
                    }
                    continue;
                }
                // Accumulate alphas for the left side and right side of the span
                alphas[from_cell] += from_area;
                alphas[to_cell  ] += to_area;
                // Assign alphas for the middle side of the span
                for(Pt::int32_t k = (from_cell + 1); k <= (to_cell - 1); ++k) {
                    alphas[k] += FSAA2X2_MID_ALPHA;
                }
            }
            for(Pt::int32_t i = 0; i < nodes1; i += 2) {
                //const Pt::int32_t from  = nodeX1[i    ];
                //const Pt::int32_t to    = nodeX1[i + 1];
                //for(Pt::int32_t k = from; k <= to; ++k) {
                //    alphas[k / FSAA2X2_SUPERSAMPLE_SIZE] += FSAA2X2_MIN_ALPHA;
                //}
                // Calculate the cells and coverage areas
                const Pt::int32_t from      = nodeX1[i    ];
                const Pt::int32_t to        = nodeX1[i + 1];
                const Pt::int32_t from_cell = from / FSAA2X2_SUPERSAMPLE_SIZE;
                const Pt::int32_t to_cell   = to   / FSAA2X2_SUPERSAMPLE_SIZE;
                const Pt::int32_t from_area = ( (from_cell * FSAA2X2_SUPERSAMPLE_SIZE) < from ) ? FSAA2X2_MIN_ALPHA : FSAA2X2_MID_ALPHA;
                const Pt::int32_t to_area   = ( (to_cell   * FSAA2X2_SUPERSAMPLE_SIZE) < to   ) ? FSAA2X2_MID_ALPHA : FSAA2X2_MIN_ALPHA;
                // If the span is short, accumulate alphas for the whole span directly
                // in order to avoid some alpha-related artifacts
                if( to - from <= FSAA2X2_SUPERSAMPLE_SIZE ) {
                    for(Pt::int32_t k = from; k <= to; ++k) {
                        alphas[k / FSAA2X2_SUPERSAMPLE_SIZE] += FSAA2X2_MIN_ALPHA;
                    }
                    continue;
                }
                // Accumulate alphas for the left side and right side of the span
                alphas[from_cell] += from_area;
                alphas[to_cell  ] += to_area;
                // Assign alphas for the middle side of the span
                for(Pt::int32_t k = (from_cell + 1); k <= (to_cell - 1); ++k) {
                    alphas[k] += FSAA2X2_MID_ALPHA;
                }
            }
        }
        //lprintf("%03d: ", pixelY); for(size_t k = 0; k < alphas.size(); ++k) lprintf("%02d ", alphas[k] / FSAA2X2_MIN_ALPHA); lprintf("\n");
        // Fill the pixels between the node pairs
        for(Pt::int32_t i = 0; i < nodes0; i += 2) {
            const Pt::int32_t iterL = nodeX0[i    ] / FSAA2X2_SUPERSAMPLE_SIZE - 1;
            const Pt::int32_t iterR = nodeX0[i + 1] / FSAA2X2_SUPERSAMPLE_SIZE + 1;
            rasterScanline<FSAA2X2_SUPERSAMPLE_SIZE, FSAA2X2_MIN_ALPHA, FSAA2X2_MUL_ALPHA>(
                iterL, iterR, pixelY - 1, minX, minY, sizeX, color, alphas
            );
        }
    }

    // Undefine the macros
    #undef FSAA2X2_SUPERSAMPLE_SIZE
    #undef FSAA2X2_MUL_ALPHA
    #undef FSAA2X2_MIN_ALPHA
    #undef FSAA2X2_MAX_ALPHA
    #undef FSAA2X2_MID_ALPHA
}


} // namespace
} // namespace
