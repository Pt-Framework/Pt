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
, _state(WindowState::Normal)
, _minimumSize(0, 0)
, _maximumSize(64000, 64000)
{
}


WindowImpl::~WindowImpl()
{
}


WindowType WindowImpl::type() const
{
    return _type;
}


void WindowImpl::setType(WindowType type)
{
    onSetType(type);
    _type = type;
}


WindowState WindowImpl::state() const
{
    return _state;
}


void WindowImpl::setState(WindowState s)
{
    onSetState(s);
    _state = s;
}


const Gfx::SizeF& WindowImpl::minimumSize() const
{
    return _minimumSize;;
}

     
void WindowImpl::setMinimumSize(const Gfx::SizeF& s)
{
    onSetMinimumSize(s);
    _minimumSize = s;
}


const Gfx::SizeF& WindowImpl::maximumSize() const
{
    return _maximumSize;
}


void WindowImpl::setMaximumSize(const Gfx::SizeF& s)
{
    onSetMaximumSize(s);
    _maximumSize = s;
}

///////////////////////////////////////////////////////////////////////
// Window
///////////////////////////////////////////////////////////////////////

Window::Window(WindowManager* parent, WindowType type)
: _impl(0)
, _parent(0)
, _nextResponder(0)
, _capture(0)
, _invalidates(0)
, _visible(false)
, _isActive(false)
, _enabled(true)
, _enabledState(true)
, _isClosed(false)
, _requestedPosition(0, 0)
, _requestedSize(80, 80)
, _geometry(_requestedPosition, _requestedSize)
, _type(type)
, _minimumSize(0, 0)
, _maximumSize(64000, 64000)
, _state(WindowState::Normal)
, _isAbove(false)
{
    _sheet.setParent(this);

    _eventReceived += Pt::slot(*this, &Window::onProcessMouseEvent);
    _eventReceived += Pt::slot(*this, &Window::onProcessTouchEvent);
    _eventReceived += Pt::slot(*this, &Window::onProcessScrollEvent);
    _eventReceived += Pt::slot(*this, &Window::onProcessEnterEvent);
    _eventReceived += Pt::slot(*this, &Window::onProcessLeaveEvent);
    _eventReceived += Pt::slot(*this, &Window::onProcessKeyEvent);

    _eventReceived += Pt::slot(*this, &Window::onProcessInvalidateEvent);
    _eventReceived += Pt::slot(*this, &Window::onProcessPaintEvent);
    _eventReceived += Pt::slot(*this, &Window::onProcessRescaleEvent);
    _eventReceived += Pt::slot(*this, &Window::onProcessMoveEvent);
    _eventReceived += Pt::slot(*this, &Window::onProcessResizeEvent);
    _eventReceived += Pt::slot(*this, &Window::onProcessShowEvent);
    _eventReceived += Pt::slot(*this, &Window::onProcessEnableEvent);

    _eventReceived += Pt::slot(*this, &Window::onProcessActivateEvent);
    _eventReceived += Pt::slot(*this, &Window::onProcessCloseEvent);
    _eventReceived += Pt::slot(*this, &Window::onProcessWindowStateEvent);

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

    if(_impl)
    {
        _impl->setState(_state);
        _impl->setMinimumSize(_minimumSize);
        _impl->setMaximumSize(_maximumSize);
    }

    _parent->onSetTitle(*this, _title);
    _parent->onSetIcon(*this, _icon);
    _parent->onSetAbove(*this, _isAbove);
    _parent->onMove(*this, _requestedPosition);
    _parent->onResize(*this, _requestedSize);
    _parent->onActivate(*this, _isActive);
    _parent->onEnable(*this, _enabled);
    _parent->onStateChanged(*this);
    _parent->onShow(*this, _visible);
    
    onParentChanged(_parent);
}


void Window::unparent()
{
    if( ! _parent )
        return;

    release();

    _parent->onRelease(*this);
    _parent->onDetach(*this);
    _parent = 0;

    delete _impl;
    _impl = 0;
        
    onParentChanged(0);
}


void Window::onRelease()
{
    setPointer(false);
    setCapture(false);

    _sheet.release();
}


void Window::onParentChanged(WindowManager* )
{
}


PixmapSurface& Window::surface()
{
    return _surface;
}


const PixmapSurface& Window::surface() const
{
    return _surface;
}


void Window::setNextResponder(Responder* r)
{
    _nextResponder = r;
}


double Window::scaleFactor() const
{
    return _surface.scaleFactor();
}


const Gfx::PointF& Window::position() const
{
    return _geometry.topLeft();
}


