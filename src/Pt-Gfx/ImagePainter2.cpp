/* Copyright (C) 2017-2017 Aloysius Indrayanto
   Copyright (C) 2006-2015 Marc Boris Duerner
   Copyright (C) 2006-2015 Laurentiu-Gheorghe Crisan

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
#include <Pt/Gfx/ImagePainter2.h>
#include <Pt/Gfx/Image.h>
#include <Pt/Gfx/Pen.h>
#include <Pt/Gfx/Point.h>
#include <Pt/Gfx/Brush.h>
#include <Pt/Gfx/Font.h>
#include <Pt/Gfx/FontMetrics.h>
#include <Pt/String.h>
#include "Pt/System/Clock.h"
#include "Rasterizer2.h"
#include "FreeType.h"

namespace Pt {
namespace Gfx {

ImagePainter2::ImagePainter2(Image& image)
: _rasterizer( new Rasterizer2(image))
{
}


ImagePainter2::~ImagePainter2()
{
  delete _rasterizer;
}


void ImagePainter2::setImage(Image& image)
{
    _rasterizer->setImage(image);
}


const ImageFormat& ImagePainter2::format() const
{
    return _rasterizer->format();
}


const CompositionMode& ImagePainter2::compositionMode() const
{
  return _rasterizer->compositionMode();
}


void ImagePainter2::setCompositionMode(const CompositionMode& mode)
{
  _rasterizer->setCompositionMode(mode);
}


const Gfx::RectF& ImagePainter2::clip() const
{
  return _clip;
}


void ImagePainter2::setClip( const RectF& clipIn )
{
   Rect  clip( Point( (int)(clipIn.x()), (int)(clipIn.y())), Size((int) (clipIn.width()),(int) (clipIn.height())));
  _rasterizer->setClip( clip );
  _clip = clipIn;
}


void ImagePainter2::setPen( const Pen& pen )
{
  _rasterizer->setPen( pen ) ;
}


const Pen& ImagePainter2::pen() const
{
    return _rasterizer->pen();
}


void ImagePainter2::setBrush(const Brush& brush)
{
  _rasterizer->setBrush( brush);
}


const Brush& ImagePainter2::brush() const
{
    return _rasterizer->brush();
}


void ImagePainter2::setFont(const Font& font)
{
    _rasterizer->setFont( font );
}


const Font& ImagePainter2::font() const
{
    return _rasterizer->font();
}


FontMetrics ImagePainter2::fontMetrics(const String& text) const
{
    return _rasterizer->fontMetrics( text );
}


void ImagePainter2::drawLine(const PointF& from, const  PointF& to)
{
    Point points[] = { Point((int)(from.x()), (int)(from.y())) ,  Point((int)(to.x()), (int)(to.y()))  };
    _rasterizer->stroke( points, 2);
}


void ImagePainter2::drawText( const PointF& toIn, const String& text )
{
  Point to((int)(toIn.x()), (int)(toIn.y()));
  _rasterizer->strokeText( to, text );
}


void ImagePainter2::drawRect( const  RectF& rect )
{
    Point points[5] = { Point(rect.topLeft().x(),rect.topLeft().y()) ,
                        Point(rect.topRight().x(),rect.topRight().y()) ,
                        Point(rect.bottomRight().x(),rect.bottomRight().y()) ,
                        Point(rect.bottomLeft().x(),rect.bottomLeft().y()) ,
                        Point(rect.topLeft().x(),rect.topLeft().y()),
                        };
    _rasterizer->stroke( points, 5);
}


void ImagePainter2::fillRect( const  RectF& rIn )
{
    Rect r( Point( (int)(rIn.x()), (int)(rIn.y()) ), Size((int) (rIn.width()), (int)(rIn.height())));
    _rasterizer->fillRect(r);
}


void ImagePainter2::drawEllipse( const PointF& topLeftIn, const  SizeF& sizeIn )
{
    Point topLeft((int)(topLeftIn.x()), (int)(topLeftIn.y()));
    Size size((int)(sizeIn.width()), (int)(sizeIn.height()) );

  _rasterizer->strokeEllipse( topLeft, size );
}


void ImagePainter2::fillEllipse( const PointF& topLeftIn, const  SizeF& sizeIn )
{
    Point topLeft((int) (topLeftIn.x()), (int)(topLeftIn.y()));
    Size size((int)(sizeIn.width()), (int)(sizeIn.height()) );

   _rasterizer->fillEllipse( topLeft, size );
}


void ImagePainter2::drawPolyline( const PointF* ps, const size_t pointCount )
{
   std::vector<Point> points(pointCount);

   for( size_t i = 0; i < pointCount; ++i)
        points[i].set( (int)(ps[i].x()), (int)(ps[i].y()));

  _rasterizer->stroke( &points[0], points.size() );
}


void ImagePainter2::fillPolygon( const PointF* ps, const size_t pointCount )
{
   std::vector<Point> points(pointCount);

   for( size_t i = 0; i < pointCount; ++i)
        points[i].set( (int)(ps[i].x()), (int)(ps[i].y()));

  _rasterizer->fill( &points[0], points.size() );
}


void ImagePainter2::drawImage( const PointF& toIn, const Image& image)
{
   Point to( (int)(toIn.x()), (int)(toIn.y()));
  _rasterizer->image( to, image);
}


void ImagePainter2::drawImage(const PointF& toIn, const Image& image, const RectF& imageRectIn)
{
   Point to( (int)(toIn.x()), (int)(toIn.y()));
   Rect  imageRect( Point( (int)(imageRectIn.x()), (int)(imageRectIn.y())), Size((int)( imageRectIn.width()),(int) (imageRectIn.height()) ));
  _rasterizer->image( to, image, imageRect );
}


FontMetrics ImagePainter2::fontMetrics( const Font& font, const Pt::String& text )
{
  return Rasterizer2::fontMetrics( font, text );
}


void ImagePainter2::setFontDir(const Pt::System::Path& path)
{
   FreeType::instance().setFontDir(path);
}


std::string ImagePainter2::defaultFont()
{
    return  FreeType::instance().defaultFont();
}

void ImagePainter2::setDefaultFont(const std::string& f)
{
   FreeType::instance().setDefaultFont(f);
}


std::vector<std::string> ImagePainter2::fontNames()
{
    return FreeType::instance().fontNames();
}

} // namespace

} // namespace
