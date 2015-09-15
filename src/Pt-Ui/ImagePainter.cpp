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
, _origin(0,0)
, _clipRect( Ui::PointF(0,0), Ui::SizeF( 600000, 600000 ) )
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
   PointF p1 = fromOrigin( to );
   PointF p2 = p1;
  _clipLine(p1, p2, _clipRect.left(), _clipRect.right(), _clipRect.top(), _clipRect.bottom() );
   _image.setColor( p1.x(), p1.y(), _pen.color() );
}


void ImagePainter::drawLine(const PointF& from, const  PointF& to)
{
    if( _pen.size()  == 0 )
        return;

    PointF points[] = { fromOrigin( from ),  fromOrigin( to )  };

    _clipLine(points[0], points[1], _clipRect.left(), _clipRect.right(), _clipRect.top(), _clipRect.bottom() );

    _drawPolyline->draw(_image, _pen, points, 2);
}


void ImagePainter::drawText( const PointF& to, const String& text, const Color* outline )
{
   PointF p1 = fromOrigin( to );
   PointF p2 = p1;
  _clipLine(p1, p2, _clipRect.left(), _clipRect.right(), _clipRect.top(), _clipRect.bottom() );

    _drawText->draw( _image, _pen.color(), p1 , text, outline );
}


void ImagePainter::drawRect(const  RectF& rect)
{    

    this->drawLine( rect.topLeft(), rect.topRight() );

    this->drawLine( PointF( rect.topRight().x(), rect.topRight().y()),
                   PointF( rect.bottomRight().x(), rect.bottomRight().y()) );

    this->drawLine( PointF( rect.bottomRight().x(), rect.bottomRight().y() ),
                   PointF( rect.bottomLeft().x(), rect.bottomLeft().y() ) );

    this->drawLine( PointF( rect.bottomLeft().x(), rect.bottomLeft().y() ),
                   rect.topLeft() );
}

void ImagePainter::fillRect(const  RectF& r)
{
    std::vector<PointF> points( 4);

    points.push_back( r.topLeft()  ); 
    points.push_back( r.topRight() );
    points.push_back( r.bottomRight() );
    points.push_back( r.bottomLeft() );        
    
    
    this->fillPolygon( &points[0], points.size() );
}

void ImagePainter::drawEllipse( const  PointF& topLeft, const  SizeF& size )
{
    _drawEllipse->draw( _image, _pen,  fromOrigin( topLeft ), size );
}

void ImagePainter::fillEllipse( const  PointF& topLeft, const  SizeF& size )
{
    _fillEllipse->draw( _image, _brush, fromOrigin( topLeft ), size );
}

void ImagePainter::drawPolyline( const  PointF* ps, const size_t pointCount )
{
  if( _pen.size()  == 0 )
      return;	
	
  std::vector<PointF> points(pointCount) ;

  for( size_t i = 0; i < pointCount; ++i )
      points[i] = fromOrigin( ps[i] );

  _clipPolygon( points, _clipRect );
  _drawPolyline->draw( _image, _pen, &points[0], points.size() );
}


void ImagePainter::fillPolygon( const  PointF* ps, const size_t pointCount )
{
    std::vector<PointF> points(pointCount) ;

  for( size_t i = 0; i < pointCount; ++i )
      points[i] = fromOrigin( ps[i] );

	
   _clipPolygon( points, _clipRect );

  _fillPolygon->draw( _image, _brush, &points[0], points.size() );
}


void ImagePainter::drawImage( const  PointF& p, const Image& sourceImage )
{
	if( sourceImage.format() != _image.format() )
		throw std::logic_error( "wrong image format");

  PointF to = fromOrigin(p);
  //source
  int xSourceBegin = 0;
  int ySourceBegin = 0;

  //target
  int xTargetBegin = to.x();
  int yTargetBegin = to.y();

  if( to.x() >= _image.width() )
     return;

  if( to.x() < 0 )
  {
    xSourceBegin = -to.x();
    xTargetBegin = 0;
  }

  if( to.y() > _image.height() )
     return;

  if( to.y() < 0 )
  {
    ySourceBegin = -to.y();
    yTargetBegin = 0;
  }

  int lineLength = sourceImage.width();   

  if( to.x()  < 0 )
    lineLength += to.x();

  if( (xTargetBegin + lineLength) > _image.width()  )
      lineLength -= (xTargetBegin + lineLength) - _image.width() ;
 
  if( lineLength  <=  0 )
    return;  
   
  const int endYOffset = to.y() + sourceImage.height();

  int lines = sourceImage.height();   

  if( endYOffset >  _image.height()  )
      lines = _image.height() - yTargetBegin;

  if( endYOffset  <  0 )
    return;
  
  const Pt::uint8_t* scanLineSource = sourceImage.pixel( xSourceBegin, ySourceBegin );
  Pt::uint8_t* scanLineTarget = _image.pixel( xTargetBegin, yTargetBegin );

  const int targetStride = _image.width() * _image.format().pixelSize() +  _image.stride();
  const int sourceStride = sourceImage.width() * sourceImage.format().pixelSize() +  sourceImage.stride();

  const std::size_t lineSize = lineLength * _image.format().pixelSize();

  //Render
  switch( _renderMode )
  {
    case RenderMode::NoAlpha:
    {	//Copy to new image	          
			for(int i = 0; i < lines; ++i )
			{                
        memcpy( scanLineTarget, scanLineSource, lineSize );
        scanLineSource += sourceStride;
        scanLineTarget += targetStride;
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


void ImagePainter::drawPath( const RenderPath& path )
{  
  for( size_t i = 0; i < path.size(); ++i )
    path[i]->execute(  *this );  
}

}} //namespace

