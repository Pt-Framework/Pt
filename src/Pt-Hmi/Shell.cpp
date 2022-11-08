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

#include "WindowFrame.h"

#include <Pt/Hmi/Shell.h>
#include <Pt/Hmi/Window.h>
#include <Pt/Hmi/Application.h>
#include <Pt/Hmi/MouseEvent.h>
#include <Pt/Hmi/WindowStateEvent.h>
#include <Pt/Hmi/KeyEvent.h>
#include <Pt/Hmi/LayoutEvent.h> // RescaleEvent
#include <Pt/Hmi/PaintEvent.h>
#include <Pt/Gfx/Pen.h>
#include <Pt/Gfx/Brush.h>
#include <Pt/Gfx/Point.h>
#include <Pt/Gfx/Painter.h>
#include <Pt/Gfx/PaintSurface.h>
#include <Pt/String.h>

#include <cmath>

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

    protected:
        virtual void onSetType(WindowType type);

        virtual void onSetTitle(const std::string& text);

        virtual void onSetIcon(const Gfx::Image& p);

        virtual void onSetTopMost(bool top);

        virtual void onSetState(WindowState s);

        virtual void onSetMinimumSize(const Gfx::SizeF& s);

        virtual  void onSetMaximumSize(const Gfx::SizeF& s);
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


void ShellWindowImpl::onSetType(WindowType type)
{
}


void ShellWindowImpl::onSetTitle(const std::string& text)
{
}


void ShellWindowImpl::onSetIcon(const Gfx::Image& p)
{
}


void ShellWindowImpl::onSetTopMost(bool top)
{
}


void ShellWindowImpl::onSetState(WindowState s)
{
}


void ShellWindowImpl::onSetMinimumSize(const Gfx::SizeF& s)
{
}


void ShellWindowImpl::onSetMaximumSize(const Gfx::SizeF& s)
{
}

///////////////////////////////////////////////////////////////////////
// Shell
///////////////////////////////////////////////////////////////////////

Shell::Shell()
: _content(0)
, _pointer(0)
, _capture(0)
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
    setFocusPolicy(Widget::AcceptFocus);
}


Shell::~Shell()
{
    while( ! _windows.empty() )
    {
        _windows.front()->window()->unparent();
    }
}


void Shell::addWindow(Window& w)
{
    w.setParent(*this);
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

///////////////////////////////////////////////////////////////////////
// Widget
///////////////////////////////////////////////////////////////////////

void Shell::onRemoveWidget(Widget& w)
{
    Widget::onRemoveWidget(w);

    if(&w == _content)
        _content = 0;
}

///////////////////////////////////////////////////////////////////////
// Visual
///////////////////////////////////////////////////////////////////////

Visual* Shell::onHitTest(const Gfx::PointF& p)
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

    return Widget::onHitTest(p);
}


void Shell::onSetCapture(bool capture)
{
    Widget::onSetCapture(capture);

    if( ! capture )
    {
        if(_grabbedFrame)
            _grabbedFrame = 0;
    }
}


void Shell::onRelease()
{
    Widget::onRelease();

    setPointer(false);
    setCapture(false);

    std::vector<WindowFrame*>::iterator wit;
    for(wit = _windows.begin(); wit != _windows.end(); ++wit)
    {
        WindowFrame* windowFrame = *wit;
        windowFrame->window()->release();
    }
}

///////////////////////////////////////////////////////////////////////
// WindowManager
///////////////////////////////////////////////////////////////////////

WindowImpl* Shell::onCreateWindow(const WindowType& type)
{
    return new ShellWindowImpl(type);
}


void Shell::onAttach(Window& w)
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


void Shell::onDetach(Window& w)
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

    if(_capture == &w)
        _capture = 0;
}


void Shell::onInit(Window& w)
{
    double scaling = surface().scaleFactor();
    
    RescaleEvent ev(w, scaling);
    //w.processEvent(ev);
    Application::instance().loop().commitEvent(ev);
}


void Shell::onRelease(Window& w)
{
}


Gfx::PointF Shell::onToWindow(const Window& w, 
                              const Gfx::PointF& pos) const
{
    WindowFrame* frame = getWindowFrame(w);
    if( ! frame )
        return pos;

    Gfx::PointF p = frame->fromFrame(pos) - w.position();
    return p;
}


Gfx::PointF Shell::onFromWindow(const Window& w, 
                                const Gfx::PointF& pos) const
{
    WindowFrame* frame = getWindowFrame(w);
    if( ! frame )
        return pos;

    return w.position() + frame->toFrame(pos);
}


void Shell::onRepaint(Window& w, const Gfx::RectF& rect)
{
    Gfx::PointF windowPos = onFromWindow( w, rect.topLeft() );
    Gfx::RectF windowRect( windowPos, rect.size() );

    repaint(windowRect);
}


void Shell::onShow(Window& w, bool visible)
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


void Shell::onActivate(Window& w, bool active)
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


void Shell::onEnable(Window& w, bool enable)
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


