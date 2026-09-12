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
  02110-1301  USA
*/

#ifndef PT_FORMS_WINDOW_H
#define PT_FORMS_WINDOW_H

#include <Pt/Forms/Api.h>
#include <Pt/Forms/Form.h>
#include <Pt/Forms/PixmapSurface.h>
#include <Pt/Forms/WindowType.h>
#include <Pt/Forms/SizePolicy.h>
#include <Pt/Forms/ActivateEvent.h>
#include <Pt/Forms/CloseEvent.h>
#include <Pt/Forms/ResizeEvent.h>
#include <Pt/Forms/KeyEvent.h>
#include <Pt/Forms/MouseEvent.h>
#include <Pt/Forms/TouchEvent.h>
#include <Pt/Forms/ScrollEvent.h>
#include <Pt/Forms/MoveEvent.h>
#include <Pt/Forms/EnterEvent.h>
#include <Pt/Forms/LeaveEvent.h>
#include <Pt/Forms/ShowEvent.h>
#include <Pt/Forms/EnableEvent.h>
#include <Pt/Forms/InvalidateEvent.h>
#include <Pt/Gfx/Image.h>
#include <Pt/Signal.h>

namespace Pt {

namespace Forms {

class WindowFrame;
class WindowManager;
class WindowStateEvent;
class PaintEvent;

/** @brief Form presented by a window manager.

    A %Window is a %Form shown by a %WindowManager. Show it after attaching
    its content with %setContent(). The window translates form requests for
    geometry, visibility, activation, repainting, and input into operations
    on the platform window. Its content remains owned by the caller and must
    outlive its attachment.

    Construct a window, configure its content, title, icon, geometry, and
    presentation state, and then call %show(). An unparented window is added
    to the primary screen's window manager when it is shown. Use %setParent()
    only to select another manager, for example the manager of a workspace.
    The manager supplies a frame that connects the window to the screen and
    applies pending configuration when that connection is made.

    %move() and %resize() request normal-window geometry. %autoSize() measures
    the content with a %SizePolicy, while %autoCenter() asks the current frame
    to choose a position in its manager's available area. A manual move or
    resize returns the window to the normal state and can end the corresponding
    automatic behavior. %setAbove() controls stacking within the current
    manager; it is not an operating-system-wide always-on-top guarantee.

    Visibility, enabled state, and activation are requested from the frame and
    confirmed by the corresponding Forms events. A window accepts pointer,
    touch, scroll, and keyboard input only while it is visible and enabled.
    %close() requests a close transition. Once processed, the window is hidden,
    detached, marked closed, and emits %closed(); it is not destroyed.
    %showModal() runs a nested event loop and temporarily disables the other
    top-level windows, so reserve it for intentionally synchronous dialogs.

    @ingroup Pt-Forms-Application
*/
class PT_FORMS_API Window : public Form
{
    friend class WindowManager;
    friend class WindowFrame;

    public:
        typedef Form Base;
        typedef WindowType Type;
        typedef WindowState State;

    public:
        /** @brief Creates a detached, enabled window of @a type.

            The window has an initial requested size of 80 by 80 logical
            units. If @a parent is not null, the constructor attaches the
            window to that manager.
        */
        explicit Window(WindowManager* parent = 0,
                        WindowType type = WindowType::Default);

        /** @brief Detaches the window from its manager and releases its frame.
        */
        virtual ~Window();

    public:
        /** @brief Attaches the window to @a parent.

            Detaches the window from its current manager first. Reattaching a
            closed window clears its closed state. Calling this function with
            the current manager has no effect.
        */
        void setParent(WindowManager& parent);

        /** @brief Detaches the window from its current manager.

            Does nothing while the window is detached.
        */
        void unparent();

        /** @brief Returns the current non-owning window manager, or null.
        */
        WindowManager* windowManager();

        /** @brief Returns the current non-owning window manager, or null.
        */
        const WindowManager* windowManager() const;

