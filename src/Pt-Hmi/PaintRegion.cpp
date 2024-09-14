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

#include <Pt/Hmi/PaintRegion.h>

namespace Pt {

namespace Hmi {

PaintRegion::PaintRegion()
{
}


PaintRegion::PaintRegion(Hmi::PaintSurface& surface, const Gfx::RectF& rect)
:  _region(surface, rect)
{
}


PaintRegion::~PaintRegion()
{
}


void PaintRegion::attach(Hmi::PaintSurface& surface, const Gfx::RectF& rect)
{
    _region.attach(surface, rect);
}


void PaintRegion::detach()
{
    _region.detach();
}


Hmi::PaintSurface* PaintRegion::surface() const
{
    Gfx::PaintSurface* s = _region.surface();
    return static_cast<Hmi::PaintSurface*>(s);
}


const Gfx::PointF& PaintRegion::position() const
{
    return _region.position();
}


void PaintRegion::move(const Gfx::PointF& pos)
{
    _region.move(pos);
}


void PaintRegion::resize(const Gfx::SizeF& size)
{
    _region.resize(size);
}


const Gfx::PaintInfo& PaintRegion::onGetPaintInfo() const
{
    return _region.info();
}


Gfx::PaintContext* PaintRegion::onGetPaint(Gfx::PaintContext* context)
{
    return _region.getPaint(context);
}


void PaintRegion::onDraw(Gfx::PaintContext& paint, 
                         const Gfx::PointF& to) const
{
    _region.draw(paint, to);
}


void PaintRegion::onDraw(Gfx::PaintContext& paint, 
                         const Gfx::PointF& to, 
                         const Gfx::RectF& rect) const
{
    _region.draw(paint, to, rect);
}


void PaintRegion::onDrawPixmap(const Gfx::PointF& to, 
                               const PixmapSurface& pixmap,
                               const Gfx::CompositionMode& mode)
{
    Hmi::PaintSurface* s = surface();
    if(s)
    {
        Gfx::PointF pos = to + _region.position();
        s->drawPixmap(pos, pixmap, mode);
    }
}


void PaintRegion::onDrawPixmap(const Gfx::PointF& to,
                               const PixmapSurface& pixmap, 
                               const Gfx::RectF& rect,
                               const Gfx::CompositionMode& mode)
{
    Hmi::PaintSurface* s = surface();
    if(s)
    {
        Gfx::PointF pos = to + _region.position();
        s->drawPixmap(pos, pixmap, rect, mode);
    }
}

} // namespace

} // namespace
