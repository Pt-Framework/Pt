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
#include <Pt/Math/Point.h>
#include <Pt/Math/MathUtils.h>

#include "DrawThickLine.h"
#include "DrawThinLine.h"

namespace Pt {
namespace Gfx {

DrawThickLine::DrawThickLine()
{}

void DrawThickLine::rasterize( ARgbImage& image, const Pen& pen,
                               const Math::Point& from, const Math::Point to, RasterBuffer& rasterBuffer )
{
    std::vector<Math::Point>   polygon;

    const int   dx      = to.x() - from.x() ;
    const int   dy      = to.y() - from.y() ;
    const float halfPen = pen.size() / 2;
    const float L       = Math::hypot( dx,  dy );
    const float r       = halfPen / L;
    const size_t ya      = static_cast<size_t>( -r * dx );
    const size_t xa      = static_cast<size_t>( r * dy );

    polygon.push_back( Math::Point( from.x() - xa, from.y() - ya ) );
    polygon.push_back( Math::Point( to.x() - xa, to.y() - ya ) );
    polygon.push_back( Math::Point( to.x() + xa, to.y() + ya) );
    polygon.push_back( Math::Point( from.x() + xa, from.y() + ya) );

    Math::Rect clippingRect( Math::Point( 0, 0 ), Math::Size( image.width() - 1, image.height() - 1 ) );

    _fillConvexPolygon.clip( clippingRect, polygon );

    rasterBuffer.clear();
    rasterBuffer.spans().resize( image.height() );

    if( polygon.size() == 0 )
        return;

    _fillConvexPolygon.rasterize(  polygon, rasterBuffer );
}


void DrawThickLine::draw( ARgbImage& image, const Pen& pen, const Math::Point& from, const Math::Point& to )
{
    Brush brush( pen.color() );

    rasterize( image, pen, from, to , _rasterBuffer );

    _fillConvexPolygon.output( image, brush, _rasterBuffer );
}


} // namespace Gfx

} // namespace Pt
