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

//#include <stdio.h>

#include "ArcMode.h"

#include "Polygonizer.h"


namespace Pt {

namespace Gfx {

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
    const Pt::int32_t nSegs   = (circSeg <  9) ? 9 : circSeg;
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

#if 0
    double minX = 99999;
    double minY = 99999;
    for(size_t i = 0; i < dst.size(); ++i)
    {
        const double x = dst[i].x();
        const double y = dst[i].y();
        if(x < minX) minX = x;
        if(y < minY) minY = y;
    }
    for(size_t i = 0; i < dst.size(); ++i)
    {
        fprintf(stderr, "            points.push_back( Pt::Gfx::PointF( x + %17.14f * scale, y + %17.14f * scale ) );\n", dst[i].x() - minX, dst[i].y() - minY );
    }
#endif

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


} // namespace

} // namespace
