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
, _r1(0)
{ 
    Application::instance().registerVisual(*this);
}


Visual::~Visual()
{
    setPointer(false);

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


void Visual::setPointer(bool isPointer)
{
    Application::instance().onSetPointer(*this, isPointer);
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
    Visual* parent = onGetParent();
    if(parent)
        Application::instance().onSetCapture(*this, capture);
}


void Visual::onRelease()
{
}


void Visual::onEvent(const Pt::Event& ev)
{
    if( ev.typeInfo() == typeid(PaintEvent) )
    {
      const PaintEvent& e = static_cast<const PaintEvent&>(ev);
      onProcessPaintEvent(e);
    }
    else if( ev.typeInfo() == typeid(InvalidateEvent) )
    {
      const InvalidateEvent& e = static_cast<const InvalidateEvent&>(ev);
      onProcessInvalidateEvent(e);
    }
}

//
// invalidation
//

void Visual::onInvalidateRequest()
{
    //++_invalidates;

    //InvalidateEvent ev(*this);
    //Application::instance().commitEvent(ev);
}


void Visual::onProcessInvalidateEvent(const InvalidateEvent& ev)
{
    //--_invalidates;

    //if(_invalidates > 0)
    //  return;

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

///////////////////////////////////////////////////////////////////////
// Extended Visual API
///////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////
// View
///////////////////////////////////////////////////////////////////////

View::View()
{
}


View::~View()
{
}


void View::onEvent(const Pt::Event& ev)
{
    Visual::onEvent(ev);

    if( ev.typeInfo() == typeid(MoveEvent) )
    {
      const MoveEvent& e = static_cast<const MoveEvent&>(ev);
      onProcessMoveEvent(e);
    }
    else if( ev.typeInfo() == typeid(ResizeEvent) )
    {
      const ResizeEvent& e = static_cast<const ResizeEvent&>(ev);
      onProcessResizeEvent(e);
    }
}


void View::onProcessMoveEvent(const MoveEvent& ev)
{
    onMoveEvent(ev);
}


void View::onMoveEvent(const MoveEvent& ev)
{
    _alignedGeometry.setOrigin( ev.position() );
}


void View::onProcessResizeEvent(const ResizeEvent& ev)
{
    onResizeEvent(ev);
}


void View::onResizeEvent(const ResizeEvent& ev)
{
    _alignedGeometry.setSize( ev.size() );
    _bounds.setSize( ev.size() );
}

} // namespace

} // namespace
