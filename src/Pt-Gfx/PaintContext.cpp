/* Copyright (C) 2015 Marc Boris Duerner 

  
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

#include <Pt/Gfx/PaintContext.h>
#include <Pt/Gfx/PaintSurface.h>
#include <Pt/Gfx/Painter.h>

namespace Pt {

namespace Gfx {

PaintContext::PaintContext(PaintSurface& surface)
: _surface(&surface)
, _painter(0)
{
}


PaintContext::~PaintContext()
{
    if(_painter)
    {
        _painter->onDetachContext(*this);
    }

    finish();
}


PaintSurface* PaintContext::surface()
{
    return _surface;
}


const Gfx::ImageFormat& PaintContext::format() const
{
    return _surface->format();
}


const Gfx::SizeF& PaintContext::size() const
{
    return _surface->size();
}


const Scaling& PaintContext::scaling() const
{
    return _surface->scaling();
}


Canvas* PaintContext::getCanvas(Canvas* reuse)
{
    return _surface->getCanvas(reuse);
}


void PaintContext::sync()
{
    if(_surface)
        _surface->sync();
}


void PaintContext::finish()
{
    if(_surface)
        _surface->finish();
}


void PaintContext::attachPainter(Painter& painter)
{
    if(_painter)
    {
        _painter->onDetachContext(*this);
        _painter = 0;
    }

    _painter = &painter;
}


void PaintContext::detachPainter(Painter& painter)
{
    if(_painter)
        _painter = 0;
}

} // namespace

} // namespace
