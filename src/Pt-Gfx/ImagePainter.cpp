/*
 * Copyright (C) 2006-2007 Laurentiu-Gheorghe Crisan
 * Copyright (C) 2006-2007 Marc Boris Duerner
 * Copyright (C) 2006-2007 PTV AG
 * Copyright (C) 2010 Aloysius Indrayanto
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * As a special exception, you may use this file as part of a free
 * software library without restriction. Specifically, if other files
 * instantiate templates or use macros or inline functions from this
 * file, or you compile this file and link it with other files to
 * produce an executable, this file does not by itself cause the
 * resulting executable to be covered by the GNU General Public
 * License. This exception does not however invalidate any other
 * reasons why the executable file might be covered by the GNU Library
 * General Public License.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */
#include "Pt/Math.h"

#include "Pt/Gfx/ImagePainter.h"
#include "Pt/Gfx/ARgbImage.h"
#include "Pt/Gfx/Pen.h"
#include "Pt/Gfx/Point.h"
#include "Pt/Gfx/Brush.h"
#include "Pt/Gfx/Font.h"
#include "Pt/Gfx/FontMetrics.h"
#include "Pt/String.h"
#include "Pt/System/Clock.h"

#include "DrawPolyline.h"
#include "DrawThinPolyline.h"
#include "DrawWideSolidPolyline.h"
#include "DrawWideDashPolyline.h"

#include "DrawEllipse.h"
#include "DrawThinEllipse.h"
#include "DrawThickEllipse.h"
#include "DrawText.h"
#include "FillEllipse.h"
#include "FillPolygon.h"
#include "Fill.h"
#include "Stroke.h"


