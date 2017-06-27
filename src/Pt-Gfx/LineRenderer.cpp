/* Copyright (C) 2017-2017 Aloysius Indrayanto
   Copyright (C) 2017-2017 Marc Boris Duerner

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

#include "LineRenderer.h"
#include "Rasterizer2.h" // FIXED constants

namespace Pt {

namespace Gfx {

LineRenderer::LineRenderer()
{
}


void LineRenderer::renderWidePolyline(std::vector<Polygon>& polygons,
                                      const PointF* points, const std::size_t n,
                                      const Pen& pen)
{
    if( n < 2 )
        return;

    //polygon.reserve( pointCount * 2 );

    bool isSolid = pen.style() == Pen::Solid;
    bool isClosed = points[0] == points[n - 1];

    if(isSolid) // solid line
    {       
        if(isClosed) 
        {
            renderSolidClosedWidePolyline(polygons, points, n - 1, pen);
        }
        else 
        {
            renderSolidOpenWidePolyline(polygons, points, n, pen);
        }
    }
    else // dashed line
    {
        renderDashedWidePolyLine(polygons, points, n, pen);
    }
}


void LineRenderer::renderSolidClosedWidePolyline(std::vector<Polygon>& polygons, 
                                                 const PointF* basePtr, size_t curPCnt,
                                                 const Pen& pen)
{
    Pt::int32_t* segmentIndexMarker = 0;

    // Prepare the buffers
    std::vector<PointF> pointsFOuter;
    std::vector<PointF> pointsFInner;
    std::vector<PointF> pointsFSegment;

    const size_t pz2 = (pen.size() > 2) ? (pen.size() / 2) : 1;
    const size_t pz4 = (pen.size() > 4) ? (pen.size() / 4) : 1;

    pointsFOuter  .reserve(curPCnt * pz2);
    pointsFInner  .reserve(curPCnt * pz4);
    pointsFSegment.reserve(          pz2);

    // Get the number of point
    const size_t curPC1 = curPCnt - 1;

    // Save the original base pointer
    const PointF* ptrZero = basePtr;

    // Walk through the polygon's lines
    for(size_t i = 0; i <= curPC1; ++i) 
    {
        const PointF& from = *basePtr++;
        const PointF& to   = (i == curPC1) ? *ptrZero : *basePtr;

        // Check if the "to" point belongs to the same segment

        //
        // ALOY: can this ever be true if segmentIndexMarker is null?
        //

        bool inSameSegment = segmentIndexMarker && (i != curPC1);

        if(inSameSegment && *segmentIndexMarker < (Pt::int32_t) (i + 1)) {
            inSameSegment = false;
            ++segmentIndexMarker;
        }
        
        // Generate and combine line segments
        pointsFSegment.clear();

        renderSolidLineSegment(pointsFSegment, 
                               from.x(), from.y(), 
                               to.x(), to.y(), 
                               pen, false, false);

        if( ! joinClosedWidePolyline(pointsFOuter, pointsFInner, 
                                     pointsFSegment, from, pen,
                                     i == 1, false, inSameSegment) ) 
            return;
    }

    // reprocess the first and second segments to render the last join
    
    const PointF& from = *ptrZero++;
    const PointF& to   = *ptrZero;
    
    // Generate and combine line segments
    pointsFSegment.clear();
    
    renderSolidLineSegment(pointsFSegment, 
                           from.x(), from.y(), 
                           to.x(), to.y(), 
                           pen, false, false);
    
    if( ! joinClosedWidePolyline(pointsFOuter, pointsFInner, 
                                 pointsFSegment, from, pen,
                                 false, true, false) )
        return;

    // Combine the polygon data
    if( pointsFOuter.empty() || pointsFInner.empty() ) 
        return;

    polygons.resize( polygons.size() + 1 );
    polygons.back().assign( &pointsFOuter[0], pointsFOuter.size() );
    
    polygons.resize( polygons.size() + 1 );
    polygons.back().assign( &pointsFInner[0], pointsFInner.size() );
}


void LineRenderer::renderSolidOpenWidePolyline(std::vector<Polygon>& polygons, 
                                               const PointF* basePtr, size_t curPCnt,
                                               const Pen& pen)
{
    Pt::int32_t* segmentIndexMarker = 0;

    // Prepare the buffers
    std::vector<PointF> pointsFPolygon;
    std::vector<PointF> pointsFInner;
    std::vector<PointF> pointsFSegment;

    const size_t pz2 = (pen.size() > 2) ? (pen.size() / 2) : 1;
    const size_t pz4 = (pen.size() > 4) ? (pen.size() / 4) : 1;

    pointsFPolygon.reserve(curPCnt * pz2);
    pointsFInner  .reserve(curPCnt * pz4);
    pointsFSegment.reserve(          pz2);

    // Get the number of point
    const size_t curPC1 = curPCnt - 1;
    const size_t curPC2 = curPCnt - 2;

    // Walk through the polygon's lines
    for(size_t i = 0; i < curPC1; ++i) 
    {
        // Get the coordinates
        const PointF& from = *basePtr++;
        const PointF& to   = *basePtr;

        //
        // ALOY: can this ever be true if segmentIndexMarker is null?
        //

        // Check if the "to" point belongs to the same segment
        bool inSameSegment = !!segmentIndexMarker;

        if(inSameSegment && *segmentIndexMarker < (Pt::int32_t) (i + 1)) {
            inSameSegment = false;
            ++segmentIndexMarker;
        }
        
        // Generate and combine line segments
        pointsFSegment.clear();
        
        renderSolidLineSegment(pointsFSegment, from.x(), from.y(), 
                                 to.x(), to.y(), pen, i == 0, i == curPC2);
        
        if( ! joinOpenWidePolyline( pointsFPolygon, pointsFInner, 
                                    pointsFSegment, from, pen, inSameSegment ) )
            return;
    }

    // Process and store the "inside" lines' points to the main polygon buffer in reverse
    pointsFPolygon.insert( pointsFPolygon.end(), 
                           pointsFInner.rbegin(), pointsFInner.rend() );
    
    polygons.resize( polygons.size() + 1 );
    Polygon& polygon = polygons.back();

    polygon.assign( &pointsFPolygon[0], pointsFPolygon.size() );
}


void LineRenderer::renderDashedWidePolyLine(std::vector<Polygon>& polygons, //pointsF
                                            const PointF* src, size_t pointCount,
                                            const Pen& pen)
{
    //// Initialize the operational state
    //SAGOpState state(pointsF, src, pointCount, _rasterizer->pen().size());

    //// The pattern buffer and its counter
    //const Pt::uint8_t* pBuff      = _rasterizer->patternBufferMP64();
    //      Pt::int32_t  piCtrInOut = 0;

    //// Loop until all the polygon's points are processed
    //bool done = false;
    //while( ! done ) 
    //{
    //    // Calculate the "pattern" segment length
    //    const Pt::uint8_t refPat = pBuff[piCtrInOut];
    //    state.patSegLen = 0.0f;
    //    for(;;) 
    //    {
    //        // Get and compare the pattern bit
    //        const Pt::uint8_t curPat = pBuff[piCtrInOut++];
    //        
    //        if(piCtrInOut >= PATTERN_BUFFER_COUNTER_MAXMP) 
    //            piCtrInOut -= PATTERN_BUFFER_COUNTER_MAXMP;
    //        
    //        if(curPat == refPat) 
    //        {
    //            state.patSegLen += state.cellSize;
    //            continue;
    //        }
    //        
    //        // We have got a different pattern bit, exit to process the "pattern" segment
    //        --piCtrInOut;
    //        if(piCtrInOut < 0) 
    //            piCtrInOut += PATTERN_BUFFER_COUNTER_MAXMP;
    //        
    //        break;
    //    }
    //    
    //    // Bail out if the "pattern" segment is shorter than the cell size
    //    if(state.patSegLen < state.cellSize) 
    //        return;
    //    
    //    // Process the "pattern" segment
    //    done = sagPolygonPoints(state, !!refPat);
    //}
}


void LineRenderer::renderSolidLineSegment(std::vector<PointF>& dst, 
                                          float x1, float y1, float x2, float y2, 
                                          const Pen& pen, bool openingCap, bool closingCap)
{
    // Calculate the line's parameters
    float wh, dx, dy, nx, ny;

    calculateLineParams(wh, dx, dy, nx, ny, x1, y1, x2, y2, pen.size());

    // Generate points (CCW)
    
    // --- Begin point ---
    if( openingCap ) 
    {
        switch( pen.capStyle() ) 
        {
            case Pen::SquareCap        : renderLineSquareCap       (dst, x1, y1,     dx, dy, nx, ny); break;
            case Pen::RoundCap         : renderLineRoundCap        (dst, x1, y1, wh, dx, dy, nx, ny); break;
            case Pen::TriangularOutCap : renderLineTriangularOutCap(dst, x1, y1,     dx, dy, nx, ny); break;
            case Pen::TriangularInCap  : renderLineTriangularInCap (dst, x1, y1,     dx, dy, nx, ny); break;
            case Pen::RoundHoleCap     : renderLineRoundHoleCap    (dst, x1, y1, wh, dx, dy, nx, ny); break;
            case Pen::Arrow1Cap        : renderLineArrow1Cap       (dst, x1, y1,     dx, dy, nx, ny); break;
            case Pen::Arrow2Cap        : renderLineArrow2Cap       (dst, x1, y1,     dx, dy, nx, ny); break;
            default                    : openingCap = false;
        }
    }
    
    if( ! openingCap ) 
        renderLineButtCap(dst, x1, y1, nx, ny);
    
    // --- End point ---
    if(closingCap) 
    {
        switch( pen.capStyle() ) 
        {
            case Pen::SquareCap        : renderLineSquareCap       (dst, x2, y2,     -dx, -dy, -nx, -ny); break;
            case Pen::RoundCap         : renderLineRoundCap        (dst, x2, y2, wh, -dx, -dy, -nx, -ny); break;
            case Pen::TriangularOutCap : renderLineTriangularOutCap(dst, x2, y2,     -dx, -dy, -nx, -ny); break;
            case Pen::TriangularInCap  : renderLineTriangularInCap (dst, x2, y2,     -dx, -dy, -nx, -ny); break;
            case Pen::RoundHoleCap     : renderLineRoundHoleCap    (dst, x2, y2, wh, -dx, -dy, -nx, -ny); break;
            case Pen::Arrow1Cap        : renderLineArrow1Cap       (dst, x2, y2,     -dx, -dy, -nx, -ny); break;
            case Pen::Arrow2Cap        : renderLineArrow2Cap       (dst, x2, y2,     -dx, -dy, -nx, -ny); break;
            default                    : closingCap = false;
        }
    }
    
    if( ! closingCap ) 
        renderLineButtCap(dst, x2, y2, -nx, -ny);
}


bool LineRenderer::joinClosedWidePolyline(std::vector<PointF>& outer, 
                                          std::vector<PointF>& inner, 
                                          const std::vector<PointF>& segment, 
                                          const PointF& origMeetingPoint, const Pen& pen,
                                          bool isFirst, bool isLast, bool inSameSegment)
{
    // If the main polygon buffer is still empty, simply copy the points
    if( outer.empty() ) {
        outer.push_back(segment[1]);
        outer.push_back(segment[2]);
        inner.push_back(segment[0]);
        inner.push_back(segment[3]);
        return true;
    }

    // Get the width of the pen
    const size_t penSize = pen.size();

    // Get the "outside" lines
    const PointF& oline1a = outer[outer.size() - 2];
    const PointF& oline1b = outer[outer.size() - 1];
    const PointF& oline2a = segment[1];
    const PointF& oline2b = segment[2];

    // Intersect the "outside" lines
    bool   inLine;
    PointF intersect;
    if( ! intersectLine(inLine, intersect, oline1a, oline1b, oline2a, oline2b, penSize) ) 
        return false;

    /*
    const PointF& ochk1 = oline1b - intersect;
    const PointF& ochk2 = oline2a - intersect;

    inLine |= ( fabs(ochk1.x()) <= 0.8f && fabs(ochk1.y()) <= 0.8f ) || // For preventing artifacts
              ( fabs(ochk2.x()) <= 0.8f && fabs(ochk2.y()) <= 0.8f );
    */

    // Store the "outside" line's points
    const Pen::JoinStyle js1 = (inSameSegment || inLine) ? Pen::MiterJoin : pen.joinStyle();
    outer.pop_back();
    if(isFirst) outer.pop_back();
    switch(js1) {
        // No join
        case Pen::NoJoin:
            outer.push_back(oline1b);
            outer.push_back(origMeetingPoint);
            outer.push_back(oline2a);
            break;
        // Bevel join
        case Pen::BevelJoin:
            outer.push_back(oline1b);
            outer.push_back(oline2a);
            break;
        // Miter join
        case Pen::MiterJoin:
            outer.push_back(intersect);
            break;
        // Round join
        case Pen::RoundJoin:
            renderQuadraticBezierPoints(
                outer,
                lround(oline1b  .x()), lround(oline1b  .y()),
                lround(intersect.x()), lround(intersect.y()),
                lround(oline2a  .x()), lround(oline2a  .y()),
                penSize / 2 + 2
            );
            break;
        // Invalid join type
        default:
            return false;
    }
    if(!isLast) outer.push_back(oline2b);

    // Get the "inside" lines
    const PointF& iline1a = inner[inner.size() - 2];
    const PointF& iline1b = inner[inner.size() - 1];
    const PointF& iline2a = segment[0];
    const PointF& iline2b = segment[3];

    // Intersect the "inside" lines
    if(!intersectLine(inLine, intersect, iline1a, iline1b, iline2a, iline2b, penSize)) return false;

    /*
    const PointF& ichk1 = iline1b - intersect;
    const PointF& ichk2 = iline2a - intersect;

    inLine |= ( fabs(ichk1.x()) <= 0.8f && fabs(ichk1.y()) <= 0.8f ) || // For preventing artifacts
              ( fabs(ichk2.x()) <= 0.8f && fabs(ichk2.y()) <= 0.8f );
    */

    // Store the "inside" line's points
    const Pen::JoinStyle js2 = (inSameSegment || inLine) ? Pen::MiterJoin : pen.joinStyle();
    inner.pop_back();
    if(isFirst) inner.pop_back();
    switch(js2) {
        // No join
        case Pen::NoJoin:
            inner.push_back(iline1b);
            inner.push_back(origMeetingPoint);
            inner.push_back(iline2a);
            break;
        // Bevel join
        case Pen::BevelJoin:
            inner.push_back(iline1b);
            inner.push_back(iline2a);
            break;
        // Miter join
        case Pen::MiterJoin:
            inner.push_back(intersect);
            break;
        // Round join
        case Pen::RoundJoin:
            renderQuadraticBezierPoints(
                inner,
                lround(iline1b  .x()), lround(iline1b  .y()),
                lround(intersect.x()), lround(intersect.y()),
                lround(iline2a  .x()), lround(iline2a  .y()),
                penSize / 2 + 2
            );
            break;
        // Invalid join type
        default:
            return false;
    }
    if(!isLast) inner.push_back(iline2b);

    // Done
    return true;
}


