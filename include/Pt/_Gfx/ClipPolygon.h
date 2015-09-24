/*
 * Copyright (C) 2006-2007 Laurentiu-Gheorghe Crisan
 * Copyright (C) 2006-2007 Marc Boris Duerner
 * Copyright (C) 2006-2007 PTV AG
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
#ifndef PT_GFX_CLIPPOLYGON_H
#define PT_GFX_CLIPPOLYGON_H

#include <Pt/Gfx/Point.h>
#include <Pt/Gfx/Gfx.h>
#include <Pt/Gfx/Rect.h>

#include <vector>


namespace Pt{

namespace Gfx{

/** @brief Polygon clipper

    This class is a function object that can perform clipping
    of polygons against a specified area. The polygon may have
    a complex shape.
 */
class ClipPolygon
{
    public:
        /** @brief Default constructor
            The default constructor does nothing.
        */
        ClipPolygon();

        /** @brief Perform clipping

            @see ClipPolygon::clip
        */
        void operator() (std::vector<Pt::Gfx::PointF>& in,
                         const Pt::Gfx::RectF& clippingArea )
        { this-> clip(in, clippingArea); }

        /** @brief Perform clipping

            The polygon described by a vector of points is clipped
            against a clipping rectangle. The vector of points will
            be modified, thus the clipping results in a new polygon.

            @param in Polygon points
            @param clippingArea Rectangle to clip against

        */
        void clip( std::vector<Pt::Gfx::PointF>& in,
                   const Pt::Gfx::RectF& clippingArea );

    private:
        enum Orientation{Left, Right, Top, Bottom} ;

        void clipEdge( const std::vector<Pt::Gfx::PointF>& in,
                       std::vector<Pt::Gfx::PointF>& out,
                       Pt::Gfx::PointF edgePoint0, Pt::Gfx::PointF edgePoint1);

        Pt::Gfx::PointF intersect( const Pt::Gfx::PointF& from,
                                   const Pt::Gfx::PointF& to,
                                   const Pt::Gfx::PointF& edge0,
                                   Pt::Gfx::PointF& edge1 );

        bool inside( const Pt::Gfx::PointF& p, const Pt::Gfx::PointF& edge0,
                     Pt::Gfx::PointF& edge1 );
};

}

}

#endif
