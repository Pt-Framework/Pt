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
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
  MA  02110-1301  USA
*/

#ifndef Pt_Forms_Screen_H
#define Pt_Forms_Screen_H

#include <Pt/Forms/Api.h>
#include <Pt/Forms/Widget.h>
#include <Pt/Gfx/Size.h>
#include <Pt/System/Clock.h>
#include <Pt/Signal.h>

#include <vector>

namespace Pt {

namespace Forms {

class ScreenImpl;
class ApplicationImpl;

class Window;
class WindowManager;
class RescaleEvent;

/** @brief Display root for top-level windows.

    A %Screen is the runtime root for the top-level windows on one display.
    %Application provides the primary screen and connects it to the platform.
    The screen tracks attached %Window objects, routes input to the widget
    below the pointer, and requests drawing of changed regions. Applications
    normally use the screen through %Application::screen() rather than
    creating one directly.

    A screen provides the top-level %WindowManager for its display. Showing
    an unparented %Window attaches it through that manager. The manager and
    screen use non-owning relationships to the windows, so the application
    must keep every attached window alive and detach it before destruction.
    Use a %Workspace when an application needs a window manager within a
    control instead of another platform display.

    Screen coordinates are global Forms coordinates. The screen hit-tests
    windows and their content to choose the input target. When that target
    changes, %setPointer() delivers a leave event to the old widget before an
    enter event is delivered to the new one. Pointer capture and transient
    popups can override normal hit-test routing through %Application.

    Repaint requests from the hierarchy reach the screen as dirty rectangles.
    The screen combines pending rectangles and schedules one paint pass, then
    passes the resulting update to the platform implementation. %Screen is a
    backend extension point; applications should use its window manager and
    should not call its protected event and platform hooks.

    @ingroup Pt-Forms-Application
*/
class PT_FORMS_API Screen : public Widget
{
    friend class ScreenImpl;

    typedef Widget Base;

    public:
        /** @brief Creates the screen and connects it to @a app.
        */
        Screen(ApplicationImpl& app);

        /** @brief Destructor.
        */
        virtual ~Screen();


        /** @brief Attaches @a w to this screen's window manager.
        */
        void addWindow(Window& w);

        /** @brief Detaches @a w from this screen's window manager.
        */
        void removeWindow(Window& w);

        /** @brief Returns the windows currently attached to this screen.
        */
        const std::vector<Window*>& windows() const;

        /** @brief Returns the window manager that presents this screen's windows.
        */
        WindowManager& windowManager();


        /** @brief Returns the widget currently under the pointer, or null.
        */
        Widget* underPointer();

        /** @brief Makes @a widget the widget under the pointer.

            Sends a leave event to the previous widget under the pointer, if
            any, then makes @a widget the new one and sends it an enter event.
            Pass null to clear the widget under the pointer.
        */
        void setPointer(Widget* widget);

        /** @brief Sets or clears @a widget as the widget under the pointer.

            Equivalent to calling %setPointer(&widget) when @a isPointer is
            true, or clearing the widget under the pointer when it is
            @a widget and @a isPointer is false.
        */
        void setPointer(Widget& widget, bool isPointer);

    public:
        /** @brief Returns the platform implementation backing this screen.

            Intended for backend and platform implementation code, not for
            application use.
        */
        ScreenImpl* impl();

    protected:
        /** @brief Called after the platform implementation @a s has connected.
        */
        virtual void onInit(ScreenImpl& s);

        /** @brief Called before the platform implementation @a s disconnects.
        */
        virtual void onRelease(ScreenImpl& s);

        /** @brief Dispatches a %ResizeEvent for a platform resize of @a s to @a size.
        */
        virtual void onResize(ScreenImpl& s, const Gfx::SizeF& size);

        /** @brief Dispatches a %ShowEvent for a platform visibility change of @a s.
        */
        virtual void onShow(ScreenImpl& s, bool isShow);

        // TODO: name clash with Widget base class
        using Base::onShow;

    //
    // Widget
    //
    protected:
        /** @brief Finds the deepest widget at @a pos, or this screen if none.
        */
        virtual Widget* onHitTest(const Gfx::PointF& pos);

