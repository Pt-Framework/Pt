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
#include <Pt/Gfx/ImagePainter.h>
#include <Pt/Gfx/Image.h>
#include <Pt/Gfx/Pen.h>
#include <Pt/Gfx/Point.h>
#include <Pt/Gfx/Brush.h>
#include <Pt/Gfx/Font.h>
#include <Pt/Gfx/FontMetrics.h>
#include <Pt/String.h>
#include "Pt/System/Clock.h"

namespace Pt {
namespace Gfx {

ImagePainter::ImagePainter( Image& image )
: _rasterizer( image  )
{
}


ImagePainter::~ImagePainter()
{

}


void ImagePainter::setRenderMode( RenderMode::Type mode)
{
	_rasterizer.setRenderMode( mode);
}


void ImagePainter::setPen( const Pen& pen )
{
  _rasterizer.setPen( pen ) ;
}

const Pen& ImagePainter::pen() const
{
    return _rasterizer.pen();
}

void ImagePainter::setBrush(const Brush& brush)
{
  _rasterizer.setBrush( brush);
}

const Brush& ImagePainter::brush() const
{
    return _rasterizer.brush();
}

void ImagePainter::setFont(const Font& font)
{    
    _rasterizer.setFont( font );
}

const Font& ImagePainter::font() const
{
    return _rasterizer.font();
}

FontMetrics ImagePainter::fontMetrics( String text) const
{
    return _rasterizer.fontMetrics( text );
}

void ImagePainter::drawLine(const PointF& from, const  PointF& to)
{
    PointF points[] = { from ,  to  };
    _rasterizer.stroke( points, 2);
}


void ImagePainter::drawText( const PointF& to, const String& text )
{ 
  _rasterizer.strokeText( to, text );
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
        
    _rasterizer.fill(  &points[0], points.size() );
}


void ImagePainter::drawEllipse( const  PointF& topLeft, const  SizeF& size )
{
  _rasterizer.strokeEllipse( topLeft, size );
}


void ImagePainter::fillEllipse( const  PointF& topLeft, const  SizeF& size )
{
   _rasterizer.fillEllipse( topLeft, size );
}


void ImagePainter::drawPolyline( const  PointF* ps, const size_t pointCount )
{
  _rasterizer.stroke( ps, pointCount );
}


void ImagePainter::fillPolygon( const  PointF* ps, const size_t pointCount )
{    
  _rasterizer.fill( ps, pointCount );
}


void ImagePainter::drawImage( const  PointF& to, const Image& image )
{
  _rasterizer.image( to, image );
}


FontMetrics ImagePainter::fontMetrics( const Font& font, const Pt::String& text )
{
  return Rasterizer::fontMetrics( font, text );
}

void ImagePainter::drawPath( const RenderPath& path )
{  
   path.render( *this );
}


void ImagePainter::setClip( const RectF& clip )
{
  _rasterizer.setClip( clip );
}
        
const Gfx::RectF& ImagePainter::clip() const
{
  return _rasterizer.clip();
}

void ImagePainter::clear( const Gfx::Color& color )
{
  _rasterizer.clear( color );
}


}} //namespace

