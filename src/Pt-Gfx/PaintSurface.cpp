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
#include <Pt/Gfx/PaintContext.h>
#include <Pt/Gfx/Painter.h>

#include <limits>

namespace Pt {

namespace Gfx {

PaintSurface::PaintSurface()
: _context(0)
, _painter(0)
{
}


PaintSurface::~PaintSurface()
{
    if(_context)
    {
        _context->detachSurface(*this);
        _context = 0;
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


const Scaling& PaintSurface::scaling() const
{
    return onGetScaling();
}


PaintContext* PaintSurface::getContext(PaintContext* reuse)
{
    return onGetContext(reuse);
}


Gfx::PaintContext* PaintSurface::onGetContext(Gfx::PaintContext* reuse)
{
    if(_context)
    {
        onReleaseContext();
        _context->detachSurface(*this);
        _context = 0;
    }
    
    _context = onCreateContext(reuse);
    _context->attachSurface(*this);

    RectF nobounds(PointF(0, 0),
                   SizeF( std::numeric_limits<double>::max(), 
                          std::numeric_limits<double>::max() ) );
    _context->setRegion(nobounds);

    const Scaling& scale = scaling();
    _context->setScaling(scale);

    return _context;
}


Gfx::PaintContext* PaintSurface::onCreateContext(Gfx::PaintContext* reuse)
{
    return 0;
}


void PaintSurface::onReleaseContext()
{
}


void PaintSurface::invalidate()
{
    if(_context)
    {
        onReleaseContext();
        
        _context->detachSurface(*this);
        _context = 0;
    }
}


void PaintSurface::onDetachContext(PaintContext& context)
{
    if(_context)
    {
        onReleaseContext();
        _context = 0;
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
