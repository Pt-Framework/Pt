/* Copyright (C) 2015-2026 Marc Boris Duerner

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

/** @brief Common type of every visual Forms object.

    Every visual Forms object is a %Widget. %Application is not. A widget
    supplies identity, screen connection, parent relationships, geometry,
    visibility, enabled state, scaling, coordinate conversion, repaint
    requests, event dispatch, and pointer capture. Derive ordinary visual
    content from %Control. Derive directly from %Widget only for a runtime
    or host base with its own parent coordinate system; derived classes must
    implement %onToParent() and %onFromParent().

    Construct widgets only after an %Application exists. The application
    gives each widget an ID, registers it for event routing, and removes that
    entry during destruction. A name is optional metadata, not an identifier.
    Parent and screen pointers are non-owning runtime relationships. A widget
    is connected when its parent hierarchy reaches a screen, independent of
    its visibility. Containers attach and detach widgets through their public
    APIs; callers keep attached widgets alive.

    Geometry uses logical coordinates. %position() is in parent coordinates,
    while %size() and %bounds() are local and %bounds() starts at the local
    origin. %toParent(), %fromParent(), %toGlobal(), and %fromGlobal() convert
    between these systems. The default global conversion walks the parent
    chain. Coordinate hooks must agree with hit testing, painting, and input
    delivery.

    %show(), %enable(), %activate(), %move(), and %resize() are requests. A
    parent, frame, or backend confirms them by sending Forms events, which
    then update the observable state. Names, size limits, cursor overrides,
    and the next responder change local state immediately. %invalidate()
    queues coalesced deferred work; %repaint() asks to redraw a local dirty
    region.

    A widget is a %Responder. Unhandled input continues to the next
    responder, with mouse and touch coordinates converted locally.

    Peers are non-hierarchical, bidirectional associations. They do not
    establish ownership, parenting, or screen connection. A widget can also
    provide a %Cursor while it is under the pointer; clearing the override
    restores the default cursor.

    @ingroup Pt-Forms-Application
*/
class PT_FORMS_API Widget : public Responder
                          , public Pt::Connectable
{
    protected:
        /** @brief Creates and registers a widget with the Forms application.
        */
        Widget();

    public:
        /** @brief Releases pointer capture and peer associations.

            The widget unregisters itself from the Forms application. It does
            not destroy a parent, screen, peer, or next responder.
        */
        virtual ~Widget();

        /** @brief Returns the application-unique ID of this live widget.
        */
        Pt::uint64_t id() const;

        /** @brief Returns the optional name.
        */
        const std::string& name() const;

        /** @brief Sets the optional name immediately.
        */
        void setName(const std::string& n);

        /** @brief Sets the next responder for unhandled input.

            The widget does not own @a r. Do not create a responder cycle.
        */
        void setNextResponder(Responder* r);

    public:
        /** @brief Returns true when the widget is connected to a screen.
        */
        bool isConnected() const;

        /** @brief Returns the connected screen, or 0 when disconnected.
        */
        Screen* screen();

        /** @brief Returns the connected screen, or 0 when disconnected.
        */
        const Screen* screen() const;

        /** @brief Returns the non-owning parent, or 0 when unparented.
        */
        Widget* parent();

        /** @brief Returns the non-owning parent, or 0 when unparented.
        */
        const Widget* parent() const;

        /** @brief Returns true when this widget is a strict descendant of @a top.
        */
        bool isDescendantOf(const Widget& top) const;

        /** @brief Returns true when this widget is a strict ancestor of @a child.
        */
        bool isAncestorOf(const Widget& child) const;

        /** @brief Returns the descendant hit at local position @a pos.

            Returns 0 when no descendant is hit. Derived classes implement
            their child hit-testing policy in %onHitTest().
        */
        Widget* hitTest(const Gfx::PointF& pos);

        /** @brief Converts local position @a pos to parent coordinates.
        */
        Gfx::PointF toParent(const Gfx::PointF& pos) const;

        /** @brief Converts parent position @a pos to local coordinates.
        */
        Gfx::PointF fromParent(const Gfx::PointF& pos) const;

        /** @brief Converts local position @a pos to global coordinates.

            The default conversion walks the parent chain.
        */
        Gfx::PointF toGlobal(const Gfx::PointF& pos) const;

        /** @brief Converts global position @a pos to local coordinates.

            The default conversion walks the parent chain.
        */
        Gfx::PointF fromGlobal(const Gfx::PointF& pos) const;

    public:
        /** @brief Adds a bidirectional, non-owning peer association with @a peer.

            Peers are not a parent, child, or screen connection and do not
            transfer ownership. Add a relationship only once. Both endpoints
            receive %onAttachPeer().
        */
        void addPeer(Widget& peer);

        /** @brief Removes the bidirectional peer association with @a peer.

            Both endpoints receive %onDetachPeer().
        */
        void removePeer(Widget& peer);

    public:
        /** @brief Queues a coalesced invalidation.

            The Forms event loop eventually calls %onInvalidateEvent() after
            all pending invalidation requests for this widget have coalesced.
        */
        void invalidate();

    public:
        /** @brief Requests repainting local dirty rectangle @a rect.

            The containing view, frame, or backend converts and forwards the
            rectangle. This does not paint synchronously.
        */
        virtual void repaint(const Gfx::RectF& rect);

        /** @brief Requests repainting the complete local bounds.
        */
        virtual void repaint();

    public:
        /** @brief Returns the current logical-to-device scale factor.
        */
        double scaleFactor() const;

        /** @brief Returns the current logical-to-device scaling.
        */
        const Gfx::Scaling& scaling() const;

    public:
        /** @brief Returns whether the last show event made the widget visible.
        */
        bool isVisible() const;

        /** @brief Requests that the widget be shown or hidden.

            %isVisible() changes only when the request is confirmed by a
            %ShowEvent.
        */
        virtual void show(bool b = true);

    public:
        /** @brief Returns whether the last enable event made the widget enabled.
        */
        bool isEnabled() const;

        /** @brief Requests that the widget be enabled or disabled.

            %isEnabled() changes only when the request is confirmed by an
            %EnableEvent.
        */
        virtual void enable(bool isEnable = true);

    public:
        /** @brief Requests that the widget become active or inactive.

            Activation is defined by the receiving parent, frame, or backend.
        */
        void activate(bool active = true);

    public:
        /** @brief Returns the position in parent logical coordinates.
        */
        const Gfx::PointF& position() const;

        /** @brief Requests a new parent-local position @a pos.

            %position() changes only when a %MoveEvent confirms the request.
        */
        virtual void move(const Gfx::PointF& pos);

        /** @brief Returns the size in local logical coordinates.
        */
        const Gfx::SizeF& size() const;

        /** @brief Returns local bounds, whose origin is always local zero.
        */
        const Gfx::RectF& bounds() const;


        /** @brief Returns the locally stored minimum size.

            New widgets use `(0, 0)`.
        */
        const Gfx::SizeF& minimumSize() const;

        /** @brief Sets the local minimum size and notifies the size-limit hook.

            This does not resize the widget or validate it against the maximum
            size. The receiver of %onSetSizeLimits() applies the limits.
        */
        void setMinimumSize(const Gfx::SizeF& s);

        /** @brief Sets the local minimum width and height.
        */
        void setMinimumSize(double w, double h);

        /** @brief Sets the local minimum width.
        */
        void setMinimumWidth(double w);

        /** @brief Sets the local minimum height.
        */
        void setMinimumHeight(double h);


        /** @brief Returns the locally stored maximum size.

            New widgets use `(64000, 64000)`.
        */
        const Gfx::SizeF& maximumSize() const;

        /** @brief Sets the local maximum size and notifies the size-limit hook.

            This does not resize the widget or validate it against the minimum
            size. The receiver of %onSetSizeLimits() applies the limits.
        */
        void setMaximumSize(const Gfx::SizeF& s);

        /** @brief Sets the local maximum width and height.
        */
        void setMaximumSize(double w, double h);

        /** @brief Sets the local maximum width.
        */
        void setMaximumWidth(double w);

        /** @brief Sets the local maximum height.
        */
        void setMaximumHeight(double h);


        /** @brief Requests a new local size @a s.

            %size() and %bounds() change only when a %ResizeEvent confirms the
            request.
        */
        virtual void resize(const Gfx::SizeF& s);

    public:
        /** @brief Requests pointer input capture or release.

            Capture is owned by the application and routes pointer input to
            this widget until released. It is released during destruction.
        */
        void setCapture(bool capture);

    public:
        /** @brief Returns this widget's cursor or the default cursor.

            The returned cursor is always non-null and is owned by the widget
            or the Forms runtime.
        */
        const Cursor* cursor() const;

        /** @brief Sets a local cursor override.

            Pass 0 to remove the override and restore the default cursor.
            The cursor is copied, so the caller retains ownership of @a c.
            When this widget is under the pointer, the platform cursor is
            updated immediately.
        */
        void setCursor(const Cursor* c);

    public:
        /** @brief Processes @a ev through this widget's event dispatcher.

            Use the state-request APIs for ordinary operation. This method is
            for backends, tests, and code that deliberately injects events.
        */
        void processEvent(const Pt::Event& ev);

        /** @brief Returns the event dispatcher signal.

            Internal event-processing slots are connected to this signal.
            Additional slots can observe or process deliberately dispatched
            widget events.
        */
        Pt::Signal<const Pt::Event&>& eventReceived();

    protected:
        /** @brief Records the non-owning parent relationship.

            Container classes call this while attaching or detaching the
            widget. Overrides that retain the base relationship must call this
            implementation.
        */
        virtual void onSetParent(Widget* parent);

        /** @brief Records connection to @a screen.

            A container calls this when the hierarchy joins a screen. Overrides
            that retain the base connection state must call this implementation.
        */
        virtual void onConnect(Screen& screen);

        /** @brief Clears the screen connection.

            A container calls this when the hierarchy leaves its screen.
            Overrides that retain the base connection state must call this
            implementation.
        */
        virtual void onDisconnect();

        /** @brief Returns the descendant hit at local position @a pos.

            The base implementation returns 0. Derived containers define their
            child hit-testing order, visibility rules, and coordinate mapping.
        */
        virtual Widget* onHitTest(const Gfx::PointF& pos);

        /** @brief Converts local position @a pos to parent coordinates.

            Every direct %Widget subclass must implement this mapping.
        */
        virtual Gfx::PointF onToParent(const Gfx::PointF& pos) const = 0;

        /** @brief Converts parent position @a pos to local coordinates.

            Every direct %Widget subclass must implement this mapping.
        */
        virtual Gfx::PointF onFromParent(const Gfx::PointF& pos) const = 0;

        /** @brief Converts local position @a pos to global coordinates.

            The base implementation walks the parent chain through
            %onToParent().
        */
        virtual Gfx::PointF onToGlobal(const Gfx::PointF& pos) const;

        /** @brief Converts global position @a pos to local coordinates.

            The base implementation walks the parent chain through
            %onFromParent().
        */
        virtual Gfx::PointF onFromGlobal(const Gfx::PointF& pos) const;

    protected:
        /** @brief Attaches non-owning peer @a peer.

            The base implementation stores the peer. Overrides that retain the
            association must call this implementation.
        */
        virtual void onAttachPeer(Widget& peer);

        /** @brief Detaches non-owning peer @a peer.

            The base implementation removes one stored association. Overrides
            that retain the association must call this implementation.
        */
        virtual void onDetachPeer(Widget& peer);

    protected:
        /** @brief Forwards a repaint request for local rectangle @a rect.
        */
        virtual void onRequestRepaint(const Gfx::RectF& rect);

        /** @brief Forwards a request to show or hide the widget.
        */
        virtual void onRequestShow(bool e);

        /** @brief Forwards a request to enable or disable the widget.
        */
        virtual void onRequestEnable(bool isEnable);

        /** @brief Forwards a request to activate or deactivate the widget.
        */
        virtual void onRequestActivate(bool active);

        /** @brief Forwards a request for parent-local position @a pos.
        */
        virtual void onRequestMove(const Gfx::PointF& pos);

        /** @brief Notifies that locally stored size limits changed.

            The base implementation does nothing. A parent, frame, or backend
            can apply @a minSize and @a maxSize to its geometry policy.
        */
        virtual void onSetSizeLimits(const Gfx::SizeF& minSize,
                                     const Gfx::SizeF& maxSize);

        /** @brief Forwards a request for local size @a s.
        */
        virtual void onRequestResize(const Gfx::SizeF& s);

        /** @brief Requests application-managed pointer capture or release.
        */
        virtual void onRequestCapture(bool capture);

    protected:
        /** @brief Sends @a ev to the widget event dispatcher.
        */
        virtual void onProcessEvent(const Pt::Event& ev);

    protected:
        /** @brief Coalesces and dispatches an invalidation event.
        */
        virtual void onProcessInvalidateEvent(const InvalidateEvent& ev);

        /** @brief Handles the final coalesced invalidation event.

            The base implementation calls %onInvalidate().
        */
        virtual void onInvalidateEvent(const InvalidateEvent& ev);

        /** @brief Updates derived state after invalidation.

            The base implementation clears pending invalidation bookkeeping.
        */
        virtual void onInvalidate();

    protected:
        /** @brief Filters and dispatches a paint event.

            Empty dirty rectangles are ignored.
        */
        virtual void onProcessPaintEvent(const PaintEvent& ev);

        /** @brief Handles a paint event with a non-empty dirty rectangle.
        */
        virtual void onPaintEvent(const PaintEvent& ev);

    protected:
        /** @brief Dispatches a rescale event.
        */
        virtual void onProcessRescaleEvent(const RescaleEvent& ev);

        /** @brief Handles a rescale event.

            The base implementation calls %onRescale() with the event scale.
        */
        virtual void onRescaleEvent(const RescaleEvent& ev);

        /** @brief Updates the local scaling to @a scaling.
        */
        virtual void onRescale(double scaling);

    protected:
        /** @brief Ignores redundant show events and dispatches other ones.
        */
        virtual void onProcessShowEvent(const ShowEvent& ev);

        /** @brief Handles a show event.

            The base implementation calls %onShow().
        */
        virtual void onShowEvent(const ShowEvent& ev);

        /** @brief Updates the local visibility state to @a visible.
        */
        virtual void onShow(bool visible);

    protected:
        /** @brief Dispatches an enable event.
        */
        virtual void onProcessEnableEvent(const EnableEvent& ev);

        /** @brief Updates the enabled state and handles an enable event.

            The base implementation updates %isEnabled() before calling
            %onEnable().
        */
        virtual void onEnableEvent(const EnableEvent& ev);

        /** @brief Performs behavior associated with enabled state @a e.
        */
        virtual void onEnable(bool e);

    protected:
        /** @brief Dispatches a move event.
        */
        virtual void onProcessMoveEvent(const MoveEvent& ev);

        /** @brief Updates the parent-local position from a move event.
        */
        virtual void onMoveEvent(const MoveEvent& ev);

        /** @brief Dispatches a resize event.
        */
        virtual void onProcessResizeEvent(const ResizeEvent& ev);

        /** @brief Updates the local size and bounds from a resize event.
        */
        virtual void onResizeEvent(const ResizeEvent& ev);

    protected:
        /** @brief Updates pointer tracking and delivers a mouse event.
        */
        virtual void onProcessMouseEvent(const MouseEvent& ev);

        /** @brief Updates pointer tracking and delivers a touch event.
        */
        virtual void onProcessTouchEvent(const TouchEvent& ev);

        /** @brief Delivers a scroll event through the responder chain.
        */
        virtual void onProcessScrollEvent(const ScrollEvent& ev);

        /** @brief Delivers an enter event through the responder chain.
        */
        virtual void onProcessEnterEvent(const EnterEvent& ev);

        /** @brief Delivers a leave event through the responder chain.
        */
        virtual void onProcessLeaveEvent(const LeaveEvent& ev);

        /** @brief Delivers a key event through the responder chain.
        */
        virtual void onProcessKeyEvent(const KeyEvent& ev);

    //
    // Responder
    //
    protected:
        /** @brief Returns the non-owning next responder, or 0.
        */
        virtual Responder* onNextResponder();

        /** @brief Handles a mouse event in local coordinates.

            Return true to consume it. Returning false forwards the original
            event to the next responder.
        */
        virtual bool onMouseEvent(const MouseEvent& ev);

        /** @brief Handles a touch event in local coordinates.

            Return true to consume it. Returning false forwards the original
            event to the next responder.
        */
        virtual bool onTouchEvent(const TouchEvent& ev);

        /** @brief Handles a scroll event.

            Return true to consume it; otherwise it is forwarded to the next
            responder.
        */
        virtual bool onScrollEvent( const ScrollEvent& ev);

        /** @brief Handles pointer entry and installs the widget cursor.

            The base implementation consumes the event. Override and return
            false only when it should continue through the responder chain.
        */
        virtual bool onEnterEvent( const EnterEvent& ev);

        /** @brief Handles pointer leave and restores the default cursor.

            The base implementation consumes the event. Override and return
            false only when it should continue through the responder chain.
        */
        virtual bool onLeaveEvent(const LeaveEvent& ev);

        /** @brief Handles a key event.

            Return true to consume it; otherwise it is forwarded to the next
            responder.
        */
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