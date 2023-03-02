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
   MA 02110-1301 USA
*/

#include "ApplicationImpl.h"
#include "ScreenImpl.h"

#include <Pt/Hmi/Application.h>
#include <Pt/Hmi/Window.h>
#include <Pt/Hmi/Popup.h>
#include <Pt/Hmi/Widget.h>
#include <Pt/Hmi/MouseEvent.h>
#include <Pt/Hmi/TouchEvent.h>
#include <Pt/Hmi/ScrollEvent.h>
#include <Pt/Hmi/EnterEvent.h>
#include <Pt/Hmi/LeaveEvent.h>
#include <Pt/Hmi/KeyEvent.h>
#include <Pt/Hmi/InvalidateEvent.h>
#include <Pt/Hmi/LayoutEvent.h>
#include <Pt/Hmi/PaintEvent.h>
#include <Pt/Hmi/ResizeEvent.h>
#include <Pt/Hmi/MoveEvent.h>
#include <Pt/Hmi/ActivateEvent.h>
#include <Pt/Hmi/EnableEvent.h>
#include <Pt/Hmi/ShowEvent.h>
#include <Pt/Hmi/CloseEvent.h>
#include <Pt/Hmi/FocusEvent.h>
#include <Pt/Hmi/WindowStateEvent.h>

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
, _defaultInputMethod(0)
, _inputMethod(0)
, _onScroll(false)
, _scaling(1)
{
    this->init(*_impl);

    // default style with default options
    _style = PlatinumStyle();

    _mainScreen = new Screen(*_impl);

    loop().eventReceived() += Pt::slot(*this, &Application::onDispatchMouseEvent);
    _eventReceived += Pt::slot(*this, &Application::onProcessMouseEvent);
    
    loop().eventReceived() += Pt::slot(*this, &Application::onDispatchTouchEvent);
    _eventReceived += Pt::slot(*this, &Application::onProcessTouchEvent);
    
    loop().eventReceived() += Pt::slot(*this, &Application::onDispatchScrollEvent);
    _eventReceived += Pt::slot(*this, &Application::onProcessScrollEvent);
    
    loop().eventReceived() += Pt::slot(*this, &Application::onDispatchEnterEvent);
    _eventReceived += Pt::slot(*this, &Application::onProcessEnterEvent);
    
    loop().eventReceived() += Pt::slot(*this, &Application::onDispatchLeaveEvent);
    _eventReceived += Pt::slot(*this, &Application::onProcessLeaveEvent);
    
    loop().eventReceived() += Pt::slot(*this, &Application::onDispatchKeyEvent);
    _eventReceived += Pt::slot(*this, &Application::onProcessKeyEvent);
    
    loop().eventReceived() += Pt::slot(*this, &Application::onDispatchInvalidateEvent );
    _eventReceived += Pt::slot(*this, &Application::onProcessInvalidateEvent);

    loop().eventReceived() += Pt::slot(*this, &Application::onDispatchLayoutEvent);
    _eventReceived += Pt::slot(*this, &Application::onProcessLayoutEvent);

    loop().eventReceived() += Pt::slot(*this, &Application::onDispatchRescaleEvent);
    _eventReceived += Pt::slot(*this, &Application::onProcessRescaleEvent);

    loop().eventReceived() += Pt::slot(*this, &Application::onDispatchPaintEvent);
    _eventReceived += Pt::slot(*this, &Application::onProcessPaintEvent);

    loop().eventReceived() += Pt::slot(*this, &Application::onDispatchResizeEvent);
    _eventReceived += Pt::slot(*this, &Application::onProcessResizeEvent);

    loop().eventReceived() += Pt::slot(*this, &Application::onDispatchMoveEvent);
    _eventReceived += Pt::slot(*this, &Application::onProcessMoveEvent);

    loop().eventReceived() += Pt::slot(*this, &Application::onDispatchActivateEvent );
    _eventReceived += Pt::slot(*this, &Application::onProcessActivateEvent);

    loop().eventReceived() += Pt::slot(*this, &Application::onDispatchEnableEvent );
    _eventReceived += Pt::slot(*this, &Application::onProcessEnableEvent );

    loop().eventReceived() += Pt::slot(*this, &Application::onDispatchShowEvent );
    _eventReceived += Pt::slot(*this, &Application::onProcessShowEvent );

    loop().eventReceived() += Pt::slot(*this, &Application::onDispatchCloseEvent );
    _eventReceived += Pt::slot(*this, &Application::onProcessCloseEvent );

    loop().eventReceived() += Pt::slot(*this, &Application::onDispatchFocusEvent );
    _eventReceived += Pt::slot(*this, &Application::onProcessFocusEvent );

    loop().eventReceived() += Pt::slot(*this, &Application::onDispatchWindowStateEvent);
    _eventReceived += Pt::slot(*this, &Application::onProcessWindowStateEvent);

    _defaultInputMethod = new DefaultInputMethod;
    setInputMethod(*_defaultInputMethod);
}


