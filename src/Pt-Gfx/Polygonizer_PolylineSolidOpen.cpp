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
#include "Polygonizer_Inline.h"


namespace Pt {

namespace Gfx {


void Polygonizer::renderSolidOpenWidePolyline(std::vector<Polygon>& polygons,
                                              const PointF* basePtr, size_t curPCnt,
                                              const Pen& pen, bool cleanUpSelfIntersection)
{
#if 0
    fprintf(stderr, "### CALLING renderSolidOpenWidePolyline() ###\n");
    for (size_t i = 0; i < curPCnt; ++i) {
        fprintf(stderr, "%7.3f, %7.3f\n", (basePtr + i)->x(), (basePtr + i)->y());
    }
#endif

    //Pt::int32_t* segmentIndexMarker = 0;

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

        // Check if the "to" point belongs to the same segment
        //bool inSameSegment = !!segmentIndexMarker;

        //if(inSameSegment && *segmentIndexMarker < (Pt::int32_t) (i + 1)) {
        //    inSameSegment = false;
        //    ++segmentIndexMarker;
        //}

        // Generate and combine line segments
        pointsFSegment.clear();

        renderSolidLineSegment(pointsFSegment, from.x(), from.y(),
                               to.x(), to.y(), pen, i == 0, i == curPC2);

        if( ! joinOpenWidePolyline( pointsFPolygon, pointsFInner,
                                    pointsFSegment, from, pen, false /*inSameSegment*/ ) )
            return;
    }

    // Process and store the "inside" lines
    pointsFPolygon.insert( pointsFPolygon.end(),
                           pointsFInner.rbegin(), pointsFInner.rend() );

    // Clean-up self-intersection as needed
    if(cleanUpSelfIntersection && selfIntersecting(pointsFPolygon)) {
        cleanupOnePolygon(pointsFPolygon, true);
    }

    // Store the points to the main polygon buffer
    if(pointsFPolygon.empty()) return;
    polygons.push_back( Polygon(&pointsFPolygon[0], pointsFPolygon.size()) );
}


bool Polygonizer::joinOpenWidePolyline(std::vector<PointF>& polygon,
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
    bool        inLine;
    PointF      intersect;

    const bool  in1 = intersectLine(inLine, intersect, oline1a, oline1b, oline2a, oline2b, penSize);

#if 1
    const PointF& ochk1 = oline1b - intersect;
    const PointF& ochk2 = oline2a - intersect;

    inLine |= ( fabs(ochk1.x()) <= 0.8f && fabs(ochk1.y()) <= 0.8f ) || // For preventing artifacts
              ( fabs(ochk2.x()) <= 0.8f && fabs(ochk2.y()) <= 0.8f );
#endif

    // Store the "outside" line's points to the main polygon buffer
    Pen::JoinStyle js1 = pen.joinStyle();
         if(!in1                   ) js1 = Pen::NoJoin;
    else if(inSameSegment || inLine) js1 = Pen::MiterJoin;

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
                penSize / 2/* + 2*/
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
    const bool in2 = intersectLine(inLine, intersect, iline1a, iline1b, iline2a, iline2b, penSize);

#if 1
    const PointF& ichk1 = iline1b - intersect;
    const PointF& ichk2 = iline2a - intersect;

    inLine |= ( fabs(ichk1.x()) <= 0.8f && fabs(ichk1.y()) <= 0.8f ) || // For preventing artifacts
              ( fabs(ichk2.x()) <= 0.8f && fabs(ichk2.y()) <= 0.8f );
#endif

    // Store the "inside" line's points to the auxiliary polygon buffer

    Pen::JoinStyle js2 = pen.joinStyle();
         if(!in2                   ) js2 = Pen::NoJoin;
    else if(inSameSegment || inLine) js2 = Pen::MiterJoin;

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
                penSize / 2/* + 2*/
            );
            break;
        // Invalid join type
        default:
            return false;
    }

    // Done
    return true;
}


} // namespace

} // namespace
