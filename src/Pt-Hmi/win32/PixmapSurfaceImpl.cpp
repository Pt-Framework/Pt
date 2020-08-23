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

#include "PixmapSurfaceImpl.h"
#include "ScreenImpl.h"
#include "ApplicationImpl.h"
#include <Pt/Hmi/Application.h>
#include <Pt/Hmi/Picture.h>
#include <Pt/Hmi/PixmapSurface.h>
#include "PictureImpl.h"

namespace Pt {

namespace Hmi {

PixmapSurfaceImpl::PixmapSurfaceImpl()
: _size(10,10)
, _image( Pt::Gfx::ImageFormat::argb32(),
          Gfx::Size(_size.width(), _size.height()) )
, _painter(_image)
{
}


PixmapSurfaceImpl::~PixmapSurfaceImpl()
{
}


void PixmapSurfaceImpl::clear(const Gfx::Color& c)
{
}


const Gfx::ImageFormat& PixmapSurfaceImpl::format() const
{
    return _painter.format();
}


void PixmapSurfaceImpl::begin(Painter& painter)
{
}


void PixmapSurfaceImpl::finish()
{
}


//void PixmapSurfaceImpl::resize(const Gfx::Size& size, size_t padding)
//{
//    _size.set(size.width(), size.height());
//    _image.reset(_image.format(), size, padding);
//    _painter.setImage(_image);
//}


void PixmapSurfaceImpl::resize(const Gfx::SizeF& size)
{
    _size = size;
    _image.reset(_image.format(), round(size));
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


void PixmapSurfaceImpl::resetClip()
{
    _painter.resetClip();
}


void PixmapSurfaceImpl::setCompositionMode(const Gfx::CompositionMode& mode)
{
    _painter.setCompositionMode(mode);
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


void PixmapSurfaceImpl::drawText(const Gfx::PointF& to, const Pt::String& text, const Gfx::Transform& trans)
{
    _painter.drawText(to, text, trans);
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


void PixmapSurfaceImpl::drawPath(const Gfx::Path& path, float smoothness)
{
    _painter.drawPath(path, smoothness);

}


void PixmapSurfaceImpl::drawSurface(const Gfx::PointF& to, const PixmapSurface& surface)
{
    const Gfx::Image& image = surface.pixmapImpl()->image();
    _painter.drawImage(to, image);
}


void PixmapSurfaceImpl::drawSurface(const Gfx::PointF& to, 
                                  const PixmapSurface& pm,
                                  const Gfx::RectF& pmRect)
{
    const Gfx::Image& image = pm.pixmapImpl()->image();
    _painter.drawImage(to, image, pmRect);
}


void PixmapSurfaceImpl::drawImage(const Gfx::PointF& to, const Gfx::Image& image)
{
    _painter.drawImage(to, image);
}


void PixmapSurfaceImpl::drawImage(const Gfx::PointF& to, const Gfx::Image& image, const Gfx::RectF& r)
{
    _painter.drawImage( to, image, r);
}


void PixmapSurfaceImpl::drawPicture(const Gfx::PointF& to, const Picture& pic)
{
    const PictureImpl* picImpl = pic.impl();
    const Gfx::Image& image = picImpl->image();

    if( picImpl->empty() )
        return;

    _painter.drawImage(to, image);
}

} // namespace

} // namespace
