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
#include "ScreenImpl.h"
#include <Pt/Hmi/Application.h>
#include <Pt/Hmi/PaintEvent.h>
#include <Pt/Hmi/WindowStateEvent.h>
#include <Pt/Hmi/Window.h>
#include <Pt/Hmi/Widget.h>
#include <Pt/Hmi/InputMethod.h>
#include <cmath>
#include <cassert>
#include <fstream>

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
, _inputMethod(0)
, _onScroll(false)
, _scaling(1)
{
    this->init(*_impl);

    // default style with default options
    _style = PlatinumStyle();

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
    loop().eventReceived() += Pt::slot(*this, &Application::onLayoutEvent);

    setInputMethod(_defaultInputMethod);
}


Application::~Application()
{
    if(_inputMethod)
        removeInputMethod(*_inputMethod);

    delete _mainScreen;
    delete _impl;
}


Application& Application::instance()
{
    return static_cast<Application&>( System::Application::instance() );
}


const Screen& Application::screen() const
{
    return *_mainScreen;
}


Screen& Application::screen()
{
    return *_mainScreen;
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


void Application::setPointerWidget(Widget* widget) 
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
   //std::clog << "GRAB:    " << typeid(grabber).name() << std::endl;
    
    std::vector<Visual*>::iterator it =
        std::find(_grabbers.begin(), _grabbers.end(), &grabber);

    if( it != _grabbers.end() )
        _grabbers.erase(it);
    
    _impl->grabPointer(grabber);

    setPointerWidget(0);

    _pointerGrabber = &grabber;
    _grabbers.push_back(&grabber);
}


void Application::releasePointer(Window& grabber)
{  
    _pointerGrabber = 0;

    std::vector<Visual*>::iterator it =
        std::find(_grabbers.begin(), _grabbers.end(), &grabber);

    if( it != _grabbers.end() )
    {
        //std::clog << "RELEASE: " << typeid(grabber).name() << std::endl;
        _grabbers.erase(it);
        _impl->releasePointer(grabber);

        grabLast();
    }
}


void Application::grabPointer(Widget& grabber)
{
    //std::clog << "GRAB:    " << typeid(grabber).name() << " " << grabber.vid() << std::endl;

    std::vector<Visual*>::iterator it =
        std::find(_grabbers.begin(), _grabbers.end(), &grabber);

    if( it != _grabbers.end() )
        _grabbers.erase(it);

    _impl->grabPointer(grabber);

    setPointerWidget(&grabber);

    _pointerGrabber = &grabber;
    _grabbers.push_back(&grabber);
}


void Application::releasePointer(Widget& grabber)
{
    _pointerGrabber = 0;

    std::vector<Visual*>::iterator it =
        std::find(_grabbers.begin(), _grabbers.end(), &grabber);

    if( it != _grabbers.end() )
    {
        //std::clog << "RELEASE: " << typeid(grabber).name() << std::endl;
        _grabbers.erase(it);
        _impl->releasePointer(grabber);

        grabLast();
    }
}


void Application::grabLast()
{
    // TODO: the ugly dynamic_Cast can be removed if Visual could return a
    //       pointer to its window. Then grabPointer and releasePointer()
    //       do not need to be overloaded for Window and Widget anymore. Add
    //       such a virtual function to Visual later.

    if( ! _grabbers.empty() )
    {
        Visual* visual = _grabbers.back();
        Window* window = dynamic_cast<Window*>(visual);
        if(window)
        {
            grabPointer(*window);
        }
        else
        {
            Widget* widget = dynamic_cast<Widget*>(visual);
            if(widget)
                grabPointer(*widget);
        }
    }
}


void Application::processTouchEvent(const TouchEvent& ev)
{
    VisualMap::iterator vit = _visuals.find( ev.vid() );
    if( vit == _visuals.end() )
        return;

    //
    // Detect scroll gestures
    //

    if( ev.isPress() )
    {
        //std::clog << "SCROLL START " << std::endl;
        _scrollFrom = vit->second->toScreen( ev.position() );
    }
    else if( ev.isMove() )
    {
        Gfx::PointF scrollTo = vit->second->toScreen( ev.position() );
        
        double delta = scrollTo.y() - _scrollFrom.y();

        if( ! _onScroll && std::fabs(delta) > 10 )
        {
            //std::clog << "SCROLL STARTED" << std::endl;
            _onScroll = true;
            _scrollFrom = scrollTo;
        }
        else if(_onScroll)
        {
            //std::clog << "SCROLLING: " << delta << std::endl;
            Visual* grabber = pointerGrabber();

            ScrollEvent sev(grabber ? grabber->vid() : ev.vid() );
            sev.set(ScrollEvent::Vertical, delta);

            if(grabber)
                loop().commitEvent(sev);
            else
                _mainScreen->impl()->dispatchScrollEvent(sev);
            
            _scrollFrom = scrollTo;
        }
    }
    else
    {
        //std::clog << "SCROLL STOP" << std::endl;
        _onScroll = false;
    }

    //
    // Dispatch event to the pointer grabber or the screen
    //

    TouchEvent tev = ev;

    // IME window has priority
    Window* ime = inputMethod().activeWindow();
    if(ime)
    {
        // find grabber in IME window
        for(std::vector<Visual*>::reverse_iterator it = _grabbers.rbegin(); 
            it != _grabbers.rend(); ++it)
        {
            Widget* grabber = ime->findWidget( (*it)->vid() );
            if(grabber)
            {
                Gfx::PointF screenPos = vit->second->toScreen( ev.position() );
                tev.setPosition( grabber->fromScreen(screenPos) );
                tev.setId( grabber->vid() );
                loop().commitEvent(tev);
                return;
            }
        }

        Gfx::PointF screenPos = vit->second->toScreen( ev.position() );
        Gfx::PointF p = ime->fromScreen(screenPos);
        Gfx::RectF rect( ime->size() );
        if( rect.contains(p) )
        {
            tev.setPosition(p);
            tev.setId( ime->vid() );
            loop().commitEvent(tev);
            return;
        }
    }

    // pointer grab has priority
    Visual* grabber = Application::instance().pointerGrabber();
    if(grabber)
    {
        Gfx::PointF screenPos = vit->second->toScreen( ev.position() );
        tev.setPosition( grabber->fromScreen(screenPos) ); 
        tev.setId( grabber->vid() );
        loop().commitEvent(tev);
        return;
    }

    _mainScreen->impl()->dispatchTouchEvent(tev);
}


