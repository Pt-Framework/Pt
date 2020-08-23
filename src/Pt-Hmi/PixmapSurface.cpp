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

#include <Pt/Hmi/PixmapSurface.h>
#include "PixmapSurfaceImpl.h"
#include <Pt/Hmi/PaintSurface.h>
#include <Pt/Hmi/Widget.h>

namespace Pt {

namespace Hmi {

PixmapSurface::PixmapSurface()
: _impl( new PixmapSurfaceImpl )
, _scaleFactor(1)
{
}


PixmapSurface::~PixmapSurface()
{
    _impl->finish();
    delete _impl;
}


void PixmapSurface::resize(const Gfx::SizeF& size)
{
    _logicSize = size;
    _impl->resize( toPhysical(size) );
}


void PixmapSurface::clear(const Gfx::Color& c)
{
    _impl->clear(c);
}


const Gfx::SizeF& PixmapSurface::onSize() const
{
    return _logicSize;
}


void PixmapSurface::onBegin(Painter& painter)
{
    _impl->begin(painter);
}


void PixmapSurface::onFinish()
{
    _impl->finish();
}


const Gfx::ImageFormat& PixmapSurface::format() const
{
    return _impl->format();
}


void PixmapSurface::setPen(const Gfx::Pen& pen)
{
    _impl->setPen(pen);
}


void PixmapSurface::setBrush(const Gfx::Brush& brush)
{
    _impl->setBrush(brush);
}


void PixmapSurface::setFont(const Gfx::Font& font)
{
    _impl->setFont(font);
}


void PixmapSurface::setClip( const Gfx::RectF& clip)
{
    _impl->setClip(toPhysical(clip));
}


void PixmapSurface::resetClip()
{
    _impl->resetClip();
}


void PixmapSurface::setCompositionMode(const Gfx::CompositionMode& mode)
{
    _impl->setCompositionMode(mode);
}


Gfx::FontMetrics PixmapSurface::fontMetrics(const Pt::String& text) const
{
    return _impl->fontMetrics(text);
}


void PixmapSurface::drawLine(const Gfx::PointF& from, const Gfx::PointF& to)
{

    _impl->drawLine(toPhysical(from), toPhysical(to));
}


void PixmapSurface::drawText(const Gfx::PointF& to, const Pt::String& text)
{
    Gfx::Transform trans;
    trans.scale(_scaleFactor, _scaleFactor);

    _impl->drawText(toPhysical(to), text, trans);
}


void PixmapSurface::drawText(const Gfx::PointF& to, const Pt::String& text,
                             const Gfx::Transform& t)
{
    Gfx::Transform trans = t;

    trans.scale(_scaleFactor, _scaleFactor);

    _impl->drawText(toPhysical(to), text, trans);
}



void PixmapSurface::drawRect(const Gfx::RectF& r)
{
    _impl->drawRect(toPhysical(r));
}


void PixmapSurface::fillRect(const Gfx::RectF& r)
{
    _impl->fillRect(toPhysical(r));
}


void PixmapSurface::drawEllipse(const Gfx::PointF& topLeft, const Gfx::SizeF& size)
{
    _impl->drawEllipse(toPhysical(topLeft), toPhysical(size));
}


void PixmapSurface::fillEllipse(const Gfx::PointF& topLeft, const Gfx::SizeF& size)
{
    _impl->fillEllipse(toPhysical(topLeft), toPhysical(size));
}


void PixmapSurface::drawPolyline(const Gfx::PointF* points, size_t pointCount)
{
    std::vector<Gfx::PointF> ps;

    for (size_t i = 0; i < pointCount; ++i)
        ps.push_back(toPhysical(points[i]));

    _impl->drawPolyline(&ps[0], pointCount);
}


void PixmapSurface::fillPolygon(const Gfx::PointF* points, size_t pointCount)
{
    std::vector<Gfx::PointF> ps;

    for (size_t i = 0; i < pointCount; ++i)
        ps.push_back(toPhysical(points[i]));

    _impl->fillPolygon(&ps[0], pointCount);
}


void PixmapSurface::drawPath(const Gfx::Path& path, float smoothness)
{
    _impl->drawPath(path, smoothness);
}


void PixmapSurface::drawSurface(const Gfx::PointF& to, const PixmapSurface& surface)
{
    _impl->drawSurface(toPhysical(to), surface);
}


void PixmapSurface::drawSurface(const Gfx::PointF& to,
                                  const PixmapSurface& pm,
                                  const Gfx::RectF& pmRect)
{
    _impl->drawSurface(toPhysical(to), pm, toPhysical(pmRect));
}


void PixmapSurface::drawImage(const Gfx::PointF& to, const Gfx::Image& image)
{
    _impl->drawImage(toPhysical(to), image);
}

void PixmapSurface::drawImage(const Gfx::PointF& to, const Gfx::Image& image, const Gfx::RectF& r)
{
    _impl->drawImage(toPhysical(to), image, r);
}


void PixmapSurface::drawPicture(const Gfx::PointF& to, const Picture& pic)
{
    _impl->drawPicture(toPhysical(to) , pic);
}


PixmapSurfaceImpl* PixmapSurface::pixmapImpl() const
{
    return _impl;
}

} // namespace

} // namespace
