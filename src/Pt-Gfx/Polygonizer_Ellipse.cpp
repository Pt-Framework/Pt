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
    const Pt::int32_t nSegs   = (circSeg <  9) ? 9 : circSeg;
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


} // namespace

} // namespace
