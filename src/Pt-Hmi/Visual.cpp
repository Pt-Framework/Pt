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
  
  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  
  02110-1301 USA
*/

#include <Pt/Hmi/Visual.h>
#include <Pt/Hmi/Widget.h>
#include <Pt/Hmi/Application.h>
#include <Pt/Hmi/MouseEvent.h>
#include <Pt/Hmi/TouchEvent.h>
#include <Pt/Hmi/ScrollEvent.h>
#include <Pt/Hmi/EnterEvent.h>
#include <Pt/Hmi/LeaveEvent.h>
#include <Pt/Hmi/KeyEvent.h>

namespace Pt {

namespace Hmi {

///////////////////////////////////////////////////////////////////////
// Responder
///////////////////////////////////////////////////////////////////////

Responder::Responder()
{ 
}


Responder::~Responder()
{
}


bool Responder::mouseEvent(const MouseEvent& ev)
{
    Gfx::PointF localPos = onFromGlobal( ev.position() );

    MouseEvent localEv(ev);
    localEv.setPosition(localPos);

    bool consumed = onMouseEvent(localEv);
    if(consumed)
        return true;

    Responder* next = onNextResponder();
    if(next)
        return next->mouseEvent(ev);

    return false;
}


bool Responder::onMouseEvent(const MouseEvent& ev)
{
    if( ev.isPress() )
        return onMousePress(ev);
            
    if( ev.isRelease() )
        return onMouseRelease(ev);
             
      return onMouseMove(ev);
}


void Responder::touchEvent(const TouchEvent& ev)
{
    Gfx::PointF localPos = onFromGlobal( ev.position() );

    TouchEvent localEv(ev);
    localEv.setPosition(localPos);

    bool consumed = onTouchEvent(localEv);
    if(consumed)
        return;

    Responder* next = onNextResponder();
    if(next)
        next->touchEvent(ev);
}


bool Responder::onTouchEvent(const TouchEvent& ev)
{
    return false;
}


void Responder::scrollEvent(const ScrollEvent& ev)
{
    bool consumed = onScrollEvent(ev);
    if(consumed)
        return;

    Responder* next = onNextResponder();
    if(next)
        next->scrollEvent(ev);
}


bool Responder::onScrollEvent(const ScrollEvent& ev)
{ 
    return false; 
}


void Responder::enterEvent(const EnterEvent& ev)
{
    bool consumed = onEnterEvent(ev);
    if(consumed)
        return;

    Responder* next = onNextResponder();
    if(next)
        next->enterEvent(ev);
}


bool Responder::onEnterEvent(const EnterEvent& ev)
{ 
    return false; 
}


void Responder::leaveEvent(const LeaveEvent& ev)
{
    bool consumed = onLeaveEvent(ev);
    if(consumed)
        return;

    Responder* next = onNextResponder();
    if(next)
        next->leaveEvent(ev);
}


bool Responder::onLeaveEvent(const LeaveEvent& ev)
{ 
    return false; 
}


void Responder::keyEvent(const KeyEvent& ev)
{
    bool consumed = onKeyEvent(ev);
    if(consumed)
        return;

    Responder* next = onNextResponder();
    if(next)
        next->keyEvent(ev);
}


bool Responder::onKeyEvent(const KeyEvent& ev)
{ 
    return false; 
}

///////////////////////////////////////////////////////////////////////
// Visual
///////////////////////////////////////////////////////////////////////

Visual::Visual()
: _vid( Application::instance().makeId()  )
, _parent(0)
, _nextResponder(0)
, _invalidates(0)
, _scaleFactor(1.0)
, _enabledState(true)
, _isVisible(false)
, _minimumSize(0, 0)
, _maximumSize(64000, 64000)
, _r1(0)
{ 
    Application::instance().registerVisual(*this);

    _dispatcher += Pt::slot(*this, &Visual::onProcessInvalidateEvent);
    _dispatcher += Pt::slot(*this, &Visual::onProcessPaintEvent);
    _dispatcher += Pt::slot(*this, &Visual::onProcessRescaleEvent);
    _dispatcher += Pt::slot(*this, &Visual::onProcessMoveEvent);
    _dispatcher += Pt::slot(*this, &Visual::onProcessResizeEvent);
    _dispatcher += Pt::slot(*this, &Visual::onProcessEnableEvent);
    _dispatcher += Pt::slot(*this, &Visual::onProcessShowEvent);
    _dispatcher += Pt::slot(*this, &Visual::onProcessMouseEvent);
    _dispatcher += Pt::slot(*this, &Visual::onProcessTouchEvent);
    _dispatcher += Pt::slot(*this, &Visual::onProcessScrollEvent);
    _dispatcher += Pt::slot(*this, &Visual::onProcessEnterEvent);
    _dispatcher += Pt::slot(*this, &Visual::onProcessLeaveEvent);
    _dispatcher += Pt::slot(*this, &Visual::onProcessKeyEvent);
}


Visual::~Visual()
{
    setCapture(false);
    
    Application::instance().onSetPointer(*this, false);

    while( ! _peers.empty() )
    {
        removePeer( *_peers.back() );
    }

    Application::instance().unregisterVisual(*this);
}

//
// global identifier
// 

Pt::uint64_t Visual::vid() const
{
    return _vid;
}


const std::string& Visual::name() const
{
    return _name;
}
        

void Visual::setName(const std::string& n)
{
    _name = n;
}

//
// hierachy management
// 

Visual* Visual::parent()
{
    return _parent;
}


const Visual* Visual::parent() const
{
    return _parent;
}


void Visual::onSetParent(Visual* visual)
{
    _parent = visual;
}


bool Visual::isDescendantOf(const Visual& v) const
{
    const Visual* parent = this->parent();
    if( ! parent )
        return false;

    if( parent == &v )
        return true;

    return parent->isDescendantOf(v);
}


bool Visual::isAncestorOf(const Visual& v) const
{
    return v.isDescendantOf(*this);
}


Visual* Visual::hitTest(const Gfx::PointF& pos)
{
    return onHitTest(pos);
}


Visual* Visual::onHitTest(const Gfx::PointF& pos)
{
    return 0;
}


Gfx::PointF Visual::toParent(const Gfx::PointF& pos) const
{
    return onToParent(pos);
}


Gfx::PointF Visual::fromParent(const Gfx::PointF& pos) const
{
    return onFromParent(pos);
}


Gfx::PointF Visual::toGlobal(const Gfx::PointF& pos) const
{
    return onToGlobal(pos); 
}
        

Gfx::PointF Visual::fromGlobal(const Gfx::PointF& pos) const
{
    return onFromGlobal(pos);
}


Gfx::PointF Visual::onToGlobal(const Gfx::PointF& pos) const
{
    const Visual* parent = this->parent();
    if( ! parent )
        return pos;

    Gfx::PointF parentPos = toParent(pos);
    return parent->toGlobal(parentPos);
}


Gfx::PointF Visual::onFromGlobal(const Gfx::PointF& pos) const
{
    const Visual* parent = this->parent();
    if( ! parent )
        return pos;

    Gfx::PointF parentPos = parent->fromGlobal(pos);
    return fromParent(parentPos);
}

//
// peer relationship
// 

void Visual::addPeer(Visual& peer)
{
    peer.onAttachPeer(*this);
    onAttachPeer(peer);
}


void Visual::removePeer(Visual& peer)
{
    peer.onDetachPeer(*this);
    onDetachPeer(peer);
}


void Visual::onAttachPeer(Visual& peer)
{
    _peers.push_back(&peer);
}


void Visual::onDetachPeer(Visual& peer)
{
    std::vector<Visual*>::iterator it;
    it = std::find( _peers.begin(), _peers.end(), &peer );
    
    if( it != _peers.end() )
        _peers.erase(it);
}

//
// event processing
//

void Visual::processEvent(const Pt::Event& ev)
{
    onProcessEvent(ev);
}


void Visual::onProcessEvent(const Pt::Event& ev)
{
    _dispatcher.send(ev);
}


Pt::Signal<const Pt::Event&>& Visual::eventReceived()
{
    return _dispatcher;
}

//
// invalidation
//

void Visual::invalidate()
{
    ++_invalidates;

    InvalidateEvent ev(*this);
    Application::instance().commitEvent(ev);
}


// TODO: invalidate() should lead to an update event
void Visual::onProcessInvalidateEvent(const InvalidateEvent& ev)
{
    --_invalidates;

    if(_invalidates > 0)
      return;

    onInvalidateEvent(ev);
}


// TODO: remove and only use onInvalidateEvent
void Visual::onInvalidateEvent(const InvalidateEvent& ev)
{
    onInvalidate();
}

    
void Visual::onInvalidate()
{
}

//
// painting
//

void Visual::repaint(const Gfx::RectF& rect)
{
    onRequestRepaint(rect);
}


void Visual::repaint()
{
    repaint( bounds() );
}


void Visual::onRequestRepaint(const Gfx::RectF& rect)
{
}


void Visual::onProcessPaintEvent(const PaintEvent& ev)
{
    if( ev.rect().isNull() )
        return;

    onPaintEvent(ev);
}


void Visual::onPaintEvent(const PaintEvent& ev)
{
}

//
// scaling
//

double Visual::scaleFactor() const
{
    return _scaleFactor;
}


void Visual::onProcessRescaleEvent(const RescaleEvent& ev)
{
    onRescaleEvent(ev);
}


void Visual::onRescaleEvent(const RescaleEvent& ev)
{
    onRescale( ev.scaleFactor() );
}


void Visual::onRescale(double scaling)
{
    _scaleFactor = scaling;
}

//
// visibility
//

bool Visual::isVisible() const
{
    return _isVisible;
}


void Visual::show(bool isShow)
{
    onRequestShow(isShow);
}


void Visual::onRequestShow(bool e)
{
}


void Visual::onProcessShowEvent(const ShowEvent& ev)
{
    if( _isVisible == ev.visible() )
        return;

    onShowEvent(ev);
}


void Visual::onShowEvent(const ShowEvent& ev)
{
    onShow( ev.visible() );
}


// TODO: remove and only use onShowEvent
void Visual::onShow(bool isShown)
{
    _isVisible = isShown;
}

//
// enabling
//

bool Visual::isEnabled() const
{
    return _enabledState;
}


void Visual::enable(bool isEnable)
{
    onRequestEnable(isEnable);
}


void Visual::onRequestEnable(bool e)
{
}


void Visual::onProcessEnableEvent(const EnableEvent& ev)
{
    onEnableEvent(ev);
}


void Visual::onEnableEvent(const EnableEvent& ev)
{    
    _enabledState = ev.enabled(); 

    onEnable( ev.enabled() );
}


void Visual::onEnable(bool isEnable)
{
}

//
// geometry
//

const Gfx::PointF& Visual::position() const
{
    return _pos;
}


void Visual::move(const Gfx::PointF& pos)
{
    onRequestMove(pos);
}


void Visual::onRequestMove(const Gfx::PointF& pos)
{
}


void Visual::onProcessMoveEvent(const MoveEvent& ev)
{
    onMoveEvent(ev);
}


void Visual::onMoveEvent(const MoveEvent& ev)
{
    _pos = ev.position();
}


const Gfx::SizeF& Visual::size() const
{
    return _size;
}


const Gfx::RectF& Visual::bounds() const
{
    return _bounds;
}


const Gfx::SizeF& Visual::minimumSize() const
{
    return _minimumSize;
}


void Visual::setMinimumSize(const Gfx::SizeF& s)
{
    _minimumSize = s;

    onSetSizeLimits(_minimumSize, _maximumSize);
}


void Visual::setMinimumSize(double w, double h)
{
    setMinimumSize( Gfx::SizeF(w, h) );
}


void Visual::setMinimumWidth(double w)
{
    setMinimumSize( w, _minimumSize.height() );
}


void Visual::setMinimumHeight(double h)
{
    setMinimumSize( _minimumSize.width(), h );
}


const Gfx::SizeF& Visual::maximumSize() const
{
    return _maximumSize;
}


void Visual::setMaximumSize(const Gfx::SizeF& s)
{
    _maximumSize = s;

    onSetSizeLimits(_minimumSize, _maximumSize);
}


void Visual::setMaximumSize(double w, double h)
{
    setMaximumSize( Gfx::SizeF(w, h) );
}


void Visual::setMaximumWidth(double w)
{
    setMaximumSize( w, _maximumSize.height() );
}


void Visual::setMaximumHeight(double h)
{
    setMaximumSize( _maximumSize.width(), h );
}


void Visual::onSetSizeLimits(const Gfx::SizeF& minSize, 
                             const Gfx::SizeF& maxSize)
{
}


void Visual::resize(const Gfx::SizeF& s)
{
    onRequestResize(s);
}


void Visual::onRequestResize(const Gfx::SizeF& s)
{
}


void Visual::onProcessResizeEvent(const ResizeEvent& ev)
{
    onResizeEvent(ev);
}


void Visual::onResizeEvent(const ResizeEvent& ev)
{
    _size = ev.size();
    _bounds.setSize( ev.size() );
}

//
// input capture
//

void Visual::setCapture(bool capture)
{
    onRequestCapture(capture);
}


void Visual::onRequestCapture(bool capture)
{
    Application::instance().onRequestCapture(*this, capture);
}

//
// input processing
//

void Visual::setNextResponder(Responder* r)
{
    _nextResponder = r;
}


Responder* Visual::onNextResponder()
{
    return _nextResponder;
}


void Visual::onProcessMouseEvent(const MouseEvent& ev)
{
    Application::instance().onSetPointer(*this, true);

    mouseEvent(ev);
}


bool Visual::onMouseEvent(const MouseEvent& ev)
{
    return false;
}


void Visual::onProcessTouchEvent(const TouchEvent& ev)
{
    Application::instance().onSetPointer( *this, ev.isPressed() );

    touchEvent(ev);
}


bool Visual::onTouchEvent(const TouchEvent& ev)
{
    return false;
}


void Visual::onProcessScrollEvent(const ScrollEvent& ev)
{
    scrollEvent(ev);
}


bool Visual::onScrollEvent(const ScrollEvent& ev)
{
    return false;
}


void Visual::onProcessEnterEvent(const EnterEvent& ev)
{
    enterEvent(ev);
}


bool Visual::onEnterEvent( const EnterEvent& ev)
{
    return true;
}


void Visual::onProcessLeaveEvent(const LeaveEvent& ev)
{
    leaveEvent(ev);
}


bool Visual::onLeaveEvent(const LeaveEvent& ev)
{
    return true;
}


void Visual::onProcessKeyEvent(const KeyEvent& ev)
{
    keyEvent(ev);
}


bool Visual::onKeyEvent(const KeyEvent& ev)
{
    return false;
}

///////////////////////////////////////////////////////////////////////
// View
///////////////////////////////////////////////////////////////////////

View::View()
: _isMeasureInvalid(true)
{
    eventReceived() += Pt::slot(*this, &View::onProcessLayoutEvent);
}


View::~View()
{
}


Gfx::PointF View::toWidget(const Widget& widget, 
                      const Gfx::PointF& pos) const
{ 
    return onToWidget(widget, pos); 
}


Gfx::PointF View::fromWidget(const Widget& widget,
                        const Gfx::PointF& pos) const
{ 
    return onFromWidget(widget, pos);
}


void View::relayout()
{
    _isMeasureInvalid = true;
    onRequestRelayout();
}


const SizePolicy& View::sizePolicy() const
{
    return _sizePolicy;
}


void View::setSizePolicy(const SizePolicy& policy)
{
    _sizePolicy = policy;

    relayout();
}


Gfx::SizeF View::preferredSize() const
{
    return _preferredSize;
}


Gfx::SizeF View::measure(const SizePolicy& policy)
{
    SizePolicy contentPolicy = _sizePolicy;

    // use stricter size mode of parent and, if parent is fixed,
    // we also use the parents fixed width
    if( policy.horizontal() > _sizePolicy.horizontal() ||
        policy.horizontal() == SizePolicy::Fixed )
    {
        contentPolicy.setHorizontal( policy.horizontal() );
        contentPolicy.setWidth( policy.width() );
    }

    // use stricter size mode of parent and, if parent is fixed,
    // we also use the parents fixed height
    if( policy.vertical() > _sizePolicy.vertical() ||
        policy.vertical() == SizePolicy::Fixed )
    {
        contentPolicy.setVertical( policy.vertical() );
        contentPolicy.setHeight( policy.height() );
    }

    // apply minimum height, unless the size mode is fixed
    if( contentPolicy.vertical() != SizePolicy::Fixed &&
        contentPolicy.height() < minimumSize().height() )
    {
        contentPolicy.setHeight( minimumSize().height() );
    }

    // apply minimum width, unless the size mode is fixed
    if( contentPolicy.horizontal() != SizePolicy::Fixed &&
        contentPolicy.width() < minimumSize().width() )
    {
        contentPolicy.setWidth( minimumSize().width() );
    }

    bool doMeasure = contentPolicy != _lastPolicy || _isMeasureInvalid;
    if(doMeasure)
    {
        _lastPolicy = contentPolicy;

        _preferredSize = onProcessMeasure(contentPolicy);

        // use fixed height, if size mode is fixed
        if(contentPolicy.vertical() == SizePolicy::Fixed)
            _preferredSize.setHeight( contentPolicy.height() );
        else if( _preferredSize.height() < minimumSize().height() )
            _preferredSize.setHeight( minimumSize().height() );

        if(contentPolicy.vertical() == SizePolicy::Maximum)
            _preferredSize.setHeight( std::min( _preferredSize.height(),
                                                contentPolicy.height() ) );

        // use fixed width, if size mode is fixed
        if(contentPolicy.horizontal() == SizePolicy::Fixed)
            _preferredSize.setWidth( contentPolicy.width() );
        else if( _preferredSize.width() < minimumSize().width() )
            _preferredSize.setWidth( minimumSize().width() );

        if(contentPolicy.horizontal() == SizePolicy::Maximum)
            _preferredSize.setWidth( std::min( _preferredSize.width(),
                                               contentPolicy.width() ) );
    
        _isMeasureInvalid = false;
    }

    return preferredSize();
}


Gfx::SizeF View::onProcessMeasure(const SizePolicy& policy)
{
   return onMeasure(policy);
}


Gfx::SizeF View::onMeasure(const SizePolicy& policy)
{
   return Gfx::SizeF(0, 0);
}


void View::onProcessLayoutEvent(const LayoutEvent& ev)
{
    onLayoutEvent(ev);
}


void View::onLayoutEvent(const LayoutEvent& ev)
{
}


void View::onRescaleEvent(const RescaleEvent& ev)
{
    Base::onRescaleEvent(ev);
}


void View::onRescale(double scaling)
{
    Base::onRescale(scaling);
    
    relayout();
}

} // namespace

} // namespace
