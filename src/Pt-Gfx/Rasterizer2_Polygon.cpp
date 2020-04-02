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

//#include <iomanip>

#include "Rasterizer2.h"
#include "ClipShape.h"


namespace {


template<typename T>
static inline void bubbleSortAscending(T& basket, Pt::int32_t size)
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


template<typename T>
static inline void insertionsortAscending(T& basket, Pt::int32_t size)
{
    if(size <= 1) return;

    for(Pt::int32_t i = 1; i < size; ++i)
    {
        const typename T::value_type key = basket[i];
        Pt::int32_t                  j   = i - 1;

        while(j >= 0 && basket[j] > key)
        {
            basket[j + 1] = basket[j];
            --j;
        }
        basket[j + 1] = key;

    }
}


template<typename T>
static inline void selectionsortAscending(T& basket, Pt::int32_t size)
{
    for(Pt::int32_t i = 0; i < size - 1; ++i)
    {
        Pt::int32_t min = i;
        for(Pt::int32_t j = i+1; j < size; ++j)
        {
            if(basket[j] < basket[min]) min = j;
        }
        std::swap(basket[min], basket[i]);
    }
}


template<typename T>
static inline Pt::int32_t quicksortAscending_partition(T& basket, Pt::int32_t start, Pt::int32_t end)
{
    const typename T::value_type pivot = basket[end];
    Pt::int32_t                  pidx  = start;

    for(Pt::int32_t i = start; i < end; ++i)
    {
        if(basket[i] <= pivot)
        {
            std::swap(basket[i], basket[pidx]);
            ++pidx;
        }
    }

    std::swap(basket[pidx], basket[end]);

    return pidx;
}

template<typename T>
static inline void quicksortAscending(T& basket, Pt::int32_t start, Pt::int32_t end)
{
    if(start >= end) return;

    const Pt::int32_t pidx = quicksortAscending_partition(basket, start, end);

    quicksortAscending(basket, start,    pidx - 1);
    quicksortAscending(basket, pidx + 1, end     );
}

template<typename T>
static inline void quicksortAscending(T& basket, Pt::int32_t size)
{
    quicksortAscending(basket, 0, size - 1);
}


} // namespace


namespace Pt {

namespace Gfx {

// ======================================================================================
// ===== Private Member Functions =======================================================
// ======================================================================================

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
            const float curYi = points[i].y();
            const float curYj = points[j].y();

