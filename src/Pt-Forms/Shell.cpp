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

#include <Pt/Forms/Shell.h>
#include <Pt/Forms/Window.h>
#include <Pt/Forms/MouseEvent.h>
#include <Pt/Forms/KeyEvent.h>
#include <Pt/Forms/LayoutEvent.h> // RescaleEvent
#include <Pt/Forms/PaintEvent.h>
#include <Pt/Gfx/Point.h>
#include <Pt/Gfx/Painter.h>
#include <Pt/Gfx/PaintSurface.h>
#include <Pt/String.h>

#include <cmath>

namespace Pt {

namespace Forms {

///////////////////////////////////////////////////////////////////////
// Shell
///////////////////////////////////////////////////////////////////////

Shell::Shell()
: _content(0)
, _pointer(0)
{
    _wm.setParent(this);

    setFocusPolicy(Control::AcceptFocus);
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


const std::vector<Window*>& Shell::windows() const
{
    return _wm.windows();
}


WindowManager& Shell::windowManager()
{
    return _wm;
}


Control* Shell::content() 
{
    return _content;
}


const Control* Shell::content()  const 
{
    return _content;
}


void Shell::setContent(Control* control)
{
    if(_content)
    {
        _content->unparent();
    }

    _content = control;

    if(control)
    {
        control->setParent(*this);
    }
}

///////////////////////////////////////////////////////////////////////
// Control
///////////////////////////////////////////////////////////////////////

void Shell::onSetSurface(Gfx::PaintSurface* surface, const Gfx::PointF& pos)
{
    Base::onSetSurface(surface, pos);

    //_wm.setSurface( surface, pos );
}


void Shell::onRemoveControl(Control& control)
{
    Control::onRemoveControl(control);

    if(&control == _content)
        _content = 0;
}

///////////////////////////////////////////////////////////////////////
// Visual
///////////////////////////////////////////////////////////////////////

Visual* Shell::onHitTest(const Gfx::PointF& p)
{
    if( ! bounds().contains(p) )
        return 0;

    // TODO: convert to WM coordinates

    Visual* hit = _wm.hitTest(p);
    if(hit)
        return hit;

    return Control::onHitTest(p);
}

///////////////////////////////////////////////////////////////////////
// Control
///////////////////////////////////////////////////////////////////////

Gfx::SizeF Shell::onMeasure(const SizePolicy& policy)
{
    if(_content)
        return _content->measure(policy);

    return policy.size();
}


void Shell::onLayout(const Gfx::RectF& rect)
{
    Control::onLayout(rect);

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
    Control::onPaint(surface, rect);
}


void Shell::onProcessRescaleEvent(const RescaleEvent& ev)
{
    Base::onProcessRescaleEvent(ev);

    double scaling = ev.scaleFactor();
    RescaleEvent rev(_wm, scaling);
    _wm.processEvent(rev);
}


void Shell::onProcessPaintEvent(const PaintEvent& ev)
{
    Control::onProcessPaintEvent(ev);

    const Gfx::RectF& rect = ev.rect();

    PaintEvent pev(_wm, rect);
    _wm.processEvent(pev);
}


void Shell::onProcessResizeEvent(const ResizeEvent& ev)
{
    Control::onProcessResizeEvent(ev);

    ResizeEvent rev(_wm, ev.size());
    _wm.processEvent(rev);
}


void Shell::onProcessEnableEvent(const EnableEvent& ev)
{
    Control::onProcessEnableEvent(ev);

    EnableEvent eev(_wm, ev.enabled());
    _wm.processEvent(eev);
}


void Shell::onProcessMouseEvent(const MouseEvent& ev)
{
    if( ! acceptsInput() )
        return;

    bool consumed = _wm.processMouseEvent(ev);
    if(consumed)
        return;

    Control::onProcessMouseEvent(ev);
}


void Shell::onProcessTouchEvent(const TouchEvent& ev)
{
    bool consumed = _wm.processTouchEvent(ev);
    if(consumed)
        return;

    Control::onProcessTouchEvent(ev);
}


void Shell::onProcessEnterEvent(const EnterEvent& ev)
{
    Control::onProcessEnterEvent(ev);
}


void Shell::onProcessLeaveEvent(const LeaveEvent& ev)
{
    Control::onProcessLeaveEvent(ev);
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

    Control::onProcessScrollEvent(ev);
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

    Control::onProcessKeyEvent(ev);
}

///////////////////////////////////////////////////////////////////////
// WindowManager
///////////////////////////////////////////////////////////////////////

void Shell::onRepaint(WindowManager& wm, const Gfx::RectF& rect)
{
    repaint(rect);
}


void Shell::onActivate(WindowManager& wm, bool active)
{
    activate(active);
}

} // namespace

} // namespace
