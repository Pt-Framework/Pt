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

WindowImpl::WindowImpl(WindowType type) 
: _type(type)
{
}


WindowImpl::~WindowImpl()
{
}


WindowType WindowImpl::type() const
{
    return _type;
}

///////////////////////////////////////////////////////////////////////
// Window
///////////////////////////////////////////////////////////////////////

Window::Window(WindowManager* parent, WindowType type)
: _impl(0)
, _nextResponder(0)
, _parent(0)
, _show(false)
, _isActive(false)
, _enabled(true)
, _isClosed(false)
, _requestedPosition(0, 0)
, _requestedSize(80, 80)
, _type(type)
, _minimumSize(0, 0)
, _maximumSize(64000, 64000)
, _state(WindowState::Normal)
, _isAbove(false)
{
    _form.setParent(this);

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

    parent.onAttach(*this);
    _parent = &parent;

    _impl = _parent->onCreateWindow(_type);
   
    _parent->onInit(*this);
    _parent->onSetSizeLimits(*this, _minimumSize, _maximumSize);
    _parent->onSetState(*this, _state);
    _parent->onSetTitle(*this, _title);
    _parent->onSetIcon(*this, _icon);
    _parent->onSetAbove(*this, _isAbove);
    _parent->onMove(*this, _requestedPosition);
    _parent->onResize( *this, _requestedSize);
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
    return _surface.toImage();
}


PixmapSurface& Window::surface()
{
    return _surface;
}


