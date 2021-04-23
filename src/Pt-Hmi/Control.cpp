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
  MA  02110-1301  USA
*/

#include <Pt/Hmi/Control.h>
#include <Pt/Hmi/Application.h>
#include <Pt/Hmi/Window.h>
#include <Pt/Hmi/Style.h>
#include <Pt/Hmi/StyleOptions.h>
#include <Pt/Gfx/Painter.h>
#include <Pt/Gfx/PaintRegion.h>

namespace Pt {

namespace Hmi {

Control::Control()
: _isHighlighted(false)
{
}


Control::~Control()
{
}


Gfx::PaintSurface& Control::surface()
{
    return _surface;
}


bool Control::isHighlighted() const
{
    return _isHighlighted;
}


void Control::setStyleOptions(const StyleOptions& o)
{
    onSetStyleOptions(o);
    invalidate();
}


void Control::onSetStyleOptions(const StyleOptions& o)
{
}


void Control::onSetWindow(Window* w)
{
    if( ! w )
    {   
        _surface.detach();
        return;
    }
    
    Gfx::PointF winpos = toWindow( Gfx::PointF(0, 0) );
    Gfx::PaintSurface& windowSurface = w->surface();

    Gfx::RectF paintRect( winpos, size() );
    _surface.attach(windowSurface, paintRect);
}


void Control::onInvalidate()
{
    Widget::onInvalidate();
    update();
}


void Control::onLayout(Layouter& layouter, const Gfx::RectF& rect)
{
     Widget::onLayout(layouter, rect);
}


// void Control::onPaintContent(const Gfx::RectF& r)
// {
//     Gfx::PointF pos = toWindow( Gfx::PointF(0, 0) );
//     Gfx::RectF surfaceRect( pos, size() );
//     _surface.reset(surfaceRect);

//     Widget::onPaintContent(r);
// }


void Control::onPaintEvent(const PaintEvent& ev)
{
    Widget::onPaintEvent(ev);

    Gfx::PointF pos = toWindow( Gfx::PointF(0, 0) );
    Gfx::RectF surfaceRect( pos, size() );
    _surface.reset(surfaceRect);

    onPaint(_surface, ev.rect() );
}


void Control::onMoveEvent(const MoveEvent& ev)
{
    Widget::onMoveEvent(ev);

    // Gfx::PointF pos = toWindow( Gfx::PointF(0, 0) );
    // Gfx::RectF surfaceRect( pos, size() );
    // _surface.reset(surfaceRect);
}


void Control::onResizeEvent(const ResizeEvent& ev)
{
    Widget::onResizeEvent(ev);

    // Gfx::PointF pos = toWindow( Gfx::PointF(0, 0) );
    // Gfx::RectF surfaceRect( pos, size() );
    // _surface.reset(surfaceRect);
}


void Control::onFocusEvent(const FocusEvent& ev)
{
    Widget::onFocusEvent(ev);

    invalidate();
}


void Control::onEnterEvent( const EnterEvent& ev)
{
    Widget::onEnterEvent(ev);

    _isHighlighted = true;
    
    invalidate();
}


void Control::onLeaveEvent(const LeaveEvent& ev)
{
    Widget::onLeaveEvent(ev);

    _isHighlighted = false;

    invalidate();
}

} // namespace

} // namespace
