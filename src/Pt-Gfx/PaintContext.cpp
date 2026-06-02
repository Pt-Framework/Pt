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
#include <Pt/Gfx/PainterBase.h>

namespace Pt {

namespace Gfx {

PaintContext::PaintContext(PaintSurface& surface)
: _surface(&surface)
, _painter(0)
, _clip()
, _hasClip(false)
{
    _surface->attachContext(*this);
}


PaintContext::PaintContext(PaintSurface& surface, const RectF& clip)
: _surface(&surface)
, _painter(0)
, _clip(clip)
, _hasClip(true)
{
    _surface->attachContext(*this);
}


PaintContext::~PaintContext()
{
    if(_painter)
    {
        _painter->onDetachContext(*this);
        _painter = 0;
    }

    if(_surface)
    {
        _surface->detachContext(*this);
        _surface->finish();
        _surface = 0;
    }
}


PaintSurface* PaintContext::surface()
{
    return _surface;
}


const Gfx::ImageFormat& PaintContext::format() const
{
    if(_surface)
        return _surface->format();

    return ImageFormat::argb32();
}


const Gfx::SizeF& PaintContext::size() const
{
    if(_surface)
        return _surface->size();

    static const SizeF empty(0, 0);
    return empty;
}


const Scaling& PaintContext::scaling() const
{
    if(_surface)
        return _surface->scaling();

    static const Scaling identity;
    return identity;
}


const RectF* PaintContext::clip() const
{
    return _hasClip ? &_clip : 0;
}


void PaintContext::setClip(const RectF& clip)
{
    _clip = clip;
    _hasClip = true;

    if(_painter)
        _painter->onSetContextClip( this->clip() );
}


void PaintContext::resetClip()
{
    _hasClip = false;

    if(_painter)
    _painter->onSetContextClip(this->clip());
}


Canvas* PaintContext::getCanvas(Canvas* reuse)
{
    if(_surface)
        return _surface->getCanvas(reuse);

    return 0;
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


void PaintContext::attachPainter(PainterBase& painter)
{
    if(_painter)
    {
        _painter->onDetachContext(*this);
        _painter = 0;
    }

    _painter = &painter;
}


void PaintContext::detachPainter(PainterBase& painter)
{
    if(_painter == &painter)
        _painter = 0;
}


void PaintContext::onDetachSurface(PaintSurface& surface)
{
    if(_painter)
    {
        _painter->onDetachContext(*this);
        _painter = 0;
    }

    _surface = 0;
}

} // namespace

} // namespace
