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

namespace Pt{
namespace Gfx{

DrawThinLine::DrawThinLine()
{
    _dashPattern.push_back(true);
    _dashPattern.push_back(true);
    _dashPattern.push_back(true);
    _dashPattern.push_back(false);
}

DrawThinLine::~DrawThinLine()
{ }

void DrawThinLine::draw( ARgbImage& image, const Pen& pen, const Math::Point& from, const Math::Point& to )
{
    Math::Point clippedFrom( from );
    Math::Point clippedTo( to );

    if( !_clipLine( clippedFrom, clippedTo , 0, image.width(), 0, image.height() ) )
        return;

    if( _colorBuffer.size() < image.width() || _colorBuffer[0] != pen.color() )
        _colorBuffer.assign( image.width(), pen.color() );

    switch( pen.style() )
    {
        case Pen::SolidStyle:
            drawSolid( image, pen, clippedFrom, clippedTo );
        break;
        case Pen::DashStyle:
            drawPattern( image, pen, clippedFrom, clippedTo, _dashPattern );
        break;
    }
}

void DrawThinLine::drawPattern( ARgbImage& image, const Pen& pen, const Math::Point& from, 
                               const Math::Point& to, const std::vector<bool>& pattern )
{
    ssize_t     x0      = from.x();
    ssize_t     y0      = from.y();
    ssize_t     x1      = to.x();
    ssize_t     y1      = to.y();

    ssize_t dx = std::abs( x1 - x0 );
    ssize_t dy = std::abs( y1 - y0 );

    if( y0 == y1 )
    {//Horizontal
        ssize_t xmin = std::min( x0, x1 );

        for( ssize_t x  = 0; x < dx; ++x )
            if( pattern[ x % pattern.size() ] )
                outputPixel( image, pen, xmin+ x, y0 );

        return;
    }

    if( x0 == x1 )
    {//Vertical

        ssize_t ymin = std::min( y0, y1 );

        for( ssize_t y = 0; y < dy; ++y )
        {
            if( pattern[y%pattern.size()] )
                outputPixel( image, pen, x0, ymin+ y );
        }

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
        int             error  = -(deltax>>2);
        int             ystep  = -1;
        int             y      = y0 ;

        if( y0 < y1 )
            ystep = 1;

        for( ssize_t x = x0; x <= x1; ++x )
        {
            if( pattern[ x%pattern.size()] )
                outputPixel( image, pen, y,x);

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
            if( pattern[ x%pattern.size()] )
                outputPixel(image, pen, x, y);

            error += dy;

            if( (  error<<2 ) >= dx )
            {
                y += ystep;
                error -= dx;
            }
        }
    }
}

void DrawThinLine::drawSolid( ARgbImage& image, const Pen& pen, const Math::Point& from, const Math::Point& to )
{
    ssize_t     x0      = from.x();
    ssize_t     y0      = from.y();
    ssize_t     x1      = to.x();
    ssize_t     y1      = to.y();

    ssize_t dx = std::abs( x1 - x0 );
    ssize_t dy = std::abs( y1 - y0 );

    if( y0 == y1 )
    {//Horizontal
        outputSpan( image, pen, std::min( x1, x0), y0, dx );
        return;
    }

    if( x0 == x1 )
    {//Vertical

        ssize_t ymin = std::min( y0, y1 );

        for( ssize_t y = 0; y < dy; ++y )
            outputPixel( image, pen, x0, ymin + y );

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
        int             error  = -(deltax>>2);
        int             ystep  = -1;
        int             y      = y0 ;

        if( y0 < y1 )
            ystep = 1;

        for( ssize_t x = x0; x <= x1; ++x )
        {
            outputPixel( image, pen, y, x);

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
            outputPixel( image, pen, x, y );
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
