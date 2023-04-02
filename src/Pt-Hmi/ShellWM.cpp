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
  02110-1301  USA
*/

#include "WindowFrame.h"

#include <Pt/Hmi/ShellWM.h>
#include <Pt/Hmi/Shell.h>
#include <Pt/Hmi/Window.h>
#include <Pt/Hmi/Application.h>
#include <Pt/Hmi/WindowStateEvent.h>
#include <Pt/Gfx/PaintSurface.h>

namespace Pt {

namespace Hmi {

ShellWM::ShellWM()
: _parent(0)
, _activeWindow(0)
, _grabbedFrame(0)
, _topMostWindow(0)
, _borderWidth(4.0)
, _titleHeight(20.0)
, _activeColor( Gfx::Color::fromRgb8(100, 128, 205) )
, _inactiveColor( Gfx::Color::fromRgb8(173, 178, 191) )
, _textColor( Gfx::Color::fromRgb8(255, 255, 255) )
, _inactiveTextColor( Gfx::Color::fromRgb8(50, 50, 50) )
{
}


ShellWM::~ShellWM()
{
    while( ! _windowList.empty() )
    {
        _windowList.front()->unparent();
    }
}


void ShellWM::setParent(Shell* shell)
{
    _parent = shell;

    onSetParent(_parent);
}


Gfx::PaintSurface& ShellWM::surface()
{
    return _surface;
}


const Gfx::PaintSurface& ShellWM::surface() const
{
    return _surface;
}


void ShellWM::setSurface(Gfx::PaintSurface* surface, const Gfx::PointF& pos)
{
    if( ! surface )
    {
        _surface.detach();
    }
    else
    {
        Gfx::RectF surfaceRect( pos, size() );
        _surface.attach(*surface, surfaceRect);
    }
}


void ShellWM::onProcessEvent(const Pt::Event& ev)
{
    WindowManager::onProcessEvent(ev);
}


Window* ShellWM::activeWindow()
{
    return _activeWindow;
}


void ShellWM::onRequestActivate(bool active)
{
    if(_parent)
        _parent->onActivate(*this, active);
}


const std::vector<Window*>& ShellWM::windows() const
{
    return _windowList;
}

///////////////////////////////////////////////////////////////////////
// Visual
///////////////////////////////////////////////////////////////////////

Gfx::PointF ShellWM::onToParent(const Gfx::PointF& pos) const
{
    if( ! _parent )
        return pos;

    return _parent->toParent(pos);
}


Gfx::PointF ShellWM::onFromParent(const Gfx::PointF& pos) const
{
    if( ! _parent )
        return pos;

    return _parent->fromParent(pos);
}


Visual* ShellWM::onHitTest(const Gfx::PointF& p)
{
    if( ! bounds().contains(p) )
        return 0;

    std::vector<Window*>::const_reverse_iterator rit;
    for(rit = _windowList.rbegin() ; rit != _windowList.rend(); ++rit )
    {
        Window* window = *rit;
        
        if( ! window->isVisible() )
            continue;

        Gfx::PointF pos = toWindow(*window, p);
        Visual* hit = window->hitTest(pos);
        if(hit)
            return hit;

        WindowFrame* frame = static_cast<WindowFrame*>( window->impl() );
        if( frame->frameRect().contains(p) )
            return 0;
    }

    return 0;
}


WindowFrame* ShellWM::onHitTestFrame(const Gfx::PointF& pos)
{
    WindowFrame* windowFrame = 0;

    std::vector<Window*>::const_reverse_iterator rit;
    for(rit = _windowList.rbegin() ; rit != _windowList.rend(); ++rit )
    {
        Window* window = *rit;
        WindowFrame* frame = static_cast<WindowFrame*>( window->impl() );

        if( frame->frameRect().contains(pos) && 
            window->acceptsInput() )
        {
            windowFrame = frame;
            break;
        }
    }

    return windowFrame;
}


void ShellWM::onRequestCapture(bool capture)
{
    Visual::onRequestCapture(capture);

    if( ! capture )
    {
        if(_grabbedFrame)
            _grabbedFrame = 0;
    }
}

///////////////////////////////////////////////////////////////////////
// WindowManager
///////////////////////////////////////////////////////////////////////

WindowImpl* ShellWM::onAttach(Window& w)
{
    WindowFrame* frame = new WindowFrame(*this, w);

    if(_topMostWindow)
        _windowList.insert( --_windowList.end(), &w );
    else
        _windowList.push_back(&w);

    w.setNextResponder(this);

    return frame;
}


void ShellWM::onDetach(Window& w)
{
    w.setNextResponder(0);

    std::vector<Window*>::iterator wit;
    for(wit = _windowList.begin(); wit != _windowList.end(); ++wit)
    {
        Window* window = *wit;

        if(window == &w)
        {
            if( _grabbedFrame && _grabbedFrame == w.impl() )
            {
                setCapture(false);
                _grabbedFrame = 0;
            }

            if(_activeWindow && _activeWindow == &w)
                _activeWindow  = 0;

            if(_topMostWindow && _topMostWindow == &w)
                _topMostWindow = 0;

            //delete *wit;
            _windowList.erase(wit);
            break;
        }
    }
}


void ShellWM::onInit(Window& w)
{
    double scaling = scaleFactor();
    
    RescaleEvent ev(w, scaling);
    w.processEvent(ev);
    //Application::instance().loop().commitEvent(ev);
}


void ShellWM::onRelease(Window& w)
{
    if( w.isVisible() )
    {
        const WindowFrame* frame = static_cast<const WindowFrame*>( w.impl() );
        if(frame)
            repaint( frame->frameRect() );
    }
}


Gfx::PointF ShellWM::onToWindow(const Window& w, 
                                const Gfx::PointF& pos) const
{
    const WindowFrame* frame = static_cast<const WindowFrame*>( w.impl() );
    if( ! frame )
        return pos;

    return frame->toWindow(pos) - w.position();
}


Gfx::PointF ShellWM::onFromWindow(const Window& w, 
                                  const Gfx::PointF& pos) const
{
    const WindowFrame* frame = static_cast<const WindowFrame*>( w.impl() );
    if( ! frame )
        return pos;

    return w.position() + frame->fromWindow(pos);
}


void ShellWM::onRepaint(Window& w, const Gfx::RectF& rect)
{
    Gfx::PointF windowPos = onFromWindow( w, rect.topLeft() );
    Gfx::RectF windowRect( windowPos, rect.size() );

    repaint(windowRect);
}


void ShellWM::onShow(Window& w, bool visible)
{
    ShowEvent windowEvent( w, visible );
    Application::instance().loop().commitEvent(windowEvent);
}


void ShellWM::onActivate(Window& w, bool active)
{
    if(active)
    {
        //
        // deactivate other active window
        //
        if(_activeWindow && _activeWindow != &w)
            _activeWindow->activate(false);

        //
        // raise to top of window stack
        //
        std::vector<Window*>::iterator it =
            std::find(_windowList.begin(), _windowList.end(), &w);

        _windowList.erase(it);

        if(_topMostWindow && _topMostWindow != &w)
        {
            _windowList.insert(--_windowList.end(), &w);
        }
        else
        {
            _windowList.push_back(&w);
        }

        _activeWindow = &w;
    }
    else
    {
        if(_activeWindow == &w)
            _activeWindow = 0;
    }

    ActivateEvent aev(w, active);
    Application::instance().loop().commitEvent(aev);
    
    activate(active);
}


void ShellWM::onEnableRequest(Window& w, bool enable)
{
    if( ! isEnabled() )
      enable = false;

    EnableEvent eev(w, enable);
    Application::instance().loop().commitEvent(eev);
}



void ShellWM::onSetAbove(Window& w, bool above)
{
    if(above)
    {
        if(_topMostWindow && _topMostWindow != &w)
            _topMostWindow->setAbove(false);

        // move top most to the back
        std::vector<Window*>::iterator it = std::find(_windowList.begin(), 
                                                      _windowList.end(), &w);
        if( it != _windowList.end() )
            _windowList.erase(it);
        
        _windowList.push_back(&w);

        _topMostWindow = &w;
    }
    else if(_topMostWindow == &w)
    {
        _topMostWindow = 0;
    }
}


void ShellWM::onSetTitle(Window& w, const std::string& text)
{
    w.impl()->setTitle(text);
}


void ShellWM::onSetIcon(Window& w, const Gfx::Image& icon)
{
    w.impl()->setIcon(icon);
}


void ShellWM::onSetState(Window& w, const WindowState& state)
{
    w.impl()->setState(state);
}


void ShellWM::onSetSizeLimits(Window& w, const Gfx::SizeF& minSize, 
                                         const Gfx::SizeF& maxSize)
{
}


void ShellWM::onClosing(Window& w)
{         
    CloseEvent ev(w);
    Application::instance().loop().commitEvent(ev);
}

///////////////////////////////////////////////////////////////////////
// Implementation
///////////////////////////////////////////////////////////////////////

void ShellWM::onProcessRescaleEvent(const RescaleEvent& ev)
{
    Base::onProcessRescaleEvent(ev);

    double scaling = ev.scaleFactor();

    std::vector<Window*>::iterator wit;
    for(wit = _windowList.begin(); wit != _windowList.end(); ++wit)
    {
        Window* window = *wit;

        RescaleEvent rev(*window, scaling);
        window->processEvent(rev);

        if(window->state() == WindowState::Maximized)
        {
            window->setState(WindowState::Maximized);
        }
    }
}


void ShellWM::onRequestRepaint(const Gfx::RectF& rect)
{
    if(_parent)
        _parent->onRepaint(*this, rect);
}


void ShellWM::onProcessPaintEvent(const PaintEvent& ev)
{
    const Gfx::RectF& rect = ev.rect();
    if( rect.isNull() )
        return;

    Visual::onProcessPaintEvent(ev);

    //std::clog << "SHELL: " << ev.rect().width() << "x" << ev.rect().height() << std::endl;

    //
    // paint child windows
    //
    std::vector<Window*>::iterator wit;
    for(wit = _windowList.begin(); wit != _windowList.end(); ++wit)
    {
        Window* window = *wit;

        if( ! window->isVisible() )
            continue;

        WindowImpl* frame = window->impl();

        Gfx::RectF frameRect( frame->position(), frame->size() );
        frameRect = rect.intersect(frameRect);

        Gfx::PointF winPos = onToWindow( *window, frameRect.topLeft() );
        Gfx::RectF winRect( winPos, rect.size() );

        PaintEvent pev( *window, winRect );
        window->processEvent(pev);

        Gfx::PointF surfacePos = frameRect.topLeft() - frame->position();
        Gfx::RectF surfaceRect( surfacePos, frameRect.size() );

        Pt::Gfx::Painter painter( surface() );
        painter.drawSurface(frameRect.topLeft(), frame->surface(), surfaceRect);

        //std::clog << "BLIT: " << surfaceRect.width() << "x" 
        //                      << surfaceRect.height() << std::endl;
    }
}


void ShellWM::onProcessEnableEvent(const EnableEvent& ev)
{
    Base::onProcessEnableEvent(ev);

    std::vector<Window*>::iterator wit;
    for(wit = _windowList.begin(); wit != _windowList.end(); ++wit)
    {
        Window* window = *wit;

        EnableEvent eev( *window, ev.enabled() );
        Application::instance().loop().commitEvent(eev);
    }
}


void ShellWM::onMove(Window& w, const Gfx::PointF& pos)
{
    Gfx::PointF aligedPos = _surface.align(pos);

    MoveEvent mev(w, aligedPos);
    Application::instance().commitEvent(mev);
}


void ShellWM::onResize(WindowFrame& frame, const Gfx::SizeF& to)
{
    ResizeEvent rev(frame, to);
    Application::instance().commitEvent(rev);
}


void ShellWM::onProcessResizeEvent(const ResizeEvent& ev)
{
    Base::onProcessResizeEvent(ev);
}


void ShellWM::onResizeEvent(const ResizeEvent& ev)
{
    Base::onResizeEvent(ev);

    std::vector<Window*>::iterator wit;
    for(wit = _windowList.begin(); wit != _windowList.end(); ++wit)
    {
        Window* window = *wit;

        if(window->state() == WindowState::Maximized)
        {
            window->setState(WindowState::Maximized);
        }
    }
}


bool ShellWM::processMouseEvent(const MouseEvent& ev)
{
    //
    // continue press sequence capture
    // 
    if(_grabbedFrame)
    {
        _grabbedFrame->onProcessMouseEvent(ev);

        if( ev.isRelease() )
        {
            setCapture(false);
            _grabbedFrame = 0;
        }

        return true;
    }

    Gfx::PointF pos = fromGlobal( ev.position() );

    //
    // hit test
    //
    WindowFrame* windowFrame = onHitTestFrame(pos);
    
    //
    // window activation
    //
    if( ev.isPress() )
    {
        if( ! windowFrame && _activeWindow )
        {
            _activeWindow->activate(false);
        }
        
        if( windowFrame && ! windowFrame->window()->isActive() )
        {
            windowFrame->window()->activate();
        }
    }

    //
    // propagate event to window frame
    //
    if(windowFrame)
    {
        bool isClient = windowFrame->clientRect().contains(pos);
        if( ! isClient )
        {
            if( ev.isPress() )
            {
                _grabbedFrame = windowFrame;
                setCapture(true);

                // TODO: make WindowFrame a proper Visual to handle events
                //windowFrame->setPointer(true);
            }
        }

        windowFrame->onProcessMouseEvent(ev);
        return true;
    }

    return false;
}


bool ShellWM::processTouchEvent(const TouchEvent& ev)
{
    //
    // continue press sequence capture
    // 
    if(_grabbedFrame)
    {
        _grabbedFrame->onProcessTouchEvent(ev);

        if( ev.isRelease() )
        {
            setCapture(false);
            _grabbedFrame = 0;
        }

        return true;
    }

    Gfx::PointF pos = fromGlobal( ev.position() );

    //
    // hit test
    //
    WindowFrame* windowFrame = onHitTestFrame(pos);
    
    //
    // window activation
    //
    if( ev.isPress() )
    {
        if( ! windowFrame && _activeWindow )
        {
            _activeWindow->activate(false);
        }
        
        if( windowFrame && ! windowFrame->window()->isActive() )
        {
            windowFrame->window()->activate();
        }
    }

    //
    // propagate event to window frame
    //
    if(windowFrame)
    {
        bool isClient = windowFrame->clientRect().contains(pos);
        if( ! isClient )
        {
            if( ev.isPress() )
            {
                _grabbedFrame = windowFrame;
                setCapture(true);

                // TODO: make WindowFrame a proper Visual to handle events
                //windowFrame->setPointer(true);
            }
        }

        windowFrame->onProcessTouchEvent(ev);
        return true;
    }

    return false;
}


void ShellWM::onProcessMouseEvent(const MouseEvent& ev)
{
    processMouseEvent(ev);
}


void ShellWM::onProcessTouchEvent(const TouchEvent& ev)
{
    processTouchEvent(ev);
}


void ShellWM::onProcessEnterEvent(const EnterEvent& ev)
{
    Base::onProcessEnterEvent(ev);
}


void ShellWM::onProcessLeaveEvent(const LeaveEvent& ev)
{
    Base::onProcessLeaveEvent(ev);
}


void ShellWM::onProcessScrollEvent(const ScrollEvent& ev)
{
    //if( ! acceptsInput() )
    //    return;

    if( _activeWindow )
    {
        _activeWindow->processEvent(ev);
        return;
    }
}


void ShellWM::onProcessKeyEvent(const KeyEvent& ev)
{
    //if( ! acceptsInput() )
    //    return;


    if(_activeWindow)
    {
        KeyEvent kev = ev;
        kev.setVisual(_activeWindow);
        _activeWindow->processEvent(ev);
        return;
    }
}

} // namespace

} // namespace
