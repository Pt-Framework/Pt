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

#include "Polygonizer.h"
#include "Rasterizer2.h" // FIXED constants

#include "ArcMode.h"

#include "clipper_aj/clipper.hpp"

//
// TODO: move this to Point class
//

namespace {

//
// Based on: Geometric Primitives
//           http://algs4.cs.princeton.edu/91primitives
//           Article and original code by Robert Sedgewick and Kevin Wayne, 2016
double ccw3(const Pt::Gfx::PointF& a,
            const Pt::Gfx::PointF& b,
            const Pt::Gfx::PointF& c)
{
   return (b.x() - a.x()) * (c.y() - a.y()) - (c.x() - a.x()) * (b.y() - a.y());
}

//
// Based on: Geometric Primitives
//           http://algs4.cs.princeton.edu/91primitives
//           Article and original code by Robert Sedgewick and Kevin Wayne, 2016
bool lineIntersecting(const Pt::Gfx::PointF& ap, const Pt::Gfx::PointF& aq,
                      const Pt::Gfx::PointF& bp, const Pt::Gfx::PointF& bq)
{
   if( ccw3(ap, aq, bp) * ccw3(ap, aq, bq) >= 0.0 ) return false;
   if( ccw3(bp, bq, ap) * ccw3(bp, bq, aq) >= 0.0 ) return false;

   return true;
}


bool selfIntersecting(const Pt::Gfx::PointF* points, const size_t n)
{
    if(n <= 2) return false;

    const size_t size1 = n - 1;
    const size_t size2 = n - 2;

    // Check the edges
    for(size_t i = 0; i < size2; ++i) {
        const Pt::Gfx::PointF& ap = points[i    ];
        const Pt::Gfx::PointF& aq = points[i + 1];
        for(size_t j = i + 1; j < size1; ++j) {
            const Pt::Gfx::PointF& bp = points[j    ];
            const Pt::Gfx::PointF& bq = points[j + 1];
            if( lineIntersecting(ap, aq, bp, bq) ) return true;
        }
    }

#if 0
    // Check the last edge against the first edge
    const Pt::Gfx::PointF& ap = points[size2];
    const Pt::Gfx::PointF& aq = points[size1];
    const Pt::Gfx::PointF& bp = points[size1];
    const Pt::Gfx::PointF& bq = points[0    ];
    if( lineIntersecting(ap, aq, bp, bq) ) return true;
#endif

    // No intersection
    return false;
}


bool selfIntersecting(const std::vector<Pt::Gfx::PointF>& points)
{
    return selfIntersecting(&points[0], points.size());
}


// bool selfIntersecting(const std::vector<Pt::Gfx::Polygon>& polygons)
// {
//     for(size_t i = 0; i < polygons.size(); ++i) {
//         if( selfIntersecting(polygons[i].points()) ) return true;
//     }

//     return false;
// }

} // namespace

namespace Pt {

namespace Gfx {

struct PatternState
{
    std::vector<Polygon>& dstPolygons;  // Destination vector

    const PointF*         srcPoints;  // Source points
    size_t                srcCount;   // The number of source points

    float                 cellSize;   // Cell size
    float                 patSegLen;  // Length of the currently processed "pattern" segment

    size_t                idx1;       // Index to the first point which is currently being processed;
                                      // the index to the second point is always (idx1 + 1)

    float                 px, py;     // Current interpolation coordinate (in-between the two points)
    float                 ex, ey;     // Current end coordinate (coordinate of the the second point)
    float                 uvx, uvy;   // Unit vector from the first point to the second point
    float                 cvx, cvy;   // Cell vector from the first point to the second point
    float                 remLen;     // Remaining length between the two points that has not been "consumed" by the "pattern" segment(s)

    std::vector<PointF>   gather;     // Gathered polygon points
    float                 gatherLen;  // Length of the gathered points

