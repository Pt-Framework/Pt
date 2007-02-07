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
#include "FillPolygon.h"
#include "Pt/Math/Rect.h"
#include "Pt/System/Clock.h"

#include <iostream>
#include <algorithm>


namespace Pt {

namespace Gfx {

FillPolygon::FillPolygon()
: _colorBuffer( 3000 )
{ }


void FillPolygon::draw( ARgbImage& image, const Brush& brush, std::vector<Math::Point>& points )
{
    //Pt::System::Clock clock;
    //clock.start();

    _clipper(points, Pt::Math::Rect( Pt::Math::Point(0,0), Pt::Math::Size( image.width(), image.height() )) );

    if( points.end() != points.begin() )
        points.push_back( points[0] );


    // Time: 8e-06

    if( points.empty())
        return;

    if( _colorBuffer.size() < image.width() || _colorBuffer[0] != brush.color() )
        _colorBuffer.assign( image.width(), brush.color() );

    // might as well create a new table here...
    _globalEdgeTable.clear();

    //
    // Fill the global edge table. Two points yield an edge.
    //
    Edge edge;
    Pt::Math::Point* bottom = 0;
    Pt::Math::Point* top = 0;
    Pt::ssize_t xmin = std::numeric_limits<Pt::ssize_t>::max();
    Pt::ssize_t ymin = std::numeric_limits<Pt::ssize_t>::max();
    for( size_t i = 1; i < points.size(); ++i )
    {
        //
        // Find out which point is above and which is below
        //
        if ( points[i-1].y() > points[i].y() )
        {
            bottom = &( points[i-1] );
            top = &( points[i] );
        }
        else
        {
            bottom = &(points[i]);
            top = &(points[i-1]);
        }

        xmin = std::min( xmin, top->x() );
        xmin = std::min( xmin, bottom->x() );
        ymin = std::min( ymin, top->y() );
        ymin = std::min( ymin, bottom->y() );

        //
        // Omit horizontal edges, add others to global edge table. The GET
        // is sorted by primarily by the edges ymin and secondarily by
        // the x value of the edge
        //
        if( top->y() != bottom->y() )
        {
            const int dy   = bottom->y() - top->y();
            const int dx   = bottom->x() - top->x();

            edge.ymax = bottom->y();  //- 1;   -1 so we don't get last scanline */
            edge.ymin = top->y();
            edge.x    = top->x();

            //
            // Bresenham stuff...
            //
            if (dx < 0)
            {
                edge.m = dx / dy;
                edge.m1 = edge.m - 1; 
                edge.incr1 = -2 * dx + 2 * dy * edge.m1;
                edge.incr2 = -2 * dx + 2 * dy * edge.m;
                edge.d = 2 * edge.m * dy - 2 * dx - 2 * dy;
            }
            else
            {
                edge.m = dx / dy;
                edge.m1 = edge.m + 1;
                edge.incr1 = 2 * dx - 2 * dy * edge.m1;
                edge.incr2 = 2 * dx - 2 * dy * edge.m;
                edge.d = -2 * edge.m * dy + 2 * dx;
            }

            _globalEdgeTable.insert( edge );
        }
    }

	//
	// if all polygon points are on one line the GET will be empty
	// 
	if( _globalEdgeTable.empty() )
		return;
		
    // Time: 1.5e-05

    //
    // Start at ymin of the first entry in the GET.
    //
    size_t scanLine = _globalEdgeTable.begin()->ymin;

    //
    // move active edges to AET for current scanline. Keep iterator where
    // we stopped for later use.
    //
    EdgeSet::iterator it = _globalEdgeTable.begin();
    for( ; it != _globalEdgeTable.end() && it->ymin == scanLine; ++it )
        _activeEdgeTable.addEdge( *it );

    // Time: 1.9e-05

    do
    {
        //
        // Fill all spans in the target image
        //
        this->outputTexture( image, brush, xmin, ymin, scanLine );

        //
        // now we are done with the current active edges and can update
        // them for the next scanline.
        //
        scanLine++;
        _activeEdgeTable.update( scanLine );

        //
        // move active edges to AET for current scanline
        //
        for( ; it != _globalEdgeTable.end() && it->ymin == scanLine; ++it )
            _activeEdgeTable.addEdge( *it );

        //
        // Need to resort the AET, because of update and new edges
        //
        _activeEdgeTable.sort();
    }
    while( !_activeEdgeTable.empty() );

    // Time             : 0.000458
    // Time( no output ): 0.000102

    //Pt::System::TimeValue time = clock.stop();
    //std::cerr << "Image Time: " << time.seconds() + time.microSeconds() / 1000000.0 << std::endl;
}


void FillPolygon::output( Pt::Gfx::ARgbImage& image, size_t scanLine )
{
    //
    // fill every even span, starting at even (even-odd-rule)
    //
    for( size_t i = 1; i < _activeEdgeTable.size(); i += 2 )
    {
        // TODO: Investigate why we need max/min out the x values. :-/
        const size_t xmax   = std::max(_activeEdgeTable[i].x, _activeEdgeTable[i-1].x);
        const size_t xmin   = std::min(_activeEdgeTable[i].x, _activeEdgeTable[i-1].x);
        const size_t length = (xmax - xmin);
        const size_t size   = length * sizeof(ARgbColor);
        if(length)
            memcpy( &image.pixel( xmin, scanLine ), &_colorBuffer[0], size );
    }
}


void FillPolygon::outputTexture( Pt::Gfx::ARgbImage& image, const Brush& brush, Pt::ssize_t xmin, Pt::ssize_t ymin, size_t scanLine )
{
    // texture to copy to image
    const Pt::Gfx::ARgbImage& texture = brush.texture();

    // determine the scanline of the texture to copy
    const size_t textureYPos = (scanLine-ymin) % texture.height();

    for( size_t i = 1; i < _activeEdgeTable.size(); i += 2 )
    {
        // start and length of the span to fill
        const size_t xmax  = std::max(_activeEdgeTable[i].x, _activeEdgeTable[i-1].x);
        size_t xpos   = std::min(_activeEdgeTable[i].x, _activeEdgeTable[i-1].x);
        size_t length = (xmax - xpos);

        if(length)
        {
            // x position in the texture to copy from
            const size_t textureXPos = (xpos - xmin) % texture.width();

            // number of pixels to copy from texture
            const size_t fillLength = std::min( length, texture.width() - textureXPos );

            if(fillLength)
            {
                std::memcpy( &image.pixel( xpos, scanLine ),
                             &texture.pixel(textureXPos, textureYPos),
                             fillLength * sizeof(ARgbColor) );
            }

            length -= fillLength;
            xpos   += fillLength;
        }

        // copy source texture scanline until end of target span
        while(length)
        {
            const size_t fillLength = std::min(texture.width(), length);

            if(fillLength)
            {
                std::memcpy( &image.pixel( xpos, scanLine ),
                             &texture.pixel(0, textureYPos),
                             fillLength * sizeof(ARgbColor) );
            }

            length -= fillLength;
            xpos   += fillLength;
        }
    }
}









/* OLD
void FillPolygon::setupGlobalEdgeTable(  std::vector<Math::Point>& points )
{
    Edge edge;

    _globalEdgeTable.clear();

    for( size_t i = 1; i < points.size(); ++i )
    {
        edge.dy = points[i].y() - points[i-1].y();

        if( edge.dy == 0 )
            continue;

        edge.dx     = points[i].x() - points[i-1].x();
        edge.xaccu  = edge.dx;

        if( points[i-1].y() < points[i].y() )
        {
            edge.ymin = points[i-1].y();
            edge.ymax = points[i].y() ;
            edge.x    = points[i-1].x();
        }
        else
        {
            edge.ymin = points[i].y();
            edge.ymin = points[i].y();
            edge.ymax = points[i-1].y() ;
            edge.x    = points[i].x();
        }

        _globalEdgeTable.insert( edge );
    }
}
*/



}//namespace Pt
}//namespace Gfx


