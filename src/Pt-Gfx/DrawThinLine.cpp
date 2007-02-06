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
#include "DrawThinLine.h"


namespace Pt {
namespace Gfx {

void DrawThinLine::draw( ARgbImage& image, const Pen& pen, const Math::Point& from, const Math::Point& to )
{
    Math::Point clippedFrom( from );
    Math::Point clippedTo( to );

    if( !_clipLine( clippedFrom, clippedTo , 0, image.width() -1, 0, image.height()- 1 ) )
        return;

    if( _colorBuffer.size() < image.width() || _colorBuffer[0] != pen.color() )
        _colorBuffer.assign( image.width(), pen.color() );

    ssize_t     x0      = clippedFrom.x();
    ssize_t     y0      = clippedFrom.y();
    ssize_t     x1      = clippedTo.x();
    ssize_t     y1      = clippedTo.y();

    ssize_t dx = std::abs( x1 - x0 );
    ssize_t dy = std::abs( y1 - y0 );

    if( y0 == y1 )
    {//Horizontal
        memcpy( &image.pixel(  std::min( x1, x0) , y0 ), &_colorBuffer[0], dx * sizeof( ARgbColor)  );
        return;
    }

    if( x0 == x1 )
    {//Vertical

        ssize_t ymin = std::min( y0, y1 );

        for( ssize_t y = 0; y < dy; ++y )
            image.pixel( x0, ymin + y ) = _colorBuffer[0];

        return;
    }

    if( dy > dx )
    { //Steep

        std::swap( x0, y0 );
        std::swap( x1, y1 );

        if( x0 > x1 )
        {
            std::swap( x0, x1 );
            std::swap( y0, y1 );
        }

        const int       deltax = ( x1 - x0 ) ;
        const int       deltay = std::abs( y1 - y0 );
        int             error  = 0;
        int             ystep  = -1;
        int             y      = y0 ;
	
        if( y0 < y1 )
            ystep = 1;            

        for( ssize_t x = x0; x <= x1; ++x )
        {
            image.pixel( y, x ) = _colorBuffer[0];

            error += deltay;

            if( (  error<<2 ) >= deltax )
            {
                y += ystep;
                error -= deltax;
            }
        }                
    }
    else
    {//Flat

        if( x0 > x1 )
        {
            std::swap( x0, x1 );
            std::swap( y0, y1 );
        }

        int error  = -(dx>>2);
        int ystep  = -1;
        int y      = y0;

        if( y0 < y1 )
            ystep = 1;

        for( ssize_t x = x0; x < x1; ++x )
        {
            image.pixel( x, y ) = _colorBuffer[0];
            error += dy;

            if( (  error<<2 ) >= dx )
            {
                y += ystep;
                error -= dx;
            }
        }
    }
}

} // namespace Gfx

} // namespace Pt
