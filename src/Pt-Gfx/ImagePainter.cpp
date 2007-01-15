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
#include "freetype/include/ft2build.h"
#include FT_FREETYPE_H
#include FT_GLYPH_H

#include "Vera.h"
#include "Pt/Gfx/ImagePainter.h"
#include "Pt/Gfx/ARgbImage.h"
#include "Pt/Gfx/Pen.h"
#include "Pt/Math/Point.h"
#include "Pt/Gfx/Brush.h"
#include "Pt/Gfx/Font.h"
#include "Pt/Gfx/FontMetrics.h"
#include "Pt/Text/String.h"

#include "DrawThinLine.h"
#include "DrawThickLine.h"
#include "DrawPolyline.h"
#include "DrawThinPolyline.h"
#include "DrawThickPolyline.h"
#include "FillConvexPolygon.h"
#include "DrawText.h"
#include <cmath>
#include <Pt/System/Clock.h>

namespace Pt{
namespace Gfx{


ImagePainter::ImagePainter( ARgbImage& image )
: _image( image )
, _drawLine( 0 )
, _drawThinLine( new DrawThinLine() )
, _drawThickLine( new DrawThickLine() )
, _drawPolyline( 0 )
, _drawThinPolyline( new DrawThinPolyline() )
, _drawThickPolyline( new DrawThickPolyline() )
, _fillConvexPolygon( new FillConvexPolygon() )
, _drawText( new DrawText() )
{
     _drawLine      = _drawThinLine.get();
     _drawPolyline  = _drawThinPolyline.get();
}

ImagePainter::~ImagePainter()
{ }

void ImagePainter::setPen( const Pen& pen )
{
    _pen = pen;

    if( _pen.size() == 1 )
    {
        _drawLine       = _drawThinLine.get();
        _drawPolyline   = _drawThinPolyline.get();
    }
    else
    {
        _drawLine       = _drawThickLine.get();
        _drawPolyline   = _drawThickPolyline.get();
    }
}


const Pen& ImagePainter::pen() const
{
    return _pen;
}

void ImagePainter::setBrush(const Brush& brush)
{
    _brush = brush;
}

const Brush& ImagePainter::brush() const
{
    return _brush;
}

void ImagePainter::setFont(const Font& font)
{
    _drawText->setFont(font);

}
const Font& ImagePainter::font() const
{
    static const Font font( "", 10 );
    return font;
}

FontMetrics ImagePainter::fontMetrics() const
{
    return fontMetrics(L"");
}

FontMetrics ImagePainter::fontMetrics( Text::String text) const
{
    return _drawText->fontMetrics( text );
}

const std::list<std::string>& ImagePainter::fontFamilyNames()
{
    static const std::list<std::string> empty;
    return empty;
}

void ImagePainter::drawPixel(const  Math::Point& to)
{
}

void ImagePainter::drawLine(const Math::Point& from, const  Math::Point& to)
{
    if( _pen.size()  == 0 )
        return;

    _drawLine->draw( _image, _pen, from, to );
}

void ImagePainter::drawText( const Math::Point& to, const Text::String& text, const Pt::Gfx::ARgbColor* outline )
{
    _drawText->draw( _image, _pen,  to, text, outline );   
}

void ImagePainter::drawRect(const  Math::Rect& rect)
{
}

void ImagePainter::fillRect(const  Math::Rect& rect)
{
}

void ImagePainter::drawEllipse(const  Math::Point& topLeft, const  Math::Size& size)
{
}

void ImagePainter::fillEllipse(const  Math::Point& topLeft, const  Math::Size& size)
{
}

void ImagePainter::drawPolyline(const  Math::Point* points, const size_t pointCount)
{
    if( _pen.size()  == 0 )
        return;

    for( size_t i = 1; i < pointCount; ++i)
        _drawLine->draw( _image, _pen, points[ i - 1], points[i] );

/*
    std::vector<Math::Point> p( pointCount );
    memcpy( &p[0], points, sizeof( Math::Point) * pointCount );

    _drawPolyline->draw(_image, _pen, p );
    */
}

void ImagePainter::fillPolygon(const  Math::Point* points, const size_t pointCount)
{
    std::vector<Math::Point> p( pointCount );
    memcpy( &p[0], points, sizeof( Math::Point) * pointCount );

    _fillConvexPolygon->draw( _image, _brush, p );
}

void ImagePainter::drawImage(const  Math::Point& to, const ARgbImage& image)
{
}

void ImagePainter::drawImage(const  Math::Point& to, const ARgbImage& image, const Region& imageRegion)
{
}

}
}
