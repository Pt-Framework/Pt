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
#include "Pt/Gfx/ImagePainter.h"
#include "Pt/Gfx/ARgbImage.h"
#include "Pt/Gfx/Pen.h"
#include "Pt/Math/Point.h"
#include "Pt/Gfx/Brush.h"
#include "Pt/Gfx/Font.h"
#include "Pt/Gfx/FontMetrics.h"
#include "Pt/String.h"
#include "Pt/System/Clock.h"
#include "Pt/Math/MathUtils.h"

#include "DrawThinLine.h"
#include "DrawThickLine.h"
#include "DrawPolyline.h"
#include "DrawThinPolyline.h"
#include "DrawEllipse.h"
#include "DrawThinEllipse.h"
#include "DrawThickEllipse.h"
#include "DrawText.h"
#include "FillEllipse.h"
#include "FillPolygon.h"
#include "Fill.h"


namespace Pt {

namespace Gfx {

ImagePainter::ImagePainter( ARgbImage& image )
: _image( image )
, _font("Vera", 12)
, _drawLine( 0 )
, _drawThinLine(0 )
, _drawThickLine( 0)
, _drawPolyline( 0 )
, _drawThinPolyline( 0 )
, _drawEllipse( 0 )
, _drawThinEllipse( 0 )
, _drawThickEllipse( 0 )
, _fillPolygon( 0 )
, _drawText( 0 )
{
    std::auto_ptr<FillSolid>              fillSolid( new FillSolid() );
    std::auto_ptr<FillTexture>            fillTexture( new FillTexture() );
    std::auto_ptr<DrawThinLine>           dThinLine( new DrawThinLine );
    std::auto_ptr<DrawThickLine>          dThickLine( new DrawThickLine );
    std::auto_ptr<DrawThinPolyline>       dThinPolyline( new DrawThinPolyline() );
    std::auto_ptr<DrawText>               dText( new DrawText() );
    dText->setFont(_font);
    std::auto_ptr<DrawThinEllipse>        dThinEllipse( new DrawThinEllipse() );
    std::auto_ptr<DrawThickEllipse>       dThickEllipse( new DrawThickEllipse() );
    std::auto_ptr<FillPolygon>            fillPolygon( new FillPolygon() );
    fillPolygon->setOutput( *fillSolid );
    std::auto_ptr<FillEllipse>            fillEllipse( new FillEllipse() );

    _drawThinLine       = dThinLine.release();
    _drawThickLine      = dThickLine.release();
    _drawLine           = _drawThinLine;
    _drawThinPolyline   = dThinPolyline.release();
    _drawPolyline       = _drawThinPolyline;
    _drawText           = dText.release();
    _drawThinEllipse    = dThinEllipse.release();
    _drawThickEllipse    = dThickEllipse.release();
    _drawEllipse        = _drawThinEllipse;
    _fillEllipse        = fillEllipse.release();
    _fillPolygon        = fillPolygon.release();
    _fillSolid          = fillSolid.release();
    _fillTexture        = fillTexture.release();
}

ImagePainter::~ImagePainter()
{
    try {
        delete _drawThinLine;
        delete _drawThickLine;
        delete _drawThinPolyline;
        delete _fillPolygon;
        delete _drawText;
        delete _drawThinEllipse;
        delete _drawThickEllipse;
        delete _fillEllipse;
   }
   catch(...) {}
}

void ImagePainter::setPen( const Pen& pen )
{
    _pen = pen;

    if( _pen.size() == 1 )
    {
        _drawLine       = _drawThinLine;
        _drawPolyline   = _drawThinPolyline;
        _drawEllipse    = _drawThinEllipse;
    }
    else
    {
        _drawLine       = _drawThickLine;
        _drawEllipse    = _drawThickEllipse;
    }
}

const Pen& ImagePainter::pen() const
{
    return _pen;
}

void ImagePainter::setBrush(const Brush& brush)
{
    _brush = brush;

    if(_brush.fillStyle() == Brush::TextureFill)
    {
        _fillPolygon->setOutput( *_fillTexture );
        _fillEllipse->setOutput( *_fillTexture );
    }
    else
    {
        _fillPolygon->setOutput( *_fillSolid );
        _fillEllipse->setOutput( *_fillSolid );
    }
}

const Brush& ImagePainter::brush() const
{
    return _brush;
}

void ImagePainter::setFont(const Font& font)
{
    _font = font;
    _drawText->setFont(_font);
}

const Font& ImagePainter::font() const
{
    return _font;
}

FontMetrics ImagePainter::fontMetrics() const
{
    return fontMetrics(L"");
}

FontMetrics ImagePainter::fontMetrics( String text) const
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
    _image.pixel( to.x(), to.y()) = _pen.color();
}

void ImagePainter::drawLine(const Math::Point& from, const  Math::Point& to)
{
    if( _pen.size()  == 0 )
        return;

    _drawLine->draw(_image, _pen, from, to );
}

void ImagePainter::drawText( const Math::Point& to, const String& text, const Pt::Gfx::ARgbColor* outline )
{
    _drawText->draw( _image, _pen.color(),  to, text, outline );
}

void ImagePainter::drawRect(const  Math::Rect& rect)
{
    //
    // Emulate ImagePainter::drawRect with four calls to
    // ImagePainter::drawLine and correct corner coordinates.
    //
    this->drawLine(rect.topLeft(), rect.topRight() );

    this->drawLine(Pt::Math::Point( rect.topRight().x() - 1, rect.topRight().y()),
                   Pt::Math::Point( rect.bottomRight().x() - 1, rect.bottomRight().y() -1 ) );

    this->drawLine(Pt::Math::Point( rect.bottomRight().x(), rect.bottomRight().y() -1 ),
                   Pt::Math::Point( rect.bottomLeft().x(), rect.bottomLeft().y() -1 ) );

    this->drawLine(Pt::Math::Point( rect.bottomLeft().x(), rect.bottomLeft().y() -1 ),
                   rect.topLeft() );
}

void ImagePainter::fillRect(const  Math::Rect& rect)
{
    std::vector<Pt::Math::Point> points(4);
    points[0] = rect.topLeft();
    points[1] = rect.topRight();
    points[2] = rect.bottomRight();
    points[3] = rect.bottomLeft();
    this->fillPolygon( &points[0], points.size() );
}

void ImagePainter::drawEllipse( const  Math::Point& topLeft, const  Math::Size& size )
{
    _drawEllipse->draw( _image, _pen, topLeft, size );
}

void ImagePainter::fillEllipse( const  Math::Point& topLeft, const  Math::Size& size )
{
    _fillEllipse->draw( _image, _brush, topLeft, size );
}

void ImagePainter::drawPolyline( const  Math::Point* points, const size_t pointCount )
{
    if( _pen.size()  == 0 )
        return;

    for( size_t i = 1; i < pointCount; ++i)
        _drawLine->draw( _image, _pen, points[ i - 1], points[i] );
}

void ImagePainter::fillPolygon( const  Math::Point* points, const size_t pointCount )
{
    std::vector<Math::Point> p( pointCount );
    memcpy( &p[0], points, sizeof( Math::Point) * pointCount );

    _fillPolygon->draw( _image, _brush, p );
}

void ImagePainter::drawImage( const  Math::Point& to, const ARgbImage& image )
{
}

void ImagePainter::drawImage( const  Math::Point& to, const ARgbImage& image, const Region& imageRegion )
{
}

}

}

