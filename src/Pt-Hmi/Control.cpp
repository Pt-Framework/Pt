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
#include <Pt/Hmi/PaintRegion.h>
#include <Pt/Hmi/Painter.h>
#include <Pt/Hmi/Window.h>
#include <Pt/Hmi/Style.h>
#include <Pt/Hmi/StyleOptions.h>

namespace Pt {

namespace Hmi {

Control::Control()
: _style(0)
, _isHighlighted(false)
{    
}


Control::~Control()
{
    delete _style;
}


void Control::setStyle(const Style& style)
{
    delete _style;
    _style = 0;
    _style = new Style(style);
}    


const Style& Control::style() const
{
    return _style ? *_style : Application::instance().style();
}


bool Control::isHighlighted() const
{
    return _isHighlighted;
}


const Gfx::Brush& Control::foreground() const
{
    return _foreground.isValid() ? _foreground.get()
                                 : Application::instance().styleOptions().foreground();
}

void Control::setForeground(const Gfx::Brush& b)
{
    _foreground.set(b);
}


const Gfx::Pen& Control::contour() const
{
    return _contour.isValid() ? _contour.get()
                              : Application::instance().styleOptions().contour();
}


void Control::setContour(const Gfx::Pen& p)
{
    _contour.set(p);
}


void Control::onInvalidate()
{
    Widget::onInvalidate();
    update();
}


void Control::onPaintEvent(const PaintEvent& ev)
{
    Widget::onPaintEvent(ev);

    Gfx::PointF winpos = toWindow( Gfx::PointF(0,0) );
    PaintSurface& windowSurface = this->window()->surface();

    Gfx::RectF paintRect(winpos, size());
    PaintRegion region(windowSurface, paintRect);
    
    onPaint(region, ev.rect());
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
