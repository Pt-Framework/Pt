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
#ifndef PT_GFX_FILLPOLYGON_H
#define PT_GFX_FILLPOLYGON_H

#include <Pt/Gfx/ARgbImage.h>
#include <Pt/Gfx/Brush.h>
#include <Pt/Math/Point.h>
#include <Pt/System/Clock.h>
#include "Edge.h"
#include "EdgeTable.h"


namespace Pt{

namespace Gfx{

/** @brief Fill polygons on images

    This class is an interface for all function objects that can
    fill polygons.
 */
class FillPolygon
{
    public:
        FillPolygon();
        
        virtual ~FillPolygon()
        { }

        /** @brief Fill a polygon on an image

            @see FillPolygon::draw
        */
        void operator() ( ARgbImage& image, const Brush& brush,
                          std::vector<Math::Point>& points )
        { this->draw(image, brush, points); }

        /** @brief Fill a polygon on an image

            The polygon described by a vector of points will be filled on an
            ARgbImage. The attributes for the fill operation are taken from
            the passed Brush object.

            @param image Target image
            @param pen Brush to be used
            @param points Polygon points
        */
        virtual void draw( ARgbImage& image, const Brush& brush,
                           std::vector<Math::Point>& points );

private:

    void setupGlobalEdgeTable( const std::vector<Math::Point>& points );

    inline void output( Pt::Gfx::ARgbImage& image, size_t scanLine )
    {
        for( size_t i = 1; i < _activeEdgeTable.size(); i += 2 )
        {
            const size_t        deltax  = ( _activeEdgeTable[i].x - _activeEdgeTable[i-1].x );

            if( deltax == 0)
                continue;

            const size_t        size    = deltax * sizeof(ARgbColor) ;
            const Pt::ssize_t   x       = (size_t) _activeEdgeTable[i-1].x;

            memcpy( &image.pixel( x, scanLine ), &_colorBuffer[0], size );
        }
    }

    inline void addEdgeToActiveTable( EdgeSet::iterator& it, size_t scanLine )
    {
        for( ; it != _globalEdgeTable.end() && it->ymin == scanLine; ++it )
            _activeEdgeTable.addEdge( *it );
    }

    EdgeSet                 _globalEdgeTable;
    ActiveEdgeTable         _activeEdgeTable;
    EdgeSet::iterator       _currentPos;
    std::vector<ARgbColor>  _colorBuffer;
    Pt::System::Clock       _clock; 
};

} //namespace Gfx
} //namespace Pt

#endif