bool LineRenderer::joinOpenWidePolyline(std::vector<PointF>& polygon, 
                                         std::vector<PointF>& inner, 
                                         const std::vector<PointF>& segment, 
                                         const PointF& origMeetingPoint, 
                                         const Pen& pen,
                                         bool inSameSegment)
{
    // If the main polygon buffer is still empty, simply copy the points
    if(polygon.empty()) {
        polygon.insert(polygon.end(), segment.begin(), segment.end());
        return true;
    }

    // Get the width of the pen
    const size_t penSize = pen.size();

    // Copy point #(N-1) and #(N-2) from the main polygon buffer
    const size_t N1           = polygon.size() - 1;
    const PointF polyPointN   = polygon[N1    ];
    const PointF polyPointNm1 = polygon[N1 - 1];
    const PointF polyPointNm2 = polygon[N1 - 2];

    // Remove point #(N-1) and #N from the main polygon buffer
    polygon.pop_back();
    polygon.pop_back();

    // Get the "outside" lines
    const PointF& oline1a = polyPointNm2;
    const PointF& oline1b = polyPointNm1;
    const PointF& oline2a = segment[1];
    const PointF& oline2b = segment[2];

    // Intersect the "outside" lines
    bool   inLine;
    PointF intersect;
    if(!intersectLine(inLine, intersect, oline1a, oline1b, oline2a, oline2b, penSize)) return false;

    /*
    const PointF& ochk1 = oline1b - intersect;
    const PointF& ochk2 = oline2a - intersect;

    inLine |= ( fabs(ochk1.x()) <= 0.8f && fabs(ochk1.y()) <= 0.8f ) || // For preventing artifacts
              ( fabs(ochk2.x()) <= 0.8f && fabs(ochk2.y()) <= 0.8f );
    */

    // Store the "outside" line's points to the main polygon buffer
    const Pen::JoinStyle js1 = (inSameSegment || inLine) ? Pen::MiterJoin 
                                                         : pen.joinStyle();
    
    switch(js1) {
        // No join
        case Pen::NoJoin:
            polygon.push_back(oline1b);
            polygon.push_back(origMeetingPoint);
            polygon.push_back(oline2a);
            break;
        // Bevel join
        case Pen::BevelJoin:
            polygon.push_back(oline1b);
            polygon.push_back(oline2a);
            break;
        // Miter join
        case Pen::MiterJoin:
            polygon.push_back(intersect);
            break;
        // Round join
        case Pen::RoundJoin:
            renderQuadraticBezierPoints(
                polygon,
                lround(oline1b  .x()), lround(oline1b  .y()),
                lround(intersect.x()), lround(intersect.y()),
                lround(oline2a  .x()), lround(oline2a  .y()),
                penSize / 2 + 2
            );
            break;
        // Invalid join type
        default:
            return false;
    }

    // Store points #2 to #N from the segment to the main polygon buffer
    const size_t N2 = segment.size() - 1;
    for(size_t i = 2; i <= N2; ++i) polygon.push_back(segment[i]);

    // Get the "inside" lines
    const PointF& iline1a = inner.empty() ? polygon[0 ] : inner.back();
    const PointF& iline1b =                 polyPointN;
    const PointF& iline2a =                 segment[0 ];
    const PointF& iline2b =                 segment[N2];

    // Intersect the "inside" lines
    if(!intersectLine(inLine, intersect, iline1a, iline1b, iline2a, iline2b, penSize)) return false;

    /*
    const PointF& ichk1 = iline1b - intersect;
    const PointF& ichk2 = iline2a - intersect;

    inLine |= ( fabs(ichk1.x()) <= 0.8f && fabs(ichk1.y()) <= 0.8f ) || // For preventing artifacts
              ( fabs(ichk2.x()) <= 0.8f && fabs(ichk2.y()) <= 0.8f );
    */

    // Store the "inside" line's points to the auxiliary polygon buffer
    const Pen::JoinStyle js2 = (inSameSegment || inLine) ? Pen::MiterJoin 
                                                         : pen.joinStyle();
    switch(js2) {
        // No join
        case Pen::NoJoin:
            inner.push_back(iline1b);
            inner.push_back(origMeetingPoint);
            inner.push_back(iline2a);
            break;
        // Bevel join
        case Pen::BevelJoin:
            inner.push_back(iline1b);
            inner.push_back(iline2a);
            break;
        // Miter join
        case Pen::MiterJoin:
            inner.push_back(intersect);
            break;
        // Round join
        case Pen::RoundJoin:
            renderQuadraticBezierPoints(
                inner,
                lround(iline1b  .x()), lround(iline1b  .y()),
                lround(intersect.x()), lround(intersect.y()),
                lround(iline2a  .x()), lround(iline2a  .y()),
                penSize / 2 + 2
            );
            break;
        // Invalid join type
        default:
            return false;
    }

    // Done
    return true;
}


