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

#ifndef PT_HMI_VISUAL_H
#define PT_HMI_VISUAL_H

#include <Pt/Hmi/Api.h>
#include <Pt/Hmi/Spacing.h>
#include <Pt/Hmi/SizePolicy.h>
#include <Pt/Gfx/PaintSurface.h>
#include <Pt/Gfx/Point.h>
#include <Pt/Gfx/Size.h>
#include <Pt/Gfx/Rect.h>
#include <Pt/String.h>
#include <Pt/Event.h>
#include <Pt/Signal.h>
#include <Pt/Types.h>

#include <string>

namespace Pt {

namespace Hmi {

///////////////////////////////////////////////////////////////////////
// Responder
///////////////////////////////////////////////////////////////////////

class MouseEvent;
class TouchEvent;
class ScrollEvent;
class EnterEvent;
class LeaveEvent;
class KeyEvent;

class PT_HMI_API Responder
{
    protected:
        Responder();

    public:
        virtual ~Responder();

        bool mouseEvent(const MouseEvent& ev);

        void touchEvent(const TouchEvent& ev);

        void scrollEvent(const ScrollEvent& ev);

        void enterEvent(const EnterEvent& ev);

        void leaveEvent(const LeaveEvent& ev);

        void keyEvent(const KeyEvent& ev);

    protected:
        virtual Responder* onNextResponder() = 0;

        virtual Gfx::PointF onToGlobal(const Gfx::PointF& pos) const = 0;

        virtual Gfx::PointF onFromGlobal(const Gfx::PointF& pos) const = 0;

    protected:
        virtual bool onMouseEvent(const MouseEvent& ev);

        virtual bool onTouchEvent(const TouchEvent& ev);

        virtual bool onScrollEvent(const ScrollEvent& ev);

        virtual bool onEnterEvent(const EnterEvent& ev);

        virtual bool onLeaveEvent(const LeaveEvent& ev);

        virtual bool onKeyEvent(const KeyEvent& ev);

    protected:
        virtual bool onMouseUp(const MouseEvent& ev) 
        { return false; }

        virtual bool onMouseDown(const MouseEvent& ev) 
        { return false; }

        virtual bool onMouseMove(const MouseEvent& ev) 
        { return false; }
};

///////////////////////////////////////////////////////////////////////
// Visual
///////////////////////////////////////////////////////////////////////

class PT_HMI_API Visual : public Responder
{
    protected:
        Visual();

    public:
        virtual ~Visual();
        
        /** @brief Returns the ID.
        */
        Pt::uint64_t vid() const
        {
            return _vid;
        }

        /** @brief Returns the name.
        */
        const std::string& name() const
        {
            return _name;
        }
        
        /** @brief Sets the name.
        */
        void setName(const std::string& n)
        {
            _name = n;
        }

        /** @brief Returns the parent.
        */
        Visual* parent()
        {
            return onGetParent();
        }

        /** @brief Returns the parent.
        */
        const Visual* parent() const
        {
            return onGetParent();
        }


        void addPeer(Visual& peer);

        void removePeer(Visual& peer);

        
        /** @brief Returns true if an descendant of @top.
        */
        bool isDescendantOf(const Visual& top) const;

        /** @brief Returns true if an ancestor of @child.
        */
        bool isAncestorOf(const Visual& child) const;

        Visual* hitTest(const Gfx::PointF& pos)
        {
            return onHitTest(pos);
        }

        /** @brief Converts to parent coordinate.
        */
        Gfx::PointF toParent(const Gfx::PointF& pos) const
        {
            return onToParent(pos);
        }

        /** @brief Converts from parent coordinate.
        */
        Gfx::PointF fromParent(const Gfx::PointF& pos) const
        {
            return onFromParent(pos);
        }

        /** @brief Converts to global coordinate.
        */
        Gfx::PointF toGlobal(const Gfx::PointF& pos) const
        {
            return onToGlobal(pos); 
        }
        
        /** @brief Converts to local coordinate.
        */
        Gfx::PointF fromGlobal(const Gfx::PointF& pos) const
        {
            return onFromGlobal(pos);
        }
        

