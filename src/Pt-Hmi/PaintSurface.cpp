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
#include "PainterImpl.h"

#include <Pt/Hmi/PaintSurface.h>
#include <Pt/Hmi/Painter.h>

namespace Pt {

namespace Hmi {

PaintSurface::PaintSurface()
: _painter(0)
{
}


PaintSurface::~PaintSurface()
{
    if(_painter)
        _painter->onDetach();
}


const Gfx::SizeF& PaintSurface::size() const
{
    return onSize();
}


void PaintSurface::begin(Painter& painter)
{
    if(_painter)
        _painter->finish();

    _painter = 0;

    onBegin(painter);
    _painter = &painter;
}


void PaintSurface::finish(Painter& painter)
{
    onFinish();
    _painter = 0;
}


void PaintSurface::setDefaultFont(const std::string& f)
{
    PainterImpl::setDefaultFont(f);
}


double PaintSurface::toPhysical(double n) const
{
    return Application::instance().screen().toPhysical(n);
}


Gfx::PointF PaintSurface::toPhysical(const Gfx::PointF& p) const
{
    return Application::instance().screen().toPhysical(p);
}


Gfx::SizeF PaintSurface::toPhysical(const Gfx::SizeF& n) const
{
    return Application::instance().screen().toPhysical(n);
}


Gfx::RectF PaintSurface::toPhysical(const Gfx::RectF& r) const
{
    return Application::instance().screen().toPhysical(r);
}


double PaintSurface::toLogical(double n) const
{
    return Application::instance().screen().toLogical(n);
}


Gfx::PointF PaintSurface::toLogical(const Gfx::PointF& p) const
{
    return Application::instance().screen().toLogical(p);
}


Gfx::SizeF PaintSurface::toLogical(const Gfx::SizeF& n) const
{
    return Application::instance().screen().toLogical(n);
}


Gfx::RectF PaintSurface::toLogical(const Gfx::RectF& r) const
{
    return Application::instance().screen().toLogical(r);
}


double PaintSurface::align(double n) const
{
    return Application::instance().screen().align(n);
}

Gfx::PointF& PaintSurface::align(const Gfx::PointF& p) const
{
    return Application::instance().screen().align(p);
}


Gfx::SizeF& PaintSurface::align(const Gfx::SizeF& s) const
{
    return Application::instance().screen().align(s);
}


Gfx::RectF PaintSurface::align(const Gfx::RectF& rect) const
{
    return Application::instance().screen().align(rect);
}


} // namespace

} // namespace
