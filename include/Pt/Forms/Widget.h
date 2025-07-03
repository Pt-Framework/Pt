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

#ifndef PT_FORMS_WIDGET_H
#define PT_FORMS_WIDGET_H

#include <Pt/Forms/Api.h>
#include <Pt/Forms/Responder.h>
#include <Pt/Forms/Cursor.h>

#include <Pt/Gfx/Scaling.h>
#include <Pt/Gfx/Point.h>
#include <Pt/Gfx/Size.h>
#include <Pt/Gfx/Rect.h>

#include <Pt/Connectable.h>
#include <Pt/Signal.h>
#include <Pt/Types.h>

#include <string>
#include <vector>

namespace Pt {

namespace Forms {

class Screen;
class MouseEvent;
class TouchEvent;
class ScrollEvent;
class EnterEvent;
class LeaveEvent;
class KeyEvent;


class PT_FORMS_API Widget : public Responder
                          , public Pt::Connectable
{
    protected:
        Widget();

    public:
        virtual ~Widget();
        
        /** @brief Returns the ID.
        */
        Pt::uint64_t id() const;

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
        Widget* parent();

        /** @brief Returns the parent.
        */
        const Widget* parent() const;

        /** @brief Returns true if an descendant of @top.
        */
        bool isDescendantOf(const Widget& top) const;

        /** @brief Returns true if an ancestor of @child.
        */
        bool isAncestorOf(const Widget& child) const;

        /** @brief Returns the descendant hit at a position.
        */
        Widget* hitTest(const Gfx::PointF& pos);

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
        void addPeer(Widget& peer);

        /** @brief Removes a peer.
        */
        void removePeer(Widget& peer);

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
        /** @brief Indicates whether the widget is visible.
        */
        bool isVisible() const;
        
        /** @brief Shows the widget.
        */
        virtual void show(bool b = true);

    public:
        /** @brief Indicates whether the widget is enabled.
        */
        bool isEnabled() const;

        /** @brief Enables the widget.
        */
        virtual void enable(bool isEnable = true);

    public:
        void activate(bool active = true);

    public:
        /** @brief Returns the current position.
        */
        const Gfx::PointF& position() const;

        /** @brief Moves the widget to a position.
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


        /** @brief Resizes the widget to a new size.
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

    public:
        Screen* screen();

    protected:
        void setScreen(Screen* screen);

    protected:
        virtual void onSetParent(Widget* parent);

        virtual void onSetScreen(Screen* screen);

        virtual Widget* onHitTest(const Gfx::PointF& pos);

        virtual Gfx::PointF onToParent(const Gfx::PointF& pos) const = 0;

        virtual Gfx::PointF onFromParent(const Gfx::PointF& pos) const = 0;

        virtual Gfx::PointF onToGlobal(const Gfx::PointF& pos) const;

        virtual Gfx::PointF onFromGlobal(const Gfx::PointF& pos) const;

    protected:
        virtual void onAttachPeer(Widget& peer);

        virtual void onDetachPeer(Widget& peer);

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

        Pt::uint64_t          _id;
        std::string           _name;

        Screen*               _screen;
        Widget*               _parent;
        std::vector<Widget*>  _peers;

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
        Forms::Cursor         _cursor;

        void*                 _r1;
};

} // namespace

} // namespace

#endif // include guard