        /** @brief Returns the internal frame that presents the window, or null.

            The frame is released when the window detaches. Do not retain it.
        */
        WindowFrame* frame();

        /** @brief Returns the internal frame that presents the window, or null.
        */
        const WindowFrame* frame() const;

    public:
        /** @brief Copies the complete window content into @a bitmap.

            The window must be attached to provide bitmap content.
        */
        void getBitmap(Gfx::Bitmap& bitmap) const;

        /** @brief Copies a window content rectangle into @a bitmap.

            @a rect is in window logical coordinates. The copied area is the
            intersection with the window content. If the intersection is
            empty, @a bitmap is reset to empty.
        */
        void getBitmap(Gfx::Bitmap& bitmap, const Gfx::RectF& rect) const;

    public:
        /** @brief Centers the window in its manager's available area.

            Defers the request until the window connects when called before
            connection.
        */
        void autoCenter();

        /** @brief Enables automatic sizing with @a policy.

            A connected window is measured immediately and requests the
            resulting size. Subsequent measurement and layout can update the
            size as its content changes.
        */
        void autoSize(const SizePolicy& policy);

        /** @brief Enables automatic sizing with preferred width and height.
        */
        void autoSize();

    public:
        /** @brief Returns the type selected when the window was created.
        */
        Type type() const;

        /** @brief Returns the window icon.
        */
        const Gfx::Image& icon() const;

        /** @brief Sets the window icon to a copy of @a icon.

            Updates the frame immediately when the window is attached.
        */
        void setIcon(const Gfx::Image& icon);

        /** @brief Returns the window title.
        */
        const std::string& title() const;

        /** @brief Sets the window title to @a title.

            Updates the frame immediately when the window is attached.
        */
        void setTitle( const std::string& title );

        /** @brief Returns whether the window requests manager-relative stacking.
        */
        bool isAbove() const;

        /** @brief Changes the manager-relative stacking preference to @a above.

            This is not an operating-system-wide always-on-top setting.
        */
        void setAbove(bool above);

        /** @brief Returns the current locally stored activation state.
        */
        bool isActive() const;


        // TODO: setFullScreen()

        /** @brief Returns the requested window state.
        */
        WindowState state() const;

        /** @brief Sets the requested window state to @a state.

            A connected window forwards the request to its frame.
        */
        void setState(const WindowState& state);

        /** @brief Runs the window as a modal dialog until it closes.

            The method enables and shows this window, disables the screen's
            other top-level windows, and processes application events until a
            close event completes. It then enables those windows and restores
            activation to the window that was active when modal operation
            started.
        */
        void showModal();

        /** @brief Returns whether a close event has completed the close transition.
        */
        bool isClosed() const;

        /** @brief Requests that the current frame close the window.

            Closing is asynchronous. A processed close event hides and detaches
            the window, marks it closed, and sends %closed().
        */
        void close();

        /** @brief Returns the signal sent after the close transition completes.
        */
        Signal<>& closed();

        /** @brief Returns whether the window accepts pointer and keyboard input.

            A window accepts input only while it is visible and enabled.
        */
        bool acceptsInput() const;

    public:
        /** @brief Returns the background override or the application background.
        */
        const Gfx::Brush& background() const;

        /** @brief Copies @a background as the window background and invalidates it.
        */
        void setBackground(const Gfx::Brush& background);

    //
    // Form
    //
    protected:
        /** @brief Updates automatic sizing and performs the standard measurement.
        */
        virtual Gfx::SizeF onProcessMeasure();

        /** @brief Applies automatic sizing and performs the standard layout.
        */
        virtual void onProcessLayout(const Gfx::RectF& rect);

        /** @brief Measures the content with @a policy.
        */
        virtual Gfx::SizeF onMeasure(const SizePolicy& policy);

        /** @brief Lays out the content in @a rect.
        */
        virtual void onLayout(const Gfx::RectF& rect);

