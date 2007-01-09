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
#include "DrawThickPolyline.h"
#include <limits>

namespace Pt{
namespace Gfx{

DrawThickPolyline::DrawThickPolyline()
{ }

DrawThickPolyline::~DrawThickPolyline()
{ }

void DrawThickPolyline::output( ARgbImage& image,  const Pen& pen, std::vector<RasterBuffer>& lineRasterBuffer)
{
    if( _colorBuffer.size() < image.width() || _colorBuffer[0] != pen.color() )
        _colorBuffer.assign( image.width(), pen.color() );

    ssize_t inc;
    size_t maxy;
    size_t miny;

    for( size_t i = 0; i < lineRasterBuffer.size(); i++ )
    {
        RasterBuffer& rasterBuffer = lineRasterBuffer[i];

        maxy = rasterBuffer.maxy();
        miny = rasterBuffer.miny();
        inc  = 1;

        if( rasterBuffer.spans()[miny].begin() == std::numeric_limits<ssize_t>::max() )
        {
            std::swap( miny, maxy );
            inc = -1;
        }

        while( miny != maxy )
        {
            memcpy( &image.pixel( rasterBuffer.spans()[miny].begin(), miny ), &_colorBuffer[0],
                ( rasterBuffer.spans()[miny].end() - rasterBuffer.spans()[miny].begin())* sizeof(ARgbColor) );

            miny += inc;
        }
    }
}

void DrawThickPolyline::draw( ARgbImage& image,  const Pen& pen, const std::vector<Math::Point>& points )
{
    _lineRasterBuffer.resize( (points.size() / 2 ) + 1);

    size_t line = 0;

    for( size_t i = 1; i < points.size(); ++i )
    {
        _drawThickLine.rasterize(image, pen, points[i-1], points[i], _lineRasterBuffer[line] );
        line++;
    }

    output( image, pen, _lineRasterBuffer );
}

}
}
