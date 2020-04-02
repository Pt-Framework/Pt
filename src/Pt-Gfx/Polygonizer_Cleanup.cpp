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


namespace Pt {

namespace Gfx {


void Polygonizer::cleanupOnePolygon(std::vector<PointF>& polygon, bool nonZeroFillingRule)
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
    if(nonZeroFillingRule)
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


void Polygonizer::cleanupAllPolygons(std::vector<Polygon>& polygons, bool nonZeroFillingRule)
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
    if(nonZeroFillingRule)
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


} // namespace

} // namespace