    PatternState(std::vector<Polygon>& polygons,
                 const PointF* src, size_t pointCount, size_t penSize)
    : dstPolygons(polygons)
    , srcPoints(src)
    , srcCount(pointCount), cellSize(penSize)
    , idx1(0)
    , remLen(-1.0f)
    , gatherLen(0.0f)
    {}
};


// Rounding factors used for cleaning-up polygons
const double Polygonizer::VecResScaleUp = 64.0;
const double Polygonizer::VecResScaleDn = 1.0 / 64.0;

// Predefined patterns
static const Pt::uint8_t dashPatternDot_ [] = { 1, 1 };
static const Pt::uint8_t dashPatternDash_[] = { 3, 1 };

std::vector<Pt::uint8_t> Polygonizer::dashPatternDot  = std::vector<Pt::uint8_t>( dashPatternDot_,  dashPatternDot_  + sizeof(dashPatternDot_ ) );
std::vector<Pt::uint8_t> Polygonizer::dashPatternDash = std::vector<Pt::uint8_t>( dashPatternDash_, dashPatternDash_ + sizeof(dashPatternDash_) );


//#define DEBUG_INTERSECT_LINE


Polygonizer::Polygonizer()
{
}


void Polygonizer::setPattern(const Pen::Style& style, const Pen::CapStyle& cap,
                             const std::vector<Pt::uint8_t>& userDashPattern, std::size_t penSize)
{
    // Select the pattern
    const std::vector<Pt::uint8_t>* selDashPattern;

    if(!userDashPattern.empty()) {
        selDashPattern = &userDashPattern;
    }
    else {
        switch(style)
        {
            default        :
            case Pen::Dot  : selDashPattern = &dashPatternDot;  break;
            case Pen::Dash : selDashPattern = &dashPatternDash; break;
        }
    }

    // Generate the pattern
    dashPatternBuffer.clear();
    dashPatternBuffer.resize(selDashPattern->size());

    for(unsigned i = 0; i < selDashPattern->size(); ++i)
        dashPatternBuffer[i] = selDashPattern->operator[](i) * penSize;
}


void Polygonizer::renderRoundedRect(std::vector<Polygon>& polygons,
                                     const RectF& rect, float radius, const Pen& pen)
{
    std::vector<PointF> pointsF;
    renderRoundedRectPoints(pointsF, rect, radius, pen );

    if( ! pointsF.empty() )
        pointsF.push_back( pointsF.front() );

    renderWidePolyline( polygons, &pointsF[0], pointsF.size(), pen, !pen.isSolid(), true );
}


void Polygonizer::fillRoundedRect(std::vector<PointF>& pointsF,
                                   const RectF& rect, float radius)
{
    static const Pen defaultPen( Color(), 10, Pen::Solid, Pen::FlatCap, Pen::BevelJoin);

    renderRoundedRectPoints( pointsF, rect, radius, defaultPen );

    if( ! pointsF.empty() )
        pointsF.push_back( pointsF.front() );
}


void Polygonizer::renderRoundedRectPoints(std::vector<PointF>& dst,
                                          const RectF& rect, float radius,
                                          const Pen& pen)
{
    // TODO: penSize is actually a smoothness value

    const float x1 = rect.topLeft    ().x();
    const float y1 = rect.topLeft    ().y();
    const float x2 = rect.bottomRight().x();
    const float y2 = rect.bottomRight().y();

    Pt::int32_t nSegs = Pt::lround( ceil(pen.size() * 0.5f) );
    if(!pen.isSolid()) nSegs = -nSegs;

    // CCW

    // --- Bottom left ---
    renderQuadraticBezierPoints(
        dst,
        x1         , y2 - radius,
        x1         , y2         ,
        x1 + radius, y2         ,
        nSegs
    );

    // --- Bottom middle ---
    //dst.push_back( PointF((x1 + x2) * 0.5f, y2) );

    // --- Bottom right ---
    renderQuadraticBezierPoints(
        dst,
        x2 - radius, y2         ,
        x2,          y2         ,
        x2,          y2 - radius,
        nSegs
    );

    // --- Center right ---
    //dst.push_back( PointF(x2, (y1 + y2) * 0.5f) );

    // --- Top right ---
    renderQuadraticBezierPoints(
        dst,
        x2,          y1 + radius,
        x2,          y1         ,
        x2 - radius, y1         ,
        nSegs
    );

    // --- Top middle ---
    //dst.push_back( PointF((x1 + x2) * 0.5f, y1) );

    // --- Top left ---
    renderQuadraticBezierPoints(
        dst,
        x1 + radius, y1         ,
        x1,          y1         ,
        x1,          y1 + radius,
        nSegs
    );

    // --- Center left ---
    //dst.push_back( PointF(x1, (y1 + y2) * 0.5f) );
}

void Polygonizer::renderEllipse(std::vector<Polygon>& polygons,
                                 const PointF& topLeft, const SizeF& size,
                                 const Pen& pen)
{
    // Calculate the ellipse's parameters
    const size_t      penSize  = pen.size();
    const Pt::int32_t radiusX  = size.width () / 2;
    const Pt::int32_t radiusY  = size.height() / 2;
    const Pt::int32_t centerX  = topLeft.x() + radiusX;
    const Pt::int32_t centerY  = topLeft.y() + radiusY;

    if(pen.isSolid())
    {
        // Copy the pen
        Pen cpen = pen;
        cpen.setSize(0);

        // Calculate the additional ellipse's parameters
        const Pt::int32_t radiusXo = ( size.width () + penSize ) / 2;
        const Pt::int32_t radiusYo = ( size.height() + penSize ) / 2;
        const Pt::int32_t radiusXi = ( size.width () - penSize ) / 2;
        const Pt::int32_t radiusYi = ( size.height() - penSize ) / 2;

        polygons.reserve(polygons.size() + 2);

        // Generate a polygon that approximates the ellipse
        polygons.resize(polygons.size() + 1);
        std::vector<PointF>& pointsOuter = polygons.back().points();
        renderEllipsePoints(pointsOuter, radiusXo, radiusYo, centerX, centerY, cpen);

        polygons.resize(polygons.size() + 1);
        std::vector<PointF>& pointsInner = polygons.back().points();
        renderEllipsePoints(pointsInner, radiusXi, radiusYi, centerX, centerY, cpen);
    }
    else // Patterned
    {
        // Generate a polygon that approximates the ellipse
        std::vector<PointF> pointsF;
        renderEllipsePoints(pointsF, radiusX, radiusY, centerX, centerY, pen);

        renderWidePolyline( polygons, &pointsF[0], pointsF.size(), pen, true, true );
    }
}


void Polygonizer::renderEllipsePoints(std::vector<PointF>& dst,
                                      Pt::int32_t radiusX, Pt::int32_t radiusY,
                                      Pt::int32_t centerX, Pt::int32_t centerY,
                                      const Pen& pen)
{
    // Calculate the ellipse's parameters
    /*
    Pt::int32_t circFac = Pt::lround( sqrt(0.5f * (radiusX * radiusX + radiusY * radiusY) ) /
                                      ( (pen.size() > 4) ? (pen.size() * 0.25f) : 1.0f )
                                    );
    */
    Pt::int32_t circFac = Pt::lround( sqrt(0.5f * (radiusX * radiusX + radiusY * radiusY) ) );

    const Pt::int32_t segMult = !pen.isSolid() ? 10 : 20;
    const Pt::int32_t circSeg = (circFac / 16) * segMult + 1;
    const Pt::int32_t nSegs   = (circSeg <  9) ?  9 : circSeg;
    const float       nSegs1i = 1.0f / (nSegs - 1);

    // Generate a polygon that approximates the ellipse
    for(Pt::int32_t i = 0; i < nSegs; ++i)
    {
        const float angle = piDouble<float>() * i * nSegs1i;

        // Calculate the coordinate
        const float x = centerX + radiusX * std::cos(angle);
        const float y = centerY - radiusY * std::sin(angle); // Sign inversion due to differences between cartesian and computer coordinate systems

        // Store the coordinate only if it is different with the previous one
        if( !dst.empty() && dst.back().x() == x && dst.back().y() == y )
            continue;

        dst.push_back( PointF(x, y) );
    }

    // Discard the last point if it has the same coordinate with the first one
    if(dst.back() == dst[0])
        dst.pop_back();
}


void Polygonizer::renderArc(std::vector<Polygon>& polygons, const ArcMode& arcMode,
                             const PointF& topLeft, const SizeF& size,
                             float degBegin, float degEnd, const Pen& pen)
{
    // Ensure that the begin angle is within the acceptable range
    while(degBegin < -360.0f) degBegin += 360.0f;
    while(degBegin >  360.0f) degBegin -= 360.0f;

    // Ensure that the end angle is within the acceptable range
    while(degEnd < -360.0f) degEnd += 360.0f;
    while(degEnd >  360.0f) degEnd -= 360.0f;

    // Calculate the coordinate shift
    const size_t penSize  = pen.size();
    const size_t penSize2 = penSize / 2;
    const float  degMid   = (degBegin + degEnd) / 2.0f * DegToRadF;
    const float  shiftX   = std::cos(degMid);
    const float  shiftY   = std::sin(degMid);
    const float  shiftXps = shiftX * penSize2;
    const float  shiftYps = shiftY * penSize2;

    // Calculate the angle adjustment factor
    const float aafa = size.width () / 2.0f;
    const float aafb = size.height() / 2.0f;
    const float aafc = Pt::piDouble<float>() * sqrt( (aafa * aafa + aafb * aafb) / 2.0f );
    const float aafd = 360.0f * penSize2 / aafc;

    // Calculate the arc's parameters
    const Pt::int32_t radiusX = size.width () / 2;
    const Pt::int32_t radiusY = size.height() / 2;
    const Pt::int32_t centerX = topLeft.x() + radiusX;
    const Pt::int32_t centerY = topLeft.y() + radiusY;

    if(pen.isSolid())
    {
        // Copy the pen
        Pen cpen = pen;
        cpen.setSize(0);

        // Calculate the additional arc's parameters
        const Pt::int32_t radiusXo   = ( size.width () + penSize ) / 2;
        const Pt::int32_t radiusYo   = ( size.height() + penSize ) / 2;
        const Pt::int32_t radiusXi   = ( size.width () - penSize ) / 2;
        const Pt::int32_t radiusYi   = ( size.height() - penSize ) / 2;
        const Pt::int32_t centerXsub = lround(centerX - shiftXps);
        const Pt::int32_t centerYsub = lround(centerY - shiftYps);
        const Pt::int32_t centerXadd = lround(centerX + shiftXps);
        const Pt::int32_t centerYadd = lround(centerY + shiftYps);

        if(arcMode == ArcMode::Chord)
        {
            // The arc's "outside" lines
            polygons.resize(polygons.size() + 1);
            std::vector<PointF>& pointsOuter = polygons.back().points();

            renderArcPoints(pointsOuter, radiusXo, radiusYo, centerX, centerY, degBegin, degEnd, cpen);
            pointsOuter.push_back( pointsOuter.front() );

            // The arc's "inside" lines
            polygons.resize(polygons.size() + 1);
            std::vector<PointF>& pointsInner = polygons.back().points();

            renderArcPoints(pointsInner, radiusXi, radiusYi, centerX + shiftX, centerY + shiftY, degBegin, degEnd, cpen);
            pointsInner.push_back( pointsInner.front() );
        }
        else if(arcMode == ArcMode::Pie)
        {
            // Calculate the adjusted angle
            const float odegBegin = (degBegin < 0) ? (degBegin - aafd) : (degBegin + aafd);
            const float odegEnd   = (degEnd   < 0) ? (degEnd   - aafd) : (degEnd   + aafd);
            const float idegBegin = (degBegin < 0) ? (degBegin + aafd) : (degBegin - aafd);
            const float idegEnd   = (degEnd   < 0) ? (degEnd   + aafd) : (degEnd   - aafd);

            // The arc's "outside" lines
            polygons.resize(polygons.size() + 1);
            std::vector<PointF>& pointsOuter = polygons.back().points();

            renderArcPoints(pointsOuter, radiusXo, radiusYo, centerX, centerY, odegBegin, odegEnd, cpen);
            pointsOuter.push_back(PointF(centerXsub, centerYsub));
            pointsOuter.push_back( pointsOuter.front() );

            // The arc's "inside" lines
            polygons.resize(polygons.size() + 1);
            std::vector<PointF>& pointsInner = polygons.back().points();

            renderArcPoints(pointsInner, radiusXi, radiusYi, centerX, centerY, idegBegin, idegEnd, cpen);
            pointsInner.push_back(PointF(centerXadd, centerYadd));
            pointsInner.push_back( pointsInner.front() );
        }
        else // ArcMode::Open
        {
            // The arc's "inside" and "outside" lines
            std::vector<PointF> inner, outer;
            renderArcPoints(inner, radiusXi, radiusYi, centerX, centerY, degBegin, degEnd, cpen);
            renderArcPoints(outer, radiusXo, radiusYo, centerX, centerY, degBegin, degEnd, cpen);

            // Combine the arc's lines and add caps
            polygons.resize(polygons.size() + 1);
            std::vector<PointF>& pointsF = polygons.back().points();

            combineLinePointsAndAddCaps(pointsF, inner, outer,
                                        pen.capStyle(), pen.capStyle(), pen.size());
        }
    }
    else // Patterned
    {
        std::vector<PointF> pointsF;

        if(arcMode == ArcMode::Chord)
        {
            renderArcPoints(pointsF, radiusX, radiusY, centerX, centerY, degBegin, degEnd, pen);
            pointsF.push_back( pointsF[0] );
        }
        else if(arcMode == ArcMode::Pie)
        {
            pointsF.push_back( PointF(centerX, centerY) );
            renderArcPoints(pointsF, radiusX, radiusY, centerX, centerY, degBegin, degEnd, pen);
            pointsF.push_back( PointF(centerX, centerY) );
        }
        else // ArcMode::Open
        {
            renderArcPoints(pointsF, radiusX, radiusY, centerX, centerY,
                            degBegin, degEnd, pen);
        }

        renderWidePolyline( polygons, &pointsF[0], pointsF.size(), pen, true, true );
    }
}


void Polygonizer::renderArcPoints(std::vector<PointF>& dst,
                                   Pt::int32_t radiusX, Pt::int32_t radiusY,
                                   Pt::int32_t centerX, Pt::int32_t centerY,
                                   float degBegin, float degEnd, const Pen& pen)
{
    // Calculate the arc's parameters
    const float       degDlt  = degEnd - degBegin;
    const float       degFac  = degDlt / 360.0f;
    /*
    const Pt::int32_t circFac = Pt::lround(
                                    degFac *
                                    sqrt( 0.5f * (radiusX * radiusX + radiusY * radiusY) ) /
                                    ( (pen.size() > 4) ? (pen.size() * 0.25f) : 1.0f )
                                );
    */
    const Pt::int32_t circFac = Pt::lround(
                                    degFac *
                                    sqrt( 0.5f * (radiusX * radiusX + radiusY * radiusY) )
                                );

    const Pt::int32_t segMult = !pen.isSolid() ? 10 : 20;
    const Pt::int32_t circSeg = (circFac / 16) * segMult + 1;
    const Pt::int32_t nSegs   = (circSeg <  9) ?  9 : circSeg;
    const float       nSegs1i = 1.0f / (nSegs - 1);

    // Generate a polygon that approximates the arc
    const float fdegInc = (degDlt   * DegToRadF) * nSegs1i;
          float angle   =  degBegin * DegToRadF;

    for(Pt::int32_t i = 0; i < nSegs; ++i)
    {
        // Calculate the coordinate
        const float x = centerX + radiusX * std::cos(angle);
        const float y = centerY - radiusY * std::sin(angle); // Sign inversion due to differences between cartesian and computer coordinate systems

        // Update the angle
        angle += fdegInc;

        // Store the coordinate only if it is different with the previous one
        if( !dst.empty() && dst.back().x() == x && dst.back().y() == y )
            continue;

        dst.push_back( PointF(x, y) );
    }

    // Discard the last point if it has the same coordinate with the first one
    if(dst.back() == dst[0])
        dst.pop_back();
}


void Polygonizer::cleanupOnePolygon(std::vector<PointF>& polygon, bool useNonZeroFillingRule)
{
    // Working variables
    ClipperLib::Path  cpath;
    ClipperLib::Paths cpaths;

    // Convert the polygon's data
    cpath.resize( polygon.size() );

    for(size_t i = 0; i < polygon.size(); ++i) {
        cpath[i].X = lround( polygon[i].x() * Polygonizer::VecResScaleUp );
        cpath[i].Y = lround( polygon[i].y() * Polygonizer::VecResScaleUp );
    }

    // Simplify the polygon
    if(useNonZeroFillingRule)
        ClipperLib::SimplifyPolygon(cpath, cpaths, ClipperLib::pftNonZero);
    else
        ClipperLib::SimplifyPolygon(cpath, cpaths, ClipperLib::pftEvenOdd);

    // Check if it produces zero polygon or more than one polygons
    if(cpaths.size() != 1) {
        polygon.clear();
        return;
    }

    // Convert back the polygon's data
    polygon.resize(cpaths[0].size());

    for(size_t i = 0; i < cpaths[0].size(); ++i) {
        polygon[i].setX( cpaths[0][i].X * Polygonizer::VecResScaleDn );
        polygon[i].setY( cpaths[0][i].Y * Polygonizer::VecResScaleDn );
    }
}


void Polygonizer::cleanupAllPolygons(std::vector<Polygon>& polygons, bool useNonZeroFillingRule)
{
    // Working variables
    ClipperLib::Paths cpaths;

    // Convert the polygons' data
    cpaths.resize( polygons.size() );

    for(size_t i = 0; i < polygons.size(); ++i) {

        cpaths[i].resize( polygons[i].size() );

        for(size_t j = 0; j < polygons[i].size(); ++j) {
            cpaths[i][j].X = lround( polygons[i].points()[j].x() * Polygonizer::VecResScaleUp );
            cpaths[i][j].Y = lround( polygons[i].points()[j].y() * Polygonizer::VecResScaleUp );
        }
    }

    // Simplify the polygons
    if(useNonZeroFillingRule)
        ClipperLib::SimplifyPolygons(cpaths, ClipperLib::pftNonZero);
    else
        ClipperLib::SimplifyPolygons(cpaths, ClipperLib::pftEvenOdd);

    // Convert back the polygons' data
    polygons.resize( cpaths.size() );

    for(size_t i = 0; i < cpaths.size(); ++i) {

        polygons[i].points().resize( cpaths[i].size() );

        for(size_t j = 0; j < cpaths[i].size(); ++j) {
            polygons[i].points()[j].setX( cpaths[i][j].X * Polygonizer::VecResScaleDn );
            polygons[i].points()[j].setY( cpaths[i][j].Y * Polygonizer::VecResScaleDn );
        }
    }
}


void Polygonizer::renderWidePolyline(std::vector<Polygon>& polygons,
                                      const PointF* points, const std::size_t n,
                                      const Pen& pen,
                                      bool useNonZeroFillingRule, bool forSmoothCurve)
{
    if( n < 2 )
        return;

    const bool isSolid  = pen.isSolid();
    const bool isClosed = points[0] == points[n - 1];
    const bool isSelfIn = selfIntersecting(points, n);

    if(isSolid) // Solid line
    {
        if(isClosed)
        {
            renderSolidClosedWidePolyline(polygons, points, n - 1, pen);
        }
        else
        {
            renderSolidOpenWidePolyline(polygons, points, n, pen, false, false);
        }
    }
    else // Dashed line
    {
        renderDashedWidePolyLine(polygons, points, n, pen, !isSelfIn, forSmoothCurve);
    }

    // Ensure that all self-intersecting polygons are cleaned-up
    if ( isSelfIn || (useNonZeroFillingRule && forSmoothCurve) )
        cleanupAllPolygons(polygons, useNonZeroFillingRule);
}


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


void Polygonizer::renderSolidOpenWidePolyline(std::vector<Polygon>& polygons,
                                              const PointF* basePtr, size_t curPCnt,
                                              const Pen& pen, bool cleanUpSelfIntersection, bool forSmoothCurve)
{
#ifdef DEBUG_INTERSECT_LINE
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
                               to.x(), to.y(), pen, i == 0, i == curPC2, forSmoothCurve);

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
    polygons.resize( polygons.size() + 1 );
    Polygon& polygon = polygons.back();

