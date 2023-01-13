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

///////////////////////////////////////////////////////////////////////
// ShellWindowImpl
///////////////////////////////////////////////////////////////////////

class ShellWindowImpl : public WindowImpl
{
    public:
        ShellWindowImpl(WindowType type = WindowType::Default);

        virtual ~ShellWindowImpl();

        virtual double scaleFactor() const;
};


ShellWindowImpl::ShellWindowImpl(WindowType type)
: WindowImpl(type)
{
}


ShellWindowImpl::~ShellWindowImpl()
{
}


double ShellWindowImpl::scaleFactor() const
{
    return 1.0;
}

///////////////////////////////////////////////////////////////////////
// ShellWM
///////////////////////////////////////////////////////////////////////

ShellWM::ShellWM()
: _parent(0)
, _surface(0)
, _nextResponder(0)
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
    while( ! _windows.empty() )
    {
        _windows.front()->window()->unparent();
    }
}


void ShellWM::setParent(Shell* shell)
{
    _parent = shell;
}


void ShellWM::setSurface(Gfx::PaintSurface* surface)
{
    _surface = surface;
}


void ShellWM::setNextResponder(Responder* r)
{
    _nextResponder = r;
}


void ShellWM::onEvent(const Pt::Event& ev)
{
    WindowManager::onEvent(ev);

    _eventReceived.send(ev);
}


Responder* ShellWM::onNextResponder()
{
    return _nextResponder;
}


WindowFrame* ShellWM::activeWindow()
{
    return _activeWindow;
}


void ShellWM::activate(bool active)
{
    if(_parent)
        _parent->onActivate(*this, active);
}


WindowFrame* ShellWM::findWindowFrame(const Gfx::PointF& p) const
{
    if(_grabbedFrame)
        return _grabbedFrame;
  
    std::vector<WindowFrame*>::const_reverse_iterator rit;
    for(rit = _windows.rbegin() ; rit != _windows.rend(); ++rit )
    {
        if( ! (*rit)->window()->isVisible() )
            continue;

        if( ! (*rit)->frameRect().contains(p) )
            continue;

        return *rit;
    }

    return 0;
}


WindowFrame* ShellWM::getWindowFrame(const Window& w) const
{
    std::vector<WindowFrame*>::const_iterator it;
    for(it = _windows.begin(); it != _windows.end(); ++it)
    {
        if( (*it)->window() == &w)
            return *it;
    }

    return 0;
}

///////////////////////////////////////////////////////////////////////
// Visual
///////////////////////////////////////////////////////////////////////

Visual* ShellWM::onGetParent() const
{
    return _parent;
}


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
    std::vector<WindowFrame*>::const_reverse_iterator rit;
    for(rit = _windows.rbegin() ; rit != _windows.rend(); ++rit )
    {
        WindowFrame* frame = *rit;
        Window* w = frame->window();

        if( ! w->isVisible() )
            continue;

        Gfx::PointF pos = toWindow(*w, p);
        Visual* hit = w->hitTest(pos);
        if(hit)
            return hit;

        if( frame->frameRect().contains(p) )
            return 0;
    }

    return 0;
}


WindowFrame* ShellWM::onHitTestFrame(const Gfx::PointF& pos)
{
    WindowFrame* windowFrame = 0;

    std::vector<WindowFrame*>::const_reverse_iterator rit;
    for(rit = _windows.rbegin() ; rit != _windows.rend(); ++rit )
    {
        WindowFrame* frame = *rit;
        Window* window = frame->window();

        if( frame->frameRect().contains(pos) && 
            window->acceptsInput() )
        {
            windowFrame = frame;
            break;
        }
    }

    return windowFrame;
}


void ShellWM::onSetCapture(bool capture)
{
    Visual::onSetCapture(capture);

    if( ! capture )
    {
        if(_grabbedFrame)
            _grabbedFrame = 0;
    }
}

///////////////////////////////////////////////////////////////////////
// WindowManager
///////////////////////////////////////////////////////////////////////

WindowImpl* ShellWM::onCreateWindow(const WindowType& type)
{
    return new ShellWindowImpl(type);
}


void ShellWM::onAttach(Window& w)
{
    WindowFrame* frame = new WindowFrame(*this, w);

    switch( w.type() )
    {
        case WindowType::Popup:
            frame->setFrame(0, 0);
            break;
        
        default:
        case WindowType::Default:
            frame->setFrame(_borderWidth, _titleHeight);
            break;
    }

    if(_topMostWindow)
        _windows.insert( --_windows.end(), frame );
    else
        _windows.push_back(frame);

    w.setNextResponder(this);
}


