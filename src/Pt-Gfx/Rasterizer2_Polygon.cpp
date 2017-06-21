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
// ===== Private Member Functions =======================================================
// ======================================================================================

void Rasterizer2::rasterOnePixelPolygonOutline(const Point* points, size_t pointCount, const Color& color)
{
    // Check if there are too few points
    if(pointCount < 2) return;

    // Mask
    DrawLineMask mask_nnp1;
    memcpy(mask_nnp1, Rasterizer2::NullLineMask, sizeof(DrawLineMask));

    // Pattern indexing counter
    const bool        solid       = (_pen.style() == Pen::Solid);
          Pt::int32_t fpiCtrInOut = PATTERN_BUFFER_COUNTER_START;

    // From point N to point (N + 1), successively
    const size_t pc1 = pointCount - 1;

    for(size_t i = 0; i < pc1; ++i) {
        if(solid) rasterOnePixelSolidLine    (points[i].x(), points[i].y(), points[i + 1].x(), points[i + 1].y(), color,              &mask_nnp1);
        else      rasterOnePixelPatternedLine(points[i].x(), points[i].y(), points[i + 1].x(), points[i + 1].y(), color, fpiCtrInOut, &mask_nnp1);
    }
}

void Rasterizer2::rasterOnePixelPolygonOutline(const PointF* points, size_t pointCount, const Color& color)
{
    // Check if there are too few points
    if(pointCount < 2) return;

    // Mask
    DrawLineMask mask_nnp1;
    memcpy(mask_nnp1, Rasterizer2::NullLineMask, sizeof(DrawLineMask));

    // Pattern indexing counter
    const bool        solid       = (_pen.style() == Pen::Solid);
          Pt::int32_t fpiCtrInOut = PATTERN_BUFFER_COUNTER_START;

    // From point N to point (N + 1), successively
    const size_t pc1 = pointCount - 1;

    for(size_t i = 0; i < pc1; ++i) {
        if(solid) rasterOnePixelSolidLine_F    (points[i].x(), points[i].y(), points[i + 1].x(), points[i + 1].y(), color,              &mask_nnp1);
        else      rasterOnePixelPatternedLine_F(points[i].x(), points[i].y(), points[i + 1].x(), points[i + 1].y(), color, fpiCtrInOut, &mask_nnp1);
    }
}

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
            const Pt::int32_t from = nodeX[i    ];
            const Pt::int32_t to   = nodeX[i + 1];
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
        scanlines.resize( (maxY - minY) + 1 + 2 );

    // Loop through the rows of the image
    for(Pt::int32_t pixelY = minY; pixelY <= maxY; ++pixelY) {
        // Base pointer for the polygons
        const PointF* curPointBase = points;
        // Build a list of nodes using all the polygons
        Pt::int32_t nodes = 0;
        for(size_t p = 0; p < polyCount; ++p) {
            // Get the current point count
            const size_t curPointCount = pointCount[p];
            // Loop through the points
            Pt::int32_t j = curPointCount - 1;
            for(size_t i = 0; i < curPointCount; ++i) {
                // Get the coordinates
                const float curXi = (curPointBase + i)->x();
                const float curYi = (curPointBase + i)->y();
                const float curXj = (curPointBase + j)->x();
                const float curYj = (curPointBase + j)->y();
                // Calculate the node's coordinate
                if( ( pixelY >= curYi && pixelY < curYj ) || ( pixelY >= curYj && pixelY < curYi ) ) {
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
            const Pt::int32_t from = Pt::lround( ceil(nodeX[i]) );
            const Pt::int32_t to   = Pt::lround( floor(nodeX[i + 1]) );
            if(to < from) continue;
            // Store the scanline coordinate as needed
            if(_compositionMode != CompositionMode::SourceCopy) {
                scanlines[pixelY - minY + 1].push_back(ScanlineElement16(from, to));
            }
            // Draw the scanline
            rasterScanline(from - minX, to - minX, pixelY - minY, minX, minY, color);
        }
    }

    // Raster the anti-aliased outline
    const PointF* curPointBase = points;
    for(size_t p = 0; p < polyCount; ++p) {
        // Mask
        DrawLineMask mask_zero;
        DrawLineMask mask_nnp1;
        memcpy(mask_zero, Rasterizer2::NullLineMask, sizeof(DrawLineMask));
        memcpy(mask_nnp1, Rasterizer2::NullLineMask, sizeof(DrawLineMask));
        // From point N to point (N + 1), successively
        const size_t pc1 = pointCount[p] - 1;
        for(size_t i = 0; i < pc1; ++i) {
            rasterOnePixelAreaGLineSegmentXWAA_F(
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
        rasterOnePixelAreaGLineSegmentXWAA_F(
             curPointBase[pc1].x(), curPointBase[pc1].y(),
             curPointBase[0  ].x(), curPointBase[0  ].y(),
             color, minX, minY_ - 1, scanlines, mask_zero
        );
        // Increment the base pointer
        curPointBase += pointCount[p];
    }
}

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
        totalPointCount += polygons.size();
    }

    // List of nodes that define the horizontal spans
    std::vector<Pt::int32_t> nodeX(totalPointCount * 2, 0);

    // Loop through the rows of the image
    for(Pt::int32_t y = minY; y <= maxY; ++y) 
    {
        // Build a list of nodes using all the polygons
        Pt::int32_t nodes = 0;
        for(size_t p = 0; p < polygons.size(); ++p) 
        {
            const Polygon* polygon = &polygons[p];

            if( polygon->size() < 2 )
                continue;
            
            // loop through the points
            Pt::int32_t j = polygon->size() - 1;
            
            for(size_t i = 0; i < polygon->size(); ++i) 
            {
                // Get the coordinates
                const Pt::int32_t curXi = lround( polygon->at(i).x() );
                const Pt::int32_t curYi = lround( polygon->at(i).y() );
                const Pt::int32_t curXj = lround( polygon->at(j).x() );
                const Pt::int32_t curYj = lround( polygon->at(j).y() );
                
                // Calculate the node's coordinate
                if( ( y >= curYi && y < curYj ) || ( y >= curYj && y < curYi ) ) 
                {
                    // Bail out if we have produced too many nodes
                    if((size_t) nodes >= nodeX.size()) 
                        return;
                    
                    // Calculate the node's coordinate
                    const Pt::int32_t deltaYp = y - curYi;
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
        }
        
        // Skip if there is no node
        if( ! nodes ) 
            continue;
        
        // Sort the nodes
        bubbleSortAscending(nodeX, nodes);
        
        // Fill the pixels between the node pairs
        for(Pt::int32_t i = 0; i < nodes; i += 2) 
        {
            const Pt::int32_t from = nodeX[i    ];
            const Pt::int32_t to   = nodeX[i + 1];
            rasterScanline(from - minX, to - minX, y - minY, minX, minY, color);
        }
    }
}


// Inspired by: Efficient Polygon Fill Algorithm With C Code Sample
//              http://alienryderflex.com/polygon_fill
//              Public-domain code by Darel Rex Finley, 2007
void Rasterizer2::rasterPolygonsXWAA(const std::vector<Polygon>& polygons, 
                                     const Color& color, 
                                     Pt::int32_t minX_, Pt::int32_t minY_, 
                                     Pt::int32_t maxX_, Pt::int32_t maxY_)
{
    std::size_t totalPointCount = 0;

    for(std::vector<Polygon>::const_iterator it = polygons.begin();
        it != polygons.end(); ++it)
    {
        totalPointCount += polygons.size();
    }

    // List of nodes that define the horizontal spans
    std::vector<float> nodeX(totalPointCount * 2, 0);

    // Calculate the scaled Y coordinates
    //const Pt::int32_t minX = Pt::lround(floor(minX_));
    // // unused: const Pt::int32_t maxX = Pt::lround(floor(maxX_));
    //const Pt::int32_t minY = Pt::lround(floor(minY_));
    //const Pt::int32_t maxY = Pt::lround(floor(maxY_));

    const Pt::int32_t minX = minX_;
    const Pt::int32_t minY = minY_;
    const Pt::int32_t maxY = maxY_;

    // List of polygon scanlines
    PolygonScanlines scanlines;

    if(_compositionMode != CompositionMode::SourceCopy)
        scanlines.resize( (maxY - minY) + 1 + 2 );

    // Loop through the rows of the image
    for(Pt::int32_t y = minY; y <= maxY; ++y) 
    {     
        // Build a list of nodes using all the polygons
        Pt::int32_t nodes = 0;
        
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
                    const float deltaYp = y - curYi;
                    const float deltaYj = curYj  - curYi;
                    const float deltaXj = curXj  - curXi;
                    const float interXf = curXi  + (deltaYp) / deltaYj * deltaXj;
                    
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
        for(Pt::int32_t i = 0; i < nodes; i += 2) 
        {
            // Calculate the coordinate
            const Pt::int32_t from = Pt::lround( ceil(nodeX[i]) );
            const Pt::int32_t to   = Pt::lround( floor(nodeX[i + 1]) );
            
            if(to < from) 
                continue;
            
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

        // Mask
        DrawLineMask mask_zero;
        DrawLineMask mask_nnp1;
        memcpy(mask_zero, Rasterizer2::NullLineMask, sizeof(DrawLineMask));
        memcpy(mask_nnp1, Rasterizer2::NullLineMask, sizeof(DrawLineMask));
        
        // From point N to point (N + 1), successively
        const size_t pc1 = polygon->size() - 1;
        
        for(size_t i = 0; i < pc1; ++i) 
        {
            rasterOnePixelAreaGLineSegmentXWAA_F(
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
        
        rasterOnePixelAreaGLineSegmentXWAA_F(
             polygon->at(pc1).x(), polygon->at(pc1).y(),
             polygon->at(0).x(), polygon->at(0).y(),
             color, minX, minY_ - 1, scanlines, mask_zero );
    }
}

} // namespace

} // namespace
