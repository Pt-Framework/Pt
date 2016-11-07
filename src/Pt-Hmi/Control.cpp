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
#include <Pt/Hmi/PaintRegion.h>
#include <Pt/Hmi/Painter.h>
#include <Pt/Hmi/Window.h>
#include <Pt/Hmi/Style.h>
#include <Pt/Hmi/StyleOptions.h>

namespace Pt {

namespace Hmi {

Control::Control()
: _style(0)
, _styleOptions(0)
{    
}


Control::~Control()
{
    delete _style;
    delete _styleOptions;
}


void Control::setStyle(const Style& style)
{
    delete _style;
    _style = 0;
    _style = new Style(style);
}    


const Style* Control::style() const
{
    return _style;
}


void Control::setStyleOptions(const StyleOptions& opts)
{
    delete _styleOptions;
    _styleOptions = 0;
    _styleOptions = new StyleOptions(opts);
}


const StyleOptions* Control::styleOptions() const
{
    return _styleOptions;
}


void Control::onClicked()
{
}


void Control::onMouseEvent(const MouseEvent& ev)
{    
    Widget::onMouseEvent(ev);

    Gfx::RectF rect( Gfx::PointF(0,0), size() );

    if( ! rect.contains( ev.position() ) )
        return;
    
    // TODO: use pointer grabbing
    if( ev.isRelease() && hasFocus() )
    {
        onClicked();
    }
}


void Control::onTouchEvent(const TouchEvent& ev)
{    
    Widget::onTouchEvent(ev);

    Gfx::RectF rect( Gfx::PointF(0,0), size() );
    if( ! rect.contains( ev.position() ) )
        return;
    
    if( ev.isRelease() && hasFocus() )
    {
        onClicked();
    }
}


void Control::onPaintEvent(const PaintEvent& ev)
{
    Widget::onPaintEvent(ev);

    Gfx::PointF winpos = toWindow( Gfx::PointF(0,0) );
    PaintSurface& windowSurface = this->window()->surface();

    Gfx::RectF paintRect(winpos, size());
    PaintRegion region(windowSurface, paintRect);
    
    onPaintBackground(region, ev.rect());
    onPaintContent(region, ev.rect());
}

} // namespace

} // namespace

