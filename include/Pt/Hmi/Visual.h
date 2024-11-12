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
#include <Pt/Hmi/Cursor.h>
#include <Pt/Gfx/PaintSurface.h>
#include <Pt/Gfx/Point.h>
#include <Pt/Gfx/Size.h>
#include <Pt/Gfx/Rect.h>
#include <Pt/String.h>
#include <Pt/Event.h>
#include <Pt/Connectable.h>
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
        virtual bool onMousePress(const MouseEvent& ev) 
        { return false; }

        virtual bool onMouseRelease(const MouseEvent& ev) 
        { return false; }

        virtual bool onMouseMove(const MouseEvent& ev) 
        { return false; }
};

///////////////////////////////////////////////////////////////////////
// Visual
///////////////////////////////////////////////////////////////////////

class PT_HMI_API Visual : public Responder
                        , public Pt::Connectable
{
    protected:
        Visual();

    public:
        virtual ~Visual();
        
        /** @brief Returns the ID.
        */
        Pt::uint64_t vid() const;

        /** @brief Returns the name.
        */
        const std::string& name() const;
        
        /** @brief Sets the name.
        */
        void setName(const std::string& n);

        /** @brief Sets the next responder.
        */
        void setNextResponder(Responder* r);

    public:
        /** @brief Returns the parent.
        */
        Visual* parent();

        /** @brief Returns the parent.
        */
        const Visual* parent() const;

        /** @brief Returns true if an descendant of @top.
        */
        bool isDescendantOf(const Visual& top) const;

        /** @brief Returns true if an ancestor of @child.
        */
        bool isAncestorOf(const Visual& child) const;

        /** @brief Returns the descendant hit at a position.
        */
        Visual* hitTest(const Gfx::PointF& pos);

        /** @brief Converts to parent coordinate.
        */
        Gfx::PointF toParent(const Gfx::PointF& pos) const;

        /** @brief Converts from parent coordinate.
        */
        Gfx::PointF fromParent(const Gfx::PointF& pos) const;

        /** @brief Converts to global coordinate.
        */
        Gfx::PointF toGlobal(const Gfx::PointF& pos) const;
        
        /** @brief Converts to local coordinate.
        */
        Gfx::PointF fromGlobal(const Gfx::PointF& pos) const;

    public:
        /** @brief Adds a peer.
        */
        void addPeer(Visual& peer);

        /** @brief Removes a peer.
        */
        void removePeer(Visual& peer);

    public:
        /** @brief Invalidates the state.
        */
        void invalidate();
        
    public:
        /** @brief Initiates a repaint cycle.
        */
        virtual void repaint(const Gfx::RectF& rect);
        
        /** @brief Initiates a repaint cycle.
        */
        virtual void repaint();

    public:  
        /** @brief Returns the current scale factor.
        */
        double scaleFactor() const;

        const Gfx::Scaling& scaling() const;

    public:
        /** @brief Indicates whether the visual is visible.
        */
        bool isVisible() const;
        
        /** @brief Shows the visual.
        */
        virtual void show(bool b = true);

    public:
        /** @brief Indicates whether the visual is enabled.
        */
        bool isEnabled() const;

        /** @brief Enables the visual.
        */
        virtual void enable(bool isEnable = true);

    public:
        void activate(bool active = true);

    public:
        /** @brief Returns the current position.
        */
        const Gfx::PointF& position() const;

        /** @brief Moves the visual to a position.
        */
        virtual void move(const Gfx::PointF& pos);

        /** @brief Returns the current size.
        */
        const Gfx::SizeF& size() const;

        /** @brief Returns the current inner bounds.
        */
        const Gfx::RectF& bounds() const;


        const Gfx::SizeF& minimumSize() const;

        void setMinimumSize(const Gfx::SizeF& s);

        void setMinimumSize(double w, double h);

        void setMinimumWidth(double w);

        void setMinimumHeight(double h);


        const Gfx::SizeF& maximumSize() const;

        void setMaximumSize(const Gfx::SizeF& s);

        void setMaximumSize(double w, double h);

        void setMaximumWidth(double w);

        void setMaximumHeight(double h);


        /** @brief Resizes the visual to a new size.
        */
        virtual void resize(const Gfx::SizeF& s);

    public:
        /** @brief Pointer input capture.
        */
        void setCapture(bool capture);

    public:
        const Cursor* cursor() const;

        void setCursor(const Cursor* c);

    public:
        /** @brief Process event.
        */
        void processEvent(const Pt::Event& ev);
        
        /** @brief Signals that an event needs to be processed.
        */
        Pt::Signal<const Pt::Event&>& eventReceived();

    protected:
        virtual void onSetParent(Visual* visual);

        virtual Visual* onHitTest(const Gfx::PointF& pos);

        virtual Gfx::PointF onToParent(const Gfx::PointF& pos) const = 0;

        virtual Gfx::PointF onFromParent(const Gfx::PointF& pos) const = 0;

        virtual Gfx::PointF onToGlobal(const Gfx::PointF& pos) const;

        virtual Gfx::PointF onFromGlobal(const Gfx::PointF& pos) const;

    protected:
        virtual void onAttachPeer(Visual& peer);

        virtual void onDetachPeer(Visual& peer);

    protected:
        // onRepaint
        virtual void onRequestRepaint(const Gfx::RectF& rect);

        // onSetVisible, onShow
        virtual void onRequestShow(bool e);

        // onSetEnabled, onEnable
        virtual void onRequestEnable(bool isEnable);

        virtual void onRequestActivate(bool active);

        // onSetPosition, onMove
        virtual void onRequestMove(const Gfx::PointF& pos);

        virtual void onSetSizeLimits(const Gfx::SizeF& minSize,
                                     const Gfx::SizeF& maxSize);

        // onSetSize, onResize
        virtual void onRequestResize(const Gfx::SizeF& s);

        // onSetCapture, onCapture
        virtual void onRequestCapture(bool capture);

    protected:
        virtual void onProcessEvent(const Pt::Event& ev);

    protected:
        virtual void onProcessInvalidateEvent(const InvalidateEvent& ev);

        virtual void onInvalidateEvent(const InvalidateEvent& ev);
    
        virtual void onInvalidate();

    protected:
        virtual void onProcessPaintEvent(const PaintEvent& ev);

        virtual void onPaintEvent(const PaintEvent& ev);

    protected:
        virtual void onProcessRescaleEvent(const RescaleEvent& ev);
        
        virtual void onRescaleEvent(const RescaleEvent& ev);

        virtual void onRescale(double scaling);

    protected:
        virtual void onProcessShowEvent(const ShowEvent& ev);

        virtual void onShowEvent(const ShowEvent& ev);

        virtual void onShow(bool visible);

    protected:
        virtual void onProcessEnableEvent(const EnableEvent& ev);

        virtual void onEnableEvent(const EnableEvent& ev);

        virtual void onEnable(bool e);

    protected:
        virtual void onProcessMoveEvent(const MoveEvent& ev);

        virtual void onMoveEvent(const MoveEvent& ev);

        virtual void onProcessResizeEvent(const ResizeEvent& ev);

        virtual void onResizeEvent(const ResizeEvent& ev);

    protected:
        virtual void onProcessMouseEvent(const MouseEvent& ev);

        virtual void onProcessTouchEvent(const TouchEvent& ev);

        virtual void onProcessScrollEvent(const ScrollEvent& ev);

        virtual void onProcessEnterEvent(const EnterEvent& ev);

        virtual void onProcessLeaveEvent(const LeaveEvent& ev);

        virtual void onProcessKeyEvent(const KeyEvent& ev);

    //
    // Responder
    //
    protected:
        virtual Responder* onNextResponder();

        virtual bool onMouseEvent(const MouseEvent& ev);

        virtual bool onTouchEvent(const TouchEvent& ev);

        virtual bool onScrollEvent( const ScrollEvent& ev);

        virtual bool onEnterEvent( const EnterEvent& ev);

        virtual bool onLeaveEvent(const LeaveEvent& ev);

        virtual bool onKeyEvent(const KeyEvent& ev);

    private:
        void setR1(void* r)
        { _r1 = r; }

    private:
        Pt::Signal<const Pt::Event&> _dispatcher;

        Pt::uint64_t          _vid;
        std::string           _name;

        Visual*               _parent;
        std::vector<Visual*>  _peers;

        Responder*            _nextResponder;

        int                   _invalidates;

        Gfx::Scaling          _scaling;

        bool                  _enabledState;
        bool                  _isVisible;

        Gfx::PointF           _pos;
        Gfx::SizeF            _size;
        Gfx::RectF            _bounds;

        Gfx::SizeF            _minimumSize;
        Gfx::SizeF            _maximumSize;

        bool                  _hasCursor;
        Hmi::Cursor           _cursor;

        void*                 _r1;
};