namespace Pt {

namespace Gfx {

ImagePainter::ImagePainter( ARgbImage& image )
: _image( image )
, _font("Vera", 12)
, _drawPolyline( 0 )
, _drawThinPolyline(0 )
, _drawWideSolidPolyline( 0)
, _drawEllipse( 0 )
, _drawThinEllipse( 0 )
, _drawThickEllipse( 0 )
, _fillPolygon( 0 )
, _drawText( 0 )
, _stroke( 0 )
, _fillSolid( 0 )
, _fillTexture( 0 )
{
    std::auto_ptr<FillSolid>              fillSolid( new FillSolid() );
    std::auto_ptr<FillTexture>            fillTexture( new FillTexture() );
    std::auto_ptr<Stroke>                 stroke( new Stroke() );
    std::auto_ptr<DrawThinPolyline>       dThinPolyline( new DrawThinPolyline );
    dThinPolyline->setOutput( *stroke );
    std::auto_ptr<DrawWideSolidPolyline>  dWidePolyline( new DrawWideSolidPolyline );
    std::auto_ptr<DrawWideDashPolyline>   dWideDashPolyline( new DrawWideDashPolyline );
    dWidePolyline->setOutput( *stroke );
    dWideDashPolyline->setOutput( *stroke );
    std::auto_ptr<DrawText>               dText( new DrawText() );
    dText->setFont(_font);
    std::auto_ptr<DrawThinEllipse>        dThinEllipse( new DrawThinEllipse() );
    std::auto_ptr<DrawThickEllipse>       dThickEllipse( new DrawThickEllipse() );
    std::auto_ptr<FillPolygon>            fillPolygon( new FillPolygon() );
    fillPolygon->setOutput( *fillSolid );
    std::auto_ptr<FillEllipse>            fillEllipse( new FillEllipse() );


    _drawThinPolyline        = dThinPolyline.release();
    _drawWideSolidPolyline   = dWidePolyline.release();
    _drawWideDashPolyline    = dWideDashPolyline.release();
    _drawPolyline            = _drawThinPolyline;
    _drawText                = dText.release();
    _drawThinEllipse         = dThinEllipse.release();
    _drawThickEllipse        = dThickEllipse.release();
    _drawEllipse             = _drawThinEllipse;
    _fillEllipse             = fillEllipse.release();
    _fillPolygon             = fillPolygon.release();
    _fillSolid               = fillSolid.release();
    _fillTexture             = fillTexture.release();
    _stroke                  = stroke.release();
}

ImagePainter::~ImagePainter()
{
    try {
        delete _drawThinPolyline;
        delete _drawWideSolidPolyline;
        delete _drawWideDashPolyline;
        delete _fillPolygon;
        delete _drawText;
        delete _drawThinEllipse;
        delete _drawThickEllipse;
        delete _fillEllipse;
        delete _stroke;
        delete _fillTexture;
        delete _fillSolid;
   }
   catch(...) {}
}

void ImagePainter::setPen( const Pen& pen )
{
    _pen = pen;

    if( _pen.size() == 1 )
    {
        _drawPolyline   = _drawThinPolyline;
        _drawEllipse    = _drawThinEllipse;
    }
    else
    {
        if( _pen.style() == Pen::SolidStyle )
            _drawPolyline = _drawWideSolidPolyline;
        else
            _drawPolyline = _drawWideDashPolyline;

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

void ImagePainter::drawPixel(const  Gfx::Point& to)
{
    _image.pixel( to.x(), to.y()) = _pen.color();
}

void ImagePainter::drawLine(const Gfx::Point& from, const  Gfx::Point& to)
{
    if( _pen.size()  == 0 )
        return;

    const Pt::Gfx::Point points[] = { from, to };
    _drawPolyline->draw(_image, _pen, points, 2);
}

void ImagePainter::drawText( const Gfx::Point& to, const String& text, const Pt::Gfx::ARgbColor* outline )
{
    _drawText->draw( _image, _pen.color(),  to, text, outline );
}

void ImagePainter::drawRect(const  Gfx::Rect& rect)
{
    //
    // Emulate ImagePainter::drawRect with four calls to
    // ImagePainter::drawLine and correct corner coordinates.
    //
    this->drawLine(rect.topLeft(), rect.topRight() );

    this->drawLine(Pt::Gfx::Point( rect.topRight().x() - 1, rect.topRight().y()),
                   Pt::Gfx::Point( rect.bottomRight().x() - 1, rect.bottomRight().y() -1 ) );

    this->drawLine(Pt::Gfx::Point( rect.bottomRight().x(), rect.bottomRight().y() -1 ),
                   Pt::Gfx::Point( rect.bottomLeft().x(), rect.bottomLeft().y() -1 ) );

    this->drawLine(Pt::Gfx::Point( rect.bottomLeft().x(), rect.bottomLeft().y() -1 ),
                   rect.topLeft() );
}

void ImagePainter::fillRect(const  Gfx::Rect& rect)
{
    std::vector<Pt::Gfx::Point> points(4);
    points[0] = rect.topLeft();
    points[1] = rect.topRight();
    points[2] = rect.bottomRight();
    points[3] = rect.bottomLeft();
    this->fillPolygon( &points[0], points.size() );
}

void ImagePainter::drawEllipse( const  Gfx::Point& topLeft, const  Gfx::Size& size )
{
    _drawEllipse->draw( _image, _pen, topLeft, size );
}

void ImagePainter::fillEllipse( const  Gfx::Point& topLeft, const  Gfx::Size& size )
{
    _fillEllipse->draw( _image, _brush, topLeft, size );
}

void ImagePainter::drawPolyline( const  Gfx::Point* points, const size_t pointCount )
{
    if( _pen.size()  == 0 )
        return;

    _drawPolyline->draw( _image, _pen, points, pointCount );
}

void ImagePainter::fillPolygon( const  Gfx::Point* points, const size_t pointCount )
{
    _fillPolygon->draw( _image, _brush, points, pointCount );
}

void ImagePainter::drawImage( const  Gfx::Point& to, const ARgbImage& image )
{
}

void ImagePainter::drawImage( const  Gfx::Point& to, const ARgbImage& image, const Region& imageRegion )
{
}

}

}

