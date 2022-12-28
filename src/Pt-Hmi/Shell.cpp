/* Copyright (C) 2015 Laurentiu-Gheorghe Crisan
   Copyright (C) 2015 Marc Boris Duerner
  
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
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  
  02110-1301 USA
*/

#include <Pt/Hmi/Shell.h>
#include <Pt/Hmi/Window.h>
#include <Pt/Hmi/MouseEvent.h>
#include <Pt/Hmi/KeyEvent.h>
#include <Pt/Hmi/LayoutEvent.h> // RescaleEvent
#include <Pt/Hmi/PaintEvent.h>
#include <Pt/Gfx/Point.h>
#include <Pt/Gfx/Painter.h>
#include <Pt/Gfx/PaintSurface.h>
#include <Pt/String.h>

#include <cmath>

namespace Pt {

namespace Hmi {

///////////////////////////////////////////////////////////////////////
// Shell
///////////////////////////////////////////////////////////////////////

Shell::Shell()
: _content(0)
, _pointer(0)
{
    _wm.setParent(this);
    _wm.setSurface( &this->surface() );

    setFocusPolicy(Widget::AcceptFocus);
}


Shell::~Shell()
{
}


void Shell::addWindow(Window& w)
{
    w.setParent(_wm);
}


void Shell::removeWindow(Window& w)
{
    w.unparent();
}


Widget* Shell::content() 
{
    return _content;
}


const Widget* Shell::content()  const 
{
    return _content;
}


void Shell::setContent(Widget* widget)
{
    if(_content)
    {
        _content->setParent(0);
    }

    _content = widget;

    if(widget)
    {
        widget->setParent(this);
    }
}

///////////////////////////////////////////////////////////////////////
// Widget
///////////////////////////////////////////////////////////////////////

void Shell::onRemoveWidget(Widget& w)
{
    Widget::onRemoveWidget(w);

    if(&w == _content)
        _content = 0;
}

///////////////////////////////////////////////////////////////////////
// Visual
///////////////////////////////////////////////////////////////////////

Visual* Shell::onHitTest(const Gfx::PointF& p)
{
    Visual* hit = _wm.hitTest(p);
    if(hit)
        return hit;

    return Widget::onHitTest(p);
}


void Shell::onSetCapture(bool capture)
{
    Widget::onSetCapture(capture);
}


void Shell::onRelease()
{
    Widget::onRelease();

    setPointer(false);
    setCapture(false);

    _wm.release();
}

///////////////////////////////////////////////////////////////////////
// Implementation
///////////////////////////////////////////////////////////////////////

Gfx::SizeF Shell::onMeasure(const SizePolicy& policy)
{
    if(_content)
        return _content->measure(policy);

    return policy.size();
}


void Shell::onLayout(const Gfx::RectF& rect)
{
    Widget::onLayout(rect);

    if(_content)
    {
        Gfx::PointF pos(padding().left() + _content->margin().left(), 
                        padding().top()  + _content->margin().top());
        
        double hspace = padding().leftRight() + _content->margin().leftRight();
        double vspace = padding().topBottom() + _content->margin().topBottom();

        Gfx::SizeF size;
        size.setWidth( rect.width() - hspace );
        size.setHeight( rect.height() - vspace );

        _content->move(pos);
        _content->resize(size);
    }
    
    //_shell.move( rect.topLeft() );
    //_shell.resize( rect.size() );
}


void Shell::onPaint(Gfx::PaintSurface& surface, const Gfx::RectF& rect)
{
    Widget::onPaint(surface, rect);
}


void Shell::onProcessRescaleEvent(const RescaleEvent& ev)
{
    Widget::onProcessRescaleEvent(ev);

    double scaling = ev.scaleFactor();

    RescaleEvent rev(_wm, scaling);
    _wm.processEvent(rev);
}


void Shell::onProcessPaintEvent(const PaintEvent& ev)
{
    Widget::onProcessPaintEvent(ev);

    const Gfx::RectF& rect = ev.rect();

    PaintEvent pev(_wm, rect);
    _wm.processEvent(pev);
}


void Shell::onProcessResizeEvent(const ResizeEvent& ev)
{
    Widget::onProcessResizeEvent(ev);

    ResizeEvent rev(_wm, ev.size());
    _wm.processEvent(rev);
}


void Shell::onProcessEnableEvent(const EnableEvent& ev)
{
    Widget::onProcessEnableEvent(ev);

    bool enable = ev.enabled();
    if( ! isEnabled() )
      enable = false;

    EnableEvent eev(_wm, enable);
    _wm.processEvent(eev);
}


void Shell::onProcessMouseEvent(const MouseEvent& ev)
{
    if( ! acceptsInput() )
        return;

    bool consumed = _wm.processMouseEvent(ev);
    if(consumed)
        return;

    Widget::onProcessMouseEvent(ev);
}


void Shell::onProcessTouchEvent(const TouchEvent& ev)
{
    if( ! acceptsInput() )
        return;

    Widget::onProcessTouchEvent(ev);
}


void Shell::onProcessEnterEvent(const EnterEvent& ev)
{
    Widget::onProcessEnterEvent(ev);
}


void Shell::onProcessLeaveEvent(const LeaveEvent& ev)
{
    Widget::onProcessLeaveEvent(ev);
}


void Shell::onProcessScrollEvent(const ScrollEvent& ev)
{
    if( ! acceptsInput() )
        return;

    if( _wm.activeWindow() )
    {
        _wm.processEvent(ev);
        return;
    }

    Widget::onProcessScrollEvent(ev);
}


void Shell::onProcessKeyEvent(const KeyEvent& ev)
{
    if( ! acceptsInput() )
        return;

    if(_wm.activeWindow())
    {
        _wm.processEvent(ev);
        return;
    }

    Widget::onProcessKeyEvent(ev);
}

} // namespace

} // namespace