Application::~Application()
{
    if(_inputMethod)
        removeInputMethod(*_inputMethod);

    delete _defaultInputMethod;

    delete _mainScreen;
    delete _impl;
}


ApplicationImpl* Application::impl()
{
    return _impl;
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


Pt::Timespan Application::inactivityTime() const
{
    return _impl->inactivityTime();
}


void Application::setCursor(const Cursor* cursor)
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


void Application::setScaleFactor(double scale)
{
    _scaling = scale;

    RescaleEvent ev(*_mainScreen, _scaling);
    _mainScreen->processEvent(ev);
}


double Application::scaleFactor() const
{
    return _scaling;
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
        _inputMethod = _defaultInputMethod;
        im.unregisterApplication(*this);
    }
}


Pt::uint64_t Application::makeId()
{
    return _lastId++;
}


Visual* Application::findVisual(Pt::uint64_t id)
{
    VisualMap::iterator it =_visuals.find(id);
    return it != _visuals.end() ? it->second : 0; 
}


void Application::registerVisual(Visual& visual)
{
    VisualMap::const_iterator it = _visuals.find( visual.vid() );
    assert( it == _visuals.end() );

    VisualMap::value_type elem(visual.vid(), &visual);
    _visuals.insert(elem);
}


void Application::unregisterVisual(Visual& visual)
{
   _visuals.erase( visual.vid() );
}


void Application::nextEvent()
{
    _impl->nextEvent();
}


void Application::commitEvent(const Event& ev)
{
    loop().commitEvent(ev);
}


void Application::processEvent(const Event& ev)
{
    _eventReceived.send(ev);
}


Pt::Signal<const Pt::Event&>&  Application::eventReceived()
{
    return _eventReceived;
}


void Application::invalidate()
{
    VisualMap::iterator it = _visuals.begin();
  
    for( ; it != _visuals.end(); ++it)
    {
        it->second->invalidate();
    }
}


void Application::sendKeyEvent(const KeyEvent& ev)
{
    _impl->sendKeyEvent(ev);
}


void Application::sendMouseEvent(const MouseEvent& ev)
{
    _impl->sendMouseEvent(ev);
}


void Application::onSetPointer(Visual& v, bool isPointer)
{
    _mainScreen->setPointer(v, isPointer);
}


Visual* Application::capture() const
{
    if( ! _capture.empty() )
        return _capture.back();

    if( ! _popups.empty() )
        return _popups.back();

    return 0;
}


void Application::onRequestCapture(Visual& target, bool isCapture)
{
    std::list<Visual*>::iterator it = std::find(_capture.begin(), 
                                                _capture.end(), &target);
    
    if( it != _capture.end() )
        _capture.erase(it);

    if(isCapture) 
        _capture.push_back(&target);

    //if(isCapture)
    //    std::clog << "SET CAPTURE " << typeid(target).name() << std::endl;
    //else
    //    std::clog << "RELEASE CAPTURE " << typeid(target).name() << std::endl;

    Visual* capture = this->capture();
    _mainScreen->impl()->setCapture(capture);
}


