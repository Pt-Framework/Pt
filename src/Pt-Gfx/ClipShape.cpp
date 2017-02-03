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


ClipShape::ClipShape()
{}

void ClipShape::clipPolygon(std::vector<Point>& in, const Rect& clippingArea)
{
    if(clippingArea.isNull()) {
        in.clear();
        return;
    }

    std::vector<Point> buf;

    clipEdge( in,  buf, clippingArea.topLeft    (), clippingArea.bottomLeft () ); in .clear();
    clipEdge( buf, in,  clippingArea.bottomLeft (), clippingArea.bottomRight() ); buf.clear();
    clipEdge( in,  buf, clippingArea.bottomRight(), clippingArea.topRight   () ); in .clear();
    clipEdge( buf, in,  clippingArea.topRight   (), clippingArea.topLeft    () );
}

void ClipShape::clipEdge(const std::vector<Point>& in, std::vector<Point>& out,
                            Point edgePoint0, Point edgePoint1)
{
    if(in.empty()) return;

    Point p;
    Point i;
    Point s = in[in.size() - 1];

    for(size_t j = 0; j < in.size(); ++j) {
        p = in[j];
        if(inside(p, edgePoint0, edgePoint1)) {
            if(inside( s, edgePoint0, edgePoint1)) {
                out.push_back(p);
            }
            else {
                i = intersect(p, s, edgePoint0, edgePoint1);
                out.push_back(i);
                out.push_back(p);
            }
        }
        else {
            if(inside(s, edgePoint0, edgePoint1)) {
                i = intersect(s, p, edgePoint0, edgePoint1);
                out.push_back(i);
            }
        }
        s = p;
    }
}

Point ClipShape::intersect( const Point& from, const Point& to, const Point& edge0, Point& edge1)
{
    Point p;

    if(edge0.y() == edge1.y()) {
        if(to.y() == from.y()) {
            if(edge0.y() == to.y()) {
                p.setX(   to.x());
                p.setY(edge0.y());
            }
        }
        else {
            p.setX(from.x() + (to.x() - from.x()) * (edge0.y() - from.y()) / (to.y() - from.y()));
            p.setY(edge0.y());
        }
    }

    if(edge0.x() == edge1.x()) {
        if(to.x() == from.x()) {
            if(to.x() == edge0.x()) {
                p.setY(   to.y());
                p.setX(edge0.x());
            }
        }
        else {
            p.setY(from.y() + (to.y() - from.y()) * (edge0.x() - from.x()) / (to.x() - from.x()));
            p.setX(edge0.x());
        }
    }

    return p;
}


bool ClipShape::inside(const Point& p, const Point& edge0, Point& edge1)
{
    // Vertical
    if(edge0.x() == edge1.x()) {
        if(edge0.y() < edge1.y()) return p.x() >= edge0.x(); // Right is inside
        else                      return p.x() <= edge1.x(); // Left is inside
    }

    // Horizontal
    if(edge0.y() == edge1.y()) {
        if(edge0.x() < edge1.x()) return p.y() <= edge0.y(); // Top is inside
        else                      return p.y() >= edge0.y(); // Left is inside
    }

    return false;
}


} // namespace
} // namespace
