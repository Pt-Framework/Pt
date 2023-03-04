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
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, 
  MA 02110-1301 USA
*/

#include <Pt/Hmi/WindowBase.h>

namespace Pt {

namespace Hmi {

/////////////////////////////////////////////////////////////////////////
//// old ShellBase
/////////////////////////////////////////////////////////////////////////

//class ShellBase : public WindowManager
//{
//    public:
//        ShellBase();
//
//        ~ShellBase();
//
//        void setParent(ShellManager* parent);
//
//        void repaint(const Gfx::RectF& rect);
//
//        void activate(bool isActive);
//
//        const Gfx::RectF& geometry() const;
//
//        const Gfx::PointF& position() const;
//
//        void move(const Gfx::PointF& pos);
//
//        const Gfx::SizeF& size() const;
//
//        void resize(const Gfx::SizeF& s);
//
//        void setNextResponder(Responder* r);
//
//    protected:
//        Responder* onNextResponder();
//
//        Gfx::PointF onToScreen(const Gfx::PointF& pos) const;
//
//        Gfx::PointF onFromScreen(const Gfx::PointF& pos) const;
//
//    protected:
//        void onProcessEvent(const Pt::Event& ev);
//
//        
//        void onProcessMoveEvent(const MoveEvent& ev);
//
//        void onMoveEvent(const MoveEvent& ev);
//
//
//        void onProcessResizeEvent(const ResizeEvent& ev);
//
//        void onResizeEvent(const ResizeEvent& ev);
//
//    private:
//        Pt::Signal<const Pt::Event&> _eventReceived;
//
//        ShellManager*                _parent;
//        Responder*                   _nextResponder;
//
//        Gfx::RectF                   _alignedGeometry;
//        Gfx::RectF                   _requestedGeometry;
//};


//ShellBase::ShellBase()
//: _parent(0)
//, _nextResponder(0)
//{
//    _eventReceived += Pt::slot(*this, &ShellBase::onProcessMoveEvent);
//    _eventReceived += Pt::slot(*this, &ShellBase::onProcessResizeEvent);
//}
//
//
//ShellBase::~ShellBase()
//{
//    setParent(0);
//}
//
//
//void ShellBase::setParent(ShellManager* parent)
//{
//    if(_parent == parent)
//        return;
//
//    if(_parent)
//    {
//        _parent->onRelease(*this);
//        _parent->onDetach(*this);
//        _parent = 0;
//    }
//
//    if(parent)
//    {
//        parent->onAttach(*this);
//        _parent = parent;
//
//        _parent->onInit(*this);
//        _parent->onRescale(*this);
//        _parent->onMove(*this, _requestedGeometry.topLeft());
//        _parent->onResize(*this, _requestedGeometry.size());
//    }
//}
//
//
//void ShellBase::repaint(const Gfx::RectF& rect)
//{
//    if(_parent)
//        _parent->onRepaint(*this, rect);
//}
//
//
//void ShellBase::activate(bool isActive)
//{
//    if(_parent)
//        _parent->onActivate(*this, isActive);
//}
//
//
//const Gfx::RectF& ShellBase::geometry() const
//{
//    return _alignedGeometry;
//}
//
//
//const Gfx::PointF& ShellBase::position() const
//{ 
//    return _alignedGeometry.topLeft(); 
//}
//
//
//void ShellBase::move(const Gfx::PointF& pos)
//{
//    _requestedGeometry.setOrigin(pos);
//
//    if(_parent)
//        _parent->onMove(*this, pos);
//    else
//        _alignedGeometry.setOrigin(pos);
//}
//
//
//const Gfx::SizeF& ShellBase::size() const
//{ 
//    return _alignedGeometry.size(); 
//}
//
//
//void ShellBase::resize(const Gfx::SizeF& s)
//{
//    _requestedGeometry.setSize(s);
//    
//    if(_parent)
//        _parent->onResize(*this, s);
//    else
//        _alignedGeometry.setSize(s);
//}
//
//
//void ShellBase::setNextResponder(Responder* r)
//{
//    _nextResponder = r;
//}
//
//
//Responder* ShellBase::onNextResponder()
//{
//    return _nextResponder;
//}
//
//
//Gfx::PointF ShellBase::onToScreen(const Gfx::PointF& pos) const
//{
//    if(_parent)
//        return _parent->onToScreen(*this, pos);
//
//    return pos;
//}
//
//
//Gfx::PointF ShellBase::onFromScreen(const Gfx::PointF& pos) const
//{
//    if(_parent)
//        return _parent->onFromScreen(*this, pos);
//
//    return pos;
//}
//
//
//void ShellBase::onProcessEvent(const Pt::Event& ev)
//{
//    _eventReceived.send(ev);
//}
//
//
//void ShellBase::onProcessMoveEvent(const MoveEvent& ev)
//{
//    onMoveEvent(ev);
//}
//
//
//void ShellBase::onMoveEvent(const MoveEvent& ev)
//{    
//    _alignedGeometry.setOrigin( ev.position() );
//}
//
//
//void ShellBase::onProcessResizeEvent(const ResizeEvent& ev)
//{
//    onResizeEvent(ev);
//}
//
//
//void ShellBase::onResizeEvent(const ResizeEvent& ev)
//{
//    _alignedGeometry.setSize( ev.size() );
//}

} // namespace

} // namespace
