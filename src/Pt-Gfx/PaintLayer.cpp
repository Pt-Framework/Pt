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

#include <Pt/Gfx/PaintLayer.h>
#include <Pt/Gfx/PaintSurface.h>
#include <Pt/Gfx/Painter.h>

namespace Pt {

namespace Gfx {

PaintLayer::PaintLayer()
{
}


PaintLayer::~PaintLayer()
{
}


PaintSurface* PaintLayer::surface()
{
    return _surface;
}


const PaintSurface* PaintLayer::surface() const
{
    return _surface;
}


void PaintLayer::setSurface(PaintSurface* surface)
{
    _surface = surface;
}


void PaintLayer::draw(PaintSurface& surface, 
                      const Paint& paint,
                      const Gfx::PointF& to,
                      const Gfx::RectF* rect) const
{
    onDraw(surface, paint, to, rect);
}


Canvas::Canvas(PaintSurface& surface)
: _surface(&surface)
, _painter(0)
{
}


Canvas::Canvas(PaintLayer& layer)
: _surface( layer.surface() )
, _painter(0)
{
}


Canvas::~Canvas()
{
    if(_painter)
    {
        _painter->onDetachCanvas(*this);
    }

    sync();
}


PaintSurface* Canvas::surface()
{
    return _surface;
}


void Canvas::sync()
{
    if(_surface)
        _surface->sync();
}


void Canvas::attachPainter(Painter& painter)
{
    if(_painter)
    {
        _painter->onDetachCanvas(*this);
        _painter = 0;
    }

    _painter = &painter;
}


void Canvas::detachPainter(Painter& painter)
{
    if(_painter)
        _painter = 0;
}

} // namespace

} // namespace
