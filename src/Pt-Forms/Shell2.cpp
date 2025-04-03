/* Copyright (C) 2022 Marc Boris Duerner
  
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
#include <Pt/Forms/ShellWM.h>
#include <Pt/Forms/Window.h>
#include <Pt/Forms/LayoutEvent.h> // RescaleEvent
#include <Pt/Forms/PaintEvent.h>
#include <Pt/Forms/EnableEvent.h>
#include <Pt/Forms/MouseEvent.h>
#include <Pt/Forms/KeyEvent.h>

namespace Pt {

namespace Forms {

Shell::Shell()
: _content(0)
, _wm(0)
{
    _wm = new ShellWM;
    _wm->setParent(this);

    Gfx::PaintSurface& s = surface();
    _wm->setSurface(&s);

    _wm->setNextResponder(this);
    
    setFocusPolicy(Widget::AcceptFocus);
}


Shell::~Shell()
{
    delete _wm;
}


void Shell::addWindow(Window& w)
{
    w.setParent(*_wm);
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


void Shell::onRemoveWidget(Widget& w)
{
    Widget::onRemoveWidget(w);

    if(&w == _content)
        _content = 0;
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

//
// ShellWM
//

void Shell::onSetCapture(bool capture)
{
    Widget::onSetCapture(capture);
}


Gfx::PointF Shell::onFromWM(const ShellWM& wm, const Gfx::PointF& pos) const
{
    return pos;
}


Gfx::PointF Shell::onToWM(const ShellWM& wm, const Gfx::PointF& pos) const
{
    return pos;
}


Gfx::PointF Shell::onToScreen(const ShellWM& wm, const Gfx::PointF& pos) const
{
    Gfx::PointF p = onFromWM(wm, pos);
    return toScreen(p);
}


Gfx::PointF Shell::onFromScreen(const ShellWM& wm, const Gfx::PointF& pos) const
{
    Gfx::PointF p = fromScreen(pos);
    return onToWM(wm, p);
}


void Shell::onRepaint(ShellWM& wm, const Gfx::RectF& rect)
{
    Gfx::PointF clientPos = onFromWM( wm, rect.topLeft() );
    Gfx::RectF clientRect( clientPos, rect.size() );

    repaint(clientRect);
}


void Shell::onActivate(ShellWM& wm, bool active)
{
    activate(active);
}


void Shell::onEnter(ShellWM& wm, Widget& widget)
{
    Widget::onEnter(*this, widget);
}


void Shell::onSetCapture(ShellWM& w, Widget& target, bool capture)
{
    Widget::onSetCapture(*this, target, capture);
}


bool Shell::onIsDescendantOf(const ShellWM& w, Widget& top) const
{    
    if(this == &top)
        return true;

    return isDescendantOf(top);
}

//
// Implementation
//

void Shell::onProcessRescaleEvent(const RescaleEvent& ev)
{
    Widget::onProcessRescaleEvent(ev);

    _wm->processEvent(ev);
}


void Shell::onProcessPaintEvent(const PaintEvent& ev)
{
    Widget::onProcessPaintEvent(ev);

    _wm->processEvent(ev);
}


void Shell::onProcessEnableEvent(const EnableEvent& ev)
{
    Widget::onProcessEnableEvent(ev);

    _wm->processEvent(ev);
}


void Shell::onProcessMouseEvent(const MouseEvent& ev)
{
    if( ! acceptsInput() )
        return;

    Gfx::PointF pos = fromScreen( ev.position() );
    
    WindowFrame* windowFrame = _wm->findWindowFrame(pos);
   
    //
    // deactivate window
    //
    if( ! windowFrame && ev.isPress() )
    {
        _wm->deactivate();
    }

    //
    // dispatch to window
    //
    if(windowFrame)
    {
        _wm->processEvent(ev);
        return;
    }

    //
    // process event
    //
    Widget::onProcessMouseEvent(ev);
}


void Shell::onProcessTouchEvent(const TouchEvent& ev)
{
    if( ! acceptsInput() )
        return;
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

    WindowFrame* activeWindow = _wm->activeWindow();
    if( activeWindow )
    {
        _wm->processEvent(ev);
        return;
    }

    Widget::onProcessScrollEvent(ev);
}


void Shell::onProcessKeyEvent(const KeyEvent& ev)
{
    if( ! acceptsInput() )
        return;

    WindowFrame* activeWindow = _wm->activeWindow();
    if( activeWindow )
    {
        _wm->processEvent(ev);
        return;
    }

    Widget::onProcessKeyEvent(ev);
}

} // namespace

} // namespace