const PixmapSurface& Window::surface() const
{
    return _surface;
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


void Window::setNextResponder(Responder* r)
{
    _nextResponder = r;
}


Responder* Window::onNextResponder()
{
    return _nextResponder;
}

///////////////////////////////////////////////////////////////////////
// geometry
///////////////////////////////////////////////////////////////////////

void Window::onRequestMove(const Gfx::PointF& pos)
{
    _requestedPosition = pos;

    if(_parent)
    {
        _parent->onMove(*this, _requestedPosition);
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


void Window::onRequestResize(const Gfx::SizeF& s)
{
    _requestedSize = s;
    
    if(_parent)
    {
        _parent->onResize(*this, _requestedSize);
    }
}


Gfx::SizeF Window::resizeToFit(const SizePolicy& policy)
{
    //
    // TODO: remove delayed initialization
    //
    if( ! _parent )
    {
        Screen& screen = Application::instance().screen();
        screen.addWindow(*this);
    }
    
    Gfx::SizeF size = _form.measure(policy);
    resize(size);
    return size;
}


void Window::onProcessResizeEvent(const ResizeEvent& ev)
{
    Base::onProcessResizeEvent(ev);
}


void Window::onResizeEvent(const ResizeEvent& ev)
{
    Base::onResizeEvent(ev);

    _surface.resize( ev.size() );
    _form.resize( ev.size() );
}

///////////////////////////////////////////////////////////////////////
// Visual
///////////////////////////////////////////////////////////////////////

Visual* Window::onHitTest(const Gfx::PointF& p)
{
    Gfx::PointF pos = toForm(_form, p);
    Visual* hit = _form.hitTest(pos);
    if(hit)
        return hit;

    if( bounds().contains(p) )
        return this;

    return 0;
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
// Sheet
///////////////////////////////////////////////////////////////////////

void Window::onAttach(Form& form)
{
    Sheet::onAttach(form);
}

    
void Window::onDetach(Form& form)
{
    Sheet::onDetach(form);
}


void Window::onInit(Form& form)
{
    form.setSurface(&_surface);
    form.setNextResponder(this);

    double scaling = scaleFactor();
    
    RescaleEvent ev(form, scaling);
    form.processEvent(ev);
}


void Window::onRelease(Form& form)
{
    form.setSurface(0);
    form.setNextResponder(0);
}


Gfx::PointF Window::onFromForm(const Form& form, const Gfx::PointF& pos) const
{
    return pos;
}


Gfx::PointF Window::onToForm(const Form& form, const Gfx::PointF& pos) const
{
    return pos;
}


void Window::onRepaint(Form& form, const Gfx::RectF& rect)
{
    Gfx::PointF clientPos = onFromForm( form, rect.topLeft() );
    Gfx::RectF clientRect( clientPos, rect.size() );

    repaint(clientRect);
}


void Window::onActivate(Form& form, bool active)
{
}


void Window::onShowRequest(Form& form, bool isShow)
{
    ShowEvent ev(form, isShow);
    Application::instance().commitEvent(ev);
}


void Window::onMove(Form& form, const Gfx::PointF& pos)
{   
    //
    // align to physical pixel grid
    //
    Gfx::PointF aligedPos = _surface.align(pos);

    //
    // send move event
    //
    MoveEvent mev(form, aligedPos);
    Application::instance().commitEvent(mev);

    //
    // request repaint
    //
    Gfx::RectF updateRect( form.position(), form.size() );
    Gfx::RectF movedRect( aligedPos, form.size() );
    updateRect.unify(movedRect);

    repaint(updateRect);
}


void Window::onResize(Form& form, const Gfx::SizeF& size)
{
    //
    // align to physical pixel grid
    //
    Gfx::SizeF alignedSize = _surface.align(size);

    //
    // send resize event
    //
    ResizeEvent rev(form, alignedSize);
    Application::instance().commitEvent(rev);

    //
    // request repaint
    //
    Gfx::RectF updateRect( form.position(), form.size() );
    Gfx::RectF resizedRect( form.position(), alignedSize );
    updateRect.unify(resizedRect);

    repaint(updateRect);
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

    if(_parent)
        _parent->onRepaint(*this, rect);
}


void Window::onProcessPaintEvent(const PaintEvent& ev)
{
    const Gfx::RectF& rect = ev.rect();
    if( rect.isNull() )
        return;

    if( ! this->isVisible() )
        return;

    Base::onProcessPaintEvent(ev);

    Gfx::RectF updateRect = bounds().intersect(rect);

    if( ! updateRect.isNull() )
    {
        PaintEvent pev( *this, updateRect );
        _form.processEvent(pev);
    }
}


void Window::onPaintEvent(const PaintEvent& ev)
{    
    //static int nnn = 0;
    //std::clog << "PAINT EVENT: " << typeid(*this).name() << " " << ++nnn << std::endl;

    Base::onPaintEvent(ev);

    onPaint( _surface, ev.rect() );
}


void Window::onPaint(Gfx::PaintSurface& surface, const Gfx::RectF& rect)
{
    Gfx::Painter painter(surface);
    painter.setBrush(_backgroundBrush);
    painter.fillRect(rect);
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


void Window::activate(bool active)
{
    _isActive = active;

    if( _parent )
        _parent->onActivate(*this, active);
    else
        _isActive = active;
}


void Window::onProcessActivateEvent(const ActivateEvent& ev)
{
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

    EnableEvent eev(*this, isEnabled);
    Base::onProcessEnableEvent(ev);

    EnableEvent formEvent(_form, isEnabled);
    _form.processEvent(formEvent);
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
    double scaling = ev.scaleFactor();

    if(_impl)
        scaling *= _impl->scaleFactor();

    RescaleEvent rev(*this, scaling);
    Base::onProcessRescaleEvent(rev);

    _form.processEvent(rev);
}


void Window::onRescaleEvent(const RescaleEvent& ev)
{
    Base::onRescaleEvent(ev);
}


void Window::onRescale(double scaling)
{   
    Base::onRescale(scaling);

    //std::clog << "+W RESCALE EVENT: " << title() << " "
    //          << _surface.scaleFactor() << std::endl;

    _surface.setScaleFactor(scaling);

    //
    // realign geometry
    //
    move(_requestedPosition);
    resize(_requestedSize);
}


bool Window::isClosed() const
{
    return _isClosed;
}


void Window::tryClose()
{
    close();
}


void Window::close(bool force)
{
    if(force)
    {
        show(false);
        
        unparent();
        _isClosed = true;
    }

    if(_parent)
        _parent->onClosing(*this);
}


void Window::onProcessCloseEvent(const CloseEvent& ev)
{
    onCloseEvent(ev);
}


void Window::onCloseEvent(const CloseEvent& ev)
{
    show(false);
    
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

    if(_parent)
        _parent->onSetIcon(*this, _icon);
}


const std::string& Window::title() const
{
    return _title;
}


void Window::setTitle(const std::string& t)
{
    _title = t;

    if(_parent)
        _parent->onSetTitle(*this, _title);
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


const Gfx::SizeF& Window::minimumSize() const
{
    return _minimumSize;
}


void Window::setMinimumSize(const Gfx::SizeF& s)
{
    _minimumSize = s;

    if(_parent)
        _parent->onSetSizeLimits(*this, _minimumSize, _maximumSize);
}


void Window::setMinimumWidth(double w)
{
    Gfx::SizeF s = _minimumSize;
    s.setWidth(w);
    setMinimumSize(s);
}


void Window::setMinimumHeight(double h)
{
    Gfx::SizeF s = _minimumSize;
    s.setHeight(h);
    setMinimumSize(s);
}


const Gfx::SizeF& Window::maximumSize() const
{
    return _maximumSize;
}


void Window::setMaximumSize(const Gfx::SizeF& s)
{
    _maximumSize = s;

    if(_parent)
        _parent->onSetSizeLimits(*this, _minimumSize, _maximumSize);
}


void Window::setMaximumWidth(double w)
{
    Gfx::SizeF s = _maximumSize;
    s.setWidth(w);
    setMaximumSize(s);
}


void Window::setMaximumHeight(double h)
{
    Gfx::SizeF s = _maximumSize;
    s.setHeight(h);
    setMaximumSize(s);
}


WindowState Window::state() const
{   
    return _state;
}


void Window::setState(const WindowState& s)
{
    _state = s;

    if(_parent)
        _parent->onSetState(*this, _state);
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

    _form.processEvent(ev);
}


void Window::onProcessTouchEvent(const TouchEvent& ev)
{
    if( ! acceptsInput() )
        return;

    _form.processEvent(ev);
}


void Window::onProcessScrollEvent(const ScrollEvent& ev)
{
    if( ! acceptsInput() )
        return;
  
    _form.processEvent(ev);
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
    
    KeyEvent kev = ev;
    kev.setVisual(&_form);
    _form.processEvent(kev);
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
