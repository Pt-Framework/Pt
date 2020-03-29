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

#include <iomanip>

#include "Rasterizer2.h"
#include "ClipShape.h"

namespace {

template<typename T>
void bubbleSortAscending(T& basket, Pt::int32_t size)
{
    for(Pt::int32_t i = 0; i < size - 1;) {
        if(basket[i] > basket[i + 1]) {
            std::swap(basket[i], basket[i + 1]);
            if(i) --i;
        }
        else {
            ++i;
        }
    }
}

} // namespace

namespace Pt {

namespace Gfx {

// ======================================================================================
// ===== Private Member Functions =======================================================
// ======================================================================================

/*

// REVIEW: Seems nothing actually uses these functions anymore?

// Inspired by: Efficient Polygon Fill Algorithm With C Code Sample
//              http://alienryderflex.com/polygon_fill
//              Public-domain code by Darel Rex Finley, 2007
void Rasterizer2::rasterPolygonAreaNoAA(const Point* points, const size_t* pointCount,
                                        size_t polyCount, size_t totalPointCount,
                                        const Color& color,
                                        Pt::int32_t minX, Pt::int32_t minY,
                                        Pt::int32_t maxX, Pt::int32_t maxY)
{
    // List of nodes that define the horizontal spans
    std::vector<Pt::int32_t> nodeX(totalPointCount * 2, 0);

    // Loop through the rows of the image
    for(Pt::int32_t pixelY = minY; pixelY <= maxY; ++pixelY)
    {
        // Pixel-by-pixel clipping
        if(pixelY < _currentClip.top   ()) continue;
        if(pixelY > _currentClip.bottom()) continue;

        // Base pointer for the polygons
        const Point* curPointBase = points;

        // Build a list of nodes using all the polygons
        Pt::int32_t nodes = 0;
        for(size_t p = 0; p < polyCount; ++p)
        {
            // Get the current point count
            const size_t curPointCount = pointCount[p];

            // Loop through the points
            Pt::int32_t j = curPointCount - 1;
            for(size_t i = 0; i < curPointCount; ++i)
            {
                // Get the coordinates
                const Pt::int32_t curXi = (curPointBase + i)->x();
                const Pt::int32_t curYi = (curPointBase + i)->y();
                const Pt::int32_t curXj = (curPointBase + j)->x();
                const Pt::int32_t curYj = (curPointBase + j)->y();

                // Calculate the node's coordinate
                if( ( pixelY >= curYi && pixelY < curYj ) || ( pixelY >= curYj && pixelY < curYi ) )
                {
                    // Bail out if we have produced too many nodes
                    if((size_t) nodes >= nodeX.size())
                        return;

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
        if( ! nodes )
            continue;

        // Sort the nodes
        bubbleSortAscending(nodeX, nodes);

        // Fill the pixels between the node pairs
        for(Pt::int32_t i = 0; i < nodes; i += 2)
        {
            // Get the coordinate
            Pt::int32_t from = nodeX[i    ];
            Pt::int32_t to   = nodeX[i + 1];

            // Pixel-by-pixel clipping
            if(from < _currentClip.left ()) from = _currentClip.left ();
            if(to   > _currentClip.right()) to   = _currentClip.right();

            if(to < from) continue;

            // Draw the scanline
            rasterScanline(from - minX, to - minX, pixelY - minY, minX, minY, color);
        }
    }
}


// Inspired by: Efficient Polygon Fill Algorithm With C Code Sample
//              http://alienryderflex.com/polygon_fill
//              Public-domain code by Darel Rex Finley, 2007
void Rasterizer2::rasterPolygonAreaXWAA(const PointF* points, const size_t* pointCount,
                                        size_t polyCount, size_t totalPointCount,
                                        const Color& color,
                                        float minX_, float minY_,
                                        float maxX_, float maxY_)
{
    // List of nodes that define the horizontal spans
    std::vector<float> nodeX(totalPointCount * 2, 0);

    // Calculate the scaled Y coordinates
    const Pt::int32_t minX = Pt::lround(floor(minX_));
  //const Pt::int32_t maxX = Pt::lround(floor(maxX_));
    const Pt::int32_t minY = Pt::lround(floor(minY_));
    const Pt::int32_t maxY = Pt::lround(floor(maxY_));

    // List of polygon scanlines
    PolygonScanlines scanlines;

    if(_compositionMode != CompositionMode::SourceCopy)
        scanlines.resize( (maxY - minY) + 1 + 4 );

    // Loop through the rows of the image
    for(Pt::int32_t pixelY = minY; pixelY <= maxY; ++pixelY)
    {
        // Pixel-by-pixel clipping
        if(pixelY < _currentClip.top   ()) continue;
        if(pixelY > _currentClip.bottom()) continue;

        // Base pointer for the polygons
        const PointF* curPointBase = points;

        // Build a list of nodes using all the polygons
        Pt::int32_t nodes = 0;
        for(size_t p = 0; p < polyCount; ++p)
        {
            // Get the current point count
            const size_t curPointCount = pointCount[p];

            // Loop through the points
            Pt::int32_t j = curPointCount - 1;
            for(size_t i = 0; i < curPointCount; ++i)
            {
                // Get the coordinates
                const float curXi = (curPointBase + i)->x();
                const float curYi = (curPointBase + i)->y();
                const float curXj = (curPointBase + j)->x();
                const float curYj = (curPointBase + j)->y();

                // Calculate the node's coordinate
                if( ( pixelY >= curYi && pixelY < curYj ) || ( pixelY >= curYj && pixelY < curYi ) )
                {
                    // Bail out if we have produced too many nodes
                    if((size_t) nodes >= nodeX.size()) return;

                    // Calculate the node's coordinate
                    const float deltaYp = pixelY - curYi;
                    const float deltaYj = curYj  - curYi;
                    const float deltaXj = curXj  - curXi;
                    const float interXf = curXi  + (deltaYp) / deltaYj * deltaXj;
                    nodeX[nodes++] = interXf;
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
            // Calculate the coordinate
#if 1
            Pt::int32_t from = Pt::lround( ceil (nodeX[i    ]) );
            Pt::int32_t to   = Pt::lround( floor(nodeX[i + 1]) );
#else
            Pt::int32_t from = Pt::lround( nodeX[i    ] );
            Pt::int32_t to   = Pt::lround( nodeX[i + 1] );
#endif

            // Pixel-by-pixel clipping
            if(from < _currentClip.left ()) from = _currentClip.left ();
            if(to   > _currentClip.right()) to   = _currentClip.right();

            if(to < from) continue;
            //if( (to - from) < 1 ) continue;

            // Store the scanline coordinate as needed
            if(_compositionMode != CompositionMode::SourceCopy)
            {
                scanlines[pixelY - minY + 1].push_back(ScanlineElement16(from, to));
            }

            // Draw the scanline
            rasterScanline(from - minX, to - minX, pixelY - minY, minX, minY, color);
        }
    }

    // Raster the anti-aliased outline
    const PointF* curPointBase = points;
    for(size_t p = 0; p < polyCount; ++p)
    {
        // Mask
        DrawLineMask mask_zero;
        DrawLineMask mask_nnp1;
        memcpy(mask_zero, Rasterizer2::NullLineMask, sizeof(DrawLineMask));
        memcpy(mask_nnp1, Rasterizer2::NullLineMask, sizeof(DrawLineMask));

        // From point N to point (N + 1), successively
        const size_t pc1 = pointCount[p] - 1;
        for(size_t i = 0; i < pc1; ++i) {
            rasterPolygonBorderXWAA_F(
                curPointBase[i    ].x(), curPointBase[i    ].y(),
                curPointBase[i + 1].x(), curPointBase[i + 1].y(),
                color, minX, minY_ - 1, scanlines, mask_nnp1
            );
            if(!i) memcpy(&mask_zero, &mask_nnp1, sizeof(mask_zero));
        }

        // From the last point to the first point
        mask_zero[2] = mask_zero[0];
        mask_zero[3] = mask_zero[1];
        mask_zero[0] = mask_nnp1[2];
        mask_zero[1] = mask_nnp1[3];
        rasterPolygonBorderXWAA_F(
             curPointBase[pc1].x(), curPointBase[pc1].y(),
             curPointBase[0  ].x(), curPointBase[0  ].y(),
             color, minX, minY_ - 1, scanlines, mask_zero
        );

        // Increment the base pointer
        curPointBase += pointCount[p];
    }
}
*/


//
// Inspired by: Efficient Polygon Fill Algorithm With C Code Sample
//              http://alienryderflex.com/polygon_fill
//              Public-domain code by Darel Rex Finley, 2007
//
void Rasterizer2::rasterPolygonNoAA(const PointF* points, std::size_t pointCount,
                                     const Color& color,
                                     Pt::int32_t minX, Pt::int32_t minY,
                                     Pt::int32_t maxX, Pt::int32_t maxY)
{
    // List of nodes that define the horizontal spans
    std::vector<float> nodeX(pointCount * 2, 0);

    // Loop through the rows of the image
    for(Pt::int32_t y = minY; y <= maxY; ++y)
    {
        // Pixel-by-pixel clipping
        if(y < _currentClip.top   ()) continue;
        if(y > _currentClip.bottom()) continue;

        // Build a list of nodes using all the polygons
        std::size_t nodes = 0;

        // Loop through the points
        Pt::int32_t j = pointCount - 1;

        for(size_t i = 0; i < pointCount; ++i)
        {
            // Get the coordinates
            const float curXi = points[i].x();
            const float curYi = points[i].y();
            const float curXj = points[j].x();
            const float curYj = points[j].y();

            // Calculate the node's coordinate
            if( ( y >= curYi && y < curYj ) || ( y >= curYj && y < curYi ) )
            {
                // Bail out if we have produced too many nodes
                if( nodes >= nodeX.size() )
                    return;

                // Calculate the node's coordinate
                const float deltaYp = y - curYi;
                const float deltaYj = curYj - curYi;
                const float deltaXj = curXj - curXi;
                const float interXf = curXi + deltaYp / deltaYj * deltaXj;

                nodeX[nodes++] = interXf;
            }

            // Update the searching index
            j = i;
        }

        // Skip if there is no node
        if( ! nodes )
            continue;

        // Sort the nodes
        bubbleSortAscending(nodeX, nodes);

        // Fill the pixels between the node pairs
        for(std::size_t i = 0; i < nodes; i += 2)
        {
            // Calculate the coordinate
            Pt::int32_t from = Pt::lround( ceil(nodeX[i]) );
            Pt::int32_t to   = Pt::lround( floor(nodeX[i + 1] - 0.5f) );

            // Pixel-by-pixel clipping
            if(from < _currentClip.left ()) from = _currentClip.left ();
            if(to   > _currentClip.right()) to   = _currentClip.right();

            if(to < from) continue;

            // Draw the scanline
            rasterScanline(from - minX, to - minX, y - minY, minX, minY, color);
        }
    }
}

/*
// THE ORIGINAL ONE USING FIXED POINTS
void Rasterizer2::rasterPolygonNoAA(const PointF* points, std::size_t pointCount,
                                     const Color& color,
                                     Pt::int32_t minX, Pt::int32_t minY,
                                     Pt::int32_t maxX, Pt::int32_t maxY)
{
    if( pointCount < 3 )
        return;

#if 0
    if(IP2_DEBUG::DUMP_POLYGON_COORDINATES && !this->isAntiAliasing()) {
        std::cerr << "### 2 ###\n";
        for (size_t i = 0; i < pointCount; ++i) {
            std::cerr << std::fixed << std::setw(5) << std::setprecision(1)
                      << points[i].x() << ", " << points[i].y() << std::endl;
        }
    }
#endif

    std::vector<Point> polygon;
    polygon.reserve( pointCount );

    for(size_t i = 0; i < pointCount; ++i)
    {
        const PointF& pf = points[i];
        const Pt::int32_t x = Pt::lround( pf.x() );
        const Pt::int32_t y = Pt::lround( pf.y() );

        polygon.push_back( Point(x, y) );
        //polygon.push_back( Point(pf.x(), pf.y()) );
    }

    // List of nodes that define the horizontal spans
    std::vector<Pt::int32_t> nodeX(pointCount * 2, 0);

    // Loop through the rows of the image
    //for(Pt::int32_t y = minY; y <= maxY; ++y)
    for(Pt::int32_t y = minY; y < maxY; ++y)
    {
        // Pixel-by-pixel clipping
        if(y < _currentClip.top   ()) continue;
        if(y > _currentClip.bottom()) continue;

        // Build a list of nodes using all the polygons
        std::size_t nodes = 0;

        // loop through the points
        Pt::int32_t j = pointCount - 1;

        for(size_t i = 0; i < pointCount; ++i)
        {
            // Get the coordinates
            const Pt::int32_t curXi = polygon[i].x();
            const Pt::int32_t curYi = polygon[i].y();
            const Pt::int32_t curXj = polygon[j].x();
            const Pt::int32_t curYj = polygon[j].y();

            // Calculate the node's coordinate
            if( ( y >= curYi && y < curYj ) || ( y >= curYj && y < curYi ) )
            {
                // Bail out if we have produced too many nodes
                if(nodes >= nodeX.size())
                    return;

                // Calculate the node's coordinate
                const Pt::int32_t deltaYp = y - curYi;
                const Pt::int32_t deltaYj = curYj  - curYi;
                const Pt::int32_t deltaXj = curXj  - curXi;
                //const Pt::int32_t interXf = FIXED_POINT_FROM_INT(curXi) +
                //                            ( (FIXED_POINT_FROM_INT(deltaYp) + FIXED_POINT_CONSTANT_HALF ) /
                //                              deltaYj * deltaXj );
                const Pt::int32_t interXf = FIXED_POINT_FROM_INT(curXi) +
                                            ( (FIXED_POINT_FROM_INT(deltaYp) + FIXED_POINT_CONSTANT_HALF ) *
                                              deltaXj / deltaYj );
                nodeX[nodes++] = FIXED_POINT_TO_INT(interXf);
            }


            j = i;
        }


        // Skip if there is no node
        if( ! nodes )
            continue;

        // Sort the nodes
        bubbleSortAscending(nodeX, nodes);

        // Fill the pixels between the node pairs
        for(std::size_t i = 0; i < nodes; i += 2)
        {
            // Get the coordinate
            Pt::int32_t from = nodeX[i    ];
            Pt::int32_t to   = nodeX[i + 1];

            // Pixel-by-pixel clipping
            if(from < _currentClip.left ()) from = _currentClip.left ();
            if(to   > _currentClip.right()) to   = _currentClip.right();

            if(to < from) continue;

            // Draw the scanline
#if 1
            if(IP2_DEBUG::DUMP_SCANLINE_COORDINATES) {
                std::cerr << std::fixed << std::setw(5) << std::setprecision(1)
                      << "RP NOAA " << (minX + (from - minX)) << ", " << (minY + y - minY) << " LEN " << ((to - minX) - (from - minX) + 1) << std::endl;
            }
#endif
            rasterScanline(from - minX, to - minX, y - minY, minX, minY, color);
        }
    }
}
*/


//
// Inspired by: Efficient Polygon Fill Algorithm With C Code Sample
//              http://alienryderflex.com/polygon_fill
//              Public-domain code by Darel Rex Finley, 2007
//
void Rasterizer2::rasterPolygonsNoAA(const std::vector<Polygon>& polygons,
                                     const Color& color,
                                     Pt::int32_t minX, Pt::int32_t minY,
                                     Pt::int32_t maxX, Pt::int32_t maxY)
{
    std::size_t totalPointCount = 0;

    for(std::vector<Polygon>::const_iterator it = polygons.begin();
        it != polygons.end(); ++it)
    {
        totalPointCount += it->size();
    }

    // List of nodes that define the horizontal spans
    std::vector<float> nodeX(totalPointCount * 2, 0);

    // Loop through the rows of the image
    for(Pt::int32_t y = minY; y <= maxY; ++y)
    {
        // Pixel-by-pixel clipping
        if(y < _currentClip.top   ()) continue;
        if(y > _currentClip.bottom()) continue;

        // Build a list of nodes using all the polygons
        std::size_t nodes = 0;

        for(size_t p = 0; p < polygons.size(); ++p)
        {
            const Polygon* polygon = &polygons[p];

            if( polygon->size() < 2 )
                continue;

            // Loop through the points
            Pt::int32_t j = polygon->size() - 1;

            for(size_t i = 0; i < polygon->size(); ++i)
            {
                // Get the coordinates
                const float curXi = polygon->at(i).x();
                const float curYi = polygon->at(i).y();
                const float curXj = polygon->at(j).x();
                const float curYj = polygon->at(j).y();

                // Calculate the node's coordinate
                if( ( y >= curYi && y < curYj ) || ( y >= curYj && y < curYi ) )
                {
                    // Bail out if we have produced too many nodes
                    if((size_t) nodes >= nodeX.size())
                        return;

                    // Calculate the node's coordinate
                    const float deltaYp = y     - curYi;
                    const float deltaYj = curYj - curYi;
                    const float deltaXj = curXj - curXi;
                    const float interXf = curXi + deltaYp / deltaYj * deltaXj;

                    nodeX[nodes++] = interXf;
                }

                // Update the searching index
                j = i;
            }
        }

        // Skip if there is no node
        if( ! nodes)
            continue;

        // Sort the nodes
        bubbleSortAscending(nodeX, nodes);

        // Fill the pixels between the node pairs
        for(std::size_t i = 0; i < nodes; i += 2)
        {
            // Calculate the coordinate
            Pt::int32_t from = Pt::lround( ceil(nodeX[i]) );
            Pt::int32_t to   = Pt::lround( floor(nodeX[i + 1] - 0.5f) );

            // Pixel-by-pixel clipping
            if(from < _currentClip.left ()) from = _currentClip.left ();
            if(to   > _currentClip.right()) to   = _currentClip.right();

            if(to < from) continue;

            // Draw the scanline
            rasterScanline(from - minX, to - minX, y - minY, minX, minY, color);
        }
    }
}

/*
// THE ORIGINAL ONE USING FIXED POINTS
void Rasterizer2::rasterPolygonsNoAA(const std::vector<Polygon>& polygons,
                                     const Color& color,
                                     Pt::int32_t minX, Pt::int32_t minY,
                                     Pt::int32_t maxX, Pt::int32_t maxY)
{
    std::size_t totalPointCount = 0;
    std::vector< std::vector<Point> > polygonPoints;
    polygonPoints.reserve( polygons.size() );

    std::vector<Polygon>::const_iterator it;
    for(it = polygons.begin(); it != polygons.end(); ++it)
    {
        if( it->size() < 3 )
            continue;

        polygonPoints.push_back( std::vector<Point>() );
        polygonPoints.back().reserve( it->size() );

        for(size_t i = 0; i < it->size(); ++i)
        {
            const PointF& pf = it->at(i);
            const Pt::int32_t x = lround( pf.x() );
            const Pt::int32_t y = lround( pf.y() );

            polygonPoints.back().push_back( Point(x, y) );
        }

        totalPointCount += polygonPoints.back().size();
    }

    // List of nodes that define the horizontal spans
    std::vector<Pt::int32_t> nodeX(totalPointCount * 2, 0);

    // Loop through the rows of the image
    for(Pt::int32_t y = minY; y <= maxY; ++y)
    {
        // Pixel-by-pixel clipping
        if(y < _currentClip.top   ()) continue;
        if(y > _currentClip.bottom()) continue;

        // Build a list of nodes using all the polygons
        std::size_t nodes = 0;
        for(size_t p = 0; p < polygonPoints.size(); ++p)
        {
            const std::vector<Point>& polygon = polygonPoints[p];

            // loop through the points
            Pt::int32_t j = polygon.size() - 1;

            for(size_t i = 0; i < polygon.size(); ++i)
            {
                // Get the coordinates
                const Pt::int32_t curXi = polygon[i].x();
                const Pt::int32_t curYi = polygon[i].y();
                const Pt::int32_t curXj = polygon[j].x();
                const Pt::int32_t curYj = polygon[j].y();

                // Calculate the node's coordinate
                if( ( y >= curYi && y < curYj ) || ( y >= curYj && y < curYi ) )
                {
                    // Bail out if we have produced too many nodes
                    if(nodes >= nodeX.size())
                        return;

                    // Calculate the node's coordinate
                    const Pt::int32_t deltaYp = y - curYi;
                    const Pt::int32_t deltaYj = curYj  - curYi;
                    const Pt::int32_t deltaXj = curXj  - curXi;
                    const Pt::int32_t interXf = FIXED_POINT_FROM_INT(curXi) +
                                                ( (FIXED_POINT_FROM_INT(deltaYp) + FIXED_POINT_CONSTANT_HALF ) /
                                                  deltaYj * deltaXj );
                    nodeX[nodes++] = FIXED_POINT_TO_INT(interXf);
                }

                j = i;
            }
        }

        // Skip if there is no node
        if( ! nodes )
            continue;

        // Sort the nodes
        bubbleSortAscending(nodeX, nodes);

        // Fill the pixels between the node pairs
        for(std::size_t i = 0; i < nodes; i += 2)
        {
            // Get the coordinate
            Pt::int32_t from = nodeX[i    ];
            Pt::int32_t to   = nodeX[i + 1];

            // Pixel-by-pixel clipping
            if(from < _currentClip.left ()) from = _currentClip.left ();
            if(to   > _currentClip.right()) to   = _currentClip.right();

            if(to < from) continue;

            // Draw the scanline
            rasterScanline(from - minX, to - minX, y - minY, minX, minY, color);
        }
    }
}
*/


// Inspired by: Efficient Polygon Fill Algorithm With C Code Sample
//              http://alienryderflex.com/polygon_fill
//              Public-domain code by Darel Rex Finley, 2007
void Rasterizer2::rasterPolygonXWAA(const PointF* points, std::size_t pointCount,
                                    const Color& color,
                                    Pt::int32_t minX, Pt::int32_t minY,
                                    Pt::int32_t maxX, Pt::int32_t maxY)
{
    std::size_t totalPointCount = pointCount;

    // List of nodes that define the horizontal spans
    std::vector<float> nodeX(totalPointCount * 2, 0);

    // List of polygon scanlines
    PolygonScanlines scanlines;

    if(_compositionMode != CompositionMode::SourceCopy)
        scanlines.resize( (maxY - minY) + 1 + 4 );

    // Loop through the rows of the image
    for(Pt::int32_t y = minY; y <= maxY; ++y)
    {
        // Pixel-by-pixel clipping
        if(y < _currentClip.top   ()) continue;
        if(y > _currentClip.bottom()) continue;

        // Build a list of nodes using all the polygons
        std::size_t nodes = 0;

        // Loop through the points
        Pt::int32_t j = pointCount - 1;

        for(size_t i = 0; i < pointCount; ++i)
        {
            // Get the coordinates
            const float curXi = points[i].x();
            const float curYi = points[i].y();
            const float curXj = points[j].x();
            const float curYj = points[j].y();

            // Calculate the node's coordinate
            if( ( y >= curYi && y < curYj ) || ( y >= curYj && y < curYi ) )
            {
                // Bail out if we have produced too many nodes
                if( nodes >= nodeX.size() )
                    return;

                // Calculate the node's coordinate
                const float deltaYp = y - curYi;
                const float deltaYj = curYj - curYi;
                const float deltaXj = curXj - curXi;
                const float interXf = curXi + deltaYp / deltaYj * deltaXj;

                nodeX[nodes++] = interXf;
            }

            // Update the searching index
            j = i;
        }

        // Skip if there is no node
        if( ! nodes )
            continue;

        // Sort the nodes
        bubbleSortAscending(nodeX, nodes);

        // Fill the pixels between the node pairs
        for(std::size_t i = 0; i < nodes; i += 2)
        {
            // #@#
            // Calculate the coordinate
            Pt::int32_t from = Pt::lround( ceil(nodeX[i]) );
            Pt::int32_t to   = Pt::lround( floor(nodeX[i + 1]/* - 0.5f*/) );

            // Pixel-by-pixel clipping
            if(from < _currentClip.left ()) from = _currentClip.left ();
            if(to   > _currentClip.right()) to   = _currentClip.right();

            if(to < from) continue;

            // Store the scanline coordinate as needed
            if(_compositionMode != CompositionMode::SourceCopy)
            {
                scanlines[y - minY + 1].push_back(ScanlineElement16(from, to));
            }

            // Draw the scanline
            rasterScanline(from - minX, to - minX, y - minY, minX, minY, color);
#if 0
        if(IP2_DEBUG::DUMP_SCANLINE_COORDINATES) {
            std::cerr << std::fixed << std::setw(5) << std::setprecision(1)
                  << "RP XWAA " << (minX + (from - minX)) << ", " << (minY + y - minY) << " LEN " << ((to - minX) - (from - minX) + 1) << std::endl;
        }
#endif
        }
    }

    // Raster the anti-aliased outline

#if 0
    // OLD POLYGON XWAA
    // Mask
    DrawLineMask mask_zero;
    DrawLineMask mask_nnp1;
    memcpy(mask_zero, Rasterizer2::NullLineMask, sizeof(DrawLineMask));
    memcpy(mask_nnp1, Rasterizer2::NullLineMask, sizeof(DrawLineMask));

    // From point N to point (N + 1), successively
    const size_t pc1 = pointCount - 1;

    for(size_t i = 0; i < pc1; ++i)
    {
        rasterPolygonBorderXWAA_F(
            points[i].x(),     points[i].y(),
            points[i + 1].x(), points[i + 1].y(),
            color, minX, minY_ - 1, scanlines, mask_nnp1 );

        if( ! i )
            memcpy(&mask_zero, &mask_nnp1, sizeof(mask_zero));
    }

    // From the last point to the first point
    mask_zero[2] = mask_zero[0];
    mask_zero[3] = mask_zero[1];
    mask_zero[0] = mask_nnp1[2];
    mask_zero[1] = mask_nnp1[3];

    rasterPolygonBorderXWAA_F(
          points[pc1].x(), points[pc1].y(),
          points[0].x(),   points[0].y(),
          color, minX, minY_ - 1, scanlines, mask_zero );
#endif

    // Mask
    DrawLineMask xwaaMask;
    memcpy(xwaaMask, Rasterizer2::NullLineMask, sizeof(DrawLineMask));

    // From point N to point (N + 1), successively
    const size_t pc1 = pointCount - 1;

    for(size_t i = 0; i < pc1; ++i)
    {
        rasterPolygonBorderXWAA_F2(
            points[i].x(),     points[i].y(),
            points[i + 1].x(), points[i + 1].y(),
            color, minX, minY - 1, scanlines, xwaaMask );
    }

    rasterPolygonBorderXWAA_F2(
          points[pc1].x(), points[pc1].y(),
          points[0].x(),   points[0].y(),
          color, minX, minY - 1, scanlines, xwaaMask );
}


// Inspired by: Efficient Polygon Fill Algorithm With C Code Sample
//              http://alienryderflex.com/polygon_fill
//              Public-domain code by Darel Rex Finley, 2007
void Rasterizer2::rasterPolygonsXWAA(const std::vector<Polygon>& polygons,
                                     const Color& color,
                                     Pt::int32_t minX, Pt::int32_t minY,
                                     Pt::int32_t maxX, Pt::int32_t maxY)
{
    std::size_t totalPointCount = 0;

    for(std::vector<Polygon>::const_iterator it = polygons.begin();
        it != polygons.end(); ++it)
    {
        totalPointCount += it->size();
    }

    // List of nodes that define the horizontal spans
    std::vector<float> nodeX(totalPointCount * 2, 0);

    // Calculate the scaled Y coordinates
    //const Pt::int32_t minX = Pt::lround(floor(minX_));
    // // unused: const Pt::int32_t maxX = Pt::lround(floor(maxX_));
    //const Pt::int32_t minY = Pt::lround(floor(minY_));
    //const Pt::int32_t maxY = Pt::lround(floor(maxY_));

    // List of polygon scanlines
    PolygonScanlines scanlines;

    if(_compositionMode != CompositionMode::SourceCopy)
        scanlines.resize( (maxY - minY) + 1 + 4 );

    // Loop through the rows of the image
    for(Pt::int32_t y = minY; y <= maxY; ++y)
    {
        // Pixel-by-pixel clipping
        if(y < _currentClip.top   ()) continue;
        if(y > _currentClip.bottom()) continue;

        // Build a list of nodes using all the polygons
        std::size_t nodes = 0;

        for(size_t p = 0; p < polygons.size(); ++p)
        {
            const Polygon* polygon = &polygons[p];

            if( polygon->size() < 2 )
                continue;

            // Loop through the points
            Pt::int32_t j = polygon->size() - 1;

            for(size_t i = 0; i < polygon->size(); ++i)
            {
                // Get the coordinates
                const float curXi = polygon->at(i).x();
                const float curYi = polygon->at(i).y();
                const float curXj = polygon->at(j).x();
                const float curYj = polygon->at(j).y();

                // Calculate the node's coordinate
                if( ( y >= curYi && y < curYj ) || ( y >= curYj && y < curYi ) )
                {
                    // Bail out if we have produced too many nodes
                    if((size_t) nodes >= nodeX.size())
                        return;

                    // Calculate the node's coordinate
                    const float deltaYp = y     - curYi;
                    const float deltaYj = curYj - curYi;
                    const float deltaXj = curXj - curXi;
                    const float interXf = curXi + deltaYp / deltaYj * deltaXj;

                    nodeX[nodes++] = interXf;
                }

                // Update the searching index
                j = i;
            }
        }

        // Skip if there is no node
        if( ! nodes)
            continue;

        // Sort the nodes
        bubbleSortAscending(nodeX, nodes);

        // Fill the pixels between the node pairs
        for(std::size_t i = 0; i < nodes; i += 2)
        {
            // Calculate the coordinate
            Pt::int32_t from = Pt::lround( ceil(nodeX[i]) );
            Pt::int32_t to   = Pt::lround( floor(nodeX[i + 1]/* - 0.5f*/) );

            // Pixel-by-pixel clipping
            if(from < _currentClip.left ()) from = _currentClip.left ();
            if(to   > _currentClip.right()) to   = _currentClip.right();

            if(to < from) continue;

            // Store the scanline coordinate as needed
            if(_compositionMode != CompositionMode::SourceCopy)
            {
                scanlines[y - minY + 1].push_back(ScanlineElement16(from, to));
            }

            // Draw the scanline
            rasterScanline(from - minX, to - minX, y - minY, minX, minY, color);
        }
    }

    // Raster the anti-aliased outline
    for(size_t p = 0; p < polygons.size(); ++p)
    {
        const Polygon* polygon = &polygons[p];

        if( polygon->size() < 2 )
            continue;

        /*
        // OLD POLYGON XWAA
        // Mask
        DrawLineMask mask_zero;
        DrawLineMask mask_nnp1;
        memcpy(mask_zero, Rasterizer2::NullLineMask, sizeof(DrawLineMask));
        memcpy(mask_nnp1, Rasterizer2::NullLineMask, sizeof(DrawLineMask));

        // From point N to point (N + 1), successively
        const size_t pc1 = polygon->size() - 1;

        for(size_t i = 0; i < pc1; ++i)
        {
            rasterPolygonBorderXWAA_F(
                polygon->at(i).x(), polygon->at(i).y(),
                polygon->at(i + 1).x(), polygon->at(i + 1).y(),
                color, minX, minY_ - 1, scanlines, mask_nnp1 );

            if( ! i )
                memcpy(&mask_zero, &mask_nnp1, sizeof(mask_zero));
        }

        // From the last point to the first point
        mask_zero[2] = mask_zero[0];
        mask_zero[3] = mask_zero[1];
        mask_zero[0] = mask_nnp1[2];
        mask_zero[1] = mask_nnp1[3];

        rasterPolygonBorderXWAA_F(
             polygon->at(pc1).x(), polygon->at(pc1).y(),
             polygon->at(0).x(), polygon->at(0).y(),
             color, minX, minY_ - 1, scanlines, mask_zero );
        */

        // Mask
        DrawLineMask xwaaMask;
        memcpy(xwaaMask, Rasterizer2::NullLineMask, sizeof(DrawLineMask));

        // From point N to point (N + 1), successively
        const size_t pc1 = polygon->size() - 1;

        for(size_t i = 0; i < pc1; ++i)
        {
            rasterPolygonBorderXWAA_F2(
                polygon->at(i).x(), polygon->at(i).y(),
                polygon->at(i + 1).x(), polygon->at(i + 1).y(),
                color, minX, minY - 1, scanlines, xwaaMask );
        }

        rasterPolygonBorderXWAA_F2(
             polygon->at(pc1).x(), polygon->at(pc1).y(),
             polygon->at(0).x(), polygon->at(0).y(),
             color, minX, minY - 1, scanlines, xwaaMask );
    }
}


#if 0
// OLD POLYGON XWAA
// REVIEW: Seems nothing actually uses this function anymore?

// Using algorithm from: Xiaolin Wu's Line Algorithm
//                       https://en.wikipedia.org/wiki/Xiaolin_Wu's_line_algorithm
//                       Last modified on January 19, 2017
void Rasterizer2::rasterPolygonBorderXWAA_F(float x1, float y1,
                                            float x2, float y2,
                                            const Color& color,
                                            Pt::int32_t minX, Pt::int32_t minY,
                                            const PolygonScanlines& exclusionZone,
                                            DrawLineMask& maskInOut)
{
    //fprintf(stderr, "XWAA: (%6.3f, %6.3f) - (%6.3f, %6.3f)\n", x1, y1, x2, y2);

    // Get the mask's coordinate
    float mx[4] = { MAXIMUM_COORD_F, MAXIMUM_COORD_F, MAXIMUM_COORD_F, MAXIMUM_COORD_F };
    float my[4] = { MAXIMUM_COORD_F, MAXIMUM_COORD_F, MAXIMUM_COORD_F, MAXIMUM_COORD_F };

    for(Pt::int32_t i = 0; i < 4; ++i) {
        mx[i] = maskInOut[i].x();
        my[i] = maskInOut[i].y();
    }

    // Used for storing back the mask's coordinates
    Pt::int32_t pCnt  = 0;
    Pt::int32_t lx[4] = { MAXIMUM_COORD, MAXIMUM_COORD, MAXIMUM_COORD, MAXIMUM_COORD };
    Pt::int32_t ly[4] = { MAXIMUM_COORD, MAXIMUM_COORD, MAXIMUM_COORD, MAXIMUM_COORD };

    // A helper macro to fill pixel
    #define XW_FILL_PIXEL(X, Y, A)                                                     \
        do {                                                                           \
            /* Clip the point */                                                       \
            if( !ClipShapeI::insideXYRange(X, Y, _currentClip) ) break;                \
            /* Check if we should skip drawing the pixel */                            \
            bool skipDrawing = false;                                                  \
            for(Pt::int32_t j = 0; j < 4; ++j) {                                       \
                if( (X) != mx[j] || (Y) != my[j] ) continue;                           \
                skipDrawing = true;                                                    \
                break;                                                                 \
            }                                                                          \
            if(skipDrawing || !(A)) break;                                             \
            /* Store back the mask's coordinates */                                    \
            lx[2] = lx[3]; lx[3] = X;                                                  \
            ly[2] = ly[3]; ly[3] = Y;                                                  \
            if(pCnt < 2) {                                                             \
                lx[pCnt] = X;                                                          \
                ly[pCnt] = Y;                                                          \
                ++pCnt;                                                                \
            }                                                                          \
            /* Fill the pixel */                                                       \
            if(_isTexture || _isGradient) {                                            \
                const Pt::int32_t bw = _brushImage->width();                           \
                const Pt::int32_t bh = _brushImage->height();                          \
                const Pt::int32_t dx = std::max<Pt::int32_t>(X - minX, 0);             \
                const Pt::int32_t dy = std::max<Pt::int32_t>(Y - minY, 0);             \
                const Pt::int32_t tx = _isGradient ? std::min(bw - 1, dx) : (dx % bw); \
                const Pt::int32_t ty = _isGradient ? std::min(bh - 1, dy) : (dy % bh); \
                ConstPixel srcPixel(_brushImage->view(), tx, ty);                      \
                Pixel      dstPixel(_image->view(), X, Y);                             \
                _image->format().setPixel(dstPixel, srcPixel, _compositionMode, A);    \
            }                                                                          \
            else { /* Solid */                                                         \
                Pixel pixel(_image->view(), X, Y);                                     \
                _image->format().setPixel(pixel, color, _compositionMode, A);          \
            }                                                                          \
        } while(false)

    // Check if the start and end coordinates are the same
    if(x1 == x2 && y1 == y2) {
        // Draw the pixel
        XW_FILL_PIXEL( lround(x1), lround(y1), 255);
        // Store back the start and end coordinates to the mask
        maskInOut[0].set(lx[0], ly[0]);
        maskInOut[1].set(lx[0], ly[0]);
        maskInOut[2].set(lx[0], ly[0]);
        maskInOut[3].set(lx[0], ly[0]);
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
            // Calculate the alphas and coordinates
            const Pt::int32_t fypxli = Pt::lround( floor(ypxli) );
            const Pt::int32_t fpart  = Pt::lround( (ypxli - fypxli) * 255.0f );
            const Pt::int32_t rfpart = 255 - fpart;
            const Pt::uint8_t a1     = Rasterizer2::XWAA_WFILTER[ fpart];
            const Pt::uint8_t a2     = Rasterizer2::XWAA_WFILTER[rfpart];
            const Pt::int32_t x1 = fypxli;
            const Pt::int32_t x2 = fypxli + 1;
            const Pt::int32_t y  = i;
            ypxli += grad;
            // Draw the pixels as needed
            bool skipPixel1 = false;
            bool skipPixel2 = false;
            if(!exclusionZone.empty()) {
                for(std::vector<ScanlineElement16>::const_iterator it = exclusionZone[y - minY].begin(); it != exclusionZone[y - minY].end(); ++it) {
                    if(x1 >= it->from && x1 <= it->to) skipPixel1 = true;
                    if(x2 >= it->from && x2 <= it->to) skipPixel2 = true;
                    if(skipPixel1 && skipPixel2) break;
                }
            }
            if(!skipPixel1) XW_FILL_PIXEL(x1, y, a1);
            if(!skipPixel2) XW_FILL_PIXEL(x2, y, a2);
        }
    }
    else {
        // Draw the pixels
        for(Pt::int32_t i = from; i <= to; ++i) {
            // Calculate the alphas and coordinates
            const Pt::int32_t fypxli = Pt::lround( floor(ypxli) );
            const Pt::int32_t fpart  = Pt::lround( (ypxli - fypxli) * 255.0f );
            const Pt::int32_t rfpart = 255 - fpart;
            const Pt::uint8_t a1     = Rasterizer2::XWAA_WFILTER[ fpart];
            const Pt::uint8_t a2     = Rasterizer2::XWAA_WFILTER[rfpart];
            const Pt::int32_t x  = i;
            const Pt::int32_t y1 = ypxli;
            const Pt::int32_t y2 = ypxli + 1;
            ypxli += grad;
            // Draw the pixels as needed
            bool skipPixel = false;
            if(!exclusionZone.empty()) {
                for(std::vector<ScanlineElement16>::const_iterator it = exclusionZone[y1 - minY].begin(); it != exclusionZone[y1 - minY].end(); ++it) {
                    if (x <= it->from || x >= it->to) continue;
                    skipPixel = true;
                    break;
                }
            }
            if(!skipPixel) XW_FILL_PIXEL(x, y1, a1);
            skipPixel = false;
            if(!exclusionZone.empty()) {
                for(std::vector<ScanlineElement16>::const_iterator it = exclusionZone[y2 - minY].begin(); it != exclusionZone[y2 - minY].end(); ++it) {
                    if (x <= it->from || x >= it->to) continue;
                    skipPixel = true;
                    break;
                }
            }
            if(!skipPixel) XW_FILL_PIXEL(x, y2, a2);
        }
    }