void Application::onShowPopup(Popup& w, bool transient)
{
    std::list<Popup*>::iterator it = std::find(_popups.begin(), 
                                               _popups.end(), &w);
    if( it != _popups.end() )
        _popups.erase(it);

    if(transient)
        _popups.push_back(&w);

    // if(transient)
    //     std::clog << "SET POPUP " << typeid(w).name() << std::endl;
    // else
    //     std::clog << "RELEASE POPUP " << typeid(w).name() << std::endl;

    Visual* capture = this->capture();
    _mainScreen->impl()->setCapture(capture);
}


bool Application::isAnchoredTo(Popup& p, Window& top) const
{
  Visual* anchor = p.anchor();
  if( ! anchor )
      return false;

  bool isAnchored = anchor == &top || anchor->isDescendantOf(top);
  return isAnchored;
}


bool Application::isPopupOf(Popup& p, Window& top) const
{
    if( isAnchoredTo(p, top) )
        return true;

    std::list<Popup*>::const_iterator it = _popups.begin();
    for(it = _popups.begin(); it != _popups.end(); ++it )
    {
        Popup* popup = *it;
        if( isAnchoredTo(p, *popup) )
            return isPopupOf(*popup, top);
    }

    return false;
}


void Application::onClosePopups(const Gfx::PointF& screenPos)
{
    Popup* popupHit = 0;

    Visual* hit = _mainScreen->hitTest(screenPos);
    if(hit)
    {
        std::list<Popup*>::iterator pit = _popups.begin();
        for(pit = _popups.begin(); pit != _popups.end(); ++pit )
        {
            Popup* popup = *pit;
            
            // popup or its content was hit
            if(popup == hit || popup->isAncestorOf(*hit) )
                popupHit = popup;

            // popup anchor or its content was hit
            Visual* anchor = popup->anchor();
            if(anchor)
            {
                if(anchor == hit || anchor->isAncestorOf(*hit) )
                    popupHit = popup;
            }
        }
    }

    // IME window or one of its popups was hit
    bool imeHit = false;
    Window* ime = inputMethod().activeWindow();
    if(ime)
    {
        if(hit)
            imeHit = ime == hit || ime->isAncestorOf(*hit);

        if( ! imeHit )
            imeHit = popupHit && isPopupOf(*popupHit, *ime);
    }

    std::list<Popup*> closePopups;

    std::list<Popup*>::iterator pit = _popups.begin();
    while( pit != _popups.end() )
    {
        Popup* popup = *pit++;

        // keep all popups that are related to the hit
        bool keepOpen = popupHit ? popup == popupHit || 
                                   isPopupOf(*popup, *popupHit) || 
                                   isPopupOf(*popupHit, *popup)
                                 : false;
        
        // if IME is used keep all popups not related to IME window
        if( imeHit && ! isPopupOf(*popup, *ime) )
            keepOpen = true;

        if( ! keepOpen )
        {
            //std::clog << "AUTO_CLOSE: " << popup->name() << std::endl;
            //popup->close();
            closePopups.push_back(popup);
            
            //pit = _popups.begin();
        }
    }

    for( pit = closePopups.begin(); pit != closePopups.end(); ++pit)
    {
        (*pit)->close();
    }
}


void Application::onDispatchMouseEvent(const MouseEvent& ev)
{
    // make sure receiver still exists
    VisualMap::iterator it = _visuals.find( ev.vid() );
    if( it == _visuals.end() )
        return;

    onProcessMouseEvent(ev);
}


