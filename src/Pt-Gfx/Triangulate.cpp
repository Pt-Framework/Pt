/* Copyright (C) 2017-2017 Aloysius Indrayanto
   Copyright (C) 2006-2015 Marc Boris Duerner

   http://vterrain.org/Implementation/Libs/triangulate.html
   COTD Entry submitted by John W. Ratcliff [jratcliff@verant.com]

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

#include <cmath>

#include "Triangulate.h"


namespace Pt {
namespace Gfx {


bool Triangulate::process(std::vector<Point>& result, const std::vector<Point>& contour)
{
    // Allocate and initialize list of Vertices in polygon
    const Pt::int32_t n = contour.size();
    if( n < 3 ) return false;

    Pt::int32_t* V = new Pt::int32_t[n];

    // We do not care about the orientation of the polygon
    for(int v = 0; v < n; ++v) V[v] = v;
    /*
    if( area(contour) > 0 ) {
        for(int v = 0; v < n; ++v) V[v] = v;
    }
    else {
        for(int v = 0; v < n; ++v) V[v] = (n - 1) - v;
    }
    */

    // Remove nv-2 Vertices, creating 1 triangle every time
    Pt::int32_t nv    = n;
    Pt::int32_t count = 2 * nv; // Error detection

    for(int m = 0, v = nv - 1; nv > 2;) {
        // If we loop, it is probably a probable bad/non-simple polygon
        if(0 >= (count--)) {
            return false; // Exit error
        }
        // Three consecutive vertices in current polygon <u, v, w>
        Pt::int32_t u = v    ; if (nv <= u) u = 0; // Previous
                    v = u + 1; if (nv <= v) v = 0; // New v
        Pt::int32_t w = v + 1; if (nv <= w) w = 0; // Next
        if( snip(contour, u, v, w, nv, V) ) {
            // True names of the vertices
            const Pt::int32_t a = V[u];
            const Pt::int32_t b = V[v];
            const Pt::int32_t c = V[w];
            // Outputs triangle
            result.push_back( Point( round(contour[a].x()), round(contour[a].y()) ) );
            result.push_back( Point( round(contour[b].x()), round(contour[b].y()) ) );
            result.push_back( Point( round(contour[c].x()), round(contour[c].y()) ) );
            ++m;
            // Remove v from remaining polygon
            Pt::int32_t s, t;
            for(s = v, t = v + 1; t < nv; ++s, ++t) V[s] = V[t];
            --nv;
            // Reset error detection counter
            count = 2 * nv;
        }
    }

    delete V;

    return true;
}

Pt::int32_t Triangulate::area(const std::vector<Point>& contour)
{
    const size_t n = contour.size();
    Pt::int32_t  A = 0;

    for(size_t p = n - 1, q = 0; q < n; p = q++) {
        A += contour[p].x() * contour[q].y() - contour[q].x() * contour[p].y();
    }

    return A / 2;
}

bool Triangulate::snip(const std::vector<Point>& contour, Pt::int32_t u, Pt::int32_t v, Pt::int32_t w, Pt::int32_t n, Pt::int32_t* V)
{

    const Pt::int32_t Ax = contour[ V[u] ].x();
    const Pt::int32_t Ay = contour[ V[u] ].y();

    const Pt::int32_t Bx = contour[ V[v] ].x();
    const Pt::int32_t By = contour[ V[v] ].y();

    const Pt::int32_t Cx = contour[ V[w] ].x();
    const Pt::int32_t Cy = contour[ V[w] ].y();

    if( ( ((Bx - Ax) * (Cy - Ay)) - ((By - Ay) * (Cx - Ax)) ) < 0 ) return false;

    for(int p = 0; p < n; ++p) {
        if( (p == u) || (p == v) || (p == w) ) continue;
        const Pt::int32_t Px = contour[ V[p] ].x();
        const Pt::int32_t Py = contour[ V[p] ].y();
        if( insideTriangle(Ax, Ay, Bx, By, Cx, Cy, Px, Py) ) return false;
    }

    return true;
}

bool Triangulate::insideTriangle(Pt::int32_t Ax, Pt::int32_t Ay, Pt::int32_t Bx, Pt::int32_t By, Pt::int32_t Cx, Pt::int32_t Cy, Pt::int32_t Px, Pt::int32_t Py)
{
    const Pt::int32_t ax  = Cx - Bx;
    const Pt::int32_t ay  = Cy - By;
    const Pt::int32_t bx  = Ax - Cx;
    const Pt::int32_t by  = Ay - Cy;
    const Pt::int32_t cx  = Bx - Ax;
    const Pt::int32_t cy  = By - Ay;
    const Pt::int32_t apx = Px - Ax;
    const Pt::int32_t apy = Py - Ay;
    const Pt::int32_t bpx = Px - Bx;
    const Pt::int32_t bpy = Py - By;
    const Pt::int32_t cpx = Px - Cx;
    const Pt::int32_t cpy = Py - Cy;

    const Pt::int32_t aCROSSbp = ax * bpy - ay * bpx;
    const Pt::int32_t cCROSSap = cx * apy - cy * apx;
    const Pt::int32_t bCROSScp = bx * cpy - by * cpx;

    return ( (aCROSSbp >= 0) && (bCROSScp >= 0) && (cCROSSap >= 0) );
}


} // namespace
} // namespace