    //
    // Widget
    //
    protected:
        /** @brief Connects the window and applies its stored frame configuration.

            Overrides must call the base implementation to apply geometry,
            title, icon, state, visibility, enabled state, and activation.
        */
        virtual void onConnect(Screen& screen);

        /** @brief Disconnects the window form from its screen.
        */
        virtual void onDisconnect();

        /** @brief Finds the deepest visible widget at @a point.

            Returns this window when @a point is inside its bounds but does not
            hit its content.
        */
        virtual Widget* onHitTest(const Gfx::PointF& point);

        /** @brief Converts window coordinates @a pos to parent coordinates.
        */
        virtual Gfx::PointF onToParent(const Gfx::PointF& pos) const;

        /** @brief Converts parent coordinates @a pos to window coordinates.
        */
        virtual Gfx::PointF onFromParent(const Gfx::PointF& pos) const;

        /** @brief Dispatches @a event through the standard form event path.
        */
        virtual void onProcessEvent(const Pt::Event& ev);

        /** @brief Requests repainting @a rect from the current frame.
        */
        virtual void onRequestRepaint(const Gfx::RectF& rect);

        /** @brief Stores and forwards the enabled request @a enabled.
        */
        virtual void onRequestEnable(bool enabled);

        /** @brief Stores and forwards the activation request @a active.
        */
        virtual void onRequestActivate(bool active);

        /** @brief Stores the visibility request @a shown and presents the window.

            An unattached window is added to the primary screen's manager.
        */
        virtual void onRequestShow(bool shown);

        /** @brief Records and forwards a normal-state move request to @a pos.

            Cancels pending automatic centering.
        */
        virtual void onRequestMove(const Gfx::PointF& pos);

        /** @brief Records and forwards a normal-state resize request to @a size.
        */
        virtual void onRequestResize(const Gfx::SizeF& size);

        /** @brief Applies and forwards the size limits to the current frame.
        */
        virtual void onSetSizeLimits(const Gfx::SizeF& minSize,
                                     const Gfx::SizeF& maxSize);

    //
    // invalidation
    //
    protected:
        /** @brief Processes an invalidation event through the standard form path.
        */
        virtual void onInvalidateEvent(const InvalidateEvent& ev);

        /** @brief Updates the cached background and repaints the complete window.
        */
        virtual void onInvalidate();

    //
    // painting
    //
    protected:
        /** @brief Paints a visible, non-empty dirty region from @a event.
        */
        virtual void onProcessPaintEvent(const PaintEvent& event);

        /** @brief Fills @a rect with the current window background.
        */
        virtual void onPaint(PaintContext& context, const Gfx::RectF& rect);

    //
    // scaling
    //
    protected:
        /** @brief Processes a scale event through the standard form path.
        */
        virtual void onProcessRescaleEvent(const RescaleEvent& ev);

        /** @brief Handles a scale event after standard form processing.
        */
        virtual void onRescaleEvent(const RescaleEvent& ev);

        /** @brief Updates scaling and realigns normal-state window geometry.
        */
        virtual void onRescale(double scaling);

    //
    // geometry
    //
    protected:
        /** @brief Processes a move event through the standard form path.
        */
        virtual void onProcessMoveEvent(const MoveEvent& ev);

        /** @brief Updates move state without repainting the window surface.
        */
        virtual void onMoveEvent(const MoveEvent& ev);

        /** @brief Processes a resize event through the standard form path.
        */
        virtual void onProcessResizeEvent(const ResizeEvent& ev);

        /** @brief Updates resize state and the requested window size.
        */
        virtual void onResizeEvent(const ResizeEvent& ev);

    //
    // visibility
    //
    protected:
        /** @brief Processes a visibility event through the standard form path.
        */
        virtual void onProcessShowEvent(const ShowEvent& ev);

        /** @brief Processes @a event and invalidates the window.
        */
        virtual void onShowEvent(const ShowEvent& event);

        /** @brief Updates the visible state to @a visible.
        */
        virtual void onShow(bool visible);

