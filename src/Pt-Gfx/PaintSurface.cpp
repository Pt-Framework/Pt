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

#include <Pt/Gfx/PaintSurface.h>
#include <Pt/Gfx/Canvas.h>
#include <Pt/Gfx/Painter.h>

#include <limits>

namespace Pt {

namespace Gfx {

///////////////////////////////////////////////////////////////////////
// PaintSurface
///////////////////////////////////////////////////////////////////////

PaintSurface::PaintSurface()
: _canvas(0)
, _painter(0)
, _reserved(0)
{
}


PaintSurface::~PaintSurface()
{
    if(_canvas)
    {
        _canvas->detachSurface(*this);
        _canvas = 0;
    }

    if(_painter)
    {
        _painter->onDetachSurface(*this);
    }
}


const Gfx::ImageFormat& PaintSurface::format() const
{
    return onGetFormat();
}


const Gfx::SizeF& PaintSurface::size() const
{
    return onGetSize();
}


const Scaling& PaintSurface::scaling() const
{
    return onGetScaling();
}


Canvas* PaintSurface::getCanvas(Canvas* reuse)
{
    return onGetCanvas(reuse);
}


Gfx::Canvas* PaintSurface::onGetCanvas(Gfx::Canvas* reuse)
{
    invalidate();

    _canvas = onCreateCanvas(reuse);
    if( ! _canvas )
        return 0;

    _canvas->attachSurface(*this);

    RectF nobounds(PointF(0, 0),
                   SizeF( std::numeric_limits<double>::max(), 
                          std::numeric_limits<double>::max() ) );
    _canvas->setRegion(nobounds);

    const Scaling& scale = scaling();
    _canvas->setScaling(scale);

    return _canvas;
}


Gfx::Canvas* PaintSurface::onCreateCanvas(Gfx::Canvas* reuse)
{
    return 0;
}


void PaintSurface::sync()
{
    onSync();
}


void PaintSurface::finish()
{
    invalidate();
    onFinish();
}


void PaintSurface::invalidate()
{
    if(_canvas)
    {
        _canvas->detachSurface(*this);
        _canvas = 0;

        onReleaseCanvas();
    }

    if(_painter)
    {
        _painter->onDetachSurface(*this);
        _painter = 0;
    }
}


void PaintSurface::onDetachCanvas(Canvas& canvas)
{
    if(_canvas)
    {
        onReleaseCanvas();
        _canvas = 0;
    }
}


void PaintSurface::attachPainter(Painter& painter)
{
    if(_painter)
    {
        _painter->onDetachSurface(*this);
        _painter = 0;
    }

    _painter = &painter;
}


void PaintSurface::detachPainter(Painter& painter)
{
    if(_painter)
        _painter = 0;
}

} // namespace

} // namespace