            // Calculate the node's coordinate
            if( ( y >= curYi && y < curYj ) || ( y >= curYj && y < curYi ) )
            {
                // Bail out if we have produced too many nodes
                if( nodes >= nodeX.size() )
                    return;

                // Get the X coordinates
                const float curXi = points[i].x();
                const float curXj = points[j].x();

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
    // Check if there is only one polygon
    if(polygons.size() == 1) {
        rasterPolygonNoAA(&polygons[0][0], polygons[0].size(), color, minX, minY, maxX, maxY);
        return;
    }

    // Calculate the total number of points
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
            const Polygon& polygon = polygons[p];

            if( polygon.size() < 2 )
                continue;

            // Loop through the points
            Pt::int32_t j = polygon.size() - 1;

            for(size_t i = 0; i < polygon.size(); ++i)
            {
                // Get the coordinates
                const float curYi = polygon[i].y();
                const float curYj = polygon[j].y();

                // Calculate the node's coordinate
                if( ( y >= curYi && y < curYj ) || ( y >= curYj && y < curYi ) )
                {
                    // Bail out if we have produced too many nodes
                    if((size_t) nodes >= nodeX.size())
                        return;

                    // Get the X coordinates
                    const float curXi = polygon[i].x();
                    const float curXj = polygon[j].x();

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


// Inspired by: Efficient Polygon Fill Algorithm With C Code Sample
//              http://alienryderflex.com/polygon_fill
//              Public-domain code by Darel Rex Finley, 2007
void Rasterizer2::rasterPolygonXWAA(const PointF* points, std::size_t pointCount,
                                    const Color& color,
                                    Pt::int32_t minX, Pt::int32_t minY,
                                    Pt::int32_t maxX, Pt::int32_t maxY)
{
    // List of nodes that define the horizontal spans
    std::vector<float> nodeX(pointCount * 2, 0);

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
            // Get the Y coordinates
            const float curYi = points[i].y();
            const float curYj = points[j].y();

            // Calculate the node's coordinate
            if( ( y >= curYi && y < curYj ) || ( y >= curYj && y < curYi ) )
            {
                // Bail out if we have produced too many nodes
                if( nodes >= nodeX.size() )
                    return;

                // Get the X coordinates
                const float curXi = points[i].x();
                const float curXj = points[j].x();

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
        //selectionsortAscending(nodeX, nodes);
        //insertionsortAscending(nodeX, nodes);
        bubbleSortAscending(nodeX, nodes);
        //quicksortAscending(nodeX, nodes);

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
    // Check if there is only one polygon
    if(polygons.size() == 1) {
        rasterPolygonXWAA(&polygons[0][0], polygons[0].size(), color, minX, minY, maxX, maxY);
        return;
    }

    // Calculate the total number of points
    std::size_t totalPointCount = 0;

    for(std::vector<Polygon>::const_iterator it = polygons.begin();
        it != polygons.end(); ++it)
    {
        totalPointCount += it->size();
    }

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

        for(size_t p = 0; p < polygons.size(); ++p)
        {
            const Polygon& polygon = polygons[p];

            if( polygon.size() < 2 )
                continue;

            // Loop through the points
            Pt::int32_t j = polygon.size() - 1;
            for(size_t i = 0; i < polygon.size(); ++i)
            {
                // Get the Y coordinates
                const float curYi = polygon[i].y();
                const float curYj = polygon[j].y();

                // Calculate the node's coordinate
                if( ( y >= curYi && y < curYj ) || ( y >= curYj && y < curYi ) )
                {
                    // Bail out if we have produced too many nodes
                    if((size_t) nodes >= nodeX.size())
                        return;

                    // Get the X coordinates
                    const float curXi = polygon[i].x();
                    const float curXj = polygon[j].x();

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
        const Polygon& polygon = polygons[p];

        if( polygon.size() < 2 )
            continue;

        // Mask
        DrawLineMask xwaaMask;
        memcpy(xwaaMask, Rasterizer2::NullLineMask, sizeof(DrawLineMask));

        // From point N to point (N + 1), successively
        const size_t pc1 = polygon.size() - 1;

        for(size_t i = 0; i < pc1; ++i)
        {
            rasterPolygonBorderXWAA_F2(
                polygon[i].x(), polygon[i].y(),
                polygon[i + 1].x(), polygon[i + 1].y(),
                color, minX, minY - 1, scanlines, xwaaMask );
        }

        rasterPolygonBorderXWAA_F2(
             polygon[pc1].x(), polygon[pc1].y(),
             polygon[0].x(), polygon[0].y(),
             color, minX, minY - 1, scanlines, xwaaMask );
    }
}


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
        const float       fpart  = yend - floor(yend);
        const float       rfpart = 1.0f - fpart;
        const Pt::int32_t ix0    = ypxl1;
        const Pt::int32_t ix1    = ix0 + 1;
        const Pt::int32_t iy     = xpxl1;
        // Draw the pixels as needed
        bool skipPixel0 = false;
        bool skipPixel1 = false;
        if(!exclusionZone.empty()) {
            for(S16V_CI it = S16V_begin(exclusionZone, iy - minY/* + 1*/); it != S16V_end(exclusionZone, iy - minY/* + 1*/); ++it) {
                if(ix0 >= it->from && ix0 <= it->to) skipPixel0 = true;
                if(ix1 >= it->from && ix1 <= it->to) skipPixel1 = true;
                if(skipPixel0 && skipPixel1) break;
            }
        }
        if(!skipPixel0) XW_FILL_PIXEL( ix0, iy, (Pt::int32_t) (rfpart * xgap * 255.0f) );
        if(!skipPixel1) XW_FILL_PIXEL( ix1, iy, (Pt::int32_t) ( fpart * xgap * 255.0f) );
        // Store the first endpoint coordinates as the output mask
        lx[0] = ix0; ly[0] = iy;
        lx[1] = ix1; ly[1] = iy;
    }
    else {
        // Calculate the alphas and coordinates
        const float       fpart  = yend - floor(yend);
        const float       rfpart = 1.0f - fpart;
        const Pt::int32_t ix     = xpxl1;
        const Pt::int32_t iy0    = ypxl1;
        const Pt::int32_t iy1    = iy0 + 1;
        // Draw the pixels as needed
        bool skipPixel = false;
        if(!exclusionZone.empty()) {
            for(S16V_CI it = S16V_begin(exclusionZone, iy0 - minY/* + 1*/); it != S16V_end(exclusionZone, iy0 - minY/* + 1*/); ++it) {
                if(ix < it->from || ix > it->to) continue;
                skipPixel = true;
                break;
            }
        }
        if(!skipPixel) XW_FILL_PIXEL( ix, iy0, (Pt::int32_t) (rfpart * xgap * 255.0f) );
        skipPixel = false;
        if(!exclusionZone.empty()) {
            for(S16V_CI it = S16V_begin(exclusionZone, iy1 - minY/* + 1*/); it != S16V_end(exclusionZone, iy1 - minY/* + 1*/); ++it) {
                if (ix < it->from || ix > it->to) continue;
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
        const float       fpart  = yend - floor(yend);
        const float       rfpart = 1.0f - fpart;
        const Pt::int32_t ix0    = ypxl2;
        const Pt::int32_t ix1    = ix0 + 1;
        const Pt::int32_t iy     = xpxl2;
        // Draw the pixels as needed
        bool skipPixel0 = false;
        bool skipPixel1 = false;
        if(!exclusionZone.empty()) {
            for(S16V_CI it = S16V_begin(exclusionZone, iy - minY/* + 1*/); it != S16V_end(exclusionZone, iy - minY/* + 1*/); ++it) {
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
        const float       fpart  = yend - floor(yend);
        const float       rfpart = 1.0f - fpart;
        const Pt::int32_t ix     = xpxl2;
        const Pt::int32_t iy0    = ypxl2;
        const Pt::int32_t iy1    = iy0 + 1;
        // Draw the pixels as needed
        bool skipPixel = false;
        if(!exclusionZone.empty()) {
            for(S16V_CI it = S16V_begin(exclusionZone, iy0 - minY/* + 1*/); it != S16V_end(exclusionZone, iy0 - minY/* + 1*/); ++it) {
                if(ix < it->from || ix > it->to) continue;
                skipPixel = true;
                break;
            }
        }
        if(!skipPixel) XW_FILL_PIXEL( ix, iy0, (Pt::int32_t) (rfpart * xgap * 255.0f) );
        skipPixel = false;
        if(!exclusionZone.empty()) {
            for(S16V_CI it = S16V_begin(exclusionZone, iy1 - minY/* + 1*/); it != S16V_end(exclusionZone, iy1 - minY/* + 1*/); ++it) {
                if (ix < it->from || ix > it->to) continue;
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
            const Pt::int32_t ix0    = floor(intery);
            const Pt::int32_t ix1    = ix0 + 1;
            const Pt::int32_t iy     = x;
            intery = intery + gradient;
            // Draw the pixels as needed
            bool skipPixel0 = false;
            bool skipPixel1 = false;
            if(!exclusionZone.empty()) {
                for(S16V_CI it = S16V_begin(exclusionZone, iy - minY/* + 1*/); it != S16V_end(exclusionZone, iy - minY/* + 1*/); ++it) {
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
            const Pt::int32_t ix     = x;
            const Pt::int32_t iy0    = floor(intery);
            const Pt::int32_t iy1    = iy0 + 1;
            intery = intery + gradient;
            // Draw the pixels as needed
            bool skipPixel = false;
            if(!exclusionZone.empty()) {
                for(S16V_CI it = S16V_begin(exclusionZone, iy0 - minY/* + 1*/); it != S16V_end(exclusionZone, iy0 - minY/* + 1*/); ++it) {
                    if(ix < it->from || ix > it->to) continue;
                    skipPixel = true;
                    break;
                }
            }
            if(!skipPixel) XW_FILL_PIXEL(ix, iy0, rfpart);
            skipPixel = false;
            if(!exclusionZone.empty()) {
                for(S16V_CI it = S16V_begin(exclusionZone, iy1 - minY/* + 1*/); it != S16V_end(exclusionZone, iy1 - minY/* + 1*/); ++it) {
                    if(ix < it->from || ix > it->to) continue;
                    skipPixel = true;
                    break;
                }
            }
            if(!skipPixel) XW_FILL_PIXEL(ix, iy1, fpart);
        }
    }

    // Output the new mask
    if(swapDir) {
        if(maskInOut[0].x() == MAXIMUM_COORD_F || maskInOut[0].y() == MAXIMUM_COORD_F) maskInOut[0].set(lx[0], ly[0]);
        if(maskInOut[1].x() == MAXIMUM_COORD_F || maskInOut[1].y() == MAXIMUM_COORD_F) maskInOut[1].set(lx[1], ly[1]);
                                                                                       maskInOut[2].set(lx[2], ly[2]);
                                                                                       maskInOut[3].set(lx[3], ly[3]);
    }
    else {
        if(maskInOut[0].x() == MAXIMUM_COORD_F || maskInOut[0].y() == MAXIMUM_COORD_F) maskInOut[0].set(lx[2], ly[2]);
        if(maskInOut[1].x() == MAXIMUM_COORD_F || maskInOut[1].y() == MAXIMUM_COORD_F) maskInOut[1].set(lx[3], ly[3]);
                                                                                       maskInOut[2].set(lx[0], ly[0]);
                                                                                       maskInOut[3].set(lx[1], ly[1]);
    }

    // Undefine the helper macro
    #undef XW_FILL_PIXEL
}


} // namespace

} // namespace