void LineRenderer::renderLineButtCap(std::vector<PointF>& dst, float x, float y, float nx, float ny)
{
    dst.push_back( PointF(x + nx, y + ny) );
    dst.push_back( PointF(x - nx, y - ny) );
}


void LineRenderer::renderLineSquareCap(std::vector<PointF>& dst, float x, float y, float dx, float dy, float nx, float ny)
{
    dst.push_back( PointF(x - dx + nx, y - dy + ny) );
    dst.push_back( PointF(x - dx - nx, y - dy - ny) );
}


void LineRenderer::renderLineRoundCap(std::vector<PointF>& dst, float x, float y, float wh, float dx, float dy, float nx, float ny)
{
#if 0
    renderQuadraticBezierPoints(
        dst,
        lround(x + nx     ), lround(y + ny     ),
        lround(x + nx - dx), lround(y + ny - dy),
        lround(x      - dx), lround(y      - dy),
        Gfx::Math::zcint(wh * 0.5f)
    );
    renderQuadraticBezierPoints(
        dst,
        lround(x      - dx), lround(y      - dy),
        lround(x - nx - dx), lround(y - ny - dy),
        lround(x - nx     ), lround(y - ny     ),
        Gfx::Math::zcint(wh * 0.5f)
    );
#else
    renderQuadraticBezierPoints(
        dst,
        lround(x + nx       ), lround(y + ny       ),
        lround(x - dx * 2.0f), lround(y - dy * 2.0f),
        lround(x - nx       ), lround(y - ny       ),
        Pt::lround(ceil(wh)) - 1
    );
#endif
}


