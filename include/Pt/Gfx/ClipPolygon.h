/***************************************************************************
 *   Copyright (C) 2006-2007 Laurentiu-Gheorghe Crisan                     *
 *   Copyright (C) 2006-2007 Marc Boris Duerner                            *
 *   Copyright (C) 2006-2007 PTV AG                                        *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this program; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef PT_GFX_CLIPPOLYGON_H
#define PT_GFX_CLIPPOLYGON_H

#include <Pt/Math/Point.h>
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
        void operator() (std::vector<Pt::Math::Point>& in,
                         const Pt::Gfx::Rect& clippingArea )
        { this-> clip(in, clippingArea); }

        /** @brief Perform clipping

            The polygon described by a vector of points is clipped
            against a clipping rectangle. The vector of points will
            be modified, thus the clipping results in a new polygon.

            @param in Polygon points
            @param clippingArea Rectangle to clip against

        */
        void clip( std::vector<Pt::Math::Point>& in,
                   const Pt::Gfx::Rect& clippingArea );

    private:
        enum Orientation{Left, Right, Top, Bottom} ;

        void clipEdge( const std::vector<Pt::Math::Point>& in,
                       std::vector<Pt::Math::Point>& out,
                       Pt::Math::Point edgePoint0, Pt::Math::Point edgePoint1);

        Pt::Math::Point intersect( const Pt::Math::Point& from,
                                   const Pt::Math::Point& to,
                                   const Pt::Math::Point& edge0,
                                   Pt::Math::Point& edge1 );

        bool inside( const Pt::Math::Point& p, const Pt::Math::Point& edge0,
                     Pt::Math::Point& edge1 );
};

}

}

#endif
