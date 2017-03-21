/* Copyright (C) 2006-2015 Laurentiu-Gheorghe Crisan
 * Copyright (C) 2006-2015 Marc Boris Duerner
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA*/
#ifndef PT_GFX_CLIPSHAPE_H
#define PT_GFX_CLIPSHAPE_H

#include <vector>

#include <Pt/Gfx/Rect.h>


namespace Pt{
namespace Gfx{


class ClipShape {
    public:
        inline ClipShape()
        {}

        static inline bool insideXRange(Pt::int32_t v, const Rect& clippingArea)
        { return ( v >= clippingArea.left() && v <= clippingArea.right() ); }

        static inline bool insideYRange(Pt::int32_t v, const Rect& clippingArea)
        { return ( v >= clippingArea.top() && v <= clippingArea.bottom() ); }

        static bool clipLine(Pt::int32_t& x0, Pt::int32_t& y0, Pt::int32_t& x1, Pt::int32_t& y1, const Rect& clip);
        static void clipPolygon(std::vector<Point>& pio, const Rect& clippingArea);

    private:
        // Clip polygon
        enum ClipMode {
            CM_Left, CM_Right, CM_Top, CM_Bottom
        };

    private:
        // Clip line
        static Pt::int32_t csComputeOutcode(Pt::int32_t x, Pt::int32_t y, const Rect& clip);

        // Clip polygon
        static void clipEdge(std::vector<Point>& out, const std::vector<Point>& in, const Point& edge0, const Point& edge1, ClipMode cm);
        static bool inside(const Point& p, const Point& corner, ClipMode cm);
        static const Point intersect(const Point& from, const Point& to, const Point& edge0, const Point& edge1);
};


} // namespace
} // namespace

#endif