void LineRenderer::renderLineTriangularOutCap(std::vector<PointF>& dst, float x, float y, float dx, float dy, float nx, float ny)
{
    dst.push_back( PointF(x + nx, y + ny) );
    dst.push_back( PointF(x - dx, y - dy) );
    dst.push_back( PointF(x - nx, y - ny) );
}


void LineRenderer::renderLineTriangularInCap(std::vector<PointF>& dst, float x, float y, float dx, float dy, float nx, float ny)
{
    dst.push_back( PointF(x + nx - dx, y + ny - dy) );
    dst.push_back( PointF(x,           y          ) );
    dst.push_back( PointF(x - nx - dx, y - ny - dy) );
}


void LineRenderer::renderLineRoundHoleCap(std::vector<PointF>& dst, float x, float y, float wh, float dx, float dy, float nx, float ny)
{
#if 0
    renderQuadraticBezierPoints(
        dst,
        lround(x + nx - dx), lround(y + ny - dy),
        lround(x + nx     ), lround(y + ny     ),
        lround(x          ), lround(y          ),
        Gfx::Math::zcint(wh * 0.5f)
    );
    renderQuadraticBezierPoints(
        dst,
        lround(x          ), lround(y          ),
        lround(x - nx     ), lround(y - ny     ),
        lround(x - nx - dx), lround(y - ny - dy),
        Gfx::Math::zcint(wh * 0.5f)
    );
#else
    renderQuadraticBezierPoints(
        dst,
        lround(x + nx - dx), lround(y + ny - dy),
        lround(x      + dx), lround(y      + dy),
        lround(x - nx - dx), lround(y - ny - dy),
        Pt::lround(ceil(wh)) - 1
    );
#endif
}


