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


void Polygonizer::renderWidePolyline(std::vector<Polygon>& polygons,
                                      const PointF* points, const std::size_t n,
                                      const Pen& pen,
                                      bool useNonZeroFillingRule)
{
    if( n < 2 )
        return;

    const bool isSolid  = pen.isSolid();
    const bool isClosed = points[0] == points[n - 1];
    const bool isSelfIn = selfIntersecting(points, n);

    // Solid line
    if(isSolid)
    {
        if(isClosed)
        {
            renderSolidClosedWidePolyline(polygons, points, n - 1, pen);
        }
        else
        {
            renderSolidOpenWidePolyline(polygons, points, n, pen, false);
        }
    }
    // Dashed line
    else
    {
        renderDashedWidePolyLine(polygons, points, n, pen, !isSelfIn);
    }

    // Ensure that all self-intersecting polygons are cleaned-up
    if ( isSelfIn || useNonZeroFillingRule )
        cleanupAllPolygons(polygons, useNonZeroFillingRule);
}


void Polygonizer::renderSolidLineSegment(std::vector<PointF>& dst,
                                         float x1, float y1, float x2, float y2,
                                         const Pen& pen, bool openingCap, bool closingCap)
{
    // Calculate the line's parameters
    float wh, dx, dy, nx, ny;

    calculateLineParams(wh, dx, dy, nx, ny, x1, y1, x2, y2, pen.size());

#if 0
    if( !forSmoothCurve && pen.capStyle() != Pen::FlatCap ) {
        x1 += (dx * 0.75f);
        y1 += (dy * 0.75f);
        x2 -= (dx * 0.75f);
        y2 -= (dy * 0.75f);
    }
#endif

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


} // namespace

} // namespace
