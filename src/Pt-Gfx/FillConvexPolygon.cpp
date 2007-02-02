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
#include "FillConvexPolygon.h"
#include <cassert>

namespace Pt{
namespace Gfx{

void FillConvexPolygon::clip( const Math::Rect& viewPort, std::vector<Math::Point>& points )
{
    _clip.clip( points, viewPort );
}

void FillConvexPolygon::rasterize( std::vector<Math::Point> points, RasterBuffer& rasterBuffer )
{
    Pt::ssize_t x0 = 0;
    Pt::ssize_t y0 = 0;
    Pt::ssize_t x1 = 0;
    Pt::ssize_t y1 = 0;

    assert( points.size() );

    if( points.front() != points.back() )
        points.push_back( points[0] );

    for( size_t lineIndex = 1; lineIndex < points.size(); ++lineIndex )
    {
        x0 = points[ lineIndex -1 ].x();
        y0 = points[ lineIndex -1 ].y();

        x1 = points[ lineIndex ].x();
        y1 = points[ lineIndex ].y();

        const bool steep = std::abs( y1 - y0 ) > std::abs( x1 - x0 ) ;

        if( steep )
        {
            std::swap( x0, y0 );
            std::swap( x1, y1 );
        }

        if( x0 > x1 )
        {
            std::swap( x0, x1 );
            std::swap( y0, y1 );
        }

        int deltax = x1 - x0 ;
        int deltay = std::abs( y1 - y0 );

        register int error = 0;
        int ystep          = -1;
        Pt::ssize_t y     = y0;

        if( y0 < y1 )
            ystep = 1;

        ssize_t x;
        for( x = x0; x <= x1; ++x )
        {
            if( steep )
            {
                rasterBuffer.setMiny( std::min( rasterBuffer.miny(), x ) );
                rasterBuffer.setMaxy( std::max( rasterBuffer.maxy(), x ) );

                rasterBuffer.spans()[x].setBegin( std::min( rasterBuffer.spans()[x].begin(), y ) );
                rasterBuffer.spans()[x].setEnd( std::max( rasterBuffer.spans()[x].end(), y ) );
            }

            error += deltay;

            if( (  error*2 ) >= deltax )
            {
                if( !steep )
                {
                    rasterBuffer.setMiny( std::min( rasterBuffer.miny(), y ) );
                    rasterBuffer.setMaxy( std::max( rasterBuffer.maxy(), y ) );

                    rasterBuffer.spans()[y].setBegin( std::min( rasterBuffer.spans()[y].begin(), x ) );
                    rasterBuffer.spans()[y].setEnd( std::max( rasterBuffer.spans()[y].end(), x ) );
                }

                y       += ystep;
                error   -= deltax;
            }
        }
    }
}

void FillConvexPolygon::output( ARgbImage& image, const Brush& brush, const RasterBuffer& rasterBuffer )
{
    if( rasterBuffer.empty() )
        return;

    if( _colorBuffer.size() < image.width() || _colorBuffer[0] != brush.color() )
        _colorBuffer.assign( image.width(), brush.color() );

    ssize_t inc  = 1;
    ssize_t maxy = rasterBuffer.maxy();
    ssize_t miny = rasterBuffer.miny();

    const std::vector<Span>& spans = rasterBuffer.spans();

    if( spans[miny].begin() == std::numeric_limits<ssize_t>::max() )
    {
        std::swap( miny, maxy );
        inc = -1;
    }

    while( miny != (maxy + 1) )
    {
        memcpy( &image.pixel( spans[miny].begin(), miny ), &_colorBuffer[0],
                ( spans[miny].end() - spans[miny].begin())* sizeof(ARgbColor) );

        miny += inc;
    }
}

void FillConvexPolygon::draw( ARgbImage& image, const Brush& brush, const std::vector<Math::Point>& points )
{
    Math::Rect clippingRect( Math::Point(0,0), Math::Size( image.width() - 1, image.height() - 1 ) );
    std::vector<Math::Point>  clippedPoints( points );

    clip( clippingRect, clippedPoints);

    if( clippedPoints.size() == 0 )
        return;

    _rasterBuffer.clear();
    _rasterBuffer.spans().resize( image.height() );

    rasterize( clippedPoints, _rasterBuffer );

    output( image,  brush, _rasterBuffer );
}


}
}