    polygon.assign( pointsFPolygon.empty() ? 0 : &pointsFPolygon[0],
                    pointsFPolygon.size() );
}


void Polygonizer::renderDashedWidePolyLine(std::vector<Polygon>& polygons,
                                           const PointF* src, size_t pointCount,
                                           const Pen& pen, bool collisionDetection, bool forSmoothCurve)
{
    // Initialize the operational state
    PatternState state(polygons, src, pointCount, 1);

    // Loop until all the polygon's points are processed
    bool     done = false;
    bool     draw = true;
    unsigned n    = 0;
    while( ! done )
    {
        state.patSegLen = dashPatternBuffer[n];

        done = sagPolygonPoints(state, draw, pen, collisionDetection, forSmoothCurve);
        draw = ! draw;

        if(++n >= dashPatternBuffer.size()) n = 0;
    }
}


//
// Based on: Collision Detection Using the Separating Axis Theorem
//           https://gamedevelopment.tutsplus.com/tutorials/collision-detection-using-the-separating-axis-theorem--gamedev-169
//           http://cdn.tutsplus.com/gamedev/uploads/legacy/008_separatingAxisTheorem/SeparatingAxisTheorem.zip
//           Article and original code by Kah Shiu Chong, 2012
void Polygonizer::satDPIProjMinMax(double& min, double& max,
                                   const PointF* points, size_t pointCount,
                                   double px, double py)
{
    min =  Rasterizer2::MaxCoordinateF * Rasterizer2::MaxCoordinateF;
    max = -Rasterizer2::MaxCoordinateF * Rasterizer2::MaxCoordinateF;

    for(size_t i = 0; i < pointCount; ++i) {
        const double val = points[i].x() * px + points[i].y() * py;
        if(val > max) max = val;
        if(val < min) min = val;
    }
}


// Based on: Collision Detection Using the Separating Axis Theorem
//           https://gamedevelopment.tutsplus.com/tutorials/collision-detection-using-the-separating-axis-theorem--gamedev-169
//           http://cdn.tutsplus.com/gamedev/uploads/legacy/008_separatingAxisTheorem/SeparatingAxisTheorem.zip
//           Article and original code by Kah Shiu Chong, 2012
bool Polygonizer::satDetectPolygonCollision(const PointF* poly1, size_t poly1Count,
                                            const PointF* poly2, size_t poly2Count)
{
    // Evaluate using the first polygon's normals
    for(size_t i = 0; i < poly1Count; ++i) {
        // Calculate the indexes
        const size_t idx1 = i;
        const size_t idx2 = ( i == (poly1Count - 1) ) ? 0 : (i + 1);
        // Calculate the normals
        const double dx = poly1[idx2].x() - poly1[idx1].x();
        const double dy = poly1[idx2].y() - poly1[idx1].y();
        const double nx =  dy;
        const double ny = -dx;
        // Get the minimum and maximum projection values
        double min1, max1, min2, max2;
        satDPIProjMinMax(min1, max1, poly1, poly1Count, nx, ny);
        satDPIProjMinMax(min2, max2, poly2, poly2Count, nx, ny);
        // Check if the polygon is separated
        if( max1 < min2 || max2 < min1 ) return false;
        //if( fabs((min2 - max1) / max1) <= 0.003f || fabs((min1 - max2) / max2) <= 0.003f ) return false;
    }

    // Evaluate using the second polygon's normals
    for(size_t i = 0; i < poly1Count; ++i) {
        // Calculate the indexes
        const size_t idx1 = i;
        const size_t idx2 = ( i == (poly2Count - 1) ) ? 0 : (i + 1);
        // Calculate the normals
        const double dx = poly2[idx2].x() - poly2[idx1].x();
        const double dy = poly2[idx2].y() - poly2[idx1].y();
        const double nx =  dy;
        const double ny = -dx;
        // Get the minimum and maximum projection values
        double min1, max1, min2, max2;
        satDPIProjMinMax(min1, max1, poly1, poly1Count, nx, ny);
        satDPIProjMinMax(min2, max2, poly2, poly2Count, nx, ny);
        // Check if the polygon is separated
        if( max1 < min2 || max2 < min1 ) return false;
        //if( fabs((min2 - max1) / max1) <= 0.003f || fabs((min1 - max2) / max2) <= 0.003f ) return false;
    }

    // There is a collision
    return true;
}

bool Polygonizer::sagPolygonPoints(PatternState& state, bool draw, const Pen& pen, bool collisionDetection, bool forSmoothCurve)
{
    // ### TODO: NOT WORKING WELL FOR SMALL-SMOOTH CURVE SUCH AS ROUNDED RECTANGLE, ELLIPSE, ETC. !!! ###

    // Temporary buffer for the generated points
    std::vector<PointF> pointsF;

    // Loop until the current "pattern" segment is completely processed
    while(state.patSegLen > 0.0f) {

        // (Re-)initialize some part of the operational state as needed
        if(state.remLen <= 0.0f) {
            //std::cerr << "A\n";
            // Check if all polygon's points have been processed
            if(state.idx1 + 1 >= state.srcCount) {
                //if(state.remLen > 0.0f) std::cerr << "### state.remLen = " << state.remLen << std::endl;
                //if(state.gatherLen > 0.0f) std::cerr << "### state.gatherLen = " << state.gatherLen << std::endl;
                // Process left-over partial segment (if any)
                if(state.gatherLen && draw) {
                    sagGeneratePolyLineSegment(state, pen, collisionDetection, forSmoothCurve);
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
            const float vz = sqrt(vx * vx + vy * vy);// + 2.0f;
            // Initialize some part of the operational state
            state.px     = x1;
            state.py     = y1;
            state.ex     = x2;
            state.ey     = y2;
            state.uvx    = vx / vz;
            state.uvy    = vy / vz;
            state.cvx    = state.uvx * state.cellSize;
            state.cvy    = state.uvy * state.cellSize;
            state.remLen = vz;
        }

        // If we have the complete length from the gathered points, process them into a thick polygon
        if(state.gatherLen >= state.patSegLen) {
            // Generate one solid polygon segment as needed
            if(draw) {
                //std::cerr << "B\n";
                if(pen.capStyle() == Pen::FlatCap) {
                    state.gather.back().set(
                        state.gather.back().x() + state.cvx,
                        state.gather.back().y() + state.cvy
                    );
                }
                sagGeneratePolyLineSegment(state, pen, collisionDetection, forSmoothCurve);
            }
            else {
                //std::cerr << "#1\n";
            }
            // Reset the "pattern" segment length
            state.patSegLen = 0.0f;
            // Reset the gather buffer
            state.gather.clear();
            state.gatherLen = 0.0f;
            continue;
        }

        // If we have enough remainder length, process the polygon's edge as a simple line segment
        if(state.gather.empty() && state.remLen >= state.patSegLen) {
            // Generate a simple line segment as needed
            if(draw) {
                //std::cerr << "C\n";
                if(pen.capStyle() == Pen::FlatCap) {
                    sagGenerateSimpleLineSegment(
                        state,
                        state.px,
                        state.py,
                        //state.px + state.cvx + state.uvx * state.patSegLen,
                        //state.py + state.cvy + state.uvy * state.patSegLen,
                        state.px + state.uvx * state.patSegLen,
                        state.py + state.uvy * state.patSegLen,
                        pen,
                        collisionDetection
                    );
                }
                else {
                    sagGenerateSimpleLineSegment(
                        state,
                        state.px,
                        state.py,
                        state.px + state.uvx * state.patSegLen - state.uvx,
                        state.py + state.uvy * state.patSegLen - state.uvy,
                        pen,
                        collisionDetection
                    );
                }
            }
            else {
                //std::cerr << "#2\n";
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

        // ------------------------------------------
        // If we got here, it means:
        //     1. The remainder  length is not enough
        //     2. The "gathered" length is not enough
        // ------------------------------------------

        // Store the current interpolation coordinate to the "gather" buffer as needed
        if(state.gather.empty() || state.gather.back().x() != state.px || state.gather.back().y() != state.py) {
            state.gather.push_back(PointF(state.px, state.py));
            continue;
        }
        // If the combined length is less than or equal to the "pattern" segment length, simply store the end coordinate
        if(state.gatherLen + state.remLen <= state.patSegLen) {
            //std::cerr << "#@# 111\n";
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
            //std::cerr << "#@# 222\n";
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

    calculateLineParams(wh, dx, dy, nx, ny, x1, y1, x2, y2, pen.size());

    // #@#
    // Adjust the coordinates (thus the line's length) based on the line and cap styles
    if( !pen.isSolid() && pen.capStyle() != Pen::FlatCap ) {
        x1 += (dx * 0.75f);
        y1 += (dy * 0.75f);
        x2 -= (dx * 0.75f);
        y2 -= (dy * 0.75f);
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


void Polygonizer::sagGeneratePolyLineSegment(PatternState& state, const Pen& pen, bool collisionDetection, bool forSmoothCurve)
{
    // Generate a new thick polygon
    std::vector<Polygon> polygons;
    renderSolidOpenWidePolyline(polygons, state.gather.data(), state.gather.size(), pen, true, forSmoothCurve);

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


void Polygonizer::renderSolidLineSegment(std::vector<PointF>& dst,
                                         float x1, float y1, float x2, float y2,
                                         const Pen& pen, bool openingCap, bool closingCap, bool forSmoothCurve)
{
    // Calculate the line's parameters
    float wh, dx, dy, nx, ny;

    calculateLineParams(wh, dx, dy, nx, ny, x1, y1, x2, y2, pen.size());

    // #@#
    // Adjust the coordinates (thus the line's length) based on the line and cap styles
    if( !forSmoothCurve && !pen.isSolid() && pen.capStyle() != Pen::FlatCap ) {
        x1 += (dx * 0.75f);
        y1 += (dy * 0.75f);
        x2 -= (dx * 0.75f);
        y2 -= (dy * 0.75f);
    }

    // Generate points (CCW)

    // --- Begin point ---
    if( openingCap )
    {
        switch( pen.capStyle() )
        {
            case Pen::SquareCap : renderLineSquareCap(dst, x1, y1,     dx, dy, nx, ny); break;
            case Pen::RoundCap  : renderLineRoundCap (dst, x1, y1, wh, dx, dy, nx, ny); break;
            default             : openingCap = false;
        }
    }

    if( ! openingCap )
        renderLineButtCap(dst, x1, y1, wh, dx, dy, nx, ny);

    // --- End point ---
    if(closingCap)
    {
        switch( pen.capStyle() )
        {
            case Pen::SquareCap : renderLineSquareCap(dst, x2, y2,     -dx, -dy, -nx, -ny); break;
            case Pen::RoundCap  : renderLineRoundCap (dst, x2, y2, wh, -dx, -dy, -nx, -ny); break;
            default             : closingCap = false;
        }
    }

    if( ! closingCap )
        renderLineButtCap(dst, x2, y2, wh, -dx, -dy, -nx, -ny);
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

    /*
    const PointF& ochk1 = oline1b - intersect;
    const PointF& ochk2 = oline2a - intersect;

    inLine |= ( fabs(ochk1.x()) <= 0.8f && fabs(ochk1.y()) <= 0.8f ) || // For preventing artifacts
              ( fabs(ochk2.x()) <= 0.8f && fabs(ochk2.y()) <= 0.8f );
    //*/

    // Store the "outside" line's points to the main polygon buffer
    //const Pen::JoinStyle js1 = (inSameSegment || inLine) ? Pen::MiterJoin : pen.joinStyle();

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
            //if(!in1) return false;
            polygon.push_back(intersect);
            break;
        // Round join
        case Pen::RoundJoin:
            //if(!in1) return false;
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
    const bool in2 = intersectLine(inLine, intersect, iline1a, iline1b, iline2a, iline2b, penSize);

    /*
    const PointF& ichk1 = iline1b - intersect;
    const PointF& ichk2 = iline2a - intersect;

    inLine |= ( fabs(ichk1.x()) <= 0.8f && fabs(ichk1.y()) <= 0.8f ) || // For preventing artifacts
              ( fabs(ichk2.x()) <= 0.8f && fabs(ichk2.y()) <= 0.8f );
    //*/

    // Store the "inside" line's points to the auxiliary polygon buffer
    //const Pen::JoinStyle js2 = (inSameSegment || inLine) ? Pen::MiterJoin : pen.joinStyle();

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

    // Done
    return true;
}


void Polygonizer::combineLinePointsAndAddCaps(std::vector<PointF>& dst,
                                              const std::vector<PointF>& inner,
                                              const std::vector<PointF>& outer,
                                              Pen::CapStyle begCap,
                                              Pen::CapStyle endCap,
                                              size_t penSize)
{
    // Calculate the end lines' parameters
    const Pt::int32_t ox2a = outer[outer.size() - 1].x();
    const Pt::int32_t oy2a = outer[outer.size() - 1].y();
    const Pt::int32_t ox2b = outer[outer.size() - 2].x();
    const Pt::int32_t oy2b = outer[outer.size() - 2].y();
    const Pt::int32_t ix2a = inner[inner.size() - 1].x();
    const Pt::int32_t iy2a = inner[inner.size() - 1].y();
    const Pt::int32_t ix2b = inner[inner.size() - 2].x();
    const Pt::int32_t iy2b = inner[inner.size() - 2].y();
    const float       x2a  = (float) (ox2a + ix2a) * 0.5f;
    const float       y2a  = (float) (oy2a + iy2a) * 0.5f;
    const float       x2b  = (float) (ox2b + ix2b) * 0.5f;
    const float       y2b  = (float) (oy2b + iy2b) * 0.5f;

    // Calculate the line parameters
    float wh2, dx2, dy2, nx2, ny2;
    calculateLineParams(wh2, dx2, dy2, nx2, ny2, x2a, y2a, x2b, y2b, penSize);

    // Generate the end cap
    switch(endCap) {
        case Pen::SquareCap:
            dst.push_back( PointF( ix2a - dx2, iy2a - dy2 ) );
            dst.push_back( PointF( ox2a - dx2, oy2a - dy2 ) );
            break;

        case Pen::RoundCap: {
            std::vector<PointF> tmp;
            renderQuadraticBezierPoints(tmp, ix2a, iy2a, x2a - dx2 * 2.0f, y2a - dy2 * 2.0f, ox2a, oy2a, Pt::lround(ceil(penSize * 0.5f)) - 1);
            if(tmp.size() <= 2) break;
            for(size_t i = 1; i < tmp.size() - 1; ++i) {
                dst.push_back( PointF( tmp[i].x(), tmp[i].y() ) );
            }
            break;
        }
        default:
            break;
    }

    // Store the "outside" points
    dst.insert(dst.end(), outer.rbegin(), outer.rend());

    // Calculate the begin lines' parameters
    const Pt::int32_t ox1a = outer[0].x();
    const Pt::int32_t oy1a = outer[0].y();
    const Pt::int32_t ox1b = outer[1].x();
    const Pt::int32_t oy1b = outer[1].y();
    const Pt::int32_t ix1a = inner[0].x();
    const Pt::int32_t iy1a = inner[0].y();
    const Pt::int32_t ix1b = inner[1].x();
    const Pt::int32_t iy1b = inner[1].y();
    const float       x1a  = (float) (ox1a + ix1a) * 0.5f;
    const float       y1a  = (float) (oy1a + iy1a) * 0.5f;
    const float       x1b  = (float) (ox1b + ix1b) * 0.5f;
    const float       y1b  = (float) (oy1b + iy1b) * 0.5f;

    // Intersect the begin lines
    float wh1, dx1, dy1, nx1, ny1;
    calculateLineParams(wh1, dx1, dy1, nx1, ny1, x1b, y1b, x1a, y1a, penSize);

    // Generate the begin cap
    switch(begCap) {
        case Pen::SquareCap:
            dst.push_back( PointF( ox1a + dx1, oy1a + dy1 ) );
            dst.push_back( PointF( ix1a + dx1, iy1a + dy1 ) );
            break;

        case Pen::RoundCap: {
            std::vector<PointF> tmp;
            renderQuadraticBezierPoints(tmp, ox1a, oy1a, x1a + dx1 * 2.0f, y1a + dy1 * 2.0f, ix1a, iy1a, Pt::lround(ceil(penSize * 0.5f)) - 1);
            if(tmp.size() <= 2) break;
            for(size_t i = 1; i < tmp.size() - 1; ++i) {
                dst.push_back( PointF( tmp[i].x(), tmp[i].y() ) );
            }
            break;
        }
        default:
            break;
    }

    // Store the "inside" points
    dst.insert(dst.end(), inner. begin(), inner. end());
}


void Polygonizer::renderLineButtCap(std::vector<PointF>& dst, float x, float y, float wh, float dx, float dy, float nx, float ny)
{
#if 0
    // Hack for small-width lines?
    if(wh <= 1.0f) {
        renderLineSquareCap(dst, x, y, dx, dy, nx, ny);
        return;
    }
#endif

    dst.push_back( PointF(x + nx, y + ny) );
    dst.push_back( PointF(x - nx, y - ny) );
}


void Polygonizer::renderLineSquareCap(std::vector<PointF>& dst, float x, float y, float dx, float dy, float nx, float ny)
{
    dst.push_back( PointF(x - dx + nx, y - dy + ny) );
    dst.push_back( PointF(x - dx - nx, y - dy - ny) );
}


void Polygonizer::renderLineRoundCap(std::vector<PointF>& dst, float x, float y, float wh, float dx, float dy, float nx, float ny)
{
#if 1
    // Hack for small-width lines?
    if(wh <= 1.5f) {
        dst.push_back( PointF(x + nx,        y + ny       ) );
        dst.push_back( PointF(x - dx * 1.5f, y - dy * 1.5f) );
        dst.push_back( PointF(x - nx,        y - ny       ) );
        return;
    }
#endif

#if 0
    // This one seems produce worse result
    renderQuadraticBezierPoints(
        dst,
        roundf((x + nx     ) * 10.0f) * 0.1f, roundf((y + ny     ) * 10.0f) * 0.1f,
        roundf((x + nx - dx) * 10.0f) * 0.1f, roundf((y + ny - dy) * 10.0f) * 0.1f,
        roundf((x      - dx) * 10.0f) * 0.1f, roundf((y      - dy) * 10.0f) * 0.1f,
        (Pt::int32_t) ceil(wh) - 1
    );
    renderQuadraticBezierPoints(
        dst,
        roundf((x      - dx) * 10.0f) * 0.1f, roundf((y      - dy) * 10.0f) * 0.1f,
        roundf((x - nx - dx) * 10.0f) * 0.1f, roundf((y - ny - dy) * 10.0f) * 0.1f,
        roundf((x - nx     ) * 10.0f) * 0.1f, roundf((y - ny     ) * 10.0f) * 0.1f,
        (Pt::int32_t) ceil(wh) - 1
    );
#else
    // This one seems produce better result
    renderQuadraticBezierPoints(
        dst,
        roundf((x + nx       ) * 10.0f) * 0.1f, roundf((y + ny       ) * 10.0f) * 0.1f,
        roundf((x - dx * 2.0f) * 10.0f) * 0.1f, roundf((y - dy * 2.0f) * 10.0f) * 0.1f,
        roundf((x - nx       ) * 10.0f) * 0.1f, roundf((y - ny       ) * 10.0f) * 0.1f,
        (Pt::int32_t) ceil(wh) - 1
    );
#endif
}


// Based on: Bitmap/B�zier curves/Quadratic
//           https://rosettacode.org/wiki/Bitmap/B%C3%A9zier_curves/Quadratic#C
//           Last modified on February 17, 2017
void Polygonizer::renderQuadraticBezierPoints(std::vector<PointF>& dst,
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

    if( !(xx * sy - yy * sx) )
    {
        // Curvature
        if( dst.empty() || dst.back().x() != x1 || dst.back().y() != y1 )
            dst.push_back( PointF(x1, y1) );

        if( dst.empty() || dst.back().x() != x3 || dst.back().y() != y3 )
            dst.push_back( PointF(x3, y3) );

        return;
    }

    // Ensure that the number of segments are not too few
    if(nSegs < 0) {
        nSegs = -nSegs;
        if(nSegs < 2) nSegs = 2;
    }
    else {
        if(nSegs < 4) nSegs = 4;
    }

    // Calculate the inverse multiplication factor
    const float nSegs1i = 1.0f / (nSegs - 1);

    for(Pt::int32_t i = 0; i < nSegs; ++i)
    {
        // Calculate the coordinates
        const float t  = i * nSegs1i;
        const float it = 1.0f - t;
        const float a  = it * it;
        const float b  = 2.0f * t  * it;
        const float c  = t * t;
        const float x  = a * x1 + b * x2 + c * x3;
        const float y  = a * y1 + b * y2 + c * y3;

        // Check if the coordinate is the same with the previous one
        if( !dst.empty() && ( dst.back().x() == x && dst.back().y() == y ) )
            continue;

        // Store the coordinate
        dst.push_back( PointF(x, y) );
    }
}


void Polygonizer::calculateLineParams(float& wh, float& dx, float& dy,
                                      float& nx, float& ny, float x1, float y1,
                                      float x2, float y2, size_t w)
{
    // Line equation : 0 = aX + By + c
    // Normal        : n = ai + bj
    const float a = y2 - y1;
    const float b = x1 - x2;
  //const float c = -(x1 * y2 - x2 * y1);

    // Line length
    // NOTE: Gfx::Math::fastInvSqrt() will produce artifacts!
    const float ll = ::sqrtf(a * a + b * b);

    // Inverse line length
    const float il = 1.0f / ll;

    // Half-line width
    wh = (float) w * 0.5f;

    // Adjust the half-line width
    wh = floor(wh);
    if( !(w & 1) && wh >= 0.5f ) { // For lines with even widths only
        wh -= 0.5f;
    }

    // Calculate the Direction vector
    dx = -b * il * wh;
    dy =  a * il * wh;

    // Calculate the normal vector
    nx =  a * il * wh;
    ny =  b * il * wh;
}


bool Polygonizer::intersectLine(bool& inLine, PointF& intersect,
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

#ifdef DEBUG_INTERSECT_LINE
    fprintf(stderr, "Line 1       : (%7.3f, %7.3f) - (%7.3f, %7.3f)\n", x11, y11, x12, y12);
    fprintf(stderr, "Line 2       : (%7.3f, %7.3f) - (%7.3f, %7.3f)\n", x21, y21, x22, y22);
    fprintf(stderr, "a1b2 a2b1 dn : (%7.3f, %7.3f) - (%7.3f, %7.3f) : %7.3f\n", a1, b2, a2, b1, denom);
#endif

    if(denom == 0.0f) {
        // Check for special cases
        if(y11 == y12 && y11 == y21 && y11 == y22 && x12 == x21) {
            intersect.set(x12, y11);
            inLine = true;
#ifdef DEBUG_INTERSECT_LINE
            fprintf(stderr, "Intersect #Y : (%7.3f, %7.3f) - %s\n", intersect.x(), intersect.y(), inLine ? "inline" : "outline");
#endif
            return true;
        }
        if(x11 == x12 && x11 == x21 && x11 == x22 && y12 == y21) {
            intersect.set(x11, y12);
            inLine = true;
#ifdef DEBUG_INTERSECT_LINE
            fprintf(stderr, "Intersect #X : (%7.3f, %7.3f) - %s\n", intersect.x(), intersect.y(), inLine ? "inline" : "outline");
#endif
            return true;
        }
        // No intersection
#ifdef DEBUG_INTERSECT_LINE
        fprintf(stderr, "Intersect NONE \n");
#endif
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

#ifdef DEBUG_INTERSECT_LINE
    fprintf(stderr, "Intersect #G : (%7.3f, %7.3f) - %s\n", intersect.x(), intersect.y(), inLine ? "inline" : "outline");
#endif

    // Done
    return true;
}


} // namespace

} // namespace
