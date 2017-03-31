/* Copyright (C) 2015 Marc Boris Duerner 
   Copyright (C) 2015 Laurentiu-Gheorghe Crisan
  
  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.
  
  As a special exception, you may use this file as part of a free
  software library without restriction. Specifically, if other files
  instantiate templates or use macros or inline functions from this
  file, or you compile this file and link it with other files to
  produce an executable, this file does not by itself cause the
  resulting executable to be covered by the GNU General Public
  License. This exception does not however invalidate any other
  reasons why the executable file might be covered by the GNU Library
  General Public License.
  
  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.
  
  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  
  02110-1301 USA
*/

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
#include "Rasterizer.h"
#include "FreeType.h"

namespace Pt {
namespace Gfx {

ImagePainter::ImagePainter(Image& image)
: _rasterizer( new Rasterizer(image))
{
}


ImagePainter::~ImagePainter()
{
  delete _rasterizer;
}


void ImagePainter::setImage(Image& image)
{
    _rasterizer->setImage(image);
}


const ImageFormat& ImagePainter::format() const
{
    return _rasterizer->format();
}


const CompositionMode& ImagePainter::compositionMode() const
{
  return _rasterizer->compositionMode();
}


void ImagePainter::setCompositionMode(const CompositionMode& mode)
{
  _rasterizer->setCompositionMode(mode);
}


const Gfx::Rect& ImagePainter::clip() const
{
  return _clip;
}


void ImagePainter::setClip( const Rect& clipIn )
{
   Rect  clip( Point( (int) (clipIn.x()), 
                      (int) (clipIn.y()) ), 
               Size( (int) (clipIn.width()),
                     (int) (clipIn.height()) ));

  _rasterizer->setClip( clip );
  _clip = clipIn;
}


void ImagePainter::setPen( const Pen& pen )
{
  _rasterizer->setPen( pen ) ;
}


const Pen& ImagePainter::pen() const
{
    return _rasterizer->pen();
}


void ImagePainter::setBrush(const Brush& brush)
{
  _rasterizer->setBrush( brush);
}


const Brush& ImagePainter::brush() const
{
	return _rasterizer->brush();
}


void ImagePainter::setFont(const Font& font)
{    
	_rasterizer->setFont( font );
}


const Font& ImagePainter::font() const
{
	return _rasterizer->font();
}


FontMetrics ImagePainter::fontMetrics(const String& text) const
{
	return _rasterizer->fontMetrics( text );
}


void ImagePainter::drawLine(const Point& from, const  Point& to)
{
	Point points[] = { from , to  };
	_rasterizer->stroke( points, 2);
}


void ImagePainter::drawText( const Point& to, const String& text )
{ 
  _rasterizer->strokeText( to, text );
}


void ImagePainter::drawRect( const  Rect& rect )
{
	Point points[5] = { Point(rect.topLeft().x(),rect.topLeft().y()) ,
                        Point(rect.topRight().x(),rect.topRight().y()) ,
                        Point(rect.bottomRight().x(),rect.bottomRight().y()) ,
                        Point(rect.bottomLeft().x(),rect.bottomLeft().y()) ,
                        Point(rect.topLeft().x(),rect.topLeft().y()),
                        };

	_rasterizer->stroke( points, 5);
}


void ImagePainter::fillRect( const  Rect& r )
{  
    _rasterizer->fillRect(r);
}


void ImagePainter::drawEllipse( const Point& topLeft, const  Size& size )
{
  _rasterizer->strokeEllipse( topLeft, size );
}


void ImagePainter::fillEllipse( const Point& topLeft, const  Size& size )
{
   _rasterizer->fillEllipse( topLeft, size );
}


void ImagePainter::drawPolyline( const Point* ps, const size_t pointCount )
{
  _rasterizer->stroke(ps, pointCount);
}


void ImagePainter::fillPolygon( const Point* ps, const size_t pointCount )
{    
  _rasterizer->fill( ps, pointCount);
}


void ImagePainter::drawImage( const Point& to, const Image& image)
{
  _rasterizer->image( to, image);
}


void ImagePainter::drawImage(const Point& to, const Image& image, const Rect& imageRect)
{
  _rasterizer->image( to, image, imageRect );
}


FontMetrics ImagePainter::fontMetrics( const Font& font, const Pt::String& text )
{
  return Rasterizer::fontMetrics( font, text );
}


void ImagePainter::setFontDir(const Pt::System::Path& path)
{
   FreeType::instance().setFontDir(path);
} 


std::string ImagePainter::defaultFont()
{
    return  FreeType::instance().defaultFont();
}

void ImagePainter::setDefaultFont(const std::string& f)
{
   FreeType::instance().setDefaultFont(f);
}


std::vector<std::string> ImagePainter::fontNames()
{
    return FreeType::instance().fontNames();
}

} // namespace

} // namespace