const Gfx::SizeF& Window::size() const
{
    return _geometry.size();
}


const Gfx::RectF& Window::geometry() const
{
    return _geometry;
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


Pt::Signal<const Pt::Event&>& Window::eventReceived()
{
    return _eventReceived;
}

///////////////////////////////////////////////////////////////////////
// Responder
///////////////////////////////////////////////////////////////////////

Responder* Window::onNextResponder()
{
    return _nextResponder;
}

///////////////////////////////////////////////////////////////////////
// Visual
///////////////////////////////////////////////////////////////////////

Visual* Window::onGetParent() const
{
    if( ! _parent )
        return 0;

    return &_parent->visual();
}


Visual* Window::onHitTest(const Gfx::PointF& p)
{
    Gfx::PointF pos = toSheet(_sheet, p);
    Visual* hit = _sheet.hitTest(pos);
    if(hit)
        return hit;

    Gfx::RectF bounds( size() );
    if( bounds.contains(p) )
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


void Window::onEvent(const Pt::Event& ev)
{
    _eventReceived.send(ev);
}

///////////////////////////////////////////////////////////////////////
// Form
///////////////////////////////////////////////////////////////////////

Visual& Window::onGetVisual()
{
    return *this;
}


void Window::onAttach(Sheet& sheet)
{
    Form::onAttach(sheet);
}

    
void Window::onDetach(Sheet& sheet)
{
    if(_capture == &sheet)
        _capture = 0;

    Form::onDetach(sheet);
}


void Window::onInit(Sheet& sheet)
{
    sheet.setSurface(&_surface);
    sheet.setNextResponder(this);

    double scaling = _surface.scaleFactor();
    
    RescaleEvent ev(sheet, scaling);
    //w.processEvent(ev);
    Application::instance().loop().commitEvent(ev);
}


void Window::onRelease(Sheet& sheet)
{
    sheet.setSurface(0);
    sheet.setNextResponder(0);
}


Gfx::PointF Window::onFromSheet(const Sheet& sheet, const Gfx::PointF& pos) const
{
    return pos;
}


Gfx::PointF Window::onToSheet(const Sheet& sheet,  const Gfx::PointF& pos) const
{
    return pos;
}


void Window::onRepaint(Sheet& s, const Gfx::RectF& rect)
{
    Gfx::PointF clientPos = onFromSheet( s, rect.topLeft() );
    Gfx::RectF clientRect( clientPos, rect.size() );

    repaint(clientRect);
}


void Window::onActivate(Sheet& w, bool active)
{
}


void Window::onMove(Sheet& sheet, const Gfx::PointF& pos)
{   
    //
    // align to physical pixel grid
    //
    Gfx::PointF aligedPos = _surface.align(pos);

    //
    // send move event
    //
    MoveEvent mev(sheet, aligedPos);
    ////Application::instance().processEvent(mev);
    Application::instance().commitEvent(mev);

    //
    // request repaint
    //
    Gfx::RectF updateRect = sheet.geometry();
    Gfx::RectF movedRect( aligedPos, sheet.size() );
    updateRect.unify(movedRect);

    repaint(updateRect);
}


void Window::onResize(Sheet& sheet, const Gfx::SizeF& size)
{
    //
    // align to physical pixel grid
    //
    Gfx::SizeF alignedSize = _surface.align(size);

    //
    // send resize event
    //
    ResizeEvent rev(sheet, alignedSize);
    ////Application::instance().processEvent(rev);
    Application::instance().commitEvent(rev);

    //
    // request repaint
    //
    Gfx::RectF updateRect = sheet.geometry();
    Gfx::RectF resizedRect( sheet.position(), alignedSize );
    updateRect.unify(resizedRect);

    repaint(updateRect);
}

///////////////////////////////////////////////////////////////////////
// Implementation
///////////////////////////////////////////////////////////////////////

void Window::invalidate()
{
    ++_invalidates;

    InvalidateEvent ev(*this);
    Application::instance().commitEvent(ev);
}


void Window::onProcessInvalidateEvent(const InvalidateEvent& ev)
{
    --_invalidates;

    if(_invalidates > 0)
      return;

    onInvalidateEvent(ev);
}


void Window::onInvalidateEvent(const InvalidateEvent& ev)
{
    onInvalidate();
}


void Window::onInvalidate()
{
    _backgroundBrush = background();

    repaint();
}


void Window::repaint()
{
    Gfx::RectF rect( size() );
    repaint(rect);
}


void Window::repaint(const Gfx::RectF& rect)
{
    //std::clog << "REPAINT: " << title() << std::endl;

    _damageRect.unify(rect);

    if(_parent)
        _parent->onRepaint(*this, rect);
}


void Window::onProcessPaintEvent(const PaintEvent& ev)
{
    //TODO: does _damageRect work if PaintEvent is generated by
    //      the platform without repaint() being called?

    const Gfx::RectF& rect = _damageRect;
    //const Gfx::RectF& rect = ev.rect();

    //std::clog << "  PAINT(" << title() << "): " 
    //          << rect.width() << "x" << rect.height() << std::endl;

    if( rect.isNull() )
        return;

    if( ! this->isVisible() )
        return;

    //
    // paint window
    //
    PaintEvent pev(*this, rect);
    onPaintEvent(pev);

    //
    // paint sheet
    //
    Gfx::RectF updateRect = _sheet.geometry().intersect(rect);
 
    if( ! updateRect.isNull() )
    {
        PaintEvent pev( _sheet, updateRect );
        _sheet.processEvent(pev);
    }

    _damageRect.clear();
}


void Window::onPaintEvent(const PaintEvent& ev)
{    
    const Gfx::RectF& rect = ev.rect();
    onPaint(_surface, rect);
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


bool Window::isVisible() const
{
    return _visible;
}


void Window::show(bool b)
{
    //if( _title == "Main_1")
    //std::clog << "SHOW: " << title() << " init: " << (_parent != 0) << std::endl;
    
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
    onShowEvent(ev);
}


void Window::onShowEvent(const ShowEvent& ev)
{
    _visible = ev.visible();

    if(_capture && ! _visible)
    {
        _capture->setCapture(false);
    }
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


bool Window::isEnabled() const
{
    return _enabledState && _enabled;
}


void Window::enable(bool e)
{
    _enabled = e;

    if( ! _parent )
    {
        _enabledState = e;
        return;
    }

    _parent->onEnable(*this, e);
}


void Window::onProcessEnableEvent(const EnableEvent& ev)
{
    bool wasEnabled = isEnabled();

    _enabledState = ev.enabled();

    if( wasEnabled != isEnabled() )
    {
        onEnableEvent(ev);
    }

    bool enable = ev.enabled();
    if( ! isEnabled() )
      enable = false;

    EnableEvent sheetEvent(_sheet, enable);
    Application::instance().loop().commitEvent(sheetEvent);

    invalidate();
}


void Window::onEnableEvent(const EnableEvent& ev)
{        
    onEnable( ev.enabled() );
}


void Window::onEnable(bool e)
{
}


void Window::onProcessRescaleEvent(const RescaleEvent& ev)
{
    onRescaleEvent(ev);

    double scaling = _surface.scaleFactor();

    RescaleEvent sheetEvent(_sheet, scaling);
    _sheet.processEvent(sheetEvent);
}


void Window::onRescaleEvent(const RescaleEvent& ev)
{
    onRescale( ev.scaleFactor() );
}


void Window::onRescale(double scaling)
{   
    if(_impl)
        scaling *= _impl->scaleFactor();

    _surface.setScaleFactor(scaling);

    //if( _title == "Main_1")
    //std::clog << "+W RESCALE EVENT: " << this->title() << " "
    //          << _surface.scaleFactor() << std::endl;

    // realign geometry
    move(_requestedPosition);
    resize(_requestedSize);
}


void Window::move(const Gfx::PointF& pos)
{
    _requestedPosition = pos;

    if(_parent)
    {
        //Gfx::PointF aligedPos = _surface.align(pos);
        Gfx::PointF aligedPos = pos;
        _parent->onMove(*this, aligedPos);
    }
    else
        _geometry.setOrigin(_requestedPosition);
}


void Window::onProcessMoveEvent(const MoveEvent& ev)
{
    onMoveEvent(ev);
}


void Window::onMoveEvent(const MoveEvent& ev)
{    
    //std::clog << "MOVE EVENT: " << this->title() << " "
    //          << ev.position().x() << ", " << ev.position().y() << std::endl;
    
    _geometry.setOrigin( ev.position() );
}


void Window::resize(const Gfx::SizeF& s)
{
    _requestedSize = s;
    
    if(_parent)
    {
        //Gfx::SizeF alignedSize = _surface.align(s);
        Gfx::SizeF alignedSize = s;
        _parent->onResize(*this, alignedSize);
    }
    else
        _geometry.setSize(_requestedSize);
}


void Window::resize(const SizePolicy& policy)
{
    Gfx::SizeF size = _sheet.measure(policy);
    ///Gfx::SizeF size = _form.measure(policy);

    resize(size);
}


void Window::onProcessResizeEvent(const ResizeEvent& ev)
{
    onResizeEvent(ev);
}


void Window::onResizeEvent(const ResizeEvent& ev)
{
    //if( _size == ev.size() )
    //    return;

    //if( _title == "Main_1")
    //std::clog << "W RESIZE EVENT: " << this->title() << " "
    //          << ev.size().width() << "x" << ev.size().height() << std::endl;

    _geometry.setSize( ev.size() );
    _surface.resize( ev.size() );
    _sheet.resize( ev.size() );
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
        _visible = false;
        
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
    _visible = false;
    
    unparent();
    _isClosed = true;
}


Window::Type Window::type() const
{
    return _type;
}


//void Window::setType(Type type)
//{
//    if( _impl )
//        _impl->setType(type);
//
//    _type = type;
//
//    if(_parent)
//        _parent->onFrameChanged(*this);
//}


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
    if( _impl ) 
        _impl->setMinimumSize(s);

    _minimumSize = s;

    // TODO: notify parent?
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
    if( _impl )
        _impl->setMaximumSize(s);

    _maximumSize = s;

    // TODO: notify parent?
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


Window::State Window::state() const
{   
    return _state;
}


void Window::setState(Window::State s)
{
    if( _impl )
        _impl->setState(s);

    _state = s;

    if(_parent)
        _parent->onStateChanged(*this);
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

    Gfx::PointF pos = fromGlobal( ev.position() );

    //std::clog << title() << ": " << pos.x() << " " << pos.y() << std::endl;

    Visual* hit = 0;

    Sheet* sheet = this->sheet();
    if(sheet && 
       sheet->geometry().contains(pos) && 
       sheet->acceptsInput() )
    {
        hit = sheet;
    }

    if(hit)
    {
      hit->processEvent(ev);
      return;
    }

    _sheet.processEvent(ev);
}


void Window::onProcessTouchEvent(const TouchEvent& ev)
{
    if( ! acceptsInput() )
        return;

    Gfx::PointF pos = fromGlobal( ev.position() );

    //std::clog << title() << ": " << pos.x() << " " << pos.y() << std::endl;

    //
    // continue press sequence capture
    //
    if(_capture)
    {
        _capture->processEvent(ev);

        if( ev.isRelease() )
        {
            //std::clog << "Widget::CAPTURE END: " << typeid(*_capture).name() << std::endl;
            _capture = 0;
        }
        
        return;
    }

    Visual* hit = sheet();
    if(hit)
    {
        //
        // start press sequence capture
        // 
        if( ev.isPress() )
        {
            _capture = hit;
            //std::clog << "Window::CAPTURE BEGIN: " << typeid(*_capture).name() << std::endl;
        }

      hit->processEvent(ev);
      return;
    }
    
    _sheet.processEvent(ev);
}


void Window::onProcessScrollEvent(const ScrollEvent& ev)
{
    if( ! acceptsInput() )
        return;
  
    _sheet.processEvent(ev);
}


void Window::onProcessEnterEvent(const EnterEvent& ev)
{
    enterEvent(ev);
}


void Window::onProcessLeaveEvent(const LeaveEvent& ev)
{
    leaveEvent(ev);
}


void Window::onProcessKeyEvent(const KeyEvent& ev)
{
    if( ! acceptsInput() )
        return;
    
    KeyEvent kev = ev;
    kev.setVisual(&_sheet);
    _sheet.processEvent(kev);
}


bool Window::onMouseEvent(const MouseEvent& ev)
{
    if(ev.isPress(MouseEvent::Left) )
        Application::instance().inputMethod().finish();

    return false;
}


bool Window::onTouchEvent(const TouchEvent& ev)
{ 
    if(ev.isPress() )
        Application::instance().inputMethod().finish();

    return false;
}


bool Window::onScrollEvent(const ScrollEvent& ev)
{
    return false;
}


bool Window::onKeyEvent(const KeyEvent& ev)
{
    return false;
}


bool Window::onEnterEvent(const EnterEvent& ev)
{
    //std::clog << "ENTER: " << _title << " " << vid() << std::endl;
    Application::instance().setCursor(0);
    return true;
}


bool Window::onLeaveEvent(const LeaveEvent& ev )
{
    //std::clog << "LEAVE: " << _title << " " << vid() << std::endl;
    Application::instance().setCursor(0);
    return true;
}

} // namespace

} // namespace
