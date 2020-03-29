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


#ifndef PT_GFX_POLYGONIZER_INLINE_H
#define PT_GFX_POLYGONIZER_INLINE_H


//#define DEBUG_INTERSECT_LINE


namespace Pt {

namespace Gfx {


//
// Pattern state information structure
//
struct PatternState
{
    std::vector<Polygon>& dstPolygons;  // Destination vector

    const PointF*         srcPoints;   // Source points
    size_t                srcCount;    // The number of source points

    float                 cellSize;    // Cell size
    float                 patSegLen;   // Length of the currently processed "pattern" segment

    size_t                idx1;        // Index to the first point which is currently being processed;
                                       // the index to the second point is always (idx1 + 1)

    float                 px, py;      // Current interpolation coordinate (in-between the two points)
    float                 ex, ey;      // Current end coordinate (coordinate of the the second point)
    float                 uvx, uvy;    // Unit vector from the first point to the second point
    float                 cvx, cvy;    // Cell vector from the first point to the second point
    float                 remLen;      // Remaining length between the two points that has not been "consumed" by the "pattern" segment(s)

    std::vector<PointF>   gather;      // Gathered polygon points
    float                 gatherLen;   // Length of the gathered points

    PatternState(std::vector<Polygon>& polygons, const PointF* src, size_t pointCount, size_t penSize)
    : dstPolygons(polygons)
    , srcPoints(src)
    , srcCount(pointCount), cellSize(penSize)
    , idx1(0)
    , remLen(-1.0f)
    , gatherLen(0.0f)
    {}
};


//
// Based on: Geometric Primitives
//           http://algs4.cs.princeton.edu/91primitives
//           Article and original code by Robert Sedgewick and Kevin Wayne, 2016
static inline double ccw3(const Pt::Gfx::PointF& a,
                          const Pt::Gfx::PointF& b,
                          const Pt::Gfx::PointF& c)
{
   return (b.x() - a.x()) * (c.y() - a.y()) - (c.x() - a.x()) * (b.y() - a.y());
}


//
// Based on: Geometric Primitives
//           http://algs4.cs.princeton.edu/91primitives
//           Article and original code by Robert Sedgewick and Kevin Wayne, 2016
static inline bool lineIntersecting(const Pt::Gfx::PointF& ap, const Pt::Gfx::PointF& aq,
                                    const Pt::Gfx::PointF& bp, const Pt::Gfx::PointF& bq)
{
   if( ccw3(ap, aq, bp) * ccw3(ap, aq, bq) >= 0.0 ) return false;
   if( ccw3(bp, bq, ap) * ccw3(bp, bq, aq) >= 0.0 ) return false;

   return true;
}


static inline bool selfIntersecting(const Pt::Gfx::PointF* points, const size_t n)
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


static inline bool selfIntersecting(const std::vector<Pt::Gfx::PointF>& points)
{
    return selfIntersecting(&points[0], points.size());
}


static inline bool selfIntersecting(const std::vector<Pt::Gfx::Polygon>& polygons)
{
    for(size_t i = 0; i < polygons.size(); ++i) {
        if( selfIntersecting(polygons[i].points()) ) return true;
    }

    return false;
}


} // namespace

} // namespace


#endif
