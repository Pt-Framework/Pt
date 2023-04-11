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
  
  You should have received a copy of the GNU Lesser General Public License 
  along with this library; if not, write to the Free Software Foundation, 
  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
*/

#include <Pt/Hmi/Window.h>
#include <Pt/Hmi/Widget.h>
#include <Pt/Hmi/Application.h>
#include <Pt/Hmi/WindowManager.h>
#include <Pt/Gfx/Painter.h>
#include <Pt/Gfx/Algorithm.h>
#include <Pt/Hmi/FocusEvent.h>
#include <Pt/Hmi/WindowStateEvent.h>

#include <cassert>
#include <fstream>

namespace Pt {

namespace Hmi {

///////////////////////////////////////////////////////////////////////
// WindowImpl
///////////////////////////////////////////////////////////////////////

WindowImpl::WindowImpl(WindowManager& wm, Window& window)
: _wm(wm)
, _window(window)
{
    eventReceived() += Pt::slot(*this, &WindowImpl::onProcessCloseEvent);
    eventReceived() += Pt::slot(*this, &WindowImpl::onProcessWindowStateEvent);
}


WindowImpl::~WindowImpl()
{
}


PixmapSurface& WindowImpl::surface()
{
    return _surface;
}


const PixmapSurface& WindowImpl::surface() const
{
    return _surface;
}


Gfx::PointF WindowImpl::onToParent(const Gfx::PointF& pos) const
{ 
    return pos; 
}
     
        
Gfx::PointF WindowImpl::onFromParent(const Gfx::PointF& pos) const
{ 
    return pos; 
}


void WindowImpl::onProcessRescaleEvent(const RescaleEvent& ev)
{
    Base::onProcessRescaleEvent(ev);
}


void WindowImpl::onRescaleEvent(const RescaleEvent& ev)
{
    _surface.setScaleFactor( ev.scaleFactor() );

    Base::onRescaleEvent(ev);
}


void WindowImpl::onProcessResizeEvent(const ResizeEvent& ev)
{
    Base::onProcessResizeEvent(ev);
}


void WindowImpl::onResizeEvent(const ResizeEvent& ev)
{
    Visual::onResizeEvent(ev);

    _surface.resize( ev.size() );
}


void WindowImpl::onProcessWindowStateEvent(const WindowStateEvent& ev)
{
    onWindowStateEvent(ev);
}


void WindowImpl::onWindowStateEvent(const WindowStateEvent& ev)
{
}


void WindowImpl::onProcessCloseEvent(const CloseEvent& ev)
{
    onCloseEvent(ev);
}


void WindowImpl::onCloseEvent(const CloseEvent& ev)
{
}

///////////////////////////////////////////////////////////////////////
// Window
///////////////////////////////////////////////////////////////////////

Window::Window(WindowManager* parent, WindowType type)
: _impl(0)
, _parent(0)
, _show(false)
, _isActive(false)
, _enabled(true)
, _isClosed(false)
, _requestedPosition(0, 0)
, _requestedSize(80, 80)
, _autoSize(false)
, _type(type)
, _state(WindowState::Normal)
, _isAbove(false)
{
    eventReceived() += Pt::slot(*this, &Window::onProcessActivateEvent);
    eventReceived() += Pt::slot(*this, &Window::onProcessCloseEvent);
    eventReceived() += Pt::slot(*this, &Window::onProcessWindowStateEvent);

    if(parent)
        setParent(*parent);
}


Window::~Window()
{
    unparent();
}


void Window::setParent(WindowManager& parent)
{
    if(_parent == &parent)
        return;

    unparent();

    _isClosed = false;

    _impl = parent.onAttach(*this);
    _parent = &parent;
   
    _parent->onInit(*this);
    _parent->onSetSizeLimits(*this, minimumSize(), maximumSize());
    _impl->onSetState(*this, _state);
    _impl->onSetTitle(*this, _title);
    _impl->onSetIcon(*this, _icon);
    _parent->onSetAbove(*this, _isAbove);

    if(_state == WindowState::Normal)
    {
        _impl->onMove(*this, _requestedPosition);

        if( ! isAutoSize() )
            _impl->onResize(*this, _requestedSize);
    }
    
    _parent->onActivate(*this, _isActive);
    _parent->onEnableRequest(*this, _enabled);
    _parent->onShow(*this, _show);
    
    onSetParent(_parent);
}


void Window::unparent()
{
    if( ! _parent )
        return;

    _parent->onRelease(*this);
    _parent->onDetach(*this);
    _parent = 0;

    delete _impl;
    _impl = 0;

    onSetParent(_parent);
}


Gfx::Image Window::getImage() const
{
    return surface().toImage();
}


const Gfx::Brush& Window::background() const
{
    return _background ? *_background
                       : Application::instance().styleOptions().background();
}


void Window::setBackground(const Gfx::Brush& b)
{
    _background.reset( new Gfx::Brush(b) );
    invalidate();
}

///////////////////////////////////////////////////////////////////////
// geometry
///////////////////////////////////////////////////////////////////////

bool Window::isAutoSize() const
{
    return _autoSize;
}


Gfx::SizeF Window::setAutoSize(const SizePolicy& policy)
{   
    _sizePolicy = policy;
    _autoSize = true;

    if( ! _parent )
    {
        Screen& screen = Application::instance().screen();
        screen.addWindow(*this);
    }

    relayout();

    Widget* mainWidget = content();
    return mainWidget ? mainWidget->measure(_sizePolicy)
                      : _sizePolicy.size();
}


Gfx::SizeF Window::onMeasure()
{
    if(_autoSize)
    {
        Widget* mainWidget = content();
        return mainWidget ? mainWidget->measure(_sizePolicy)
                          : _sizePolicy.size();
    }

    return Form::onMeasure();
}


void Window::onLayoutEvent(const LayoutEvent& ev)
{
    if(_autoSize)
    {
        Widget* mainWidget = content();
        if(mainWidget)
            resize( mainWidget->preferredSize() );
    }

    Base::onLayoutEvent(ev);
}


void Window::onRequestMove(const Gfx::PointF& pos)
{
    _requestedPosition = pos;

    setState(WindowState::Normal);

    if(_impl)
    {
        _impl->onMove(*this, _requestedPosition);
    }
}


void Window::onProcessMoveEvent(const MoveEvent& ev)
{
    Base::onProcessMoveEvent(ev);
}


void Window::onMoveEvent(const MoveEvent& ev)
{    
    Base::onMoveEvent(ev);
}


void Window::onSetSizeLimits(const Gfx::SizeF& minSize,
                             const Gfx::SizeF& maxSize)
{
    Base::onSetSizeLimits(minSize, maxSize);
    
    if(_parent)
        _parent->onSetSizeLimits(*this, minSize, maxSize);
}


void Window::onRequestResize(const Gfx::SizeF& s)
{
    _requestedSize = s;

    setState(WindowState::Normal);
    
    if(_impl)
    {
        _impl->onResize(*this, _requestedSize);
    }
}


void Window::onProcessResizeEvent(const ResizeEvent& ev)
{
    Base::onProcessResizeEvent(ev);
}


void Window::onResizeEvent(const ResizeEvent& ev)
{
    Base::onResizeEvent(ev);
}

///////////////////////////////////////////////////////////////////////
// Visual
///////////////////////////////////////////////////////////////////////

Visual* Window::onHitTest(const Gfx::PointF& p)
{
    if( ! bounds().contains(p) || ! isVisible() )
        return 0;

    Visual* hit = Form::onHitTest(p);
    if(hit)
        return hit;

    return this;
}


Gfx::PointF Window::onToParent(const Gfx::PointF& pos) const
{
    if( ! _parent )
        return pos;

    return _parent->onFromWindow(*this, pos);
}


Gfx::PointF Window::onFromParent(const Gfx::PointF& pos) const
{
    if( ! _parent )
        return pos;

    return _parent->onToWindow(*this, pos);
}


void Window::onProcessEvent(const Pt::Event& ev)
{
    Base::onProcessEvent(ev);
}

///////////////////////////////////////////////////////////////////////
// Implementation
///////////////////////////////////////////////////////////////////////

void Window::onInvalidateEvent(const InvalidateEvent& ev)
{
    Base::onInvalidateEvent(ev);
}


void Window::onInvalidate()
{
    Base::onInvalidate();

    _backgroundBrush = background();

    repaint( bounds() );
}


void Window::onRequestRepaint(const Gfx::RectF& rect)
{
    //std::clog << "REPAINT: " << title() << std::endl;

    if(_impl)
        _impl->onRepaint(*this, rect);
}


void Window::onProcessPaintEvent(const PaintEvent& ev)
{
    const Gfx::RectF& rect = ev.rect();
    if( rect.isNull() )
        return;

    if( ! this->isVisible() )
        return;

    Base::onProcessPaintEvent(ev);
}


void Window::onPaintEvent(const PaintEvent& ev)
{    
    //static int nnn = 0;
    //std::clog << "PAINT EVENT: " << typeid(*this).name() << " " << ++nnn << std::endl;

    Base::onPaintEvent(ev);

    Gfx::Painter painter( surface() );
    painter.setBrush(_backgroundBrush);
    painter.fillRect( ev.rect() );
}


bool Window::acceptsInput() const
{
    if( ! isEnabled() )
        return false;

    if( ! isVisible() )
        return false;

    return true;
}


bool Window::isActive() const
{
    return _isActive;
}


void Window::onRequestActivate(bool active)
{
    _isActive = active;

    if( _parent )
        _parent->onActivate(*this, active);
    else
        _isActive = active;
}


void Window::onProcessActivateEvent(const ActivateEvent& ev)
{
    if(_impl)
    {
        ActivateEvent aev( *_impl, ev.isActive() );
        _impl->processEvent(aev);
    }

    onActivateEvent(ev);
}


void Window::onActivateEvent(const ActivateEvent& ev)
{
    _isActive = ev.isActive();
}


void Window::onRequestShow(bool b)
{   
    _show = b;

    if( ! _parent )
    {
        Screen& screen = Application::instance().screen();
        screen.addWindow(*this);
    }
    
    invalidate();

    _parent->onShow(*this, b);
}


void Window::onProcessShowEvent(const ShowEvent& ev)
{
    if(_impl)
    {
        ShowEvent sev( *_impl, ev.visible() );
        _impl->processEvent(sev);
    }

    Base::onProcessShowEvent(ev);
}


void Window::onShowEvent(const ShowEvent& ev)
{
    Base::onShowEvent(ev);
}


void Window::onShow(bool visible)
{
    Base::onShow(visible);
}


void Window::showModal()
{
    Window* activeWindow = 0;

    Screen& screen = Application::instance().screen();
    const std::vector<Window*>& windows = screen.windows();

    // TODO: only disable enabled windows

    std::vector<Window*>::const_iterator it;
    for(it = windows.begin(); it != windows.end(); ++it)
    {
        Window* w = (*it);

        w->enable(false);

        if( w->isActive() )
            activeWindow = w;
    }

    enable(true);
    show(true);

    while( ! isClosed() )
    {
        if( ! isActive() )
            activate();

        Application::instance().nextEvent();
    }

    // TODO: only enable previously disabled windows

    for(it = windows.begin(); it != windows.end(); ++it)
    {
        Window* w = (*it);

        w->enable(true);

        if( activeWindow && activeWindow->vid() == w->vid() )
            activeWindow->activate();
    }
}


void Window::onRequestEnable(bool e)
{
    _enabled = e;

    if(_parent)
        _parent->onEnableRequest(*this, e);
}


void Window::onProcessEnableEvent(const EnableEvent& ev)
{
    bool isEnabled = ev.enabled();
    if( ! _enabled )
        isEnabled = false;

    if(_impl)
    {
        EnableEvent eev(*_impl, isEnabled);
        _impl->processEvent(eev);
    }

    EnableEvent eev(*this, isEnabled);
    Base::onProcessEnableEvent(eev);
}


void Window::onEnableEvent(const EnableEvent& ev)
{        
    Base::onEnableEvent(ev);
}


void Window::onEnable(bool e)
{
    Base::onEnable(e);

    invalidate();
}


void Window::onProcessRescaleEvent(const RescaleEvent& ev)
{
    Base::onProcessRescaleEvent(ev);
}


void Window::onRescaleEvent(const RescaleEvent& ev)
{
    Base::onRescaleEvent(ev);
}


void Window::onRescale(double scaling)
{   
    Base::onRescale(scaling);

    //
    // realign geometry
    //
    bool isInitialized = parent() != 0;
    if(isInitialized)
    {
        if(_state == WindowState::Normal)
        {
            move(_requestedPosition);
            resize(_requestedSize);
        }
    }
}


bool Window::isClosed() const
{
    return _isClosed;
}


void Window::close()
{
    if(_impl)
      _impl->onClose(*this);
}


void Window::onProcessCloseEvent(const CloseEvent& ev)
{
    onCloseEvent(ev);
}


void Window::onCloseEvent(const CloseEvent& ev)
{
    ShowEvent sev(*this, false);
    Application::instance().processEvent(sev);
    
    unparent();
    
    _isClosed = true;
}


Window::Type Window::type() const
{
    return _type;
}


const Gfx::Image& Window::icon() const
{
    return _icon;
}


void Window::setIcon(const Gfx::Image& icon)
{
    _icon = icon;

    if(_impl)
        _impl->onSetIcon(*this, _icon);
}


const std::string& Window::title() const
{
    return _title;
}


void Window::setTitle(const std::string& t)
{
    _title = t;

    if(_impl)
        _impl->onSetTitle(*this, _title);
}


bool Window::isAbove() const
{
    return _isAbove;
}


void Window::setAbove(bool above)
{
    _isAbove = above;

    if(_parent)
        _parent->onSetAbove(*this, above);
}


WindowState Window::state() const
{   
    return _state;
}


void Window::setState(const WindowState& s)
{
    _state = s;

    if(_impl)
        _impl->onSetState(*this, _state);
}


void Window::onProcessWindowStateEvent(const WindowStateEvent& ev)
{
    onWindowStateEvent(ev);
}


void Window::onWindowStateEvent(const WindowStateEvent& ev)
{
    _state = ev.state();
}


WindowImpl* Window::impl()
{
    return _impl;   
}


const WindowImpl* Window::impl() const
{
    return _impl;
}


void Window::onProcessMouseEvent(const MouseEvent& ev)
{
    if( ! acceptsInput() )
        return;

    Base::onProcessMouseEvent(ev);
}


void Window::onProcessTouchEvent(const TouchEvent& ev)
{
    if( ! acceptsInput() )
        return;

    Base::onProcessTouchEvent(ev);
}


void Window::onProcessScrollEvent(const ScrollEvent& ev)
{
    if( ! acceptsInput() )
        return;
  
    Base::onProcessScrollEvent(ev);
}


void Window::onProcessEnterEvent(const EnterEvent& ev)
{
    Base::onProcessEnterEvent(ev);
}


void Window::onProcessLeaveEvent(const LeaveEvent& ev)
{
    Base::onProcessLeaveEvent(ev);
}


void Window::onProcessKeyEvent(const KeyEvent& ev)
{
    if( ! acceptsInput() )
        return;
    
    Base::onProcessKeyEvent(ev);
}


bool Window::onMouseEvent(const MouseEvent& ev)
{
    return Base::onMouseEvent(ev);
}


bool Window::onTouchEvent(const TouchEvent& ev)
{ 
    return Base::onTouchEvent(ev);
}


bool Window::onScrollEvent(const ScrollEvent& ev)
{
    return Base::onScrollEvent(ev);
}


bool Window::onKeyEvent(const KeyEvent& ev)
{
    return Base::onKeyEvent(ev);
}


bool Window::onEnterEvent(const EnterEvent& ev)
{
    //std::clog << "ENTER: " << _title << " " << vid() << std::endl;
    Application::instance().setCursor( &Cursor::defaultCursor() );
    return Base::onEnterEvent(ev);
}


bool Window::onLeaveEvent(const LeaveEvent& ev )
{
    //std::clog << "LEAVE: " << _title << " " << vid() << std::endl;
    return Base::onLeaveEvent(ev);
}

} // namespace

} // namespace
