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
, _visible(false)
, _isActive(false)
, _enabled(true)
, _isClosed(false)
, _requestedPosition(0, 0)
, _requestedSize(80, 80)
//, _geometry(_requestedPosition, _requestedSize)
, _type(type)
, _minimumSize(0, 0)
, _maximumSize(64000, 64000)
, _state(WindowState::Normal)
, _isAbove(false)
{
    //Form::setSurface(&_surface);
    _form.setParent(this);

    _eventReceived += Pt::slot(*this, &Window::onProcessShowEvent);
    _eventReceived += Pt::slot(*this, &Window::onProcessActivateEvent);
    _eventReceived += Pt::slot(*this, &Window::onProcessCloseEvent);
    _eventReceived += Pt::slot(*this, &Window::onProcessWindowStateEvent);

    _eventReceived += Pt::slot(*this, &Window::onProcessMouseEvent);
    _eventReceived += Pt::slot(*this, &Window::onProcessTouchEvent);
    _eventReceived += Pt::slot(*this, &Window::onProcessScrollEvent);
    _eventReceived += Pt::slot(*this, &Window::onProcessEnterEvent);
    _eventReceived += Pt::slot(*this, &Window::onProcessLeaveEvent);
    _eventReceived += Pt::slot(*this, &Window::onProcessKeyEvent);

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
    _parent->onShow(*this, _visible);
    
    onParentChanged(_parent);
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
        
    onParentChanged(0);
}


void Window::onParentChanged(WindowManager* )
{
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


//double Window::scaleFactor() const
//{
//    return _surface.scaleFactor();
//}


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

//const Gfx::RectF& Window::geometry() const
//{
//    return _geometry;
//}


//const Gfx::PointF& Window::position() const
//{
//    return _geometry.topLeft();
//}


//void Window::onProcessMoveEvent(const MoveEvent& ev)
//{
//    onMoveEvent(ev);
//}
//
//
//void Window::onMoveEvent(const MoveEvent& ev)
//{    
//    //std::clog << "MOVE EVENT: " << this->title() << " "
//    //          << ev.position().x() << ", " << ev.position().y() << std::endl;
//    
//    _geometry.setOrigin( ev.position() );
//}


void Window::move(const Gfx::PointF& pos)
{
    _requestedPosition = pos;

    if(_parent)
    {
        _parent->onMove(*this, _requestedPosition);
    }
    //else
    //{
    //    _geometry.setOrigin(_requestedPosition);
    //}
}


void Window::onProcessMoveEvent(const MoveEvent& ev)
{
    Base::onProcessMoveEvent(ev);
}


void Window::onMoveEvent(const MoveEvent& ev)
{    
    Base::onMoveEvent(ev);
}


void Window::resize(const Gfx::SizeF& s)
{
    _requestedSize = s;
    
    if(_parent)
    {
        _parent->onResize(*this, _requestedSize);
    }
    //else
    //{
    //    _geometry.setSize(_requestedSize);
    //}
}


Gfx::SizeF Window::resize(const SizePolicy& policy)
{
    //Gfx::SizeF size = Form::measure(policy);
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


//const Gfx::SizeF& Window::size() const
//{
//    return _geometry.size();
//}


//void Window::onProcessResizeEvent(const ResizeEvent& ev)
//{
//    onResizeEvent(ev);
//}


//void Window::onResizeEvent(const ResizeEvent& ev)
//{
//    //if( _size == ev.size() )
//    //    return;
//
//    //if( _title == "Main_1")
//    //std::clog << "W RESIZE EVENT: " << this->title() << " "
//    //          << ev.size().width() << "x" << ev.size().height() << std::endl;
//
//    _geometry.setSize( ev.size() );
//    _surface.resize( ev.size() );
//    //_sheet.resize( ev.size() );
//
//    relayout();
//}


///////////////////////////////////////////////////////////////////////
// Visual
///////////////////////////////////////////////////////////////////////

Visual* Window::onGetParent() const
{
    return _parent;
}


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


void Window::onEvent(const Pt::Event& ev)
{
    Base::onEvent(ev);
    _eventReceived.send(ev);
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
    //Application::instance().loop().commitEvent(ev);
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
    ////Application::instance().processEvent(mev);
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
    ////Application::instance().processEvent(rev);
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


void Window::onRepaintRequest(const Gfx::RectF& rect)
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

    Gfx::RectF updateRect = bounds().intersect( ev.rect() );
    onPaint(_surface, updateRect);
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


//bool Window::isEnabled() const
//{
//    return _enabledState && _enabled;
//}


void Window::enable(bool e)
{
    _enabled = e;

    if(_parent)
        _parent->onEnableRequest(*this, e);
}


void Window::onProcessEnableEvent(const EnableEvent& ev)
{
    Base::onProcessEnableEvent(ev);

    EnableEvent eev(_form, ev.enabled());
    _form.processEvent(eev);
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

    //if( _title == "Main_1")
    //std::clog << "+W RESCALE EVENT: " << this->title() << " "
    //          << _surface.scaleFactor() << std::endl;

    _surface.setScaleFactor(scaling);

    // realign geometry
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


//void Window::onProcessMouseEvent(const MouseEvent& ev)
//{
//    if( ! acceptsInput() )
//        return;
//
//    Gfx::PointF pos = fromGlobal( ev.position() );
//
//    //std::clog << title() << ": " << pos.x() << " " << pos.y() << std::endl;
//
//    Visual* hit = 0;
//
//    Sheet* sheet = this->sheet();
//    if(sheet && 
//       sheet->geometry().contains(pos) && 
//       sheet->acceptsInput() )
//    {
//        hit = sheet;
//    }
//
//    if(hit)
//    {
//      hit->processEvent(ev);
//      return;
//    }
//
//    _sheet.processEvent(ev);
//}


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
    kev.setVisual(&_form);
    _form.processEvent(kev);
}


bool Window::onMouseEvent(const MouseEvent& ev)
{
    //if(ev.isPress(MouseEvent::Left) )
    //{
    //    InputMethod& ime = Application::instance().inputMethod();
    //    if( this != ime.activeWindow() )
    //    {
    //        Application::instance().inputMethod().finish();
    //    }
    //}

    return Base::onMouseEvent(ev);
}


bool Window::onTouchEvent(const TouchEvent& ev)
{ 
    //if(ev.isPress() )
    //    Application::instance().inputMethod().finish();

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