///////////////////////////////////////////////////////////////////////
// View
///////////////////////////////////////////////////////////////////////

class Widget;
class View;

class ViewCanvas : public Gfx::CanvasBase
{
    public:
        explicit ViewCanvas(View& view)
        : _view(&view)
        , _surface(0)
        {
        }

        Gfx::PaintSurface* surface()
        {
            return _surface;
        }

        const Gfx::PointF& position() const
        {
            return _position;
        } 

        void setSurface(Gfx::PaintSurface* surface, 
                        const Gfx::PointF& pos)
        {
            _surface = surface;
            _position = pos;
        }

    protected:
        virtual const Gfx::ImageFormat& onGetFormat() const;

        virtual const Gfx::SizeF& onGetSize() const;

        virtual const Gfx::Scaling& onGetScaling() const;

        virtual Gfx::PaintContext* onGetPaint(Gfx::PaintContext* context);

    private:
        View*                _view;
        Gfx::PaintSurface*   _surface;
        Gfx::PointF          _position;
};


class PT_HMI_API View : public Visual
                      , private Gfx::PaintSurface
{
    friend class Widget;

    typedef Visual Base;

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

        Gfx::PointF toWidget(const Widget& widget, 
                             const Gfx::PointF& pos) const;

        Gfx::PointF fromWidget(const Widget& widget,
                               const Gfx::PointF& pos) const;

        Gfx::PaintSurface& surface();

        const Gfx::PaintSurface& surface() const;

        void setSurface(Gfx::PaintSurface* surface, 
                        const Gfx::PointF& pos = Gfx::PointF() );

    protected:
        virtual void onSetSurface(Gfx::PaintSurface* surface, 
                                  const Gfx::PointF& pos);

        virtual void onPaint(Gfx::PaintSurface& surface, 
                             const Gfx::RectF& rect);
    
    protected:
        virtual void onAttach(Widget& widget);
        
        virtual void onDetach(Widget& widget);

        virtual void onInit(Widget& widget);

        virtual void onRelease(Widget& widget);

        virtual Gfx::PointF onToWidget(const Widget& widget, 
                                       const Gfx::PointF& pos) const;

        virtual Gfx::PointF onFromWidget(const Widget& widget, 
                                         const Gfx::PointF& pos) const;

    protected:
        virtual void onRepaintRequest(Widget& widget, const Gfx::RectF& rect);

        virtual void onRelayoutRequest(Widget& widget);

        virtual void onEnableRequest(Widget& widget, bool isEnable);

        virtual void onActivateRequest(Widget& w, bool active);

        virtual void onShowRequest(Widget& widget, bool isShown);

        virtual void onMoveRequest(Widget& widget, const Gfx::PointF& pos);

        virtual void onResizeRequest(Widget& widget, const Gfx::SizeF& size);

        virtual void onRaiseRequest(Widget& widget);

    //
    // Visual
    //
    protected:
        virtual void onPaintEvent(const PaintEvent& ev) override;

        virtual void onMoveEvent(const MoveEvent& ev) override;

        virtual void onResizeEvent(const ResizeEvent& ev) override;

    private:
        ViewCanvas* _canvas;
};

} // namespace

} // namespace

#endif // include guard