        /** @brief Returns @a pos unchanged; the screen has no parent.
        */
        virtual Gfx::PointF onToParent(const Gfx::PointF& pos) const;

        /** @brief Returns @a pos unchanged; the screen has no parent.
        */
        virtual Gfx::PointF onFromParent(const Gfx::PointF& pos) const;

        /** @brief Returns @a pos unchanged; screen coordinates are global.
        */
        virtual Gfx::PointF onToGlobal(const Gfx::PointF& pos) const;

        /** @brief Returns @a pos unchanged; screen coordinates are global.
        */
        virtual Gfx::PointF onFromGlobal(const Gfx::PointF& pos) const;

        /** @brief Forwards @a ev to the base %Widget event processing.
        */
        virtual void onProcessEvent( const Event& ev );

        /** @brief Unifies @a rect into the pending update and schedules one paint pass.

            Accumulated rectangles from repeated calls are merged into a
            single dirty area, and a %PaintEvent is committed to the
            application loop for each call. %onProcessPaintEvent() later
            collapses the resulting coalesced events into one paint pass.
        */
        virtual void onRequestRepaint(const Gfx::RectF& rect);

    protected:
        /** @brief Forwards @a ev to the base %Widget, then to the platform implementation.
        */
        virtual void onProcessRescaleEvent(const RescaleEvent& ev);

        /** @brief Forwards @a ev to the base %Widget event handling.
        */
        virtual void onRescaleEvent(const RescaleEvent& ev);

        /** @brief Forwards @a scaling to the base %Widget event handling.
        */
        virtual void onRescale(double scaling);

    protected:
        /** @brief Forwards the last coalesced update in @a ev to the platform implementation.

            Every %onRequestRepaint() call commits one event to the loop, but
            only the last of a batch of coalesced events reaches this method
            with pending updates; earlier ones are skipped here.
        */
        virtual void onProcessPaintEvent(const PaintEvent& ev);

        /** @brief Forwards @a ev to the base %Widget, then calls %onPaint().
        */
        virtual void onPaintEvent(const PaintEvent& ev);

        /** @brief Extension point for painting @a rect; does nothing by default.
        */
        virtual void onPaint(const Gfx::RectF& rect);

    protected:
        /** @brief Forwards @a ev to the base %Widget, then to the platform implementation.
        */
        virtual void onProcessResizeEvent(const ResizeEvent& ev);

        /** @brief Forwards @a ev to the base %Widget event handling.
        */
        virtual void onResizeEvent(const ResizeEvent& ev);

    protected:
        /** @brief Forwards @a ev to the platform implementation for hit-test routing.
        */
        virtual void onProcessMouseEvent(const MouseEvent& ev);

        /** @brief Forwards @a ev to the platform implementation for hit-test routing.
        */
        virtual void onProcessTouchEvent(const TouchEvent& ev);

        /** @brief Routes @a ev to the widget under the pointer, or to the platform implementation.

            Unlike other input events, a scroll event bypasses the platform
            hit test when a widget is currently under the pointer.
        */
        virtual void onProcessScrollEvent(const ScrollEvent& ev);

        /** @brief Forwards @a ev to the platform implementation for routing to the focused widget.
        */
        virtual void onProcessKeyEvent(const KeyEvent& ev);

    //
    // Responder
    //
    protected:
        /** @brief Forwards @a ev to the base %Widget responder chain.
        */
        virtual bool onMouseEvent(const MouseEvent& ev);

        /** @brief Forwards @a ev to the base %Widget responder chain.
        */
        virtual bool onTouchEvent(const TouchEvent& ev);

        /** @brief Forwards @a ev to the base %Widget responder chain.
        */
        virtual bool onScrollEvent(const ScrollEvent& ev);

        /** @brief Forwards @a ev to the base %Widget responder chain.
        */
        virtual bool onKeyEvent(const KeyEvent& ev);

    private:
        ScreenImpl*         _impl;
        Gfx::RectF          _updateRect;
        int                 _updates;
        Widget*             _pointer;
        Pt::System::Clock   _clock;
};

} // namespace

} // namespace

#endif
