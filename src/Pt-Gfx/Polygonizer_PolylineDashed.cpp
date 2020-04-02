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

#include "clipper_aj/clipper.hpp"


#define NON_FLAT_CAP_REDUCTION_FACTOR 0.75f


namespace Pt {

namespace Gfx {


//
// Pattern state information structure
//
struct PatternState
{
    std::vector<Polygon>& dstPolygons; // Destination vector

    const PointF*         srcPoints;   // Source points
    size_t                srcCount;    // The number of source points

    float                 patSegLen;   // Length of the currently processed "pattern" segment

    size_t                idx1;        // Index to the first point which is currently being processed;
                                       // the index to the second point is always (idx1 + 1)

    float                 px, py;      // Current interpolation coordinate (in-between the two points)
    float                 ex, ey;      // Current end coordinate (coordinate of the the second point)
    float                 uvx, uvy;    // Unit vector from the first point to the second point

    float                 remLen;      // Remaining length between the two points that has not been "consumed" by the "pattern" segment(s)

    std::vector<PointF>   gather;      // Gathered polygon points
    float                 gatherLen;   // Length of the gathered points

    PatternState(std::vector<Polygon>& dst, const PointF* src, size_t pointCount, size_t penSize)
    : dstPolygons(dst)
    , srcPoints(src)
    , srcCount(pointCount)
    , idx1(0)
    , remLen(-1.0f)
    , gatherLen(0.0f)
    {}
};


void Polygonizer::renderDashedWidePolyLine(std::vector<Polygon>& polygons,
                                           const PointF* src, size_t pointCount,
                                           const Pen& pen, bool collisionDetection)
{
    // Initialize the operational state
    PatternState state(polygons, src, pointCount, pen.size());

    // Loop until all the polygon's points are processed
    bool     done = false;
    bool     draw = true;
    unsigned n    = 0;
    while( ! done )
    {
        state.patSegLen = dashPatternBuffer[n];

        done = sagPolygonPoints(state, draw, pen, collisionDetection);
        draw = ! draw;

        if(++n >= dashPatternBuffer.size()) n = 0;
    }
}


bool Polygonizer::sagPolygonPoints(PatternState& state, bool draw, const Pen& pen, bool collisionDetection)
{
    // Temporary buffer for the generated points
    std::vector<PointF> pointsF;

    // Loop until the current "pattern" segment is completely processed
    while(state.patSegLen > 0.0f) {

        // (Re-)initialize some part of the operational state as needed
        if(state.remLen <= 0.0f) {
            // Check if all polygon's points have been processed
            if(state.idx1 + 1 >= state.srcCount) {
                // Process left-over partial segment (if any)
                if(state.gatherLen && draw) {
                    if(state.gather.size() > 2) {
                        sagGeneratePolyLineSegment(state, pen, collisionDetection);
                    }
                    else {
                        sagGenerateSimpleLineSegment(
                            state,
                            state.gather[0].x(),
                            state.gather[0].y(),
                            state.gather[1].x(),
                            state.gather[1].y(),
                            pen,
                            collisionDetection
                        );
                    }
                }
                // All done
                state.gather.clear();
                state.gatherLen = 0.0f;
                return true;
            }
            // Calculate the vector, size, and coordinates
            const float x1 = state.srcPoints[state.idx1    ].x();
            const float y1 = state.srcPoints[state.idx1    ].y();
            const float x2 = state.srcPoints[state.idx1 + 1].x();
            const float y2 = state.srcPoints[state.idx1 + 1].y();
            const float vx = x2 - x1;
            const float vy = y2 - y1;
            const float vz = sqrt(vx * vx + vy * vy);
            // Initialize some part of the operational state
            state.px     = x1;
            state.py     = y1;
            state.ex     = x2;
            state.ey     = y2;
            state.uvx    = vx / vz;
            state.uvy    = vy / vz;
            state.remLen = vz + sqrtf(2.0); // Adjustment factor because both 'vx' and 'vy' are each 1 pixel less than the real length
        }

        // If we have enough remainder length, process the polygon's edge as a simple line segment
        if(state.gather.empty() && state.remLen >= state.patSegLen) {
            // Generate a simple line segment as needed
            if(draw) {
                sagGenerateSimpleLineSegment(
                    state,
                    state.px,
                    state.py,
                    state.px + state.uvx * state.patSegLen,
                    state.py + state.uvy * state.patSegLen,
                    pen,
                    collisionDetection
                );
            }
            else {
            }
            // Substract the remainder length
            state.remLen -= state.patSegLen;
            // Reset the "pattern" segment length
            state.patSegLen = 0.0f;
            // Process excess length (if any)
            if(state.remLen > 0.0f) {
                // Update the interpolation coordinate
                state.px = state.ex - state.uvx * state.remLen;
                state.py = state.ey - state.uvy * state.remLen;
            }
            else {
                // Reset the remainder length and increment the point index so that the next time
                // this loop is running, the next point within the polygon will be processed
                state.remLen = -1.0f;
                ++state.idx1;
            }
            continue;
        }

        // If we have the complete length from the gathered points, process them into a thick polygon
        // (consume all the gathered points)
        if(state.gatherLen >= state.patSegLen) {
            // Generate one solid polygon segment as needed
            if(draw) {
                sagGeneratePolyLineSegment(state, pen, collisionDetection);
            }
            else {
            }
            // Reset the "pattern" segment length
            state.patSegLen = 0.0f;
            // Reset the gather buffer
            state.gather.clear();
            state.gatherLen = 0.0f;
            continue;
        }

        // ------------------------------------------
        // If we got here, it means:
        //     1. The remainder  length is not enough
        //     2. The "gathered" length is not enough
        // ------------------------------------------

        // Store the current interpolation coordinate to the "gather" buffer as the first gathered point
        if(state.gather.empty()) {// || state.gather.back().x() != state.px || state.gather.back().y() != state.py) {
            state.gather.push_back(PointF(state.px, state.py));
            continue;
        }

        // If the combined length is less than or equal to the "pattern" segment length, simply store the end coordinate
        if(state.gatherLen + state.remLen <= state.patSegLen) {
            // Store the end coordinate
            state.gather.push_back(PointF(state.ex, state.ey));
            state.gatherLen += state.remLen;
            // Reset the remainder length and increment the point index so that the next time
            // this loop is running, the next point within the polygon will be processed
            state.remLen = -1.0f;
            ++state.idx1;
        }
        // Otherwise, store the in-between coordinate
        else {
            // Calculate the needed length
            const float nl = state.patSegLen - state.gatherLen;
            // Update the interpolation coordinate
            state.px += state.uvx * nl;
            state.py += state.uvy * nl;
            // Store the in-between coordinate (the updated interpolation coordinate)
            state.gather.push_back(PointF(state.px, state.py));
            state.gatherLen += nl;
            // Substract and check the remainder length
            state.remLen -= nl;
            if(state.remLen <= 0.0f) {
                // Reset the remainder length and increment the point index so that the next time
                // this loop is running, the next point within the polygon will be processed
                state.remLen = -1.0f;
                ++state.idx1;
            }
        }

    } // while()

    // Indicate that the current "pattern" segment are completely processed,
    // but there are still unprocessed polygon's points
    return false;
}


void Polygonizer::sagGenerateSimpleLineSegment(PatternState& state,
                                               float x1, float y1,
                                               float x2, float y2,
                                               const Pen& pen,
                                               bool collisionDetection)
{
    // Temporary buffer for the generated points
    std::vector<PointF> pointsF;

    // Calculate the line's parameters
    float wh, dx, dy, nx, ny;

    const float ll = calculateLineParams(wh, dx, dy, nx, ny, x1, y1, x2, y2, pen.size());

    // Adjust the coordinates (thus the line's length) as needed
    // ### TODO: Exclude the first and last segment of dashed line ??? ###
    if( pen.capStyle() != Pen::FlatCap ) {
        // Check if the line is too short for adjustment
        if(ll) return;
        // Line has enough length
        x1 += (dx * NON_FLAT_CAP_REDUCTION_FACTOR);
        y1 += (dy * NON_FLAT_CAP_REDUCTION_FACTOR);
        x2 -= (dx * NON_FLAT_CAP_REDUCTION_FACTOR);
        y2 -= (dy * NON_FLAT_CAP_REDUCTION_FACTOR);
    }

    // Generate points (CCW)
    // --- Begin point ---
    switch(pen.capStyle()) {
        case Pen::SquareCap : renderLineSquareCap(pointsF, x1, y1,     dx, dy, nx, ny); break;
        case Pen::RoundCap  : renderLineRoundCap (pointsF, x1, y1, wh, dx, dy, nx, ny); break;
        default             : renderLineButtCap  (pointsF, x1, y1, wh, dx, dy, nx, ny); break;
    }
    // --- End point ---
    switch(pen.capStyle()) {
        case Pen::SquareCap : renderLineSquareCap(pointsF, x2, y2,     -dx, -dy, -nx, -ny); break;
        case Pen::RoundCap  : renderLineRoundCap (pointsF, x2, y2, wh, -dx, -dy, -nx, -ny); break;
        default             : renderLineButtCap  (pointsF, x2, y2, wh, -dx, -dy, -nx, -ny); break;
    }

    // Perform collision detection and polygon-segment combining as needed
    if(collisionDetection) {
        // Check for intersection with the first polygons in the final destination buffer
        if( ! state.dstPolygons.empty() )
        {
            Polygon& firstPolygon = state.dstPolygons[0];

            const bool r = satDetectPolygonCollision( &firstPolygon.points()[0], firstPolygon.points().size(),
                                                      pointsF.data(), pointsF.size() );
            if(r) {
                // Combine them and exit this function
                sagCombinePolygons(state.dstPolygons, firstPolygon, firstPolygon.points(), pointsF);
                return;
            }
        }
        // Check for intersection with the previous polygons in the final destination buffer
        if( state.dstPolygons.size() > 1 )
        {
            Polygon& prevPolygon = state.dstPolygons.back();

            const bool r = satDetectPolygonCollision( &prevPolygon.points()[0], prevPolygon.points().size(),
                                                      pointsF.data(), pointsF.size() );
            if(r) {
                // Combine them and exit this function
                sagCombinePolygons(state.dstPolygons, prevPolygon, prevPolygon.points(), pointsF);
                return;
            }
        }
    }

    // Simply append the segment to the polygon
    state.dstPolygons.resize(state.dstPolygons.size() + 1);
    state.dstPolygons.back().assign( &pointsF[0], pointsF.size());
}


void Polygonizer::sagGeneratePolyLineSegment(PatternState& state, const Pen& pen, bool collisionDetection)
{
    const PointF* gatherData = state.gather.data();
          size_t  gatherSize = state.gather.size();

    // Adjust the coordinates (thus the line's length) as needed
    // ### TODO: Exclude the first and last segment of dashed line ??? ###
    if( pen.capStyle() != Pen::FlatCap ) {
        // Get the sizes
        const size_t pnz = pen.size();
        const size_t eix = gatherSize - 1;
        // Process the first segment
              float  fdx;
              float  fdy;
        const float  fx1 = state.gather[0].x();
        const float  fy1 = state.gather[0].y();
        const float  fx2 = state.gather[1].x();
        const float  fy2 = state.gather[1].y();
        const float  fll = calculateLineParams(fdx, fdy, fx1, fy1, fx2, fy2, pnz);
        if(!fll) {
            state.gather[0].set(fx1 + fdx * NON_FLAT_CAP_REDUCTION_FACTOR, fy1 + fdy * NON_FLAT_CAP_REDUCTION_FACTOR );
        }
        // Process the last segment
              float  ldx;
              float  ldy;
        const float  lx1 = state.gather[eix - 1].x();
        const float  ly1 = state.gather[eix - 1].y();
        const float  lx2 = state.gather[eix    ].x();
        const float  ly2 = state.gather[eix    ].y();
        const float  lll = calculateLineParams(ldx, ldy, lx1, ly1, lx2, ly2, pnz);
        if(!lll) {
            state.gather[eix].set(lx2 - ldx * NON_FLAT_CAP_REDUCTION_FACTOR, ly2 - ldy * NON_FLAT_CAP_REDUCTION_FACTOR );
        }
        // If the segments are too small, remove them (instead of adjusting the coordinates)
        if(fll && lll) {
            if(fll <= lll) {
                ++gatherData;
                --gatherSize;
            }
            else {
                --gatherSize;
            }
        }
        else {
            if(fll) {
                ++gatherData;
                --gatherSize;
            }
            if(lll) {
                --gatherSize;
            }
        }
    }

    // Generate a new thick polygon
    std::vector<Polygon> polygons;
    renderSolidOpenWidePolyline(polygons, gatherData, gatherSize, pen, true);

    // Exit here if the generated polygon does not actually have a meaningful number of points
    if(polygons.empty() || polygons[0].size() < 3)
        return;

    std::vector<PointF>& pointsF = polygons[0].points();

    // Perform collision detection and polygon-segment combining as needed
    if(collisionDetection) {
        // Check for intersection with the first polygons in the final destination buffer
        if( ! state.dstPolygons.empty() )
        {
            Polygon& firstPolygon = state.dstPolygons[0];

            const bool r = satDetectPolygonCollision( &firstPolygon.points()[0], firstPolygon.points().size(),
                                                      pointsF.data(), pointsF.size() );
            if(r) {
                // Combine them and exit this function
                sagCombinePolygons(state.dstPolygons, firstPolygon, firstPolygon.points(), pointsF);
                return;
            }
        }
        // Check for intersection with the previous polygons in the final destination buffer
        if( state.dstPolygons.size() > 1 )
        {
            Polygon& prevPolygon = state.dstPolygons.back();

            const bool r = satDetectPolygonCollision( &prevPolygon.points()[0], prevPolygon.points().size(),
                                                      pointsF.data(), pointsF.size() );
            if(r) {
                // Combine them and exit this function
                sagCombinePolygons(state.dstPolygons, prevPolygon, prevPolygon.points(), pointsF);
                return;
            }
        }
    }

    // Simply append the segment to the polygon
    state.dstPolygons.resize(state.dstPolygons.size() + 1);
    state.dstPolygons.back().assign( &pointsF[0], pointsF.size());
}


void Polygonizer::sagCombinePolygons(std::vector<Polygon>& allPolys, Polygon& prevPoly, const std::vector<PointF>& poly1, const std::vector<PointF>& poly2)
{
    // Working variables
    ClipperLib::Clipper clipper;
    ClipperLib::Path    cpath;
    ClipperLib::Paths   cpresult;

    // Convert the 1st polygon data
    cpath.resize(poly1.size());
    for(size_t i = 0; i < poly1.size(); ++i) {
        cpath[i].X = lround( poly1[i].x() * Polygonizer::VecResScaleUp );
        cpath[i].Y = lround( poly1[i].y() * Polygonizer::VecResScaleUp );
    }
    clipper.AddPath(cpath, ClipperLib::ptClip, true);

    // Convert the 2nd polygon data
    cpath.resize(poly2.size());
    for(size_t i = 0; i < poly2.size(); ++i) {
        cpath[i].X = lround( poly2[i].x() * Polygonizer::VecResScaleUp );
        cpath[i].Y = lround( poly2[i].y() * Polygonizer::VecResScaleUp );
    }
    clipper.AddPath(cpath, ClipperLib::ptSubject, true);

    // Perform union operation on the two polygons
    clipper.Execute(ClipperLib::ctUnion, cpresult, ClipperLib::pftEvenOdd, ClipperLib::pftEvenOdd);

    // Store back the resulting polygon(s)
    for(size_t i = 0; i < cpresult.size(); ++i) {
        // Get the polygon
        const ClipperLib::Path& curPath = cpresult[i];
        std::vector<PointF>*    dstBuf  = 0;
        // Select the destination buffer
        if(!i) {
            dstBuf = &prevPoly.points();
        }
        else {
            allPolys.resize(allPolys.size() + 1);
            dstBuf = &allPolys.back().points();
        }
        // Resize the destination buffer
        dstBuf->resize(curPath.size());
        // Convert the polygon data
        for(size_t j = 0; j < curPath.size(); ++j) {
            (*dstBuf)[j].setX( curPath[j].X * Polygonizer::VecResScaleDn );
            (*dstBuf)[j].setY( curPath[j].Y * Polygonizer::VecResScaleDn );
        }
    }
}


} // namespace

} // namespace
