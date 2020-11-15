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

#include <Pt/Hmi/WindowManager.h>
#include <Pt/Gfx/Painter.h>
#include <Pt/Gfx/PaintSurface.h>
#include <Pt/Hmi/Window.h>
#include <Pt/Hmi/MouseEvent.h>
#include <Pt/Hmi/WindowStateEvent.h>
#include <Pt/Hmi/KeyEvent.h>
#include <Pt/Hmi/Window.h>
#include <Pt/Hmi/Application.h>
#include <Pt/Gfx/Pen.h>
#include <Pt/Gfx/Brush.h>
#include <Pt/Gfx/Point.h>
#include <Pt/String.h>
#include <cmath>
#include "WindowFrame.h"

namespace Pt {

namespace Hmi {

WindowManager::WindowManager()
: _parent(0)
, _activeWindow(0)
, _currentWindow(0)
, _grabbedWindow(0)
, _topMostWindow(0)
, _borderWidth(4.0)
, _titleHeight(20.0)
, _inactiveColor(65535 * 0.68f, 65535 * 0.70f, 65535 * 0.75f)
, _activeColor(65535* 0.4f, 65535 *0.5f, 65535 *0.8f)
, _textColor(65535, 65535, 65535)
, _inactiveTextColor(65535 * 0.2f, 65535 * 0.2f, 65535 * 0.2f)
{
}


WindowManager::~WindowManager()
{
    while( ! _windows.empty() )
    {
        delete _windows.front();
        _windows.erase( _windows.begin() );
    }
}


void WindowManager::init(WindowBase& parent)
{
    _parent = &parent;
}


void WindowManager::add(Window& w)
{    
    WindowFrame* frame = new WindowFrame(*this, w);

    if(_topMostWindow)
        _windows.insert( --_windows.end(), frame );
    else
        _windows.push_back(frame);

    onFrameChanged(w);
}


void WindowManager::remove(Window& w)
{
    // TODO: not when window programmatically removed
    Application::instance().inputMethod().finish();

    std::vector<WindowFrame*>::iterator wit;
    for(wit = _windows.begin(); wit != _windows.end(); ++wit)
    {
        if((*wit)->window() == &w)
        {
            if(_currentWindow && _currentWindow->window() == &w)
                _currentWindow = 0;

            if(_grabbedWindow && _grabbedWindow->window() == &w)
                _grabbedWindow = 0;

            if(_activeWindow && _activeWindow->window() == &w)
                _activeWindow  = 0;

            if(_topMostWindow && _topMostWindow->window() == &w)
                _topMostWindow = 0;

            delete *wit;
            _windows.erase(wit);
            break;
        }
    }
}


void WindowManager::setScreen(Screen* screen)
{
    std::vector<WindowFrame*>::iterator wit;
    for(wit = _windows.begin(); wit != _windows.end(); ++wit)
    {
        WindowFrame* frame = *wit;
        frame->setFrame(_borderWidth, _titleHeight);
    }
}


WindowFrame* WindowManager::findWindow(const Gfx::PointF& p)
{
    std::vector<WindowFrame*>::reverse_iterator rit;
    for(rit =  _windows.rbegin() ; rit != _windows.rend(); ++rit )
    {
        if( ! (*rit)->window()->isVisible() )
            continue;

        if( ! (*rit)->frameRect().contains(p) )
            continue;

        return *rit;
    }

    return 0;
}


WindowFrame* WindowManager::findWindow(const Window& w) const
{
    std::vector<WindowFrame*>::const_iterator it;
    for(it = _windows.begin(); it != _windows.end(); ++it)
    {
        if( (*it)->window() == &w)
            return *it;
    }

    return 0;
}


void WindowManager::enterEvent(const EnterEvent& ev)
{
}


void WindowManager::leaveEvent(const LeaveEvent& ev)
{
    _grabbedWindow = 0;

    if(_currentWindow)
    {
        WindowFrame* w = _currentWindow;
        _currentWindow = 0;
      
        LeaveEvent lev( w->window()->vid() );
        w->leaveEvent(lev);
    }
}


bool WindowManager::keyEvent(const KeyEvent& keyEvent)
{
    Window* w = 0;

    if( _activeWindow)
        w = _activeWindow->window();

    if( ! w )
        return false;

    if( w->isEnabled() )
    {
        KeyEvent kev = keyEvent;
        kev.setId( w->vid() );
        
        //w->processEvent(keyEvent);
        Application::instance().loop().commitEvent(kev);
    }

    return true;
}


bool WindowManager::mouseEvent( const MouseEvent& mev )
{
    WindowFrame* windowFrame = 0;

    if(_grabbedWindow)
        windowFrame = _grabbedWindow;
    else
        windowFrame = findWindow( mev.position() );

    //
    // window activation
    //
    if( mev.isPress() )
    {
        if( ! windowFrame && _activeWindow )
        {
            onActivate(0);
        }
        
        if( windowFrame && ! windowFrame->window()->isActive())
        {
            windowFrame->window()->activate();
        }
    }

    //
    // window enter/leave
    //
    if(_currentWindow != windowFrame)
    {
      if(_currentWindow)
      {
          LeaveEvent lev( _currentWindow->window()->vid() );
          _currentWindow->leaveEvent(lev);
      }
      if(windowFrame)
      {
          EnterEvent eev( windowFrame->window()->vid() );
          windowFrame->enterEvent(eev);
      }
    }

    //
    // forward mouse event
    //
    if(windowFrame)
    {         
        if( windowFrame->mouseEvent(mev) )
        {
            //Application::instance().inputMethod().finish();
            _grabbedWindow = windowFrame;
        }
        else
            _grabbedWindow = 0;
    }

    _currentWindow = windowFrame;

    return windowFrame != 0;
}


bool WindowManager::touchEvent( const TouchEvent& tev )
{
    WindowFrame* windowFrame = 0;

    if(_grabbedWindow)
        windowFrame = _grabbedWindow;
    else
        windowFrame = findWindow( tev.position() );

    //
    // window activation
    //
    if( tev.isPress() )
    {
        if( ! windowFrame && _activeWindow )
        {
            onActivate(0);
        }
        
        if( windowFrame && ! windowFrame->window()->isActive())
        {
            windowFrame->window()->activate();
        }
    }

    //
    // window enter/leave
    //
    if(_currentWindow != windowFrame)
    {
      if(_currentWindow)
      {
          LeaveEvent lev( _currentWindow->window()->vid() );
          _currentWindow->leaveEvent(lev);
      }
      if(windowFrame)
      {
          EnterEvent eev( windowFrame->window()->vid() );
          windowFrame->enterEvent(eev);
      }
    }

    //
    // forward touch event
    //
    if(windowFrame)
    {         
        if( windowFrame->touchEvent(tev) )
        {
            //Application::instance().inputMethod().finish();
            _grabbedWindow = windowFrame;
        }
        else
            _grabbedWindow = 0;
    }

    _currentWindow = windowFrame;

    return windowFrame != 0;
}


bool WindowManager::scrollEvent(const ScrollEvent& sev)
{
    Window* w = 0;

    if( _activeWindow)
        w = _activeWindow->window();

    if( ! w )
        return false;

    if( w->isEnabled() )
        w->processEvent(sev);

    return true;
}


void WindowManager::paint(Gfx::PaintSurface& surface, const Gfx::RectF& rect)
{
    std::vector<WindowFrame*>::iterator it;
    for(it = _windows.begin(); it != _windows.end(); ++it )
    {
        WindowFrame* frame = *it;
        
        Window* w = frame->window();
        if( ! w || ! w->isVisible() )
            continue; 

        // clip window frame rect
        Gfx::RectF frameRect = frame->frameRect().intersect(rect);
        if (frameRect.isNull())
            continue;

        // clip client rect
        Gfx::RectF updateRect = frame->clientRect().intersect(rect);

        // convert to logical units

        // paint frame rect
        frame->paint(surface, frameRect);

        // paint client rect
        Gfx::PointF surfacePos = w->fromParent( updateRect.topLeft() );
        Gfx::RectF surfaceRect( surfacePos, updateRect.size() );

        Pt::Gfx::Painter painter(surface);
        painter.drawSurface(updateRect.topLeft(), w->surface(), surfaceRect);
    }
}


void WindowManager::onUpdate(Window& w, const Gfx::RectF& rect)
{
    Gfx::PointF updatePos = toParent( w, rect.topLeft() );

    Gfx::RectF updateRect(updatePos, rect.size());

    if( ! _parent )
        throw std::logic_error("WindowManager not initialized");
           
    updateRect = updateRect.intersect( Gfx::RectF(Gfx::PointF(0,0), _parent->size()));

    _parent->update(updateRect);
}


void WindowManager::onResize(Window& w, const Gfx::SizeF& sz)
{   
    Gfx::SizeF to = sz;

    if( to.width() > w.maximumSize().width() )
        to.setWidth( w.maximumSize().width() );

    if( to.height() > w.maximumSize().height() )
        to.setHeight( w.maximumSize().height() );

    if( to.width() < w.minimumSize().width() )
        to.setWidth( w.minimumSize().width() );

    if( to.height() < w.minimumSize().height() )
        to.setHeight( w.minimumSize().height() );

    WindowFrame* frame = findWindow(w);
    if( ! frame )
        return;

    ResizeEvent rev(w.vid(), to);
    Application::instance().loop().commitEvent(rev);
    
    // TODO: move updating to frame

    Gfx::RectF updateRect = frame->frameRect();
    
    frame->resizeEvent(rev);
    
    updateRect.unify( frame->frameRect() );

    if( ! w.isVisible() )
        return;

    Gfx::PointF updatePos = fromParent( w, updateRect.topLeft() );
    updateRect.setOrigin(updatePos);       

    w.update(updateRect);
}


void WindowManager::onMove(Window& w, const Gfx::PointF& to)
{   
    WindowFrame* frame = findWindow(w);
    if( ! frame )
        return;

    MoveEvent mev(w.vid(), to);
    Application::instance().loop().commitEvent(mev);
    
    // TODO: move updating to frame

    Gfx::RectF updateRect = frame->frameRect();

    frame->moveEvent(mev);

    updateRect.unify( frame->frameRect() );

    if( ! _parent )
        throw std::logic_error("WindowManager not initialized");

    updateRect = updateRect.intersect( Gfx::RectF(Gfx::PointF(0,0), _parent->size()));

    _parent->update(updateRect);
}


void WindowManager::onShow(Window& w, bool visible)
{
    WindowFrame* frame = findWindow(w);
    if( ! frame )
        return;

    ShowEvent sev( w.vid(), visible );
    Application::instance().loop().commitEvent(sev);

    // TODO: move updating to frame

    Gfx::RectF updateRect = frame->frameRect();

    if( ! _parent )
        throw std::logic_error("WindowManager not initialized");

    Gfx::PointF updatePos = fromParent( w, updateRect.topLeft() );
    updateRect.setOrigin(updatePos);

    w.invalidate();
    w.update(updateRect);
}


void WindowManager::onActivate(Window* w)
{
    if(_activeWindow && _activeWindow->window() != w)
    {
        ActivateEvent aev(_activeWindow->window()->vid(), false);
        Application::instance().loop().commitEvent(aev);
        _activeWindow->update();
        _activeWindow = 0;
    }

    if( ! w )
        return;

    WindowFrame* frame = findWindow(*w);
    if( ! frame )
        return;

    // raise active frame
    std::vector<WindowFrame*>::iterator it =
        std::find(_windows.begin(), _windows.end(), frame);

    _windows.erase(it);

    if(_topMostWindow && frame != _topMostWindow)
    {
        _windows.insert(--_windows.end(), frame);
    }
    else
    {
        _windows.push_back(frame);
    }
    
    ActivateEvent aev(w->vid(), true);
    Application::instance().loop().commitEvent(aev);

    _activeWindow = frame;
    _activeWindow->update();
}


void WindowManager::onEnable(Window& w, bool enable)
{
    WindowFrame* frame = findWindow(w);
    if( ! frame )
        return;

    EnableEvent eev( w.vid(), enable );
    Application::instance().loop().commitEvent( eev );

    // TODO: move updating to frame
    
    Gfx::RectF updateRect = frame->frameRect();

    if( ! _parent )
        throw std::logic_error("WindowManager not initialized");

    Gfx::PointF updatePos = fromParent( w, updateRect.topLeft() );
    updateRect.setOrigin(updatePos);

    w.invalidate();
    w.update(updateRect);
}


void WindowManager::onFrameChanged(Window& w)
{
    WindowFrame* frame = findWindow(w);
    if( ! frame )
        return;

    switch( w.type() )
    {
        case Window::Popup:
            frame->setFrame(0, 0);
            break;
        
        default:
        case Window::Normal:
            frame->setFrame(_borderWidth, _titleHeight);
            break;
    }

    // TODO: move updating to frame

    Gfx::RectF updateRect = frame->frameRect();

    if( ! _parent )
        throw std::logic_error("WindowManager not initialized");

    updateRect = updateRect.intersect( Gfx::RectF(Gfx::PointF(0,0), _parent->size()));

    _parent->update(updateRect);
}


void WindowManager::onStateChanged(Window& w)
{
    WindowFrame* frame = findWindow(w);
    if( ! frame )
        return;

    Window::State state = w.state();
    Window::State oldState = frame->state();
    
    if(state != oldState)
    {
        frame->setState(state);

        if(oldState == Window::Normal)
            frame->setRestore(w.position(), w.size());

        if(state == Window::Maximized)
        {
        
            Gfx::SizeF maxSize = _parent->size();
            maxSize = frame->fromFrame(maxSize);

            w.move( Gfx::PointF(0,0) );
            w.resize(maxSize);
        }
        else if(state == Window::Minimized)
        {
            if(oldState == Window::Normal)
            {
                Gfx::SizeF minSize(w.size().width(), 0);
                w.resize(minSize);
            }
            else
            {
                w.move( frame->restorePosition() );

                Gfx::SizeF minSize(frame->restoreSize().width(), 0);
                w.resize(minSize);
            }
        }
        else if(state == Window::Normal)
        {
            w.move( frame->restorePosition() );
            w.resize( frame->restoreSize() );
        }

        WindowStateEvent wse(w.vid(), state);
        Application::instance().loop().commitEvent(wse);
    }

    if( w.isTopMost() )
    {
        if(_topMostWindow && _topMostWindow != frame)
            _topMostWindow->window()->setTopMost(false);

        // move top most frame to the back
        std::vector<WindowFrame*>::iterator it =
            std::find(_windows.begin(), _windows.end(), frame);

        _windows.erase(it);
        _windows.push_back(frame);

        _topMostWindow = frame;
    }
    else if(_topMostWindow == frame)
    {
        _topMostWindow = 0;
    }
}


void WindowManager::onClosing(Window& w)
{
    CloseEvent ev( w.vid());
    Application::instance().loop().commitEvent( ev );
}


void WindowManager::onClose(Window& w)
{
    WindowFrame* frame = findWindow(w);
    if( ! frame )
        return;

    Gfx::RectF updateRect = frame->frameRect();

    remove(w);

    // TODO: move updating to frame

    if( ! _parent )
        throw std::logic_error("WindowManager not initialized");
   
    updateRect = updateRect.intersect( Gfx::RectF(Gfx::PointF(0,0), _parent->size()));

   _parent->update(updateRect);
}


Gfx::PointF WindowManager::toParent(const Window& w, const Gfx::PointF& pos) const
{    
    WindowFrame* frame = findWindow(w);
    if( ! frame )
        return pos;

    return w.position() + frame->toFrame(pos);
}


Gfx::PointF WindowManager::fromParent(const Window& w, const Gfx::PointF& pos) const
{
    WindowFrame* frame = findWindow(w);
    if( ! frame )
        return pos;

    Gfx::PointF p = frame->fromFrame(pos) - w.position();
    return p;
}

} // namespace

} // namespace
