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
#include "Pt/Gfx/ClipPolygon.h"
#include <cassert>


namespace Pt{

namespace Gfx{

ClipPolygon::ClipPolygon()
{ }

void ClipPolygon::clip( std::vector<Pt::Math::Point>& in, const Pt::Gfx::Rect& clippingArea )
{
    if( clippingArea.isNull())
    {
        in.clear();
        return;
    }

    std::vector<Pt::Math::Point> buffer;

    clipEdge( in, buffer, clippingArea.topLeft(), clippingArea.bottomLeft() );

    in.clear();
    clipEdge( buffer, in, clippingArea.bottomLeft(), clippingArea.bottomRight() );

    buffer.clear();
    clipEdge( in, buffer, clippingArea.bottomRight(), clippingArea.topRight() );

    in.clear();
    clipEdge( buffer, in, clippingArea.topRight(), clippingArea.topLeft() );
}


void ClipPolygon::clipEdge( const std::vector<Pt::Math::Point>& in, std::vector<Pt::Math::Point>& out,
                 Pt::Math::Point edgePoint0, Pt::Math::Point edgePoint1)
{
    if( in.empty() )
        return;

    Pt::Math::Point p;
    Pt::Math::Point i;
    Pt::Math::Point s = in[ in.size() - 1 ];

    for( size_t j = 0; j < in.size(); ++j )
    {
        p = in[j];
        if( inside( p, edgePoint0, edgePoint1 ))
        {
            if( inside( s, edgePoint0, edgePoint1 ))
            {
                out.push_back( p );
            }
            else
            {
                i = intersect( p, s, edgePoint0, edgePoint1 );
                out.push_back( i );
                out.push_back( p );
            }
        }
        else
        {
            if( inside( s, edgePoint0, edgePoint1 ) )
            {
                i = intersect( s, p, edgePoint0, edgePoint1 );
                out.push_back( i );
            }
        }
        s = p;
    }
}


Pt::Math::Point ClipPolygon::intersect( const Pt::Math::Point& from, const Pt::Math::Point& to, const Pt::Math::Point& edge0, Pt::Math::Point& edge1 )
{
    Pt::Math::Point p;

    if( edge0.y() == edge1.y() )
    {
        p.setX(  from.x() + ( to.x() - from.x() ) * ( edge0.y() - from.y() ) / ( to.y() - from.y() ) );
        p.setY( edge0.y() );
    }

    if( edge0.x() == edge1.x() )
    {
        p.setY( from.y() + ( to.y() - from.y() ) * ( edge0.x() - from.x() ) / ( to.x() - from.x() ) );
        p.setX( edge0.x() );
    }

    return p;
}


bool ClipPolygon::inside( const Pt::Math::Point& p, const Pt::Math::Point& edge0, Pt::Math::Point& edge1 )
{
/*  TODO: is very performant but at time has a logic bug :).
      return ( ( (  edge0.x() == edge1.x() ) && ( edge0.y() < edge1.y()  ) && ( p.x() > edge0.x() ) ) ||
             ( (  edge0.x() == edge1.x() ) && ( edge0.y() >= edge1.y() ) && ( p.x() < edge1.x() ) ) ||
             ( (  edge0.y() == edge1.y() ) && ( edge0.x() < edge1.x()  ) && ( p.y() < edge0.y() ) ) ||
             ( (  edge0.y() == edge1.y() ) && ( edge0.x() >= edge1.x() ) && ( p.y() > edge0.y() ) ) );  */


    if( edge0.x() == edge1.x())
    {  //Vertical
        if( edge0.y() < edge1.y())
        {//Right is inside.
            return p.x() > edge0.x();
        }
        else
        {//Left is inside.
            return p.x() < edge1.x();
        }
    }

    if( edge0.y() == edge1.y() )
    {//Horizontal
        if(  edge0.x() < edge1.x() )
        {//Top is inside.
            return p.y() < edge0.y();
        }
        else
        {//Left is inside.
            return p.y() > edge0.y();
        }
    }
    assert( false );
    return false;
}

}//namespace Gfx
}//namespace Pt
