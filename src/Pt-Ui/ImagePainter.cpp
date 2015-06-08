/*
 * Copyright (C) 2006-2015 Laurentiu-Gheorghe Crisan
 * Copyright (C) 2006-2015 Marc Boris Duerner
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA*/
#include <Pt/Math.h>
#include <Pt/Ui/ImagePainter.h>
#include <Pt/Ui/Image.h>
#include <Pt/Ui/Pen.h>
#include <Pt/Ui/Point.h>
#include <Pt/Ui/Brush.h>
#include <Pt/Ui/Font.h>
#include <Pt/Ui/FontMetrics.h>
#include <Pt/String.h>
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
namespace Ui {

ImagePainter::ImagePainter( Image& image )
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
, _renderMode( RenderMode::NoAlpha )
{
    std::auto_ptr<FillSolid>              fillSolid( new FillSolid() );
    std::auto_ptr<FillTexture>            fillTexture( new FillTexture() );
    std::auto_ptr<Stroke>                 stroke( new Stroke(_renderMode) );
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
    try 
    {
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


void ImagePainter::setRenderMode( RenderMode::Type mode)
{
	_renderMode = mode;
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

        _drawEllipse = _drawThickEllipse;
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
    return fontMetrics( Pt::String() );
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

void ImagePainter::drawPixel(const  PointF& to)
{
    _image.setColor( to.x(), to.y(), _pen.color() );
}

void ImagePainter::drawLine(const PointF& from, const  PointF& to)
{
    if( _pen.size()  == 0 )
        return;

    const PointF points[] = { from ,  to  };

    _drawPolyline->draw(_image, _pen, points, 2);
}

void ImagePainter::drawText( const PointF& to, const String& text, const Color* outline )
{
    _drawText->draw( _image, _pen.color(), to , text, outline );
}

void ImagePainter::drawRect(const  RectF& rect)
{
    
    this->drawLine(rect.topLeft(), rect.topRight() );

    this->drawLine(PointF( rect.topRight().x(), rect.topRight().y()),
                   PointF( rect.bottomRight().x(), rect.bottomRight().y()) );

    this->drawLine(PointF( rect.bottomRight().x(), rect.bottomRight().y() ),
                   PointF( rect.bottomLeft().x(), rect.bottomLeft().y() ) );

    this->drawLine(PointF( rect.bottomLeft().x(), rect.bottomLeft().y() ),
                   rect.topLeft() );
}

void ImagePainter::fillRect(const  RectF& rect)
{
    std::vector<PointF> points(4);
    points[0] = rect.topLeft();
    points[1] = rect.topRight();
    points[2] = rect.bottomRight();
    points[3] = rect.bottomLeft();
    this->fillPolygon( &points[0], points.size() );
}

void ImagePainter::drawEllipse( const  PointF& topLeft, const  SizeF& size )
{
    _drawEllipse->draw( _image, _pen,  topLeft , size );
}

void ImagePainter::fillEllipse( const  PointF& topLeft, const  SizeF& size )
{
    _fillEllipse->draw( _image, _brush, topLeft, size );
}

void ImagePainter::drawPolyline( const  PointF* points, const size_t pointCount )
{
  if( _pen.size()  == 0 )
      return;	
	
  _drawPolyline->draw( _image, _pen, points, pointCount );
}

void ImagePainter::fillPolygon( const  PointF* points, const size_t pointCount )
{
    _fillPolygon->draw( _image, _brush, points, pointCount );
}

void ImagePainter::drawImage( const  PointF& to, const Image& sourceImage )
{
	if( sourceImage.format() != _image.format() )
		throw std::logic_error( "wrong image format");

  //Cliping  X,width
  Pt::ssize_t startX =  to.x();
  Pt::ssize_t stopX  =  to.x() + sourceImage.width();
    
  if( startX < 0 )
      startX = 0;
    
  if( static_cast<size_t>(startX) > _image.width() )
      startX = _image.width();

  if( stopX < 0)
      stopX = 0;
    
  if( static_cast<size_t>(stopX) > _image.width() )
      stopX = _image.width();

		//Cliping Y,height
  Pt::ssize_t startY =  to.y();
  Pt::ssize_t stopY  =  to.y() + sourceImage.height();
        
  if( startY < 0 )
      startY = 0;
    
  if( static_cast<size_t>(startY) > _image.height() )
      startY = _image.height();

  if( stopY < 0 )
      stopY = 0;
    
  if( static_cast<size_t>(stopY) > _image.height() )
      stopY = _image.height();
	
  //Render
  switch( _renderMode )
  {
    case RenderMode::NoAlpha:
    {
      for( Pt::ssize_t y = startY; y < stopY; ++y )
      {				
        Pt::uint8_t* lineOffsetTarget = (Pt::uint8_t*) _image.pixel(0, y);
        lineOffsetTarget += startX * _image.format().pixelSize();

        const Pt::uint8_t* lineOffsetSource = (const Pt::uint8_t*) sourceImage.pixel(0, y - startY);		

        memcpy( lineOffsetTarget, lineOffsetSource, (stopX - startX) * sourceImage.format().pixelSize() );
      }
    }
    break;

    case RenderMode::AlphaBlit:
      //TODO:
    break;

    case RenderMode::AlphaBlending:
      //TODO:
    break;
  }
}

void ImagePainter::drawImage( const  PointF& to, const Image& sourceImage, const Region& regionIn )
{
	//Todo: optimize this whithout sub image.
	Image subImage = sourceImage.subImage(regionIn);

	ImagePainter::drawImage(to, subImage);
}

}} //namespace