void Application::onProcessMouseEvent(const MouseEvent& ev)
{
    //std::clog << "APP MOUSE: " << ev.position().x() << ", " << ev.position().y() << std::endl;

    Visual* visual = ev.visual();
    Gfx::PointF screenPos = ev.position();

    //
    // Detect scroll gestures
    //
    onDetectScroll( visual, screenPos, ev.isPress(), ev.isPressed() );

    //
    // close popups 
    //
    if( ev.isPress(MouseEvent::Left) || ev.isPress(MouseEvent::Right) )
    {
        onClosePopups( ev.position() );
    }

    Visual* capture = this->capture();

    //
    // IME window
    //
    Window* ime = inputMethod().activeWindow();
    if(ime)
    {
        Visual* hit = _mainScreen->hitTest(screenPos);
        if(hit)
        {
            if( ime == hit || ime->isAncestorOf(*hit) )
            {
                // TODO: also if capture is in a popup that is related to the IME
                if( capture && capture->isDescendantOf(*ime) )
                {
                    capture->processEvent(ev);
                }
                else
                {
                    MouseEvent mev = ev;
                    mev.setVisual(ime);
                    _mainScreen->processEvent(mev);
                }

                return;
            }
        }
    }

    //
    // hide IME window
    //
    if( ev.isPress(MouseEvent::Left) )
    {
        Visual* hit = _mainScreen->hitTest(screenPos);
        Visual* receiver = inputMethod().receiver();
        if(hit && receiver)
        {
            bool keepOpen = receiver == hit ||
                            hit->isDescendantOf(*receiver);
            if( ! keepOpen )
                inputMethod().finish();
        }
    }

    //
    // dispatch event
    //
    if(capture)
    {
        capture->processEvent(ev);
    }
    else
    {
        _mainScreen->processEvent(ev);
    }
}


void Application::onDispatchTouchEvent(const TouchEvent& ev)
{
    // make sure receiver still exists
    VisualMap::iterator it = _visuals.find( ev.vid() );
    if( it == _visuals.end() )
        return;

    onProcessTouchEvent(ev);
}


void Application::onProcessTouchEvent(const TouchEvent& ev)
{
    //std::clog << "APP TOUCH: " << ev.position().x() << ", " << ev.position().y() << std::endl;

    Visual* visual = ev.visual();
    Gfx::PointF screenPos = ev.position();

    //
    // Detect scroll gestures
    //
    onDetectScroll( visual, screenPos, ev.isPress(), ev.isPressed() );

    //
    // close popups 
    //
    if( ev.isPress() )
    {
        onClosePopups( ev.position() );
    }

    Visual* capture = this->capture();

    //
    // IME window
    //
    Window* ime = inputMethod().activeWindow();
    if(ime)
    {
        Visual* hit = _mainScreen->hitTest(screenPos);
        if(hit)
        {
            if( ime == hit || ime->isAncestorOf(*hit) )
            {
                // TODO: also if capture is in a popup that is related to the IME
                if( capture && capture->isDescendantOf(*ime) )
                {
                    capture->processEvent(ev);
                }
                else
                {
                    TouchEvent tev = ev;
                    tev.setVisual(ime);
                    _mainScreen->processEvent(tev);
                }

                return;
            }
        }
    }

    //
    // hide IME window
    //
    if( ev.isPress() )
    {
        Visual* hit = _mainScreen->hitTest(screenPos);
        Visual* receiver = inputMethod().receiver();
        if(hit && receiver)
        {
            bool keepOpen = receiver == hit ||
                            hit->isDescendantOf(*receiver);
            if( ! keepOpen )
                inputMethod().finish();
        }
    }

    //
    // dispatch event
    //
    if(capture)
    {
        capture->processEvent(ev);
    }
    else
    {
        _mainScreen->processEvent(ev);
    }
}


