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


void Polygonizer::renderRoundedRectPoints(std::vector<PointF>& dst,
                                          const RectF& rect, float radius,
                                          const Pen& pen)
{
    const float x1 = rect.topLeft    ().x();
    const float y1 = rect.topLeft    ().y();
    const float x2 = rect.bottomRight().x();
    const float y2 = rect.bottomRight().y();

    // PenSize is actually a smoothness value (hence the number of segments of the rounded section)
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


void Polygonizer::renderRoundedRect(std::vector<Polygon>& polygons,
                                     const RectF& rect, float radius, const Pen& pen)
{
    std::vector<PointF> pointsF;
    renderRoundedRectPoints(pointsF, rect, radius, pen);

    if( ! pointsF.empty() )
        pointsF.push_back( pointsF.front() );

    renderWidePolyline( polygons, &pointsF[0], pointsF.size(), pen, !pen.isSolid());
}


void Polygonizer::fillRoundedRect(std::vector<PointF>& pointsF,
                                  const RectF& rect, float radius)
{
    static const Pen defaultPen(Color(), 10, Pen::Solid, Pen::FlatCap, Pen::BevelJoin);

    renderRoundedRectPoints( pointsF, rect, radius, defaultPen );

    if( ! pointsF.empty() )
        pointsF.push_back( pointsF.front() );
}


} // namespace

} // namespace