    // Store back the start and end coordinates to the mask
    if(swapDir) {
        maskInOut[2].set(lx[0], ly[0]);
        maskInOut[3].set(lx[1], ly[1]);
        maskInOut[0].set(lx[2], ly[2]);
        maskInOut[1].set(lx[3], ly[3]);
    }
    else {
        maskInOut[0].set(lx[0], ly[0]);
        maskInOut[1].set(lx[1], ly[1]);
        maskInOut[2].set(lx[2], ly[2]);
        maskInOut[3].set(lx[3], ly[3]);
    }

    // Undefine the helper macro
    #undef XW_FILL_PIXEL
}
#endif


// Using algorithm from: Xiaolin Wu's Line Algorithm
//                       https://en.wikipedia.org/wiki/Xiaolin_Wu's_line_algorithm
//                       Last modified on January 19, 2017
void Rasterizer2::rasterPolygonBorderXWAA_F2(float x1, float y1,
                                            float x2, float y2,
                                            const Color& color,
                                            Pt::int32_t minX, Pt::int32_t minY,
                                            const PolygonScanlines& exclusionZone,
                                            DrawLineMask& maskInOut)
{
    // NOTE: This implementation does not need to use the Rasterizer2::XWAA_WFILTER[]

    // TODO: Does the other XWAA implementation will benefit from this more conservative apparoach?
    //       (shall they be converted too?). IMHO: they seems to work fine, so nope ;-)

    // TODO: Visual (brightness) artifact in SourceOver mode!

    // Get the input mask's coordinate
    float mx[4];
    float my[4];

    for(Pt::int32_t i = 0; i < 4; ++i) {
        mx[i] = maskInOut[i].x();
        my[i] = maskInOut[i].y();
    }

    // For storing the output mask's coordinate
    float lx[4];
    float ly[4];

            /* Update the output mask's coordinates * /                                \
            if(swapDir) {                                                              \
                lx[0] = lx[1]; lx[1] = X;                                              \
                ly[0] = ly[1]; ly[1] = Y;                                              \
            }                                                                          \
            else {                                                                     \
                lx[2] = lx[3]; lx[3] = X;                                              \
                ly[2] = ly[3]; ly[3] = Y;                                              \
            }*/                                                                        \

    // A helper macro to fill pixel
    #define  XW_FILL_PIXEL(X, Y, A)                                                    \
        do {                                                                           \
            /* Clip the point */                                                       \
            if( !ClipShapeI::insideXYRange(X, Y, _currentClip) ) break;                \
            /* Check if we should skip drawing the pixel */                            \
            bool skipDrawing = false;                                                  \
            if(_compositionMode == CompositionMode::SourceOver) {                      \
                for(Pt::int32_t j = 0; j < 4; ++j) {                                   \
                    if( (X) != mx[j] || (Y) != my[j] ) continue;                       \
                    skipDrawing = true;                                                \
                    break;                                                             \
                }                                                                      \
            }                                                                          \
            if(skipDrawing || !(A)) break;                                             \
            /* Fill the pixel */                                                       \
            if(_isTexture || _isGradient) {                                            \
                const Pt::int32_t bw = _brushImage->width();                           \
                const Pt::int32_t bh = _brushImage->height();                          \
                const Pt::int32_t dx = std::max<Pt::int32_t>(X - minX, 0);             \
                const Pt::int32_t dy = std::max<Pt::int32_t>(Y - minY, 0);             \
                const Pt::int32_t tx = _isGradient ? std::min(bw - 1, dx) : (dx % bw); \
                const Pt::int32_t ty = _isGradient ? std::min(bh - 1, dy) : (dy % bh); \
                ConstPixel srcPixel(_brushImage->view(), tx, ty);                      \
                Pixel      dstPixel(_image->view(), X, Y);                             \
                _image->format().setPixel(dstPixel, srcPixel, _compositionMode, A);    \
            }                                                                          \
            else { /* Solid */                                                         \
                Pixel pixel(_image->view(), X, Y);                                     \
                _image->format().setPixel(pixel, color, _compositionMode, A);          \
            }                                                                          \
        } while(false)

    // Copy the coordinates
    float fx0 = x1;
    float fy0 = y1;
    float fx1 = x2;
    float fy1 = y2;

    // Swap the coordinates as needed
    const bool steep = ( fabs(fy1 - fy0) > fabs(fx1 - fx0) );

    if(steep) {
        std::swap(fx0, fy0);
        std::swap(fx1, fy1);
    }

    const bool swapDir = (fx0 > fx1);

    if(swapDir) {
        std::swap(fx0, fx1);
        std::swap(fy0, fy1);
    }

    // Calculate the gradient
    const float dx       = fx1 - fx0;
    const float dy       = fy1 - fy0;
    const float gradient = (dx == 0.0f) ? 1.0f : (dy / dx);

    // Handle the first endpoint
    Pt::int32_t xend = lround(fx0);
    float       yend = fy0 + gradient * (xend - fx0);
    float       xgap = 1.0f - ( (fx0 + 0.5f) - floor(fx0 + 0.5f) ); // Reverse fractional part

    const Pt::int32_t xpxl1 = xend; // This will be used in the main loop
    const Pt::int32_t ypxl1 = floor(yend);

#if 1
    if(steep) {
        // Calculate the alphas and coordinates
        const float  fpart = yend - floor(yend);
        const float rfpart = 1.0f - fpart;
        const Pt::int32_t ix0 = ypxl1;
        const Pt::int32_t ix1 = ix0 + 1;
        const Pt::int32_t iy  = xpxl1;
        // Draw the pixels as needed
        bool skipPixel0 = false;
        bool skipPixel1 = false;
        if(!exclusionZone.empty()) {
            for(std::vector<ScanlineElement16>::const_iterator it = exclusionZone[iy - minY + 1].begin(); it != exclusionZone[iy - minY + 1].end(); ++it) {
                if(ix0 >= it->from && ix0 <= it->to) skipPixel0 = true;
                if(ix1 >= it->from && ix1 <= it->to) skipPixel1 = true;
                if(skipPixel0 && skipPixel1) break;
            }
        }
        if(!skipPixel0) XW_FILL_PIXEL( ix0, iy, (Pt::int32_t) (rfpart * xgap * 255.0f));
        if(!skipPixel1) XW_FILL_PIXEL( ix1, iy, (Pt::int32_t) ( fpart * xgap * 255.0f));
        // Store the first endpoint coordinates as the output mask
        lx[0] = ix0; ly[0] = iy;
        lx[1] = ix1; ly[1] = iy;
    }
    else {
        // Calculate the alphas and coordinates
        const float  fpart = yend - floor(yend);
        const float rfpart = 1.0f - fpart;
        const Pt::int32_t ix  = xpxl1;
        const Pt::int32_t iy0 = ypxl1;
        const Pt::int32_t iy1 = iy0 + 1;
        // Draw the pixels as needed
        bool skipPixel = false;
        if(!exclusionZone.empty()) {
            for(std::vector<ScanlineElement16>::const_iterator it = exclusionZone[iy0 - minY + 1].begin(); it != exclusionZone[iy0 - minY + 1].end(); ++it) {
                if(ix <= it->from || ix >= it->to) continue;
                skipPixel = true;
                break;
            }
        }
        if(!skipPixel) XW_FILL_PIXEL( ix, iy0, (Pt::int32_t) (rfpart * xgap * 255.0f) );
        skipPixel = false;
        if(!exclusionZone.empty()) {
            for(std::vector<ScanlineElement16>::const_iterator it = exclusionZone[iy1 - minY + 1].begin(); it != exclusionZone[iy1 - minY + 1].end(); ++it) {
                if (ix <= it->from || ix >= it->to) continue;
                skipPixel = true;
                break;
            }
        }
        if(!skipPixel) XW_FILL_PIXEL( ix, iy1, (Pt::int32_t) (fpart * xgap * 255.0f) );
        // Store the first endpoint coordinates as the output mask
        lx[0] = ix; ly[0] = iy0;
        lx[1] = ix; ly[1] = iy1;
    }
#endif

    // Calculate the first y-intersection for the main loop
    float intery = yend + gradient;

    // Handle the second endpoint
    xend = lround(fx1);
    yend = fy1 + gradient * (xend - fx1);
    xgap = (fx1 + 0.5f) - floor(fx1 + 0.5f); // Fractional part

    const Pt::int32_t xpxl2 = xend; // This will be used in the main loop
    const Pt::int32_t ypxl2 = floor(yend);

#if 1
    if(steep) {
        // Calculate the alphas and coordinates
        const float  fpart = yend - floor(yend);
        const float rfpart = 1.0f - fpart;
        const Pt::int32_t ix0 = ypxl2;
        const Pt::int32_t ix1 = ix0 + 1;
        const Pt::int32_t iy  = xpxl2;
        // Draw the pixels as needed
        bool skipPixel0 = false;
        bool skipPixel1 = false;
        if(!exclusionZone.empty()) {
            for(std::vector<ScanlineElement16>::const_iterator it = exclusionZone[iy - minY + 1].begin(); it != exclusionZone[iy - minY + 1].end(); ++it) {
                if(ix0 >= it->from && ix0 <= it->to) skipPixel0 = true;
                if(ix1 >= it->from && ix1 <= it->to) skipPixel1 = true;
                if(skipPixel0 && skipPixel1) break;
            }
        }
        if(!skipPixel0) XW_FILL_PIXEL( ix0, iy, (Pt::int32_t) (rfpart * xgap * 255.0f) );
        if(!skipPixel1) XW_FILL_PIXEL( ix1, iy, (Pt::int32_t) ( fpart * xgap * 255.0f) );
        // Store the second endpoint coordinates as the output mask
        lx[2] = ix0; ly[2] = iy;
        lx[3] = ix1; ly[3] = iy;
    }
    else {
        // Calculate the alphas and coordinates
        const float  fpart = yend - floor(yend);
        const float rfpart = 1.0f - fpart;
        const Pt::int32_t ix  = xpxl2;
        const Pt::int32_t iy0 = ypxl2;
        const Pt::int32_t iy1 = iy0 + 1;
        // Draw the pixels as needed
        bool skipPixel = false;
        if(!exclusionZone.empty()) {
            for(std::vector<ScanlineElement16>::const_iterator it = exclusionZone[iy0 - minY + 1].begin(); it != exclusionZone[iy0 - minY + 1].end(); ++it) {
                if(ix <= it->from || ix >= it->to) continue;
                skipPixel = true;
                break;
            }
        }
        if(!skipPixel) XW_FILL_PIXEL( ix, iy0, (Pt::int32_t) (rfpart * xgap * 255.0f) );
        skipPixel = false;
        if(!exclusionZone.empty()) {
            for(std::vector<ScanlineElement16>::const_iterator it = exclusionZone[iy1 - minY + 1].begin(); it != exclusionZone[iy1 - minY + 1].end(); ++it) {
                if (ix <= it->from || ix >= it->to) continue;
                skipPixel = true;
                break;
            }
        }
        if(!skipPixel) XW_FILL_PIXEL( ix, iy1, (Pt::int32_t) (fpart * xgap * 255.0f) );
        // Store the second endpoint coordinates as the output mask
        lx[2] = ix; ly[2] = iy0;
        lx[3] = ix; ly[3] = iy1;
    }
#endif

    // Main loop
    if(steep) {
        for(Pt::int32_t x = xpxl1 + 1; x <= xpxl2 - 1; ++x) {
            // Calculate the alphas and coordinates
            const Pt::int32_t fpart  = (intery - floor(intery)) * 255.0f;
            const Pt::int32_t rfpart = 255 - fpart;
            const Pt::int32_t ix0 = floor(intery);
            const Pt::int32_t ix1 = ix0 + 1;
            const Pt::int32_t iy  = x;
            intery = intery + gradient;
            // Draw the pixels as needed
            bool skipPixel0 = false;
            bool skipPixel1 = false;
            if(!exclusionZone.empty()) {
                for(std::vector<ScanlineElement16>::const_iterator it = exclusionZone[iy - minY + 1].begin(); it != exclusionZone[iy - minY + 1].end(); ++it) {
                    if(ix0 >= it->from && ix0 <= it->to) skipPixel0 = true;
                    if(ix1 >= it->from && ix1 <= it->to) skipPixel1 = true;
                    if(skipPixel0 && skipPixel1) break;
                }
            }
            if(!skipPixel0) XW_FILL_PIXEL(ix0, iy, rfpart);
            if(!skipPixel1) XW_FILL_PIXEL(ix1, iy,  fpart);
        }
    }
    else {
        for(Pt::int32_t x = xpxl1 + 1; x <= xpxl2 - 1; ++x) {
            // Calculate the alphas and coordinates
            const Pt::int32_t fpart  = (intery - floor(intery)) * 255.0f;
            const Pt::int32_t rfpart = 255 - fpart;
            const Pt::int32_t ix  = x;
            const Pt::int32_t iy0 = floor(intery);
            const Pt::int32_t iy1 = iy0 + 1;
            intery = intery + gradient;
            // Draw the pixels as needed
            bool skipPixel = false;
            if(!exclusionZone.empty()) {
                for(std::vector<ScanlineElement16>::const_iterator it = exclusionZone[iy0 - minY + 1].begin(); it != exclusionZone[iy0 - minY + 1].end(); ++it) {
                    if(ix <= it->from || ix >= it->to) continue;
                    skipPixel = true;
                    break;
                }
            }
            if(!skipPixel) XW_FILL_PIXEL(ix, iy0, rfpart);
            skipPixel = false;
            if(!exclusionZone.empty()) {
                for(std::vector<ScanlineElement16>::const_iterator it = exclusionZone[iy1 - minY + 1].begin(); it != exclusionZone[iy1 - minY + 1].end(); ++it) {
                    if (ix <= it->from || ix >= it->to) continue;
                    skipPixel = true;
                    break;
                }
            }
            if(!skipPixel) XW_FILL_PIXEL(ix, iy1, fpart);
        }
    }

    // Output the new mask
    if(swapDir) {
        if(maskInOut[0].x() == MAXIMUM_COORD_F || maskInOut[0].y() == MAXIMUM_COORD_F) maskInOut[0].set(lx[2], ly[2]);
        if(maskInOut[1].x() == MAXIMUM_COORD_F || maskInOut[1].y() == MAXIMUM_COORD_F) maskInOut[1].set(lx[3], ly[3]);
                                                                                       maskInOut[2].set(lx[0], ly[0]);
                                                                                       maskInOut[3].set(lx[1], ly[1]);
    }
    else {
        if(maskInOut[0].x() == MAXIMUM_COORD_F || maskInOut[0].y() == MAXIMUM_COORD_F) maskInOut[0].set(lx[0], ly[0]);
        if(maskInOut[1].x() == MAXIMUM_COORD_F || maskInOut[1].y() == MAXIMUM_COORD_F) maskInOut[1].set(lx[1], ly[1]);
                                                                                       maskInOut[2].set(lx[2], ly[2]);
                                                                                       maskInOut[3].set(lx[3], ly[3]);
    }

    // Undefine the helper macro
    #undef XW_FILL_PIXEL
}


} // namespace

} // namespace