void Application::onDetectScroll(Visual* visual, const Gfx::PointF& screenPos,
                                 bool isPress, bool isPressed)
{
    const double threshold = 8;
    
    // TODO: start scroll only if within visual

    if(isPress)
    {
        //std::clog << "SCROLL START" << std::endl;
        _scrollFrom = screenPos;
    }
    else if(isPressed)
    {
        double deltaY = screenPos.y() - _scrollFrom.y();
        
        if( ! _onScroll && std::fabs(deltaY) > threshold )
        {
            _onScroll = true;

            if(deltaY > 0)
                _scrollFrom.addY(threshold);
            else
                _scrollFrom.subY(threshold);

            deltaY = screenPos.y() - _scrollFrom.y();
            //std::clog << "SCROLL STARTED: " << deltaY << std::endl;
            
            ScrollEvent sev(*visual);
            sev.set(ScrollEvent::Vertical, deltaY);
            processEvent(sev);

            _scrollFrom = screenPos;
        }
        else if(_onScroll)
        {
            //std::clog << "SCROLLING: " << deltaY << std::endl;
            ScrollEvent sev(*visual);
            sev.set(ScrollEvent::Vertical, deltaY);
            processEvent(sev);

            _scrollFrom = screenPos;
        }
    }
    else
    {
        if(_onScroll)
        {
            //std::clog << "SCROLL STOP" << std::endl;
            _onScroll = false;
        }
    }
}


void Application::onDispatchScrollEvent(const ScrollEvent& ev)
{
    // make sure receiver still exists
    VisualMap::iterator it = _visuals.find( ev.vid() );
    if( it == _visuals.end() )
        return;

    onProcessScrollEvent(ev);
}


void Application::onProcessScrollEvent(const ScrollEvent& ev)
{
    _mainScreen->processEvent(ev);
}


void Application::onDispatchEnterEvent(const EnterEvent& ev)
{
    // make sure receiver still exists
    VisualMap::iterator it = _visuals.find( ev.vid() );
    if( it == _visuals.end() )
        return;

    onProcessEnterEvent(ev);
}


void Application::onProcessEnterEvent(const EnterEvent& ev)
{
    Visual* visual = ev.visual();
    visual->processEvent(ev);
}


void Application::onDispatchLeaveEvent(const LeaveEvent& ev)
{
    // make sure receiver still exists
    VisualMap::iterator it = _visuals.find( ev.vid() );
    if( it == _visuals.end() )
        return;

    onProcessLeaveEvent(ev);
}


void Application::onProcessLeaveEvent(const LeaveEvent& ev)
{
    Visual* visual = ev.visual();
    visual->processEvent(ev);
}


void Application::onDispatchKeyEvent(const KeyEvent& ev)
{
    // make sure receiver still exists
    VisualMap::iterator it = _visuals.find( ev.vid() );
    if( it == _visuals.end() )
        return;

    onProcessKeyEvent(ev);
}


void Application::onProcessKeyEvent(const KeyEvent& ev)
{
    Visual* visual = ev.visual();

    if(visual)
        visual->processEvent(ev);
    else
        _mainScreen->processEvent(ev);
}


void Application::onDispatchInvalidateEvent(const InvalidateEvent& ev)
{
    // make sure receiver still exists
    VisualMap::iterator it = _visuals.find( ev.vid() );
    if( it == _visuals.end() )
        return;

    onProcessInvalidateEvent(ev);
}


void Application::onProcessInvalidateEvent(const InvalidateEvent& ev)
{
    Visual* visual = ev.visual();
    if(visual)
        visual->processEvent(ev);
}


void Application::onDispatchLayoutEvent(const LayoutEvent& ev)
{
    // make sure receiver still exists
    VisualMap::iterator it = _visuals.find( ev.vid() );
    if( it == _visuals.end() )
        return;

    onProcessLayoutEvent(ev);
}


void Application::onProcessLayoutEvent(const LayoutEvent& ev)
{
    Visual* visual = ev.visual();
    if(visual)
        visual->processEvent(ev);
}


void Application::onDispatchRescaleEvent(const RescaleEvent& ev)
{
    // make sure receiver still exists
    VisualMap::iterator it = _visuals.find( ev.vid() );
    if( it == _visuals.end() )
        return;

    onProcessRescaleEvent(ev);
}


void Application::onProcessRescaleEvent(const RescaleEvent& ev)
{
    Visual* visual = ev.visual();
    if(visual)
        visual->processEvent(ev);
}


