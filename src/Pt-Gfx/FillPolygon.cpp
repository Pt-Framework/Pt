/***************************************************************************
 *   Copyright (C) 2006-2007 Laurentiu-Gheorghe Crisan                     *
 *   Copyright (C) 2006-2007 Dr. Marc Boris Duerner                            *
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
: _fill(0)
{ }


void FillPolygon::draw( ARgbImage& image, const Brush& brush, const Math::Point* points_, size_t pointCount )
{
    std::vector<Math::Point> points( pointCount );
    std::memcpy( &points[0], points_ , sizeof( Math::Point) * pointCount );

    // find unclipped origin coordinates
    //
    Math::Point origin( std::numeric_limits<Pt::ssize_t>::max(), std::numeric_limits<Pt::ssize_t>::max() );
    //Pt::ssize_t xorig = ;
    //Pt::ssize_t yorig = std::numeric_limits<Pt::ssize_t>::max();
    for(size_t n = 0; n < points.size(); ++n)
    {
        origin.setX( std::min( origin.x(), points[n].x() ) );
        origin.setY( std::min( origin.y(), points[n].y() ) );
    }

    _clipper(points, Pt::Math::Rect( Pt::Math::Point(0,0), Pt::Math::Size( image.width(), image.height() )) );

    if( points.end() != points.begin() )
        points.push_back( points[0] );


    // Time: 8e-06

    if( points.empty())
        return;

    // might as well create a new table here...
    _globalEdgeTable.clear();

    //
    // Fill the global edge table. Two points yield an edge.
    //
    Edge edge;
    Pt::Math::Point* bottom = 0;
    Pt::Math::Point* top = 0;

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
    ssize_t scanLine = _globalEdgeTable.begin()->ymin;

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
        // fill every even span, starting at even (even-odd-rule)
        //
        for( size_t i = 1; i < _activeEdgeTable.size(); i += 2 )
        {
            const size_t xend   = std::max(_activeEdgeTable[i].x, _activeEdgeTable[i-1].x);
            const size_t xbegin   = std::min(_activeEdgeTable[i].x, _activeEdgeTable[i-1].x);
            const size_t length = (xend - xbegin);

            if(_fill)
                _fill->fill(image, brush, origin, xbegin, scanLine, length);
        }

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

}//namespace Pt

}//namespace Gfx


