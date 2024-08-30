/* Copyright (C) 2020 Marc Boris Duerner 
  
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

#include <Pt/Gfx/Canvas.h>
#include <Pt/Gfx/Paint.h>
#include <Pt/Gfx/PaintSurface.h>
#include <Pt/Gfx/Algorithm.h>

namespace Pt {

namespace Gfx {

Canvas::Canvas(PaintSurface& surface)
: _surface(&surface)
{
}


Canvas::~Canvas()
{
    if(_paint)
    {
        _paint->onDetachCanvas(*this);
        _paint = 0;
    }
}


void Canvas::attachPaint(PaintContext& paint)
{
    if(_paint)
    {
        onReleasePaint();
        _paint->onDetachCanvas(*this);
        _paint = 0;
    }

    _paint = &paint;
}


void Canvas::detachPaint(PaintContext& paint)
{
    if(_paint)
    {
        onReleasePaint();
        _paint = 0;
    }
}


const Gfx::ImageFormat& Canvas::format() const
{
    return _surface->format();
}


const Gfx::SizeF& Canvas::size() const
{   
    return _surface->size();
}


const Scaling& Canvas::scaling() const
{
    return onGetScaling();
}


bool Canvas::beginPaint(Gfx::PaintContext* context,
                        const Gfx::Paint& paint)
{
    if(_paint)
    {
        onReleasePaint();
        _paint->onDetachCanvas(*this);
        _paint = 0;
    }

    if( context->scaling() != scaling() )
        return false;

    bool isActive = onBeginPaint(context, paint);
    if(isActive)
        context->setCanvas(*this);
    
    return isActive;
}


PaintContextPtr Canvas::beginPaint(const Gfx::Paint& paint)
{
    if(_paint)
    {
        onReleasePaint();
        _paint->onDetachCanvas(*this);
        _paint = 0;
    }

    Gfx::PaintContext* context = onBeginPaint(paint);
    if(context)
    {
        context->setCanvas(*this);

        context->setCompositionMode( paint.compositionMode() );
        context->setPen( paint.pen() );
        context->setBrush( paint.brush() );
        context->setFont( paint.font() );

        const Gfx::RectF& clip = paint.clip();
        if( clip.isNull() )
            context->resetClip();
        else
            context->setClip(clip);
    }

    return PaintContextPtr(context);
}

} // namespace

} // namespace
