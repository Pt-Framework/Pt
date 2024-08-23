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
, _paint(0)
{
}


Canvas::~Canvas()
{
    if(_paint)
    {
        _paint->detachCanvas(*this);
        _paint = 0;
    }
}


void Canvas::onDetachPaintContext(PaintContext& paint)
{
    if(_paint)
    {
        onFinishPaint();
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
    return _surface->scaling();
}


//PaintContext* Canvas::beginPaint(PaintContext* p)
//{
//    finishPaint();
//
//    PaintContext* paint = onBeginPaint(p);
//    if(paint)
//    {
//        paint->attachCanvas(*this);
//        _paint = paint;
//    }
//
//    return paint;
//}


void Canvas::init(PaintContext* paint)
{
    finishPaint();

    if(paint)
    {
        paint->attachCanvas(*this);
        _paint = paint;
    }
}


void Canvas::finishPaint()
{
    if(_paint)
    {
        _paint->detachCanvas(*this);
        onDetachPaintContext(*_paint);
    }
}


//void Canvas::drawSurface(const Gfx::PointF& to, 
//                         const Gfx::PaintSurface& surface)
//{
//    Pt::Gfx::Image image = surface.toImage();
//    if( image.format() == format() )
//    {
//        drawImage(to, image);
//        return;
//    }
//
//    Pt::Gfx::Image dest( format(), image.size() );
//    Pt::Gfx::copy( image.begin(), image.end(), dest.begin() );
//    drawImage(to, dest);
//}
//
//
//void Canvas::drawSurface(const Gfx::PointF& to,
//                         const Gfx::PaintSurface& surface,
//                         const Gfx::RectF& rect)
//{
//    Pt::Gfx::Image image = surface.toImage();
//    if( image.format() == format() )
//    {
//        drawImage(to, image, rect);
//        return;
//    }
//
//    Pt::Gfx::Image dest( format(), image.size() );
//    Pt::Gfx::copy( image.begin(), image.end(), dest.begin() );
//    drawImage(to, dest, rect);
//}

} // namespace

} // namespace
