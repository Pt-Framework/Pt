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

#include "Rasterizer2.h" // FIXED constants

#include "Polygonizer.h"


namespace Pt {

namespace Gfx {


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


} // namespace

} // namespace
