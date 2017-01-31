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


static const float EPSILON = 0.0000000001f;

bool Triangulate::process(std::vector<Point>& result, const std::vector<Point>& contour)
{
    // Allocate and initialize list of Vertices in polygon
    const int n = contour.size();
    if( n < 3 ) return false;

    int* V = new int[n];

    // We want a counter-clockwise polygon in V
    if( 0.0f < area(contour) ) {
        for(int v = 0; v < n; ++v) V[v] = v;
    }
    else {
        for(int v = 0; v < n; ++v) V[v] = (n - 1) - v;
    }

    // Remove nv-2 Vertices, creating 1 triangle every time
    int nv    = n;
    int count = 2 * nv; // Error detection

    for(int m = 0, v = nv - 1; nv > 2;) {
        // If we loop, it is probably a non-simple polygon
        if(0 >= (count--)) {
            // ERROR - probable bad polygon!
            return false;
        }
        // Three consecutive vertices in current polygon <u, v, w>
        int u = v    ; if (nv <= u) u = 0; // Previous
            v = u + 1; if (nv <= v) v = 0; // New v
        int w = v + 1; if (nv <= w) w = 0; // Next

        if( snip(contour, u, v, w, nv, V) ) {
            // True names of the vertices
            const int a = V[u];
            const int b = V[v];
            const int c = V[w];
            // Outputs triangle
            result.push_back( Point( round(contour[a].x()), round(contour[a].y()) ) );
            result.push_back( Point( round(contour[b].x()), round(contour[b].y()) ) );
            result.push_back( Point( round(contour[c].x()), round(contour[c].y()) ) );
            ++m;
            // Remove v from remaining polygon
            int s, t;
            for(s = v, t = v + 1; t < nv; ++s, ++t) V[s] = V[t];
            --nv;
            // Reset error detection counter
            count = 2 * nv;
        }
    }

    delete V;

    return true;
}

float Triangulate::area(const std::vector<Point>& contour)
{
    const int n = contour.size();
    float     A = 0.0f;

    for(int p = n - 1, q = 0; q < n; p = q++) {
        A += contour[p].x() * contour[q].y() - contour[q].x() * contour[p].y();
    }

    return A * 0.5f;
}

bool Triangulate::snip(const std::vector<Point>& contour, int u, int v, int w, int n, int* V)
{

    const float Ax = contour[ V[u] ].x();
    const float Ay = contour[ V[u] ].y();

    const float Bx = contour[ V[v] ].x();
    const float By = contour[ V[v] ].y();

    const float Cx = contour[ V[w] ].x();
    const float Cy = contour[ V[w] ].y();

    if( EPSILON > ( ((Bx - Ax) * (Cy - Ay)) - ((By - Ay) * (Cx - Ax)) ) ) return false;

    for(int p = 0; p < n; ++p) {
        if( (p == u) || (p == v) || (p == w) ) continue;
        const float Px = contour[ V[p] ].x();
        const float Py = contour[ V[p] ].y();
        if( insideTriangle(Ax, Ay, Bx, By, Cx, Cy, Px, Py) ) return false;
    }

    return true;
}

bool Triangulate::insideTriangle(float Ax, float Ay, float Bx, float By, float Cx, float Cy, float Px, float Py)
{
    const float ax  = Cx - Bx;
    const float ay  = Cy - By;
    const float bx  = Ax - Cx;
    const float by  = Ay - Cy;
    const float cx  = Bx - Ax;
    const float cy  = By - Ay;
    const float apx = Px - Ax;
    const float apy = Py - Ay;
    const float bpx = Px - Bx;
    const float bpy = Py - By;
    const float cpx = Px - Cx;
    const float cpy = Py - Cy;

    const float aCROSSbp = ax * bpy - ay * bpx;
    const float cCROSSap = cx * apy - cy * apx;
    const float bCROSScp = bx * cpy - by * cpx;

    return ( (aCROSSbp >= 0.0f) && (bCROSScp >= 0.0f) && (cCROSSap >= 0.0f) );
}


} // namespace
} // namespace
