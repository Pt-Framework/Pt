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


void ImagePainter::setClip( const RectF& clipIn )
{
  _rasterizer->setClip( round(clipIn) );
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


void ImagePainter::drawLine(const PointF& from, const  PointF& to)
{
    Point points[2];
    
    points[0].set( Pt::lround(from.x() - 0.5), 
                   Pt::lround(from.y() - 0.5) );

    points[1].set( Pt::lround(to.x() - 0.5),
                   Pt::lround(to.y() - 0.5) );

    _rasterizer->stroke(points, 2);
}


void ImagePainter::drawText(const PointF& toF, const String& text)
{
    Point to(round(toF));             

    _rasterizer->strokeText(to, text);
}


void ImagePainter::drawText(const PointF& toF, const Pt::String& text, 
                            const Transform& trans)
{
    Point to(round(toF));

    _rasterizer->strokeText(to, text, trans);
}


void ImagePainter::drawRect(const RectF& r)
{
    RectF rect( r.left()   - 0.5, 
                r.right()  - 0.5, 
                r.top()    - 0.5, 
                r.bottom() - 0.5 );

    Point points[5] = { round( rect.topLeft() ),
                        round( rect.topRight() ),
                        round( rect.bottomRight() ),
                        round (rect.bottomLeft() ),
                        round( rect.topLeft() ),
                      };

    _rasterizer->stroke(points, 5);
}


void ImagePainter::fillRect(const RectF& r)
{
    Rect rect( Pt::lround( r.left() ), 
               Pt::lround( r.right() ), 
               Pt::lround( r.top() ), 
               Pt::lround( r.bottom() ) );

    _rasterizer->fillRect(rect);
}


void ImagePainter::drawEllipse(const PointF& topLeftF, const SizeF& sizeF)
{
    Point topLeft( Pt::lround(topLeftF.x() - 0.5),
                   Pt::lround(topLeftF.y() - 0.5) );

    Size size( lround(sizeF.width() - 0.5), 
               lround(sizeF.height() - 0.5) );

    _rasterizer->strokeEllipse(topLeft,  size);
}


void ImagePainter::fillEllipse(const PointF& topLeftF, const SizeF& sizeF)
{
    Point topLeft = round(topLeftF);
    Size size = round(sizeF);

    _rasterizer->fillEllipse(topLeft, size);
}


void ImagePainter::drawPolyline(const PointF* ps, const size_t n)
{    
    std::vector<Point> points(n);

    for(size_t i = 0; i < n; ++i)
    {
        Point p( Pt::lround(ps[i].x() - 0.4999),
                 Pt::lround(ps[i].y() - 0.4999) );
        
        points[i] = p;
    }

  _rasterizer->stroke( &points[0], points.size() );
}


void ImagePainter::fillPolygon(const PointF* ps, const size_t n)
{
    std::vector<Point> points(n);

    for(size_t i = 0; i < n; ++i)
    {
        Point p( Pt::lround(ps[i].x() - 0.4999),
                 Pt::lround(ps[i].y() - 0.4999) );
        
        points[i] = p;
    }

    _rasterizer->fill( &points[0], points.size() );
}


void ImagePainter::drawImage(const PointF& to, const Image& image)
{
  _rasterizer->image( round(to), image);
}


void ImagePainter::drawImage(const PointF& to, const Image& image, const RectF& imageRect)
{
  _rasterizer->image( round(to), image, round(imageRect) );
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
