/* Copyright (C) 2015-2024 Marc Boris Duerner

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

#include <Pt/Forms/Widget.h>
#include <Pt/Forms/Application.h>
#include <Pt/Forms/MouseEvent.h>
#include <Pt/Forms/TouchEvent.h>
#include <Pt/Forms/ScrollEvent.h>
#include <Pt/Forms/EnterEvent.h>
#include <Pt/Forms/LeaveEvent.h>
#include <Pt/Forms/KeyEvent.h>
#include <Pt/Forms/MoveEvent.h>
#include <Pt/Forms/ResizeEvent.h>
#include <Pt/Forms/InvalidateEvent.h>
#include <Pt/Forms/PaintEvent.h>
#include <Pt/Forms/LayoutEvent.h>
#include <Pt/Forms/ShowEvent.h>
#include <Pt/Forms/EnableEvent.h>

namespace Pt {

namespace Forms {

Widget::Widget()
: _id( Application::instance().makeId()  )
, _screen(0)
, _parent(0)
, _nextResponder(0)
, _invalidates(0)
, _enabledState(true)
, _isVisible(false)
, _minimumSize(0, 0)
, _maximumSize(64000, 64000)
, _hasCursor(false)
, _cursor()
, _r1(0)
{ 
    Application::instance().registerWidget(*this);

    _dispatcher += Pt::slot(*this, &Widget::onProcessInvalidateEvent);
    _dispatcher += Pt::slot(*this, &Widget::onProcessPaintEvent);
    _dispatcher += Pt::slot(*this, &Widget::onProcessRescaleEvent);
    _dispatcher += Pt::slot(*this, &Widget::onProcessMoveEvent);
    _dispatcher += Pt::slot(*this, &Widget::onProcessResizeEvent);
    _dispatcher += Pt::slot(*this, &Widget::onProcessEnableEvent);
    _dispatcher += Pt::slot(*this, &Widget::onProcessShowEvent);
    _dispatcher += Pt::slot(*this, &Widget::onProcessMouseEvent);
    _dispatcher += Pt::slot(*this, &Widget::onProcessTouchEvent);
    _dispatcher += Pt::slot(*this, &Widget::onProcessScrollEvent);
    _dispatcher += Pt::slot(*this, &Widget::onProcessEnterEvent);
    _dispatcher += Pt::slot(*this, &Widget::onProcessLeaveEvent);
    _dispatcher += Pt::slot(*this, &Widget::onProcessKeyEvent);
}


Widget::~Widget()
{
    setCapture(false);
    
    Application::instance().onSetPointer(*this, false);

    while( ! _peers.empty() )
    {
        removePeer( *_peers.back() );
    }

    Application::instance().unregisterWidget(*this);
}

//
// global identifier
// 

Pt::uint64_t Widget::id() const
{
    return _id;
}


const std::string& Widget::name() const
{
    return _name;
}
        

void Widget::setName(const std::string& n)
{
    _name = n;
}

//
// hierachy management
// 

Widget* Widget::parent()
{
    return _parent;
}


const Widget* Widget::parent() const
{
    return _parent;
}


void Widget::onSetParent(Widget* parent)
{
    _parent = parent;
}


bool Widget::isConnected() const
{
    return _screen != 0;
}


Screen* Widget::screen()
{
    return _screen;
}


const Screen* Widget::screen() const
{
    return _screen;
}


void Widget::onConnect(Screen& screen)
{
    _screen = &screen;
}


void Widget::onDisconnect()
{
    _screen = 0;
}


bool Widget::isDescendantOf(const Widget& widget) const
{
    const Widget* parent = this->parent();
    if( ! parent )
        return false;

    if( parent == &widget )
        return true;

    return parent->isDescendantOf(widget);
}


bool Widget::isAncestorOf(const Widget& widget) const
{
    return widget.isDescendantOf(*this);
}


Widget* Widget::hitTest(const Gfx::PointF& pos)
{
    return onHitTest(pos);
}


Widget* Widget::onHitTest(const Gfx::PointF& pos)
{
    return 0;
}


Gfx::PointF Widget::toParent(const Gfx::PointF& pos) const
{
    return onToParent(pos);
}


Gfx::PointF Widget::fromParent(const Gfx::PointF& pos) const
{
    return onFromParent(pos);
}


Gfx::PointF Widget::toGlobal(const Gfx::PointF& pos) const
{
    return onToGlobal(pos); 
}
        

Gfx::PointF Widget::fromGlobal(const Gfx::PointF& pos) const
{
    return onFromGlobal(pos);
}


Gfx::PointF Widget::onToGlobal(const Gfx::PointF& pos) const
{
    const Widget* parent = this->parent();
    if( ! parent )
        return pos;

    Gfx::PointF parentPos = toParent(pos);
    return parent->toGlobal(parentPos);
}


Gfx::PointF Widget::onFromGlobal(const Gfx::PointF& pos) const
{
    const Widget* parent = this->parent();
    if( ! parent )
        return pos;

    Gfx::PointF parentPos = parent->fromGlobal(pos);
    return fromParent(parentPos);
}

//
// peer relationship
// 

void Widget::addPeer(Widget& peer)
{
    peer.onAttachPeer(*this);
    onAttachPeer(peer);
}


void Widget::removePeer(Widget& peer)
{
    peer.onDetachPeer(*this);
    onDetachPeer(peer);
}


void Widget::onAttachPeer(Widget& peer)
{
    _peers.push_back(&peer);
}


void Widget::onDetachPeer(Widget& peer)
{
    std::vector<Widget*>::iterator it;
    it = std::find( _peers.begin(), _peers.end(), &peer );
    
    if( it != _peers.end() )
        _peers.erase(it);
}

//
// event processing
//

void Widget::processEvent(const Pt::Event& ev)
{
    onProcessEvent(ev);
}


void Widget::onProcessEvent(const Pt::Event& ev)
{
    //std::clog << typeid(ev).name() << std::endl;
    _dispatcher.send(ev);
}


Pt::Signal<const Pt::Event&>& Widget::eventReceived()
{
    return _dispatcher;
}

//
// invalidation
//

void Widget::invalidate()
{
    ++_invalidates;

    InvalidateEvent ev(*this);
    Application::instance().commitEvent(ev);
}


void Widget::onProcessInvalidateEvent(const InvalidateEvent& ev)
{
    if(_invalidates == 0)
      return;

    --_invalidates;

    if(_invalidates > 0)
      return;

    onInvalidateEvent(ev);
}


// TODO: remove and only use onInvalidateEvent
void Widget::onInvalidateEvent(const InvalidateEvent& ev)
{
    onInvalidate();
}

    
void Widget::onInvalidate()
{
    //static int nnn = 0;
    //std::clog << ++nnn << " invalidate " << name() << this << std::endl;

    // ignore pending events after direct invalidate
    _invalidates = 0;
}

//
// painting
//

void Widget::repaint(const Gfx::RectF& rect)
{
    onRequestRepaint(rect);
}


void Widget::repaint()
{
    repaint( bounds() );
}


void Widget::onRequestRepaint(const Gfx::RectF& rect)
{
}


void Widget::onProcessPaintEvent(const PaintEvent& ev)
{
    if( ev.rect().isEmpty() )
        return;

    onPaintEvent(ev);
}


void Widget::onPaintEvent(const PaintEvent& ev)
{
}

//
// scaling
//

double Widget::scaleFactor() const
{
    return _scaling.scaleFactor();
}


const Gfx::Scaling& Widget::scaling() const
{
    return _scaling;
}


void Widget::onProcessRescaleEvent(const RescaleEvent& ev)
{
    onRescaleEvent(ev);
}


void Widget::onRescaleEvent(const RescaleEvent& ev)
{
    onRescale( ev.scaleFactor() );
}


void Widget::onRescale(double scaling)
{
    _scaling.setScaleFactor(scaling);
}

//
// visibility
//

bool Widget::isVisible() const
{
    return _isVisible;
}


void Widget::show(bool isShow)
{
    onRequestShow(isShow);
}


void Widget::onRequestShow(bool e)
{
}


void Widget::onProcessShowEvent(const ShowEvent& ev)
{
    if( _isVisible == ev.visible() )
        return;

    onShowEvent(ev);
}


void Widget::onShowEvent(const ShowEvent& ev)
{
    onShow( ev.visible() );
}


// TODO: remove and only use onShowEvent
void Widget::onShow(bool isShown)
{
    _isVisible = isShown;
}

//
// enabling
//

bool Widget::isEnabled() const
{
    return _enabledState;
}


void Widget::enable(bool isEnable)
{
    onRequestEnable(isEnable);
}


void Widget::onRequestEnable(bool e)
{
}


void Widget::onProcessEnableEvent(const EnableEvent& ev)
{
    onEnableEvent(ev);
}


void Widget::onEnableEvent(const EnableEvent& ev)
{    
    _enabledState = ev.enabled(); 

    onEnable( ev.enabled() );
}


void Widget::onEnable(bool isEnable)
{
}

//
// activation
//

void Widget::activate(bool active)
{
    onRequestActivate(active);
}


void Widget::onRequestActivate(bool active)
{
}

//
// geometry
//

const Gfx::PointF& Widget::position() const
{
    return _pos;
}


void Widget::move(const Gfx::PointF& pos)
{
    onRequestMove(pos);
}


void Widget::onRequestMove(const Gfx::PointF& pos)
{
}


void Widget::onProcessMoveEvent(const MoveEvent& ev)
{
    onMoveEvent(ev);
}


void Widget::onMoveEvent(const MoveEvent& ev)
{
    _pos = ev.position();
}


const Gfx::SizeF& Widget::size() const
{
    return _size;
}


const Gfx::RectF& Widget::bounds() const
{
    return _bounds;
}


const Gfx::SizeF& Widget::minimumSize() const
{
    return _minimumSize;
}


void Widget::setMinimumSize(const Gfx::SizeF& s)
{
    _minimumSize = s;

    onSetSizeLimits(_minimumSize, _maximumSize);
}


void Widget::setMinimumSize(double w, double h)
{
    setMinimumSize( Gfx::SizeF(w, h) );
}


void Widget::setMinimumWidth(double w)
{
    setMinimumSize( w, _minimumSize.height() );
}


void Widget::setMinimumHeight(double h)
{
    setMinimumSize( _minimumSize.width(), h );
}


const Gfx::SizeF& Widget::maximumSize() const
{
    return _maximumSize;
}


void Widget::setMaximumSize(const Gfx::SizeF& s)
{
    _maximumSize = s;

    onSetSizeLimits(_minimumSize, _maximumSize);
}


void Widget::setMaximumSize(double w, double h)
{
    setMaximumSize( Gfx::SizeF(w, h) );
}


void Widget::setMaximumWidth(double w)
{
    setMaximumSize( w, _maximumSize.height() );
}


void Widget::setMaximumHeight(double h)
{
    setMaximumSize( _maximumSize.width(), h );
}


void Widget::onSetSizeLimits(const Gfx::SizeF& minSize, 
                             const Gfx::SizeF& maxSize)
{
}


void Widget::resize(const Gfx::SizeF& s)
{
    onRequestResize(s);
}


void Widget::onRequestResize(const Gfx::SizeF& s)
{
}


void Widget::onProcessResizeEvent(const ResizeEvent& ev)
{
    onResizeEvent(ev);
}


void Widget::onResizeEvent(const ResizeEvent& ev)
{
    _size = ev.size();
    _bounds.setSize( ev.size() );
}

//
// input capture
//

void Widget::setCapture(bool capture)
{
    onRequestCapture(capture);
}


void Widget::onRequestCapture(bool capture)
{
    Application::instance().onRequestCapture(*this, capture);
}

//
// cursor
//

const Cursor* Widget::cursor() const
{
    if( ! _hasCursor )
        return &Cursor::defaultCursor();

    return &_cursor;
}


void Widget::setCursor(const Cursor* csr)
{
    if( ! csr )
        _cursor.clear();
    else        
        _cursor = *csr;

    _hasCursor = csr != 0;

    Widget* underPointer = Application::instance().screen().underPointer();
    if(underPointer == this)
        Application::instance().setCursor( cursor() );
}

//
// input processing
//

void Widget::setNextResponder(Responder* r)
{
    _nextResponder = r;
}


Responder* Widget::onNextResponder()
{
    return _nextResponder;
}


void Widget::onProcessMouseEvent(const MouseEvent& ev)
{
    Application::instance().onSetPointer(*this, true);

    mouseEvent(ev);
}


bool Widget::onMouseEvent(const MouseEvent& ev)
{
    return false;
}


void Widget::onProcessTouchEvent(const TouchEvent& ev)
{
    Application::instance().onSetPointer( *this, ev.isPressed() );

    touchEvent(ev);
}


bool Widget::onTouchEvent(const TouchEvent& ev)
{
    return false;
}


void Widget::onProcessScrollEvent(const ScrollEvent& ev)
{
    scrollEvent(ev);
}


bool Widget::onScrollEvent(const ScrollEvent& ev)
{
    return false;
}


void Widget::onProcessEnterEvent(const EnterEvent& ev)
{
    enterEvent(ev);
}


bool Widget::onEnterEvent( const EnterEvent& ev)
{
    //std::clog << "ENTER: " << typeid(*this).name() << " " << id() << std::endl;
    Application::instance().setCursor( cursor() );

    return true;
}


void Widget::onProcessLeaveEvent(const LeaveEvent& ev)
{
    leaveEvent(ev);
}


bool Widget::onLeaveEvent(const LeaveEvent& ev)
{
    //std::clog << "LEAVE: " << typeid(*this).name() << " " << id() << std::endl;
    Application::instance().setCursor( &Cursor::defaultCursor() );

    return true;
}


void Widget::onProcessKeyEvent(const KeyEvent& ev)
{
    keyEvent(ev);
}


bool Widget::onKeyEvent(const KeyEvent& ev)
{
    return false;
}

} // namespace

} // namespace
