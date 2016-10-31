/* Copyright (C) 2015 Laurentiu-Gheorghe Crisan
 
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

#include "ApplicationImpl.h"
#include <Pt/Hmi/Application.h>
#include <Pt/Hmi/PaintEvent.h>
#include <Pt/Hmi/Widget.h>
#include <Pt/Hmi/Window.h>
#include <cassert>

namespace Pt {

namespace Hmi {

Application::Application(int argc, char** argv)
: System::Application(0, argc, argv)
, _impl( new ApplicationImpl() ) 
, _mainScreen(0)
, _lastId(1)
, _pointerWindow(0)
, _pointerWidget(0)
, _pointerGrabber(0)
, _font("",12)
{
    this->init(*_impl);

    _mainScreen = new Screen(*_impl);

    loop().eventReceived() += Pt::slot(*this, &Application::onResizeEvent );
    loop().eventReceived() += Pt::slot(*this, &Application::onMoveEvent );
    loop().eventReceived() += Pt::slot(*this, &Application::onKeyEvent );
    loop().eventReceived() += Pt::slot(*this, &Application::onUpdateEvent ); 
    loop().eventReceived() += Pt::slot(*this, &Application::onPaintEvent ); 
    loop().eventReceived() += Pt::slot(*this, &Application::onMouseEvent );
    loop().eventReceived() += Pt::slot(*this, &Application::onTouchEvent );
    loop().eventReceived() += Pt::slot(*this, &Application::onScrollEvent );
    loop().eventReceived() += Pt::slot(*this, &Application::onActivateEvent );
    loop().eventReceived() += Pt::slot(*this, &Application::onEnableEvent );
    loop().eventReceived() += Pt::slot(*this, &Application::onShowEvent );
    loop().eventReceived() += Pt::slot(*this, &Application::onCloseEvent );
    loop().eventReceived() += Pt::slot(*this, &Application::onEnterEvent );
    loop().eventReceived() += Pt::slot(*this, &Application::onLeaveEvent );
    loop().eventReceived() += Pt::slot(*this, &Application::onFocusEvent );
    loop().eventReceived() += Pt::slot(*this, &Application::onWindowStateEvent);
    loop().eventReceived() += Pt::slot(*this, &Application::onInvalidateEvent);
}


Application::~Application()
{
    delete _mainScreen;
    delete _impl;
}


Application& Application::instance()
{
    return static_cast<Application&>( System::Application::instance() );
}


void Application::invalidate()
{
  VisualMap::iterator it = _visuals.begin();
  
  for( ; it != _visuals.end(); ++it)
  {
    InvalidateEvent ev(it->first);
    loop().commitEvent(ev);
  }
}


const Screen& Application::screen() const
{
    return *_mainScreen;
}


Screen& Application::screen()
{
    return *_mainScreen;
}


void Application::setCursor( const Cursor* cursor )
{
  _impl->setCursor( cursor );
}


const Gfx::Font& Application::font() const
{
    return _font;
}


void Application::setFont(const Gfx::Font& font)
{
    _font = font;
    invalidate();
}


Window* Application::pointerWindow()
{
    return _pointerWindow;
}


const Window* Application::pointerWindow() const
{
    return _pointerWindow;
}


void Application::setPointerWindow(Window* w)
{
    if( _pointerWindow == w )
        return;

    if( _pointerWindow )
    {
        Pt::Hmi::LeaveEvent leaveEvent( _pointerWindow->vid() );
        loop().commitEvent(leaveEvent);
    }

    _pointerWindow = w;

    if( _pointerWindow )
    {
        Pt::Hmi::EnterEvent enterEvent(_pointerWindow->vid());
        loop().commitEvent(enterEvent);
    }
}


Widget* Application::pointerWidget()
{
    return _pointerWidget;
}


const Widget* Application::pointerWidget() const
{
    return _pointerWidget;
}


void Application::setPointerWidget( Widget* widget ) 
{
    if( _pointerWidget == widget )
        return;

    if( _pointerWidget )
    {
        Widget* w = _pointerWidget;
        _pointerWidget = widget;

        LeaveEvent ev( w->vid() );
        Application::instance().loop().commitEvent(ev);
    }
    else
    {
        _pointerWidget = widget;
    }

    if( _pointerWidget )
    {
        EnterEvent ev( _pointerWidget->vid() );
        Application::instance().loop().commitEvent(ev);
    }
}


Visual* Application::pointerGrabber()
{ 
    return _pointerGrabber; 
}


void Application::grabPointer(Window& grabber)
{    
    _impl->grabPointer(grabber);

    _pointerGrabber = &grabber;

    setPointerWidget(0);
}


void Application::releasePointer(Window& grabber)
{
    if(_pointerGrabber != static_cast<Visual*>(&grabber) )
        return;
    
    _impl->releaseMouse(grabber); 
    
    _pointerGrabber = 0;
}


void Application::grabPointer(Widget& grabber)
{
    _impl->grabPointer(grabber);

    _pointerGrabber = &grabber;

    setPointerWidget(&grabber);
}


void Application::releasePointer(Widget& grabber)
{
    if( _pointerGrabber != static_cast<Visual*>(&grabber) )
        return;

    _impl->releaseMouse(grabber);

    _pointerGrabber = 0;
}

        
Pt::uint64_t Application::makeId()
{
    return _lastId++;
}


const Visual* Application::findVisual(Pt::uint64_t id) const
{
    VisualMap::const_iterator it =_visuals.find(id);
    return it != _visuals.end() ? it->second : 0; 
}


void Application::registerVisual( Visual& visual )
{
    VisualMap::const_iterator it = _visuals.find( visual.vid() );
    assert( it == _visuals.end() );

    VisualMap::value_type elem(visual.vid(), &visual);
    _visuals.insert(elem);
}


void Application::unregisterVisual( Visual& visual )
{
   _visuals.erase( visual.vid() );
}


void Application::nextEvent()
{
    _impl->nextEvent();
}


void Application::onUpdateEvent(const UpdateEvent& ev)
{
    VisualMap::iterator vit = _visuals.find( ev.vid() );
    if( vit == _visuals.end() )
        return;

    vit->second->processEvent(ev);
}


void Application::onPaintEvent(const PaintEvent& ev)
{
    VisualMap::iterator vit = _visuals.find( ev.vid() );

    if( vit == _visuals.end() )
        return;

    vit->second->processEvent(ev);
}


void Application::onResizeEvent(const ResizeEvent& ev)
{
    VisualMap::iterator it = _visuals.find( ev.vid() );

    if( it == _visuals.end() )
        return;

    it->second->processEvent(ev);
}


void Application::onMouseEvent(const MouseEvent& ev)
{
    VisualMap::iterator it = _visuals.find( ev.vid() );

    if( it == _visuals.end() )
        return;

    it->second->processEvent(ev);
}


void Application::onTouchEvent(const TouchEvent& ev)
{
    VisualMap::iterator it = _visuals.find( ev.vid() );

    if( it == _visuals.end() )
        return;

    it->second->processEvent(ev);
}


void Application::onScrollEvent(const ScrollEvent& ev )
{
    VisualMap::iterator it = _visuals.find( ev.vid() );

    if( it == _visuals.end() )
        return;

    it->second->processEvent(ev);
}


void Application::onMoveEvent(const MoveEvent& ev )
{
    VisualMap::iterator it = _visuals.find( ev.vid() );

    if( it == _visuals.end() )
        return;

    it->second->processEvent(ev);
}


void Application::onActivateEvent( const ActivateEvent& ev )
{
    VisualMap::iterator it = _visuals.find( ev.vid() );

    if( it == _visuals.end() )
        return;

    it->second->processEvent(ev);
}


void Application::onEnableEvent( const EnableEvent& ev )
{
    VisualMap::iterator it = _visuals.find( ev.vid() );

    if( it == _visuals.end() )
        return;

    it->second->processEvent(ev);
}


void Application::onShowEvent( const ShowEvent& ev )
{
    VisualMap::iterator it = _visuals.find( ev.vid() );

    if( it == _visuals.end() )
        return;

    it->second->processEvent(ev);
}


void Application::onKeyEvent( const KeyEvent& ev )
{
    VisualMap::iterator it = _visuals.find( ev.vid() );

    if( it == _visuals.end() )
        return;

    it->second->processEvent(ev);
}


void Application::onCloseEvent(const CloseEvent& ev )
{
    VisualMap::iterator it = _visuals.find( ev.vid() );

    if( it == _visuals.end() )
        return;

    it->second->processEvent(ev);
}


void Application::onEnterEvent( const EnterEvent& ev )
{
    VisualMap::iterator it = _visuals.find( ev.vid() );

    if( it == _visuals.end() )
        return;

    it->second->processEvent(ev);
}


void Application::onLeaveEvent(const LeaveEvent& ev )
{
    VisualMap::iterator it = _visuals.find( ev.vid() );

    if( it == _visuals.end() )
        return;

    it->second->processEvent(ev);
}


void Application::onFocusEvent(const FocusEvent& ev)
{
    VisualMap::iterator it = _visuals.find( ev.vid() );

    if( it == _visuals.end() )
        return;

    it->second->processEvent(ev);
}


void Application::onWindowStateEvent(const WindowStateEvent& ev )
{
    VisualMap::iterator it = _visuals.find( ev.vid() );

    if( it == _visuals.end() )
        return;

    it->second->processEvent(ev);
}


void Application::onInvalidateEvent(const InvalidateEvent& ev)
{
    VisualMap::iterator it = _visuals.find( ev.vid() );

    if( it == _visuals.end() )
        return;

    it->second->processEvent(ev);
}


ApplicationImpl* Application::impl()
{
    return _impl;
}


const Style& Application::style() const
{
    return _ptStyle;
}


Style& Application::style()
{
    return _ptStyle;
}


Gfx::Font Application::makeFont(const Gfx::Font& userFont) const
{
  if( userFont.isNull() )
      return _font;

  if( userFont.name().empty() )
      return Gfx::Font(_font.name(), userFont);

  return _userFont;
}

} // namespace

} // namespace