        /** @brief Process event.
        */
        void processEvent(const Pt::Event& ev)
        {
            onEvent(ev);
        }

        /** @brief Pointer input capture.
        */
        void setCapture(bool capture)
        {
            onSetCapture(capture);
        }

        /** @brief Pointer enter.
        */
        void setPointer(bool isPointer);
    
        /** @brief Release to default initial state.
        */
        void release()
        {
            onRelease();
        }

        virtual void invalidate()
        {
            onInvalidateRequest();
        }

        virtual void repaint(const Gfx::RectF& rect)
        {
            onRepaintRequest(rect);
        }

        // deprecated
        void update(const Gfx::RectF& rect)
        { repaint(rect); }

    protected:
        virtual Visual* onGetParent() const = 0;

        virtual Visual* onHitTest(const Gfx::PointF& pos);


        virtual void onAttachPeer(Visual& peer);

        virtual void onDetachPeer(Visual& peer);

        
        virtual Gfx::PointF onToParent(const Gfx::PointF& pos) const = 0;

        virtual Gfx::PointF onFromParent(const Gfx::PointF& pos) const = 0;

        virtual Gfx::PointF onToGlobal(const Gfx::PointF& pos) const;

        virtual Gfx::PointF onFromGlobal(const Gfx::PointF& pos) const;


        virtual void onEvent(const Pt::Event& ev);

        virtual void onRelease();

        virtual void onSetCapture(bool capture);

    protected:
        virtual void onInvalidateRequest();

        virtual void onProcessInvalidateEvent(const InvalidateEvent& ev);

        virtual void onInvalidateEvent(const InvalidateEvent& ev);
    
        virtual void onInvalidate();

    protected:
        virtual void onRepaintRequest(const Gfx::RectF& rect);

        virtual void onProcessPaintEvent(const PaintEvent& ev);

        virtual void onPaintEvent(const PaintEvent& ev);

    private:
        void setR1(void* r)
        { _r1 = r; }

    private:
        Pt::uint64_t             _vid;
        std::string              _name;
        std::vector<Visual*>     _peers;
        void*                    _r1;
};

///////////////////////////////////////////////////////////////////////
// View
///////////////////////////////////////////////////////////////////////

class Widget;
class View;
class Key;

// View -> ViewManager (interface)
// Widget -> View
// Widget -> Control

class PT_HMI_API View
{
    friend class Widget;
    friend class Shell;

    public:
        enum FocusPolicy
        {
            NoFocus,
            AcceptFocus,
            KeepFocus
        };

    protected:
        View();

    public:
        virtual ~View();

        Visual& visual()
        {
            return onGetVisual();
        }

        Gfx::PointF toWidget(const Widget& widget, 
                             const Gfx::PointF& pos) const
        { 
            return onToWidget(widget, pos); 
        }

        Gfx::PointF fromWidget(const Widget& widget,
                               const Gfx::PointF& pos) const
        { 
            return onFromWidget(widget, pos); 
        }

    protected:
        virtual Visual& onGetVisual() = 0;

        virtual void onAttach(Widget& widget) = 0;
        
        virtual void onDetach(Widget& widget) = 0;

        virtual void onInit(Widget& widget) = 0;

        virtual void onRelease(Widget& widget) = 0;

        virtual Gfx::PointF onToWidget(const Widget& widget, 
                                       const Gfx::PointF& pos) const = 0;

        virtual Gfx::PointF onFromWidget(const Widget& widget, 
                                         const Gfx::PointF& pos) const = 0;

        virtual void onRepaint(Widget& widget, const Gfx::RectF& rect) = 0;

        virtual void onRelayout(Widget& widget) = 0;

        virtual void onEnable(Widget& widget, bool isEnable) = 0;

        virtual void onActivate(Widget& w, bool active) = 0;

        virtual void onShow(Widget& widget, bool isShown) = 0;

        virtual void onMove(Widget& widget, const Gfx::PointF& pos) = 0;

        virtual void onResize(Widget& widget, const Gfx::SizeF& size) = 0;

        virtual void onRaise(Widget& widget) = 0;
};

} // namespace

} // namespace

#endif // include guard