void Application::processMouseEvent(const MouseEvent& ev)
{
    VisualMap::iterator vit = _visuals.find( ev.vid() );
    if( vit == _visuals.end() )
        return;

    //
    // Detect scroll gestures
    //
    
    if( ev.isPress() )
    {
        //std::clog << "SCROLL START" << std::endl;
        _scrollFrom = vit->second->toScreen( ev.position() );
    }
    else if( ev.isPressed() )
    {
        Gfx::PointF scrollTo = vit->second->toScreen( ev.position() );
        
        double delta = scrollTo.y() - _scrollFrom.y();
        
        if( ! _onScroll && std::fabs(delta) > 8 )
        {
            //std::clog << "SCROLL STARTED" << std::endl;
            _onScroll = true;
            _scrollFrom = scrollTo;
        }
        else if(_onScroll)
        {
            //std::clog << "SCROLLING: " << delta << std::endl;
            Visual* grabber = pointerGrabber();

            ScrollEvent sev(grabber ? grabber->vid() : ev.vid() );
            sev.set(ScrollEvent::Vertical, delta);

            if(grabber)
                loop().commitEvent(sev);
            else
                _mainScreen->impl()->dispatchScrollEvent(sev);
            
            _scrollFrom = scrollTo;
        }
    }
    else
    {
        //std::clog << "SCROLL STOP" << std::endl;
        _onScroll = false;
    }

    //
    // Dispatch event to the pointer grabber or the screen
    //
    
    MouseEvent mev = ev;

    // IME window has priority
    Window* ime = inputMethod().activeWindow();
    if(ime)
    {
        // find grabber in IME window
        for(std::vector<Visual*>::reverse_iterator it = _grabbers.rbegin(); 
            it != _grabbers.rend(); ++it)
        {
            Widget* grabber = ime->findWidget( (*it)->vid() );
            if(grabber)
            {
                Gfx::PointF screenPos = vit->second->toScreen( ev.position() );
                mev.setPosition( grabber->fromScreen(screenPos) );
                mev.setId( grabber->vid() );
                loop().commitEvent(mev);
                return;
            }
        }

        Gfx::PointF screenPos = vit->second->toScreen( ev.position() );
        Gfx::PointF p = ime->fromScreen(screenPos);
        Gfx::RectF rect( ime->size() );
        if( rect.contains(p) )
        {
            mev.setPosition(p);
            mev.setId( ime->vid() );
            loop().commitEvent(mev);
            return;
        }
    }

    // pointer grab has priority
    Visual* grabber = pointerGrabber();
    if(grabber)
    {
        Gfx::PointF screenPos = vit->second->toScreen( ev.position() );
        mev.setPosition( grabber->fromScreen(screenPos) );
        mev.setId( grabber->vid() );
        loop().commitEvent(mev);
        return;
    }

    _mainScreen->impl()->dispatchMouseEvent(mev);
}


void Application::sendKeyEvent(const KeyEvent& ev)
{
    _impl->sendKeyEvent(ev);
}


void Application::sendMouseEvent(const MouseEvent& ev)
{
    _impl->sendMouseEvent(ev);
}


Pt::Timespan Application::inactivityTime() const
{
    return _impl->inactivityTime();
}


void Application::setCursor( const Cursor* cursor )
{
    _impl->setCursor( cursor );
}


const Style& Application::style() const
{
    return _style;
}


void Application::setStyle(const Style& s)
{
    _style = s;
}


const StyleOptions& Application::styleOptions() const
{
    return _styleOptions;
}


StyleOptions& Application::styleOptions()
{
    return _styleOptions;
}


InputMethod& Application::inputMethod()
{
    return *_inputMethod;
}


void Application::setInputMethod(InputMethod& im)
{
    if(_inputMethod)
        removeInputMethod(*_inputMethod);

    _inputMethod = &im;
    im.registerApplication(*this);
}


void Application::removeInputMethod(InputMethod& im)
{
    if(_inputMethod == &im)
    {
        _inputMethod = &_defaultInputMethod;
        im.unregisterApplication(*this);
    }
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


void Application::nextEvent()
{
    _impl->nextEvent();
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


ApplicationImpl* Application::impl()
{
    return _impl;
}


void Application::setFontDir(const Pt::System::Path& dir)
{
    _impl->setFontDir(dir);
}


void Application::setDefaultFont(const std::string& fontName)
{
    _impl->setDefaultFont(fontName);
}

void Application::loadImage(const System::Path& path, Gfx::Image& image)
{
    std::ifstream fs(path.toLocal().c_str(), std::ios::binary);

    _iconReader.reset();
    _iconReader.attach(fs, image);
    _iconReader.get();
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


void Application::onLayoutEvent(const LayoutEvent& ev)
{
    VisualMap::iterator it = _visuals.find( ev.vid() );

    if( it == _visuals.end() )
        return;

    it->second->processEvent(ev);
}

} // namespace

} // namespace
