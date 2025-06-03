/* Copyright (C) 2015 Marc Boris Duerner

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

#include "WorkspaceFrame.h"

#include <Pt/Forms/WorkspaceManager.h>
#include <Pt/Forms/Workspace.h>
#include <Pt/Forms/Window.h>
#include <Pt/Forms/Application.h>
#include <Pt/Forms/WindowStateEvent.h>
#include <Pt/Gfx/PaintSurface.h>

namespace Pt {

namespace Forms {

WorkspaceManager::WorkspaceManager()
: _parent(0)
, _activeWindow(0)
, _topMostWindow(0)
, _borderWidth(4.0)
, _titleHeight(20.0)
, _activeColor( Gfx::Color::fromRgb8(100, 128, 205) )
, _inactiveColor( Gfx::Color::fromRgb8(173, 178, 191) )
, _textColor( Gfx::Color::fromRgb8(255, 255, 255) )
, _inactiveTextColor( Gfx::Color::fromRgb8(50, 50, 50) )
{
}


WorkspaceManager::~WorkspaceManager()
{
    while( ! _windowList.empty() )
    {
        _windowList.front()->unparent();
    }
}


void WorkspaceManager::setParent(Workspace* workspace)
{
    _parent = workspace;

    onSetParent(_parent);
}


//Gfx::PaintSurface& WorkspaceManager::surface()
//{
//    return _surface;
//}
//
//
//const Gfx::PaintSurface& WorkspaceManager::surface() const
//{
//    return _surface;
//}
//
//
//void WorkspaceManager::setSurface(Gfx::PaintSurface* surface, const Gfx::PointF& pos)
//{
//    if( ! surface )
//    {
//        _surface.detach();
//    }
//    else
//    {
//        Gfx::RectF surfaceRect( pos, size() );
//        _surface.attach(*surface, surfaceRect);
//    }
//}


void WorkspaceManager::onProcessEvent(const Pt::Event& ev)
{
    WindowManager::onProcessEvent(ev);
}


Window* WorkspaceManager::activeWindow()
{
    return _activeWindow;
}


void WorkspaceManager::onRequestActivate(bool active)
{
    if(_parent)
        _parent->onActivate(*this, active);
}


const std::vector<Window*>& WorkspaceManager::windows() const
{
    return _windowList;
}


Gfx::PointF WorkspaceManager::toFrame(const WorkspaceFrame& w, 
                             const Gfx::PointF& pos) const
{
    return pos - w.position();
}


Gfx::PointF WorkspaceManager::fromFrame(const WorkspaceFrame& w, 
                               const Gfx::PointF& pos) const
{
    return pos + w.position();
}

///////////////////////////////////////////////////////////////////////
// Widget
///////////////////////////////////////////////////////////////////////

Gfx::PointF WorkspaceManager::onToParent(const Gfx::PointF& pos) const
{
    return pos + position();
}


Gfx::PointF WorkspaceManager::onFromParent(const Gfx::PointF& pos) const
{
    return pos - position();
}


Widget* WorkspaceManager::onHitTest(const Gfx::PointF& p)
{
    if( ! bounds().contains(p) )
        return 0;

    std::vector<Window*>::const_reverse_iterator rit;
    for(rit = _windowList.rbegin() ; rit != _windowList.rend(); ++rit )
    {
        Window* window = *rit;
        WorkspaceFrame* frame = static_cast<WorkspaceFrame*>( window->frame() );

        if( ! window->isVisible() )
            continue;

        Gfx::PointF pos = toFrame(*frame, p);
        Widget* hit = frame->hitTest(pos);
        if(hit)
            return hit;
    }

    return 0;
}


void WorkspaceManager::onRequestCapture(bool capture)
{
    Widget::onRequestCapture(capture);
}

///////////////////////////////////////////////////////////////////////
// WindowManager
///////////////////////////////////////////////////////////////////////

WindowFrame* WorkspaceManager::onAttach(Window& w)
{
    WorkspaceFrame* frame = new WorkspaceFrame(*this, w);
    frame->setNextResponder(this);

    if(_topMostWindow)
        _windowList.insert( --_windowList.end(), &w );
    else
        _windowList.push_back(&w);

    return frame;
}


void WorkspaceManager::onDetach(WindowFrame& frame)
{
    frame.setNextResponder(0);

    Window& w = frame.window();

    std::vector<Window*>::iterator wit;
    for(wit = _windowList.begin(); wit != _windowList.end(); ++wit)
    {
        Window* window = *wit;

        if(window == &w)
        {
            if(_activeWindow && _activeWindow == &w)
                _activeWindow  = 0;

            if(_topMostWindow && _topMostWindow == &w)
                _topMostWindow = 0;

            _windowList.erase(wit);
            break;
        }
    }
}


void WorkspaceManager::onInit(WindowFrame& frame)
{
    double scaling = scaleFactor();
    
    RescaleEvent ev(frame, scaling);
    frame.processEvent(ev);
}


void WorkspaceManager::onRelease(WindowFrame& frame)
{
    if( frame.isVisible() )
    {
        Gfx::RectF frameRect( frame.position(), frame.size() );
        repaint(frameRect);
    }
}


void WorkspaceManager::onShow(WorkspaceFrame& frame, bool visible)
{
    ShowEvent windowEvent( frame, visible );
    Application::instance().loop().commitEvent(windowEvent);
}


void WorkspaceManager::onActivate(WorkspaceFrame& frame, bool active)
{
    Window& w = frame.window();

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

    ActivateEvent aev(frame, active);
    Application::instance().commitEvent(aev);
    
    activate(active);
}


void WorkspaceManager::onEnable(WorkspaceFrame& frame, bool enable)
{
    if( ! isEnabled() )
      enable = false;

    EnableEvent eev(frame, enable);
    Application::instance().commitEvent(eev);
}


void WorkspaceManager::onSetAbove(WorkspaceFrame& frame, bool above)
{
    Window& w = frame.window();

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


//void WorkspaceManager::onSetTitle(Window& w, const std::string& text)
//{
//    w.frame())->setTitle(text);
//}


//void WorkspaceManager::onSetIcon(Window& w, const Gfx::Image& icon)
//{
//    w.frame())->setIcon(icon);
//}


//void WorkspaceManager::onSetState(Window& w, const WindowState& state)
//{
//    w.frame())->setState(state);
//}


void WorkspaceManager::onSetSizeLimits(WorkspaceFrame& w, 
                              const Gfx::SizeF& minSize, 
                              const Gfx::SizeF& maxSize)
{
}


//void WorkspaceManager::onClosing(Window& w)
//{         
//    CloseEvent ev(w);
//    Application::instance().loop().commitEvent(ev);
//}


void WorkspaceManager::onClose(WorkspaceFrame& wf)
{
    CloseEvent ev(wf);
    Application::instance().loop().commitEvent(ev);
}

///////////////////////////////////////////////////////////////////////
// Implementation
///////////////////////////////////////////////////////////////////////

void WorkspaceManager::onProcessRescaleEvent(const RescaleEvent& ev)
{
    Base::onProcessRescaleEvent(ev);

    double scaling = ev.scaleFactor();

    std::vector<Window*>::iterator wit;
    for(wit = _windowList.begin(); wit != _windowList.end(); ++wit)
    {
        Window* window = *wit;
        WindowFrame* frame = window->frame();

        RescaleEvent rev(*frame, scaling);
        frame->processEvent(rev);

        if(window->state() == WindowState::Maximized)
        {
            window->setState(WindowState::Maximized);
        }
    }
}


void WorkspaceManager::onRequestRepaint(const Gfx::RectF& rect)
{
    if(_parent)
        _parent->onRepaint(*this, rect);
}


void WorkspaceManager::onProcessPaintEvent(const PaintEvent& ev)
{
    const Gfx::RectF& rect = ev.rect();
    if( rect.isNull() )
        return;

    Widget::onProcessPaintEvent(ev);

    //
    // paint child windows
    //
    std::vector<Window*>::iterator wit;
    for(wit = _windowList.begin(); wit != _windowList.end(); ++wit)
    {
        Window* window = *wit;
        WorkspaceFrame* frame = static_cast<WorkspaceFrame*>( window->frame() );

        if( ! window->isVisible() )
            continue;

        Gfx::RectF frameRect( frame->position(), frame->size() );
        frameRect = rect.intersect(frameRect);

        Gfx::PointF winPos = toFrame( *frame, frameRect.topLeft() );
        Gfx::RectF winRect( winPos, rect.size() );

        PaintEvent pev( *frame, winRect );
        frame->processEvent(pev);

        Gfx::PointF surfacePos = frameRect.topLeft() - frame->position();
        Gfx::RectF surfaceRect( surfacePos, frameRect.size() );

        if(_parent)
        {
            // TODO: work with clip instead of using a subrect of layer
            //Gfx::Painter painter( _parent->surface() );
            //painter.setClip(rect);
            //painter.drawLayer( frame->position(), frame->pixmap() );

            Gfx::Painter painter( _parent->surface() );
            painter.drawLayer(frameRect.topLeft(), frame->pixmap(), surfaceRect);

            //_parent->surface().drawPixmap(frameRect.topLeft(), frame->pixmap(), &surfaceRect);
        }
        
        //surface().drawPixmap(frameRect.topLeft(), frame->surface(), 
        //                     surfaceRect, Gfx::CompositionMode::SourceCopy);
    }
}


void WorkspaceManager::onProcessEnableEvent(const EnableEvent& ev)
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


void WorkspaceManager::onMove(WorkspaceFrame& frame, const Gfx::PointF& pos)
{
    Gfx::PointF aligedPos = _parent ? _parent->scaling().align(pos)
                                    : pos;

    MoveEvent mev(frame, aligedPos);
    Application::instance().commitEvent(mev);
}


void WorkspaceManager::onResize(WorkspaceFrame& frame, const Gfx::SizeF& to)
{
    ResizeEvent rev(frame, to);
    Application::instance().commitEvent(rev);
}


void WorkspaceManager::onProcessResizeEvent(const ResizeEvent& ev)
{
    Base::onProcessResizeEvent(ev);
}


void WorkspaceManager::onResizeEvent(const ResizeEvent& ev)
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


bool WorkspaceManager::processMouseEvent(const MouseEvent& ev)
{
    //if( ! acceptsInput() )
    //    return;

    //
    // hit test
    //
    Gfx::PointF pos = fromGlobal( ev.position() );

    WindowFrame* hitFrame = 0;

    std::vector<Window*>::const_reverse_iterator rit;
    for(rit = _windowList.rbegin() ; rit != _windowList.rend(); ++rit )
    {
        Window* window = *rit;
        WorkspaceFrame* frame = static_cast<WorkspaceFrame*>( window->frame() );

        Gfx::PointF p = toFrame(*frame, pos);
        
        Widget* hit = frame->hitTest(p);
        if(hit)
        {
            hitFrame = frame;
            break;
        }
    }

    //
    // window activation
    //
    if( ev.isPress() )
    {
        if( ! hitFrame && _activeWindow )
        {
            _activeWindow->activate(false);
        }
        
        if( hitFrame && ! hitFrame->window().isActive() )
        {
            hitFrame->window().activate();
        }
    }

    //
    // propagate event to window frame
    //
    if(hitFrame)
    {
        hitFrame->processEvent(ev);
        return true;
    }

    return false;
}


bool WorkspaceManager::processTouchEvent(const TouchEvent& ev)
{
    //if( ! acceptsInput() )
    //    return;

    //
    // hit test
    //
    Gfx::PointF pos = fromGlobal( ev.position() );

    WindowFrame* hitFrame = 0;

    std::vector<Window*>::const_reverse_iterator rit;
    for(rit = _windowList.rbegin() ; rit != _windowList.rend(); ++rit )
    {
        Window* window = *rit;
        WorkspaceFrame* frame = static_cast<WorkspaceFrame*>( window->frame() );

        Gfx::PointF p = toFrame(*frame, pos);
        Widget* hit = frame->hitTest(p);
        if(hit)
        {
            hitFrame = frame;
            break;
        }
    }

    //
    // window activation
    //
    if( ev.isPress() )
    {
        if( ! hitFrame && _activeWindow )
        {
            _activeWindow->activate(false);
        }
        
        if( hitFrame && ! hitFrame->window().isActive() )
        {
            hitFrame->window().activate();
        }
    }

    //
    // propagate event to window frame
    //
    if(hitFrame)
    {
        hitFrame->processEvent(ev);
        return true;
    }

    return false;
}


void WorkspaceManager::onProcessMouseEvent(const MouseEvent& ev)
{
    processMouseEvent(ev);
}


void WorkspaceManager::onProcessTouchEvent(const TouchEvent& ev)
{
    processTouchEvent(ev);
}


void WorkspaceManager::onProcessEnterEvent(const EnterEvent& ev)
{
    Base::onProcessEnterEvent(ev);
}


void WorkspaceManager::onProcessLeaveEvent(const LeaveEvent& ev)
{
    Base::onProcessLeaveEvent(ev);
}


void WorkspaceManager::onProcessScrollEvent(const ScrollEvent& ev)
{
    //if( ! acceptsInput() )
    //    return;

    if( _activeWindow )
    {
        _activeWindow->processEvent(ev);
        return;
    }
}


void WorkspaceManager::onProcessKeyEvent(const KeyEvent& ev)
{
    //if( ! acceptsInput() )
    //    return;


    if(_activeWindow)
    {
        KeyEvent kev = ev;
        kev.setWidget(_activeWindow);
        _activeWindow->processEvent(ev);
        return;
    }
}

} // namespace

} // namespace