void ShellWM::onDetach(Window& w)
{
    w.setNextResponder(0);

    std::vector<WindowFrame*>::iterator wit;
    for(wit = _windows.begin(); wit != _windows.end(); ++wit)
    {
        if((*wit)->window() == &w)
        {
            if(_grabbedFrame && _grabbedFrame->window() == &w)
            {
                setCapture(false);
                _grabbedFrame = 0;
            }

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


void ShellWM::onInit(Window& w)
{
    double scaling = scaleFactor();
    
    RescaleEvent ev(w, scaling);
    w.processEvent(ev);
    //Application::instance().loop().commitEvent(ev);
}


void ShellWM::onRelease(Window& w)
{
}


Gfx::PointF ShellWM::onToWindow(const Window& w, 
                                const Gfx::PointF& pos) const
{
    WindowFrame* frame = getWindowFrame(w);
    if( ! frame )
        return pos;

    return frame->fromFrame(pos) - w.position();
}


Gfx::PointF ShellWM::onFromWindow(const Window& w, 
                                  const Gfx::PointF& pos) const
{
    WindowFrame* frame = getWindowFrame(w);
    if( ! frame )
        return pos;

    return w.position() + frame->toFrame(pos);
}


void ShellWM::onRepaint(Window& w, const Gfx::RectF& rect)
{
    Gfx::PointF windowPos = onFromWindow( w, rect.topLeft() );
    Gfx::RectF windowRect( windowPos, rect.size() );

    repaint(windowRect);
}


void ShellWM::onShow(Window& w, bool visible)
{
    WindowFrame* frame = getWindowFrame(w);
    if( ! frame )
        return;

    ShowEvent sev( w, visible );
    Application::instance().loop().commitEvent(sev);

    // TODO: move updating to frame

    Gfx::RectF updateRect = frame->frameRect();

    // w.invalidate();
    repaint(updateRect);
}


void ShellWM::onActivate(Window& w, bool active)
{
    ActivateEvent aev(*this, active);
    Application::instance().loop().commitEvent(aev);

    WindowFrame* frame = getWindowFrame(w);
    if( ! frame )
        return;

    if(active)
    {
        //
        // deactivate other active window
        //
        if(_activeWindow && _activeWindow != frame)
            _activeWindow->window()->activate(false);

        //
        // raise to top of window stack
        //
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

        _activeWindow = frame;
    }
    else
    {
        if(frame == _activeWindow)
            _activeWindow = 0;
    }

    frame->repaint();

    activate(active);
}


void ShellWM::onEnableRequest(Window& w, bool enable)
{
    // TODO: move updating to frame

    if( ! isEnabled() )
      enable = false;

    EnableEvent eev(w, enable);
    Application::instance().loop().commitEvent(eev);
    
    WindowFrame* frame = getWindowFrame(w);
    if( ! frame )
        return;

    Gfx::RectF updateRect = frame->frameRect();

    repaint(updateRect);
}


void ShellWM::onMove(Window& w, const Gfx::PointF& pos)
{
    WindowFrame* frame = getWindowFrame(w);
    if( ! frame )
        return;

    Gfx::PointF aligedPos = _surface->align(pos);

    Gfx::RectF updateRect = frame->frameRect();
    frame->moveEvent(aligedPos);

    Gfx::RectF movedRect = frame->frameRect();
    updateRect.unify(movedRect);

    MoveEvent mev(w, aligedPos);
    Application::instance().loop().commitEvent(mev);
    //Application::instance().processEvent(mev);

    repaint(updateRect);
}


void ShellWM::onResize(Window& w, const Gfx::SizeF& s)
{ 
    Gfx::SizeF alignedSize = _surface->align(s);

    if( alignedSize.width() > w.maximumSize().width() )
        alignedSize.setWidth( w.maximumSize().width() );

    if( alignedSize.height() > w.maximumSize().height() )
        alignedSize.setHeight( w.maximumSize().height() );

    if( alignedSize.width() < w.minimumSize().width() )
        alignedSize.setWidth( w.minimumSize().width() );

    if( alignedSize.height() < w.minimumSize().height() )
        alignedSize.setHeight( w.minimumSize().height() );

    WindowFrame* frame = getWindowFrame(w);
    if( ! frame )
        return;

    Gfx::RectF updateRect = frame->frameRect();
    frame->resizeEvent(alignedSize);

    Gfx::RectF resizedRect = frame->frameRect();
    updateRect.unify(resizedRect);

    ResizeEvent rev(w, alignedSize);
    Application::instance().loop().commitEvent(rev);
    //Application::instance().processEvent(rev);

    repaint(updateRect);
}


void ShellWM::onSetAbove(Window& w, bool above)
{
    WindowFrame* frame = getWindowFrame(w);
    if( ! frame )
        return;

    if(above)
    {
        if(_topMostWindow && _topMostWindow != frame)
            _topMostWindow->window()->setAbove(false);

        // move top most frame to the back
        std::vector<WindowFrame*>::iterator it = std::find(_windows.begin(), 
                                                           _windows.end(), frame);
        if( it != _windows.end() )
            _windows.erase(it);
        
        _windows.push_back(frame);

        _topMostWindow = frame;
    }
    else if(_topMostWindow == frame)
    {
        _topMostWindow = 0;
    }
}


void ShellWM::onSetTitle(Window& w, const std::string& text)
{
    WindowFrame* frame = getWindowFrame(w);
    if( ! frame )
        return;

    Gfx::RectF updateRect = frame->frameRect();
    repaint(updateRect);
}


void ShellWM::onSetIcon(Window& w, const Gfx::Image& icon)
{
}


void ShellWM::onSetState(Window& w, const WindowState& state)
{
    WindowFrame* frame = getWindowFrame(w);
    if( ! frame )
        return;

    Window::State oldState = frame->state();
    
    if(state != oldState)
    {
        frame->setState(state);

        if(oldState == WindowState::Normal)
            frame->setRestore(w.position(), w.size());

        if(state == WindowState::Maximized)
        {
            Gfx::SizeF maxSize = size();
            maxSize = frame->fromFrame(maxSize);

            w.move( Gfx::PointF(0,0) );
            w.resize(maxSize);
        }
        else if(state == WindowState::Minimized)
        {
            if(oldState == WindowState::Normal)
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
        else if(state == WindowState::Normal)
        {
            w.move( frame->restorePosition() );
            w.resize( frame->restoreSize() );
        }

        WindowStateEvent wse(w, state);
        Application::instance().loop().commitEvent(wse);
    }
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

    std::vector<WindowFrame*>::iterator wit;
    for(wit = _windows.begin(); wit != _windows.end(); ++wit)
    {
        WindowFrame* frame = *wit;
        Window* window = frame->window();

        RescaleEvent rev(*window, scaling);
        window->processEvent(rev);

        // align frame via child window
        frame->setFrame(_borderWidth, _titleHeight);
    }
}


void ShellWM::onRepaintRequest(const Gfx::RectF& rect)
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

    //
    // paint child windows
    //
    std::vector<WindowFrame*>::iterator wit;
    for(wit = _windows.begin(); wit != _windows.end(); ++wit)
    {
        WindowFrame* frame = *wit;
        Window* window = frame->window();

        Gfx::PointF winPos = onToWindow( *window, rect.topLeft() );
        Gfx::RectF winRect( winPos, rect.size() );

        winRect = winRect.intersect( Gfx::RectF( window->size() ) );

        PaintEvent pev( *window, winRect );
        window->processEvent(pev);
    }

    //
    // paint child window contents and frames on surface
    //
    std::vector<WindowFrame*>::iterator it;
    for(it = _windows.begin(); it != _windows.end(); ++it )
    {
        WindowFrame* frame = *it;
        Window* w = frame->window();
        
        if( ! w || ! w->isVisible() )
            continue; 

        // clip window frame rect
        Gfx::RectF frameRect = frame->frameRect().intersect(rect);
        if( frameRect.isNull() )
            continue;

        // paint frame rect
        frame->paint( *_surface, frameRect );

        // clip client rect
        Gfx::RectF updateRect = frame->clientRect().intersect(rect);

        // paint client rect
        Gfx::PointF surfacePos = onToWindow( *w, updateRect.topLeft() );
        Gfx::RectF surfaceRect( surfacePos, updateRect.size() );

        Pt::Gfx::Painter painter(*_surface);
        painter.drawSurface(updateRect.topLeft(), w->surface(), surfaceRect);
    }
}


void ShellWM::onProcessEnableEvent(const EnableEvent& ev)
{
    Base::onProcessEnableEvent(ev);

    for( size_t i = 0; i < _windows.size(); ++i)
    {
        WindowFrame* frame = _windows[i];
        Window* w = frame->window();

        EnableEvent eev(*w, ev.enabled());
        Application::instance().loop().commitEvent(eev);
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
            _activeWindow->window()->activate(false);
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
            _activeWindow->window()->activate(false);
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
        Window* window = _activeWindow->window();
        if(window)
        {
            window->processEvent(ev);
            return;
        }
    }
}


void ShellWM::onProcessKeyEvent(const KeyEvent& ev)
{
    //if( ! acceptsInput() )
    //    return;

    if(_activeWindow)
    {
        Window* window = _activeWindow->window();
        if(window)
        {
            KeyEvent kev = ev;
            kev.setVisual(window);
            window->processEvent(ev);
            return;
        }
    }
}

} // namespace

} // namespace
