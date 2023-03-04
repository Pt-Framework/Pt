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

#include "ScreenImpl.h"

#include <Pt/Hmi/Screen.h>
#include <Pt/Hmi/Application.h>
#include <Pt/Hmi/LayoutEvent.h> // RescaleEvent
#include <Pt/Hmi/ResizeEvent.h>
#include <Pt/Hmi/MouseEvent.h>
#include <Pt/Hmi/TouchEvent.h>
#include <Pt/Hmi/ScrollEvent.h>
#include <Pt/Hmi/KeyEvent.h>
#include <Pt/Hmi/PaintEvent.h>
#include <Pt/Hmi/Window.h>

namespace Pt {

namespace Hmi {

Screen::Screen(ApplicationImpl& app)
: _impl( new ScreenImpl(app) )
, _updates(0)
, _pointer(0)
{
    _impl->setParent(this);
    _impl->setNextResponder(this);
}


Screen::~Screen()
{
    delete _impl;
}


void Screen::onInit(ScreenImpl& s)
{
}


void Screen::onRelease(ScreenImpl& s)
{
}


void Screen::onResize(ScreenImpl& s, const Gfx::SizeF& size)
{
    ResizeEvent rev(*this, size);
    processEvent(rev);
}


void Screen::onShow(ScreenImpl& s, bool isShow)
{
    ShowEvent showEv(*this, true);
    processEvent(showEv);
}


void Screen::addWindow(Window& w)
{
    _impl->addWindow(w);
}


void Screen::removeWindow(Window& w)
{
    _impl->removeWindow(w);
}


Window* Screen::findWindow2(const std::string& name)
{
    std::vector<Window*>::const_iterator it;
    for(it = windows().begin(); it != windows().end(); ++it)
    {
        if( (*it)->name() == name )
            return *it;
    }
    
    return 0;
}


Widget* Screen::findWidget2(const std::string& name)
{
    std::vector<Window*>::const_iterator it;
    for(it = windows().begin(); it != windows().end(); ++it)
    {
        Window* window = *it;
        
        Widget* widget = window->findWidget2(name);
        if(widget)
            return widget;
    }
    
    return 0;
}


const std::vector<Window*>& Screen::windows() const
{
  return _impl->windows();
}


void Screen::setPointer(Visual* visual)
{
    if( _pointer == visual )
        return;

    if(_pointer)
    {
        //std::clog << "POINTER LEAVE: " << typeid(*_pointer).name() << std::endl;
        Pt::Hmi::LeaveEvent ev ( *_pointer );
        _pointer->processEvent(ev);
    }

    _pointer = visual;

    if(_pointer)
    {
        //std::clog << "POINTER ENTER: " << typeid(*visual).name() << std::endl;
        EnterEvent ev( *_pointer );
        _pointer->processEvent(ev);
    }
}


void Screen::setPointer(Visual& visual, bool isPointer)
{
    if( ! isPointer )
    {
        if( _pointer == &visual )
            setPointer(0);
    }

    if(isPointer)
    {
        setPointer(&visual);
    }
}


ScreenImpl* Screen::impl()
{
    return _impl;
}

///////////////////////////////////////////////////////////////////////
// Visual
///////////////////////////////////////////////////////////////////////

Visual* Screen::onHitTest(const Gfx::PointF& pos)
{
    return _impl->hitTest(pos);
}


Gfx::PointF Screen::onToParent(const Gfx::PointF& pos) const
{
    return pos;
}


Gfx::PointF Screen::onFromParent(const Gfx::PointF& pos) const
{
    return pos;
}


Gfx::PointF Screen::onToGlobal(const Gfx::PointF& pos) const
{
    return pos;
}


Gfx::PointF Screen::onFromGlobal(const Gfx::PointF& pos) const
{
    return pos;
}


void Screen::onProcessEvent(const Event& ev)
{
    Base::onProcessEvent(ev);
}

///////////////////////////////////////////////////////////////////////
// Implementation
///////////////////////////////////////////////////////////////////////

void Screen::onProcessRescaleEvent(const RescaleEvent& ev)
{   
    Base::onProcessRescaleEvent(ev);

    double scaling = ev.scaleFactor();

    RescaleEvent rev(*_impl, scaling);
    _impl->processEvent(rev);
}


void Screen::onRescaleEvent(const RescaleEvent& ev)
{
    Base::onRescaleEvent(ev);
}


void Screen::onRescale(double scaling)
{
    Base::onRescale(scaling);
}


void Screen::onRequestRepaint(const Gfx::RectF& rect)
{
    _updateRect.unify(rect);
    ++_updates;

    PaintEvent uev(*this, _updateRect);
    Application::instance().loop().commitEvent(uev);
}


void Screen::onProcessPaintEvent(const PaintEvent& ev)
{
    if(_updates == 0)
    {
        //std::clog << "PAINT EVENT screen skipped" << std::endl;
        return;
    }

    --_updates;

    // skip all updates except the last one
    if(_updates > 0)
      return;
    
    _updateRect.clear();

    //_clock.start();

    Base::onProcessPaintEvent(ev);

    const Gfx::RectF& screenRect = ev.rect();
    PaintEvent pev(*_impl, screenRect);
    
    _impl->processEvent(pev);

    //static int nnn = 0;
    //std::clog << "screen update: " 
    //          << _clock.stop().toUSecs() << " usecs. " 
    //          << ++nnn << std::endl;
    //std::clog << "    " << ev.rect().topLeft().x() << ',' << ev.rect().topLeft().y()
    //          << ' ' << ev.rect().width() << 'x' << ev.rect().height() << std::endl;
}


void Screen::onPaintEvent(const PaintEvent& ev)
{    
    Base::onPaintEvent(ev);

    const Gfx::RectF& rect = ev.rect();
    onPaint(rect);
}


void Screen::onPaint(const Gfx::RectF& rect)
{
}


void Screen::onProcessResizeEvent(const ResizeEvent& ev)
{
    Base::onProcessResizeEvent(ev);

    _impl->processEvent(ev);
}


void Screen::onResizeEvent(const ResizeEvent& ev)
{
    Base::onResizeEvent(ev);
}


void Screen::onProcessMouseEvent(const MouseEvent& ev)
{
    _impl->processEvent(ev);
}


bool Screen::onMouseEvent(const MouseEvent& ev)
{ 
    return Base::onMouseEvent(ev);
}


void Screen::onProcessTouchEvent(const TouchEvent& ev)
{
    _impl->processEvent(ev);
}


bool Screen::onTouchEvent(const TouchEvent& ev)
{ 
    return Base::onTouchEvent(ev);
}


void Screen::onProcessScrollEvent(const ScrollEvent& ev)
{
    if(_pointer)
    {
        _pointer->processEvent(ev);
        return; 
    }

    _impl->processEvent(ev);
}


bool Screen::onScrollEvent(const ScrollEvent& ev)
{ 
    return Base::onScrollEvent(ev);
}


void Screen::onProcessKeyEvent(const KeyEvent& ev)
{
    _impl->processEvent(ev);
}


bool Screen::onKeyEvent(const KeyEvent& ev)
{ 
    return Base::onKeyEvent(ev);
}

} // namespace

} // namespace
