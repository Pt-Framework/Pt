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
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, 
  MA 02110-1301 USA
*/

#include "PaintSurfaceImpl.h"
#include "ScreenImpl.h"
#include "ApplicationImpl.h"
#include <Pt/Hmi/Application.h>
#include <Pt/Hmi/Picture.h>
#include "PictureImpl.h"

namespace Pt {

namespace Hmi {

/////////////////////////////////////////////////////////////////////
// PaintSurfaceImpl
/////////////////////////////////////////////////////////////////////

std::string PaintSurfaceImpl::defaultFont()
{
    return "Vera";
}


std::list<std::string> PaintSurfaceImpl::fontFamilyNames()
{
    return std::list<std::string>();
}


Gfx::FontMetrics PaintSurfaceImpl::fontMetrics(const Gfx::Font& font, const Pt::String& text)
{
    return Gfx::ImagePainter::fontMetrics(font, text);   
}

/////////////////////////////////////////////////////////////////////
// PixmapSurfaceImpl
/////////////////////////////////////////////////////////////////////

PixmapSurfaceImpl::PixmapSurfaceImpl()
: _size(10,10)
, _image( Gfx::Size(_size.width(), _size.height()), 
          Application::instance().impl()->frameBuffer().format() )
, _painter(_image)
{
}


PixmapSurfaceImpl::~PixmapSurfaceImpl()
{
}


void PixmapSurfaceImpl::clear()
{
}


void PixmapSurfaceImpl::resize(const Gfx::Size& size, size_t stride)
{
    _size.set(size.width(), size.height());
    _image.resize(size, stride);
    _painter.setImage(_image);
}


void PixmapSurfaceImpl::resize(const Gfx::SizeF& size)
{
    _size = size;
    _image.resize( Gfx::Size(_size.width(), _size.height() ) );

    _painter.setImage(_image);
}


const Gfx::SizeF& PixmapSurfaceImpl::size() const
{
    return _size;
}


void PixmapSurfaceImpl::setClip(const Gfx::RectF& clip)
{
    _painter.setClip(clip);
}


void PixmapSurfaceImpl::setPen(const Gfx::Pen& pen)
{
    _painter.setPen(pen);
}


void PixmapSurfaceImpl::setBrush(const Gfx::Brush& brush)
{
    _painter.setBrush(brush);
}


void PixmapSurfaceImpl::setFont(const Gfx::Font& font)
{
    _painter.setFont(font); 
}


Gfx::FontMetrics PixmapSurfaceImpl::fontMetrics(const Pt::String& text) const
{
    return _painter.fontMetrics(text);
}


void PixmapSurfaceImpl::drawLine(const Gfx::PointF& from, const Gfx::PointF& to)
{
    _painter.drawLine(from, to);
}


void PixmapSurfaceImpl::drawText(const Gfx::PointF& to, const Pt::String& text)
{
    _painter.drawText(to, text);
}


void PixmapSurfaceImpl::drawRect(const Gfx::RectF& rect)
{
    _painter.drawRect(rect); 
}


void PixmapSurfaceImpl::fillRect(const Gfx::RectF& rect)
{
    _painter.fillRect(rect);
}


void PixmapSurfaceImpl::drawEllipse(const Gfx::PointF& topLeft, const Gfx::SizeF& size)
{
    _painter.drawEllipse(topLeft, size);
}


void PixmapSurfaceImpl::fillEllipse(const Gfx::PointF& topLeft, const Gfx::SizeF& size)
{
    _painter.fillEllipse(topLeft, size);
}


void PixmapSurfaceImpl::drawPolyline(const Gfx::PointF* points, const size_t pointCount)
{
    _painter.drawPolyline(points, pointCount);
}


void PixmapSurfaceImpl::fillPolygon(const Gfx::PointF* points, const size_t pointCount)
{
    _painter.fillPolygon(points, pointCount);
}


void PixmapSurfaceImpl::drawSurface(const Gfx::PointF& to, const PixmapSurface& surface)
{
    const Gfx::Image& image = surface.pixmapImpl()->image();
    _painter.drawImage(to, image, Pt::Gfx::RenderFlags::AlphaBlend);
}


void PixmapSurfaceImpl::drawSurface(const Gfx::PointF& to, 
                                  const PixmapSurface& pm,
                                  const Gfx::RectF& pmRect)
{
    const Gfx::Image& image = pm.pixmapImpl()->image();
    _painter.drawImage(to, image, pmRect, Pt::Gfx::RenderFlags::AlphaBlend);
}


void PixmapSurfaceImpl::drawImage(const Gfx::PointF& to, const Gfx::Image& image, Pt::Gfx::RenderFlags::Type flags)
{
    _painter.drawImage(to, image, flags);
}


void PixmapSurfaceImpl::drawPicture(const Gfx::PointF& to, const Picture& pic)
{
  if( pic.empty() )
    return;
  
  for(size_t w = 0; w < pic.impl()->width() ; ++w )
  {
    for( size_t h = 0; h < pic.impl()->height(); ++h)
    {
        const size_t x = to.x() + w;
        const size_t y = to.y() + h;

        if(!( x >= _painter.clip().left() &&  x < _painter.clip().right() && y  >=  _painter.clip().top()  && y < _painter.clip().bottom() ))
            continue;

        const Pt::uint8_t* srcPix = pic.impl()->image().pixel(w,h);
        const Pt::uint8_t alpha = (Pt::uint8_t)(*(srcPix+3));


        switch( pic.impl()->flags() )
        {
          case Gfx::RenderFlags::AlphaMask:
          {
            if( alpha == 0)
              continue;

            *(( Pt::uint32_t*) _image.pixel(x,y))  = *(( Pt::uint32_t*)srcPix);
          }
          break;

          case Gfx::RenderFlags::AlphaBlend:
          {
            switch(alpha)
            {
                case 0:                  
                break;

                case 255:
                   *(( Pt::uint32_t*) _image.pixel(x,y))  = *(( Pt::uint32_t*)srcPix);
                break;

                default:
                {
                  Pt::uint8_t* dstPix  =  _image.pixel(x,y);
                  const Pt::uint8_t alpha255 =  (255 - alpha);
    	            dstPix[0] = (srcPix[0] * alpha + dstPix[0] * alpha255) / 255;
			            dstPix[1] = (srcPix[1] * alpha + dstPix[1] * alpha255) / 255;
			            dstPix[2] = (srcPix[2] * alpha + dstPix[2] * alpha255) / 255;                
                }
                break;
          }
          break;

          case Gfx::RenderFlags::IgnoreAlpha:
            *(( Pt::uint32_t*) _image.pixel(x,y)) = *(( Pt::uint32_t*)srcPix);
          break;
        }
    }
  }
}

}

}} // namespace