void Application::onDispatchPaintEvent(const PaintEvent& ev)
{
    // make sure receiver still exists
    VisualMap::iterator it = _visuals.find( ev.vid() );
    if( it == _visuals.end() )
        return;

    onProcessPaintEvent(ev);
}


void Application::onProcessPaintEvent(const PaintEvent& ev)
{
    Visual* visual = ev.visual();
    if(visual)
        visual->processEvent(ev);
}


void Application::onDispatchMoveEvent(const MoveEvent& ev)
{
    // make sure receiver still exists
    VisualMap::iterator it = _visuals.find( ev.vid() );
    if( it == _visuals.end() )
        return;

    processEvent(ev);
}


void Application::onProcessMoveEvent(const MoveEvent& ev)
{
    Visual* visual = ev.visual();
    if(visual)
        visual->processEvent(ev);
}


void Application::onDispatchResizeEvent(const ResizeEvent& ev)
{
    // make sure receiver still exists
    VisualMap::iterator it = _visuals.find( ev.vid() );
    if( it == _visuals.end() )
        return;

    processEvent(ev);
}


void Application::onProcessResizeEvent(const ResizeEvent& ev)
{
    Visual* visual = ev.visual();
    if(visual)
        visual->processEvent(ev);
}


void Application::onDispatchActivateEvent(const ActivateEvent& ev)
{
    // make sure receiver still exists
    VisualMap::iterator it = _visuals.find( ev.vid() );
    if( it == _visuals.end() )
        return;

    processEvent(ev);
}


void Application::onProcessActivateEvent(const ActivateEvent& ev)
{
    Visual* visual = ev.visual();
    if(visual)
        visual->processEvent(ev);
}


void Application::onDispatchEnableEvent(const EnableEvent& ev)
{
    // make sure receiver still exists
    VisualMap::iterator it = _visuals.find( ev.vid() );
    if( it == _visuals.end() )
        return;

    onProcessEnableEvent(ev);
}


void Application::onProcessEnableEvent(const EnableEvent& ev)
{
    Visual* visual = ev.visual();
    if(visual)
        visual->processEvent(ev);
}


void Application::onDispatchShowEvent(const ShowEvent& ev)
{
    // make sure receiver still exists
    VisualMap::iterator it = _visuals.find( ev.vid() );
    if( it == _visuals.end() )
        return;

    onProcessShowEvent(ev);
}


void Application::onProcessShowEvent(const ShowEvent& ev)
{
    Visual* visual = ev.visual();
    if(visual)
        visual->processEvent(ev);
}


void Application::onDispatchCloseEvent(const CloseEvent& ev )
{
    // make sure receiver still exists
    VisualMap::iterator it = _visuals.find( ev.vid() );
    if( it == _visuals.end() )
        return;

    onProcessCloseEvent(ev);
}


void Application::onProcessCloseEvent(const CloseEvent& ev)
{
    Visual* visual = ev.visual();
    if(visual)
        visual->processEvent(ev);
}


void Application::onDispatchFocusEvent(const FocusEvent& ev)
{
    // make sure receiver still exists
    VisualMap::iterator it = _visuals.find( ev.vid() );
    if( it == _visuals.end() )
        return;

    onProcessFocusEvent(ev);
}


void Application::onProcessFocusEvent(const FocusEvent& ev)
{
    Visual* visual = ev.visual();
    if(visual)
        visual->processEvent(ev);
}


void Application::onDispatchWindowStateEvent(const WindowStateEvent& ev )
{
    // make sure receiver still exists
    VisualMap::iterator it = _visuals.find( ev.vid() );
    if( it == _visuals.end() )
        return;

    onProcessWindowStateEvent(ev);
}


void Application::onProcessWindowStateEvent(const WindowStateEvent& ev )
{
    Visual* visual = ev.visual();
    if(visual)
        visual->processEvent(ev);
}

} // namespace

} // namespace