void LineRenderer::renderLineArrow1Cap(std::vector<PointF>& dst, float x, float y, float dx, float dy, float nx, float ny)
{
    dst.push_back( PointF(x + nx,        y + ny       ) );
    dst.push_back( PointF(x + nx * 2.0f, y + ny * 2.0f) );
    dst.push_back( PointF(x - dx,        y - dy       ) );
    dst.push_back( PointF(x - nx * 2.0f, y - ny * 2.0f) );
    dst.push_back( PointF(x - nx,        y - ny       ) );
}


void LineRenderer::renderLineArrow2Cap(std::vector<PointF>& dst, float x, float y, float dx, float dy, float nx, float ny)
{
    dst.push_back( PointF(x + dx * 0.5f + nx,        y + dy * 0.5f + ny       ) );
    dst.push_back( PointF(x + dx        + nx * 2.0f, y + dy        + ny * 2.0f) );
    dst.push_back( PointF(x - dx,                    y - dy                   ) );
    dst.push_back( PointF(x + dx        - nx * 2.0f, y + dy        - ny * 2.0f) );
    dst.push_back( PointF(x + dx * 0.5f - nx,        y + dy * 0.5f - ny       ) );
}


// Based on: Bitmap/Bézier curves/Quadratic
//           https://rosettacode.org/wiki/Bitmap/B%C3%A9zier_curves/Quadratic#C
//           Last modified on February 17, 2017
void LineRenderer::renderQuadraticBezierPoints(std::vector<PointF>& dst, 
                                                 float x1, float y1, 
                                                 float x2, float y2, 
                                                 float x3, float y3, 
                                                 Pt::int32_t nSegs)
{
    // Check if the points actually specify a straight line
    const float sx = x3 - x2;
    const float sy = y3 - y2;
    const float xx = x1 - x2;
    const float yy = y1 - y2;

    if( !(xx * sy - yy * sx) ) { // Curvature
        if( dst.empty() || dst.back().x() != x1 || dst.back().y() != y1 ) dst.push_back( PointF(x1, y1) );
        if( dst.empty() || dst.back().x() != x3 || dst.back().y() != y3 ) dst.push_back( PointF(x3, y3) );
        return;
    }

    // Ensure that the number of segments are not too few
    if(nSegs < 4) nSegs = 4;

    // Calculate the inverse multiplication factor
    const float nSegs1i = 1.0f / (nSegs - 1);

    for(Pt::int32_t i = 0; i < nSegs; ++i) {
        // Calculate the coordinates
        const float t  = i * nSegs1i;
        const float it = 1.0f - t;
        const float a  = it * it;
        const float b  = 2.0f * t  * it;
        const float c  = t * t;
        const float x  = a * x1 + b * x2 + c * x3;
        const float y  = a * y1 + b * y2 + c * y3;
        // Check if the coordinate is the same with the previous one
        if( !dst.empty() && ( dst.back().x() == x && dst.back().y() == y ) ) continue;
        // Store the coordinate
        dst.push_back( PointF(x, y) );
    }
}