    //
    // enabling
    //
    protected:
        /** @brief Processes an enabled event without enabling against a disable request.
        */
        virtual void onProcessEnableEvent(const EnableEvent& ev);

        /** @brief Handles an enabled event through the standard form path.
        */
        virtual void onEnableEvent(const EnableEvent& ev);

        /** @brief Updates the enabled state to @a enabled and invalidates the window.
        */
        virtual void onEnable(bool enabled);

    //
    // activation
    //
    protected:
        /** @brief Dispatches @a event to onActivateEvent().
        */
        virtual void onProcessActivateEvent(const ActivateEvent& event);

        /** @brief Updates the active state from @a event.
        */
        virtual void onActivateEvent(const ActivateEvent& event);

    //
    // window state
    //
    protected:
        /** @brief Dispatches @a event to onWindowStateEvent().
        */
        virtual void onProcessWindowStateEvent(const WindowStateEvent& event);

        /** @brief Updates the window state from @a event.
        */
        virtual void onWindowStateEvent(const WindowStateEvent& event);

    //
    // closing
    //
    protected:
        /** @brief Dispatches @a event to onCloseEvent().
        */
        virtual void onProcessCloseEvent(const CloseEvent& event);

        /** @brief Hides, detaches, closes, and sends %closed().
        */
        virtual void onCloseEvent(const CloseEvent& ev);

    //
    // input
    //
    protected:
        /** @brief Routes @a event when the window accepts input.
        */
        virtual void onProcessMouseEvent(const MouseEvent& event);

        /** @brief Routes @a event when the window accepts input.
        */
        virtual void onProcessTouchEvent(const TouchEvent& event);

        /** @brief Routes @a event when the window accepts input.
        */
        virtual void onProcessScrollEvent(const ScrollEvent& event);

        /** @brief Routes the pointer-entry @a event.
        */
        virtual void onProcessEnterEvent(const EnterEvent& event);

        /** @brief Routes the pointer-leave @a event.
        */
        virtual void onProcessLeaveEvent(const LeaveEvent& event);

        /** @brief Routes @a event when the window accepts input.
        */
        virtual void onProcessKeyEvent(const KeyEvent& event);

    //
    // Responder
    //
    protected:
        /** @brief Handles a mouse event and returns whether it was consumed.
        */
        virtual bool onMouseEvent(const MouseEvent& ev);

        /** @brief Handles a touch event and returns whether it was consumed.
        */
        virtual bool onTouchEvent( const TouchEvent& ev );

        /** @brief Handles a scroll event and returns whether it was consumed.
        */
        virtual bool onScrollEvent(const ScrollEvent& ev);

        /** @brief Handles a key event and returns whether it was consumed.
        */
        virtual bool onKeyEvent(const KeyEvent& ev);

        /** @brief Restores the default cursor, handles the event, and returns consumption.
        */
        virtual bool onEnterEvent(const EnterEvent& ev);

        /** @brief Handles a pointer-leave event and returns whether it was consumed.
        */
        virtual bool onLeaveEvent(const LeaveEvent& ev);

    //
    // Implementation
    //
    private:
        bool isAutoSizeActive(const Gfx::SizeF& s);

    private:
        WindowFrame*                 _frame;
        WindowManager*               _wm;

        bool                         _show;
        bool                         _isActive;
        bool                         _enabled;
        bool                         _isClosed;

        Pt::Signal<>                 _closed;

        Gfx::PointF                  _requestedPosition;
        Gfx::SizeF                   _requestedSize;

        SizePolicy                   _autoSizePolicy;
        bool                         _autoSize;
        Gfx::SizeF                   _lastAutoSize;
        bool                         _autoCenter;

        Type                         _type;
        std::string                  _title;
        Gfx::Image                   _icon;
        State                        _state;
        bool                         _isAbove;

        AutoPtr<Gfx::Brush>          _background;
        Gfx::Brush                   _backgroundBrush;
};

} // namespace

} // namespace

#endif // include guard
