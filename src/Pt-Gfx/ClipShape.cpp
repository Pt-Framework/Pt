/*
 * Copyright (C) 2006-2007 Laurentiu-Gheorghe Crisan
 * Copyright (C) 2006-2007 Marc Boris Duerner
 * Copyright (C) 2017-2017 Aloysius Indrayanto
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * As a special exception, you may use this file as part of a free
 * software library without restriction. Specifically, if other files
 * instantiate templates or use macros or inline functions from this
 * file, or you compile this file and link it with other files to
 * produce an executable, this file does not by itself cause the
 * resulting executable to be covered by the GNU General Public
 * License. This exception does not however invalidate any other
 * reasons why the executable file might be covered by the GNU Library
 * General Public License.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "ClipShape.h"


namespace Pt{
namespace Gfx{


// ======================================================================================
// ===== Clip Line ======================================================================
// ======================================================================================

static const int CS_Inside = 0; // 0000
static const int CS_Left   = 1; // 0001
static const int CS_Right  = 2; // 0010
static const int CS_Bottom = 4; // 0100
static const int CS_Top    = 8; // 1000

// Cohen–Sutherland clipping algorithm clips a line from (x0, y0) to (x1, y1)
// against a clip rectangle (https://en.wikipedia.org/wiki/Cohen–Sutherland_algorithm)
bool ClipShape::clipLine(Pt::int32_t& x0, Pt::int32_t& y0, Pt::int32_t& x1, Pt::int32_t& y1, const Rect& clip)
{
    // Compute the initial outcodes for the endpoints
    int outcode0 = csComputeOutcode(x0, y0, clip);
    int outcode1 = csComputeOutcode(x1, y1, clip);

    bool        accept = false;
    Pt::int32_t x      = 0;
    Pt::int32_t y      = 0;

    while(true) {
        // Both endpoints are inside the clip region
        if(!(outcode0 | outcode1)) {
            accept = true;
            break;
        }
        // Both endpoints are outside the clip region
        else if (outcode0 & outcode1) {
            break;
        }
        // At least one endpoint is outside the clip rectangle
        else {
            // Pick the one that is outside the clip rectangle
            // and find the intersection point using:
            //     y = y0 + (x - x0) * slope
            //     x = x0 + (y - y0) * slope
            int outcodeOut = outcode0 ? outcode0 : outcode1;
            // Endpoint is above the clip rectangle
            if(outcodeOut & CS_Top) {
                x = x0 + (x1 - x0) * (clip.top   () - y0) / (y1 - y0);
                y = clip.top();
            }
            // Endpoint is below the clip rectangle
            else if(outcodeOut & CS_Bottom) {
                x = x0 + (x1 - x0) * (clip.bottom() - y0) / (y1 - y0);
                y = clip.bottom();
            }
            // Endpoint is to the right of clip rectangle
            else if(outcodeOut & CS_Right) {
                y = y0 + (y1 - y0) * (clip.right () - x0) / (x1 - x0);
                x = clip.right();
            }
            // Endpoint is to the left of clip rectangle
            else if(outcodeOut & CS_Left) {
                y = y0 + (y1 - y0) * (clip.left  () - x0) / (x1 - x0);
                x = clip.left();
            }
            // Replace the endpoint which is outside the clip rectangle
            // with the intersection point and run the next pass
            if(outcodeOut == outcode0) {
                x0 = x;
                y0 = y;
                outcode0 = csComputeOutcode(x0, y0, clip);
            }
            else {
                x1 = x;
                y1 = y;
                outcode1 = csComputeOutcode(x1, y1, clip);
            }
        }
    }

    return accept;
}

// Compute the bit code for a point (x, y) using the clip rectangle
Pt::int32_t ClipShape::csComputeOutcode(Pt::int32_t x, Pt::int32_t y, const Rect& clip)
{
    int code = CS_Inside; // Initialised as being inside of the clip region

         if(x < clip.left  ()) code |= CS_Left;   // to the left of clip region
    else if(x > clip.right ()) code |= CS_Right;  // to the right of clip region
         if(y < clip.top   ()) code |= CS_Top;    // above the clip region
    else if(y > clip.bottom()) code |= CS_Bottom; // below the clip region

    return code;
}


// ======================================================================================
// ===== Clip Polygon ===================================================================
// ======================================================================================

void ClipShape::clipPolygon(std::vector<Point>& pio, const Rect& clippingArea)
{
    if(clippingArea.isNull()) {
        pio.clear();
        return;
    }

    std::vector<Point> tmp;

    clipEdge(tmp, pio, clippingArea.topLeft   (), clippingArea.bottomLeft  (), CM_Left  );
    clipEdge(pio, tmp, clippingArea.topRight  (), clippingArea.bottomRight (), CM_Right );
    clipEdge(tmp, pio, clippingArea.topLeft   (), clippingArea.topRight    (), CM_Top   );
    clipEdge(pio, tmp, clippingArea.bottomLeft(), clippingArea.bottomRight (), CM_Bottom);
}

void ClipShape::clipEdge(std::vector<Point>& out, const std::vector<Point>& in, const Point& edge0, const Point& edge1, ClipMode cm)
{
    out.clear();
    if(in.empty()) return;

    const size_t size1 = in.size() - 1;

    for(size_t i = 0; i <= size1; ++i) {
        const Point& s       = in[i];
        const Point& p       = in[(i == size1) ? 0 : (i + 1)];
        const bool   sInside = inside(s, edge0, cm);
        const bool   pInside = inside(p, edge0, cm);
        if(sInside && pInside) {
            out.push_back(p);
        }
        else if(sInside && !pInside) {
            out.push_back(intersect(s, p, edge0, edge1));
        }
        else if(!sInside && pInside) {
            out.push_back(intersect(s, p, edge0, edge1));
            out.push_back(p);
        }
    }
}

bool ClipShape::inside(const Point& p, const Point& corner, ClipMode cm)
{
    switch(cm) {
        case CM_Left   : return p.x() >= corner.x();
        case CM_Right  : return p.x() <= corner.x();
        case CM_Top    : return p.y() >= corner.y();
        case CM_Bottom : return p.y() <= corner.y();
    }

    return false;
}

const Point ClipShape::intersect(const Point& from, const Point& to, const Point& edge0, const Point& edge1)
{
    Point p;

    // Horizontal clip edge
    if(edge0.y() == edge1.y()) {
        // Abnormal case - the polygon edge is parallel with the clip edge
        if(to.y() == from.y()) {
                 if(edge0.y() == to  .y()) p.set( to  .x(),   to.y() );
            else if(edge0.y() == from.y()) p.set( from.x(), from.y() );
        }
        // Normal case
        else {
            const Pt::int32_t dx = to   .x() - from.x();
            const Pt::int32_t dy = to   .y() - from.y();
            const Pt::int32_t de = edge0.y() - from.y();
            p.set( from.x() + dx * de / dy, edge0.y() );
        }
    }

    // Vertical clip edge
    if(edge0.x() == edge1.x()) {
        // Abnormal case - the polygon edge is parallel with the clip edge
        if(to.x() == from.x()) {
                 if(to  .x() == edge0.x())  p.set( to  .x(),  to  .y() );
            else if(from.x() == edge0.x())  p.set( from.x(),  from.y() );
        }
        // Normal case
        else {
            const Pt::int32_t dx = to   .x() - from.x();
            const Pt::int32_t dy = to   .y() - from.y();
            const Pt::int32_t de = edge0.x() - from.x();
            p.set( edge0.x(), from.y() + dy * de / dx );
        }
    }

    return p;
}


} // namespace
} // namespace
