/* Copyright (C) 2017-2020 Aloysius Indrayanto
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

#include "Polygonizer.h"


namespace Pt {

namespace Gfx {


void Polygonizer::renderSolidClosedWidePolyline(std::vector<Polygon>& polygons,
                                                const PointF* basePtr, size_t curPCnt,
                                                const Pen& pen)
{
    //Pt::int32_t* segmentIndexMarker = 0;

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
        //bool inSameSegment = segmentIndexMarker && (i != curPC1);

        //if(inSameSegment && *segmentIndexMarker < (Pt::int32_t) (i + 1)) {
        //    inSameSegment = false;
        //    ++segmentIndexMarker;
        //}

        // Generate and combine line segments
        pointsFSegment.clear();

        renderSolidLineSegment(pointsFSegment,
                               from.x(), from.y(),
                               to.x(), to.y(),
                               pen, false, false, false);

        if( ! joinClosedWidePolyline(pointsFOuter, pointsFInner,
                                     pointsFSegment, from, pen,
                                     i == 1, false, false /*inSameSegment*/) )
            return;
    }

    // Reprocess the first and second segments to render the last join
    const PointF& from = *ptrZero++;
    const PointF& to   = *ptrZero;

    // Generate and combine line segments
    pointsFSegment.clear();

    renderSolidLineSegment(pointsFSegment,
                           from.x(), from.y(),
                           to.x(), to.y(),
                           pen, false, false, false);

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


bool Polygonizer::joinClosedWidePolyline(std::vector<PointF>& outer,
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
    bool       inLine;
    PointF     intersect;

    const bool in1 = intersectLine(inLine, intersect, oline1a, oline1b, oline2a, oline2b, penSize);

    /*
    const PointF& ochk1 = oline1b - intersect;
    const PointF& ochk2 = oline2a - intersect;

    inLine |= ( fabs(ochk1.x()) <= 0.8f && fabs(ochk1.y()) <= 0.8f ) || // For preventing artifacts
              ( fabs(ochk2.x()) <= 0.8f && fabs(ochk2.y()) <= 0.8f );
    */

    // Store the "outside" line's points
    //Pen::JoinStyle js1 = (inSameSegment || inLine) ? Pen::MiterJoin : pen.joinStyle();

    Pen::JoinStyle js1 = pen.joinStyle();
         if(!in1                   ) js1 = Pen::NoJoin;
    else if(inSameSegment || inLine) js1 = Pen::MiterJoin;

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
            //if(!in1) return false;
            outer.push_back(intersect);
            break;
        // Round join
        case Pen::RoundJoin:
            //if(!in1) return false;
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
    const bool in2 = intersectLine(inLine, intersect, iline1a, iline1b, iline2a, iline2b, penSize);

    /*
    const PointF& ichk1 = iline1b - intersect;
    const PointF& ichk2 = iline2a - intersect;

    inLine |= ( fabs(ichk1.x()) <= 0.8f && fabs(ichk1.y()) <= 0.8f ) || // For preventing artifacts
              ( fabs(ichk2.x()) <= 0.8f && fabs(ichk2.y()) <= 0.8f );
    */

    // Store the "inside" line's points
    //const Pen::JoinStyle js2 = (inSameSegment || inLine) ? Pen::MiterJoin : pen.joinStyle();

    Pen::JoinStyle js2 = pen.joinStyle();
         if(!in2                   ) js2 = Pen::NoJoin;
    else if(inSameSegment || inLine) js2 = Pen::MiterJoin;

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
            //if(!in2) return false;
            inner.push_back(intersect);
            break;
        // Round join
        case Pen::RoundJoin:
            //if(!in2) return false;
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


} // namespace

} // namespace
