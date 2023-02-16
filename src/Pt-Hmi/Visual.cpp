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
        return onMouseDown(ev);
            
    if( ev.isRelease() )
        return onMouseUp(ev);
             
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
, _invalidates(0)
, _enabledState(true)
, _scaleFactor(1.0)
, _isVisible(false)
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


Visual* Visual::onHitTest(const Gfx::PointF& pos)
{
    return 0;
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


void Visual::onSetCapture(bool capture)
{
    Application::instance().onSetCapture(*this, capture);
}


void Visual::onEvent(const Pt::Event& ev)
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


void Visual::onProcessInvalidateEvent(const InvalidateEvent& ev)
{
    --_invalidates;

    if(_invalidates > 0)
      return;

    onInvalidateEvent(ev);
}


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

void Visual::onRepaintRequest(const Gfx::RectF& rect)
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
// enabling
//

void Visual::onProcessEnableEvent(const EnableEvent& ev)
{
    onEnableEvent(ev);
}


void Visual::onEnableEvent(const EnableEvent& ev)
{    
    _enabledState = ev.enabled(); 

    onEnable( ev.enabled() );
}


void Visual::onEnable(bool e)
{
}

//
// visibility
//

bool Visual::isVisible() const
{
    return _isVisible;
}


void Visual::onProcessShowEvent(const ShowEvent& ev)
{
    onShowEvent(ev);
}


void Visual::onShowEvent(const ShowEvent& ev)
{
    onShow( ev.visible() );
}


void Visual::onShow(bool isShown)
{
    _isVisible = isShown;
}

//
// scaling
//

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
// geometry
//

void Visual::onProcessMoveEvent(const MoveEvent& ev)
{
    onMoveEvent(ev);
}


void Visual::onMoveEvent(const MoveEvent& ev)
{
    _pos = ev.position();
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
    Application::instance().onSetPointer(*this, true);

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
{
}


View::~View()
{
}

} // namespace

} // namespace