void LineRenderer::calculateLineParams(float& wh, float& dx, float& dy, 
                                       float& nx, float& ny, float x1, float y1, 
                                       float x2, float y2, size_t w)
{
    // Line equation : 0 = aX + By + c
    // Normal        : n = ai + bj
    const float a = y2 - y1;
    const float b = x1 - x2;
  //const float c = -(x1 * y2 - x2 * y1);

    // Inverse line length
    // NOTE: Gfx::Math::fastInvSqrt() will produce artifacts!
    const float il = 1.0f / ::sqrtf(a * a + b * b);

    // Half line width
    wh = (float) w * 0.5f;

    // Direction vector
    dx = -b * il * wh;
    dy =  a * il * wh;

    // Normal vector
    nx =  a * il * wh;
    ny =  b * il * wh;
}


bool LineRenderer::intersectLine(bool& inLine, PointF& intersect, 
                                 const PointF& line1a, const PointF& line1b, 
                                 const PointF& line2a, const PointF& line2b, size_t penSize)
{
    // The first line
    const float x11   = line1a.x();
    const float y11   = line1a.y();
    const float x12   = line1b.x();
    const float y12   = line1b.y();
    const float minX1 = std::min(x11, x12);
    const float minY1 = std::min(y11, y12);
    const float maxX1 = std::max(x11, x12);
    const float maxY1 = std::max(y11, y12);
    const float a1    = y12 - y11;
    const float b1    = x11 - x12;
    const float c1    = -(x11 * y12 - x12 * y11);

    // The second line
    const float x21   = line2a.x();
    const float y21   = line2a.y();
    const float x22   = line2b.x();
    const float y22   = line2b.y();
    const float minX2 = std::min(x21, x22);
    const float minY2 = std::min(y21, y22);
    const float maxX2 = std::max(x21, x22);
    const float maxY2 = std::max(y21, y22);
    const float a2    = y22 - y21;
    const float b2    = x21 - x22;
    const float c2    = -(x21 * y22 - x22 * y21);

    // Check if the line is parallel
    const float denom = a1 * b2 - a2 * b1;
    if(denom == 0.0f) {
        // Check for special cases
        if(y11 == y12 && y11 == y21 && y11 == y22 && x12 == x21) {
            intersect.set(x12, y11);
            inLine = true;
            return true;
        }
        if(x11 == x12 && x11 == x21 && x11 == x22 && y12 == y21) {
            intersect.set(x11, y12);
            inLine = true;
            return true;
        }
        // No intersection
        return false;
    }

    // Calculate the intersection point
    const float idenom = 1.0f / denom;
          float ipX    = (b1 * c2 - b2 * c1) * idenom;
          float ipY    = (a2 * c1 - a1 * c2) * idenom;

    // Check and fix the coordinate of the intersection point
    // (for very steep lines, the coordinate of the intersection point can be incorrectly calculated)
    const size_t pzf = FIXED_POINT_TO_INT(penSize * FIXED_POINT_CONSTANT_SQRT2);
          if(ipX < minX1 - pzf && ipX < minX2 - pzf) ipX = (minX1 + minX2) * 0.5f;
    else if(ipX > maxX1 + pzf && ipX > maxX2 + pzf) ipX = (maxX1 + maxX2) * 0.5f;
          if(ipY < minY1 - pzf && ipY < minY2 - pzf) ipY = (minY1 + minY2) * 0.5f;
    else if(ipY > maxY1 + pzf && ipY > maxY2 + pzf) ipY = (maxY1 + maxY2) * 0.5f;

    // Store the intersection point
    intersect.set(ipX, ipY);

    // Determine if the intersection point is inside the line
    inLine = (ipX >= minX1 && ipX <= maxX1 && ipY >= minY1 && ipY <= maxY1)
            | (ipX >= minX2 && ipX <= maxX2 && ipY >= minY2 && ipY <= maxY2);

    // Done
    //lprintf("Line 1       : (%7.3f, %7.3f) - (%7.3f, %7.3f)\n", x11, y11, x12, y12);
    //lprintf("Line 2       : (%7.3f, %7.3f) - (%7.3f, %7.3f)\n", x21, y21, x22, y22);
    //lprintf("Intersection : (%7.3f, %7.3f) - %s \n", ipX, ipY, inLine ? "inline" : "outline");
    //lprintf("\n");
    return true;
}

} // namespace

} // namespace