void Shell::onMove(Window& w, const Gfx::PointF& pos)
{
    WindowFrame* frame = getWindowFrame(w);
    if( ! frame )
        return;

    Gfx::PointF aligedPos = surface().align(pos);

    Gfx::RectF updateRect = frame->frameRect();
    frame->moveEvent(aligedPos);

    Gfx::RectF movedRect = frame->frameRect();
    updateRect.unify(movedRect);

    MoveEvent mev(w, aligedPos);
    Application::instance().loop().commitEvent(mev);
    //Application::instance().processEvent(mev);

    repaint(updateRect);
}


void Shell::onResize(Window& w, const Gfx::SizeF& s)
{ 
    Gfx::SizeF alignedSize = surface().align(s);

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


void Shell::onSetAbove(Window& w, bool above)
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


void Shell::onSetTitle(Window& w, const std::string& text)
{
    WindowFrame* frame = getWindowFrame(w);
    if( ! frame )
        return;

    Gfx::RectF updateRect = frame->frameRect();
    repaint(updateRect);
}


void Shell::onSetIcon(Window& w, const Gfx::Image& icon)
{
}


void Shell::onStateChanged(Window& w)
{
    WindowFrame* frame = getWindowFrame(w);
    if( ! frame )
        return;

    Window::State state = w.state();
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


void Shell::onClosing(Window& w)
{         
    CloseEvent ev(w);
    Application::instance().loop().commitEvent(ev);
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


void Shell::onProcessRescaleEvent(const RescaleEvent& ev)
{
    Widget::onProcessRescaleEvent(ev);

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


void Shell::onProcessPaintEvent(const PaintEvent& ev)
{
    Widget::onProcessPaintEvent(ev);

    const Gfx::RectF& rect = ev.rect();

    //std::clog << "  PAINT(" << "Shell" << "): " 
    //          << rect.width() << "x" << rect.height() << std::endl;

    if( rect.isNull() )
        return;

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
        frame->paint( surface(), frameRect );

        // clip client rect
        Gfx::RectF updateRect = frame->clientRect().intersect(rect);

        // paint client rect
        Gfx::PointF surfacePos = onToWindow( *w, updateRect.topLeft() );
        Gfx::RectF surfaceRect( surfacePos, updateRect.size() );

        Pt::Gfx::Painter painter( surface() );
        painter.drawSurface(updateRect.topLeft(), w->surface(), surfaceRect);
    }
}


void Shell::onProcessEnableEvent(const EnableEvent& ev)
{
    Widget::onProcessEnableEvent(ev);

    bool enable = ev.enabled();
    if( ! isEnabled() )
      enable = false;

    for( size_t i = 0; i < _windows.size(); ++i)
    {
        WindowFrame* frame = _windows[i];
        Window* w = frame->window();
        onEnable( *w, enable );
    }
}


void Shell::onProcessMouseEvent(const MouseEvent& ev)
{
    if( ! acceptsInput() )
        return;

    //
    // continue press sequence capture
    // 
    if(_grabbedFrame)
    {
        _grabbedFrame->mouseEvent(ev);

        if( ev.isRelease() )
        {
            setCapture(false);
            _grabbedFrame = 0;
        }

        return;
    }

    Gfx::PointF pos = fromGlobal( ev.position() );

    //
    // hit test
    //
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
    // window frame
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

        windowFrame->mouseEvent(ev);
        return;
    }

    //
    // process event
    //
    Widget::onProcessMouseEvent(ev);
}


WindowFrame* Shell::activeWindow()
{
    return _activeWindow;
}


void Shell::onProcessTouchEvent(const TouchEvent& ev)
{
    if( ! acceptsInput() )
        return;

    //
    // continue press sequence capture
    // 
    if(_capture)
    {
        if(_capture == this)
            Widget::onProcessTouchEvent(ev);
        else
            _capture->processEvent(ev);

        if( ev.isRelease() )
            _capture = 0;

        return;
    }

    if(_grabbedFrame)
    {
        _grabbedFrame->touchEvent(ev);

        if( ev.isRelease() )
            _grabbedFrame = 0;

        return;
    }

    Gfx::PointF pos = fromGlobal( ev.position() );

    //
    // hit test
    //
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
    // window frame
    //
    if(windowFrame)
    {
        bool isClient = windowFrame->clientRect().contains(pos);

        if(isClient)
        {
            if( ev.isPress() )
                _capture = windowFrame->window();
        }
        else
        {
            if( ev.isPress() )
                _grabbedFrame = windowFrame;

            // TODO: make WindowFrame a proper Visual to handle events
            //windowFrame->setPointer(true);
        }

        windowFrame->touchEvent(ev);
        return;
    }

    //
    // handle event
    //
    if( ev.isPress() )
    {
        _capture = this;
    }

    Widget::onProcessTouchEvent(ev);
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

    if( _activeWindow )
    {
        Window* window = _activeWindow->window();
        if(window)
        {
            window->processEvent(ev);
            return;
        }
    }

    Widget::onProcessScrollEvent(ev);
}


void Shell::onProcessKeyEvent(const KeyEvent& ev)
{
    if( ! acceptsInput() )
        return;

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

    Widget::onProcessKeyEvent(ev);
}


WindowFrame* Shell::getWindowFrame(const Window& w) const
{
    std::vector<WindowFrame*>::const_iterator it;
    for(it = _windows.begin(); it != _windows.end(); ++it)
    {
        if( (*it)->window() == &w)
            return *it;
    }

    return 0;
}

} // namespace

} // namespace
