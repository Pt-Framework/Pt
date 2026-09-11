/* Copyright (C) 2022 Marc Boris Duerner

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

#ifndef PT_FORMS_FORM_H
#define PT_FORMS_FORM_H

#include <Pt/Forms/Api.h>
#include <Pt/Forms/Widget.h>
#include <Pt/Forms/Control.h>
#include <Pt/Gfx/Point.h>
#include <Pt/Gfx/Size.h>
#include <Pt/Gfx/Rect.h>

namespace Pt {

namespace Forms {

///////////////////////////////////////////////////////////////////////
// TODO: move base functionality to Widget API
//
//       where to align
//       when is invalidate, relayout, repaint called
//       some onXYZRequested handlers in Widget
//
///////////////////////////////////////////////////////////////////////

//
// TODO:
//  - alignment for move/resize of window
//  - WindowImpl for native and framework windows
//  - set Decorator on Window to translate positions
//

/** @brief A view that hosts one content control and manages its form state.

    A %Form attaches one content %Control and is the point where the content
    hierarchy joins a window or another form host. It delegates measuring and
    layout to that content, forwards view requests, and coordinates focus
    traversal, shortcuts, and mnemonics for all attached controls. Use
    %setContent() to replace the content; the form does not own the control,
    so the caller must keep it alive until it is detached.

    A form is normally a %Window, but custom form hosts can derive from it.
    It has exactly one content root. That root can itself contain an arbitrary
    control tree. Replacing the content detaches the old root before attaching
    the new one. Attachment connects the complete tree to the screen, paint
    surface, responder chain, and form; detachment reverses those links. These
    links are non-owning, so destruction order remains the caller's
    responsibility.

    %relayout() queues a layout event only while the form is connected. Pending
    layout requests are combined before the form first measures its content
    and then assigns it the available bounds. Controls request layout from
    their parent rather than laying out an enclosing form directly. Custom
    form hosts override the protected measuring and layout hooks when their
    content needs a different allocation rule.

    The form maintains the focusable controls in its content tree and delivers
    focus changes as %FocusEvent objects. %focusNext() and %focusPrev() follow
    their focus indices, skipping controls whose focus policy is %NoFocus. It
    also dispatches registered shortcuts, Alt mnemonics, and Tab traversal
    before delivering ordinary keyboard input to the focused control. A
    control with %KeepFocus can prevent focus traversal away from itself.

    @ingroup Pt-Forms-Architecture
*/
class PT_FORMS_API Form : public View
{
    friend class Control;

    typedef View Base;

    protected:
        /** @brief Creates a form without a content control.
        */
        Form();

    public:
        /** @brief Detaches the current content control.
        */
        virtual ~Form();

    public:
        /** @brief Returns the current content control, or 0 when none is attached.
        */
        Control* content();

        /** @brief Returns the current content control, or 0 when none is attached.
        */
        const Control* content()  const;

        /** @brief Replaces the content root with @a control.

            The form detaches the previous root before attaching @a control. Passing 0
            removes the current root. The form borrows @a control; it must remain alive
            until it has been detached.
        */
        void setContent(Control* control);

    //
    // layouting
    //
    public:
        /** @brief Measures the preferred content size for @a policy.

            The default implementation delegates to the content control. It returns the
            policy size when no content is attached.
        */
        Gfx::SizeF measure(const SizePolicy& policy);

        /** @brief Queues a layout pass while the form is connected.

            Repeated calls before the queued layout event is processed are combined into
            one measure and layout pass.
        */
        void relayout();

    protected:
        /** @brief Queues the layout event for a requested layout pass.
        */
        virtual void onRequestRelayout();

        /** @brief Processes a queued layout event.

            The default implementation measures the content first and then lays it out.
        */
        virtual void onProcessLayoutEvent(const LayoutEvent& ev);

        /** @brief Creates the size policy used by the default layout pass.

            The default policy fixes both dimensions to the form size.
        */
        virtual Gfx::SizeF onProcessMeasure();

        /** @brief Applies the result of the default layout pass to @a rect.
        */
        virtual void onProcessLayout(const Gfx::RectF& rect);

        /** @brief Measures the content for @a policy.
        */
        virtual Gfx::SizeF onMeasure(const SizePolicy& policy);

        /** @brief Lays out the content in @a rect.

            The default implementation positions the content at the form origin and
            gives it the complete size of @a rect.
        */
        virtual void onLayout(const Gfx::RectF& rect);

    //
    // focus handling
    //
    public:
        /** @brief Returns the focused control, or 0 when no control has focus.
        */
        Control* focusControl();

        /** @brief Moves focus to the next eligible control.

            Focus traversal follows increasing focus indices, wraps at the end, and
            skips controls whose focus policy is %NoFocus.
        */
        void focusNext();

        /** @brief Moves focus to the previous eligible control.

            Focus traversal follows decreasing focus indices, wraps at the beginning,
            and skips controls whose focus policy is %NoFocus.
        */
        void focusPrev();

    protected:
        /** @brief Registers @a control for focus traversal, shortcuts, and mnemonics.
        */
        virtual void onAddElement(Control& control);

        /** @brief Removes @a control from focus traversal, shortcuts, and mnemonics.
        */
        virtual void onRemoveElement(Control& control);

        /** @brief Updates focus after @a control changes to @a policy.
        */
        virtual void onSetFocusPolicy(Control& control, FocusPolicy policy);

        /** @brief Updates the focus order after @a control changes its focus index.
        */
        virtual void onSetFocusIndex(Control& control, unsigned index);

        /** @brief Gives focus to @a control and sends the corresponding focus events.

            A control with %KeepFocus prevents focus from moving to another control.
        */
        virtual void onSetFocus(Control& control);

        /** @brief Replaces the shortcut registrations for @a control with @a keys.
        */
        virtual void onSetShortcut(Control& control, const std::vector<Key>& keys);

        /** @brief Replaces the mnemonic registrations for @a control with @a chs.
        */
        virtual void onSetMnemonic(Control& control, const std::vector<Char>& chs);

    //
    // View
    //
    protected:
        /** @brief Assigns the paint surface and translates it to the content root.
        */
        virtual void onSetSurface(PaintSurface* surface, const Gfx::PointF& pos);

        /** @brief Handles attachment of @a control to this form.
        */
        virtual void onAttach(Control& control);

        /** @brief Handles detachment of @a control from this form.
        */
        virtual void onDetach(Control& control);

        /** @brief Connects @a control to this form's screen and responder chain.
        */
        virtual void onInit(Control& control);

        /** @brief Disconnects @a control from this form's screen and responder chain.
        */
        virtual void onRelease(Control& control);

        /** @brief Converts form-local @a pos to coordinates local to @a control.
        */
        virtual Gfx::PointF onToControl(const Control& control,
                                        const Gfx::PointF& pos) const;

        /** @brief Converts coordinates local to @a control into form-local @a pos.
        */
        virtual Gfx::PointF onFromControl(const Control& control,
                                          const Gfx::PointF& pos) const;

    protected:
        /** @brief Translates a repaint request from @a control to form coordinates.
        */
        virtual void onRepaintRequest(Control& control, const Gfx::RectF& rect);

        /** @brief Queues a form layout after a request from @a control.
        */
        virtual void onRelayoutRequest(Control& control);

        /** @brief Delivers an effective enable state to @a control.

            A disabled form cannot enable its content. The default implementation
            sends an %EnableEvent directly to @a control.
        */
        virtual void onEnableRequest(Control& control, bool isEnable);

        /** @brief Makes @a control the active input target when @a active is true.

            The active control receives scroll and keyboard events before form-level
            shortcut, mnemonic, and focus handling.
        */
        virtual void onActivateRequest(Control& control, bool active);

        /** @brief Delivers a visibility request to @a control.
        */
        virtual void onShowRequest(Control& control, bool isShown);

        /** @brief Aligns and commits a move request for @a control.
        */
        virtual void onMoveRequest(Control& control, const Gfx::PointF& pos);

        /** @brief Aligns and commits a resize request for @a control.
        */
        virtual void onResizeRequest(Control& control, const Gfx::SizeF& size);

        /** @brief Receives a request to raise @a control.

            A form has one content root, so the default implementation does not
            change stacking order.
        */
        virtual void onRaiseRequest(Control& control);

    //
    // Widget
    //
    protected:
        /** @brief Connects the form and its content to @a screen.
        */
        virtual void onConnect(Screen& screen);

        /** @brief Disconnects the form and its content from the screen.
        */
        virtual void onDisconnect();


        /** @brief Returns the deepest content widget at form-local @a pos.
        */
        virtual Widget* onHitTest(const Gfx::PointF& pos);

        /** @brief Forwards a pointer capture request to the Forms application.
        */
        virtual void onRequestCapture(bool capture);

        /** @brief Receives a resize request for the form itself.

            The default implementation has no resize policy. Window-like form hosts
            override this hook to request a native or enclosing host resize.
        */
        virtual void onRequestResize(const Gfx::SizeF& s);


    protected:
        /** @brief Dispatches @a ev through the widget event dispatcher.

            The form registers its layout-event handler with that dispatcher during
            construction.
        */
        virtual void onProcessEvent(const Pt::Event& ev);

    //
    // invalidation
    //
    protected:
        /** @brief Handles the final coalesced invalidation event.
        */
        virtual void onInvalidateEvent(const InvalidateEvent& ev);

        /** @brief Clears invalidation state and requests a layout pass.
        */
        virtual void onInvalidate();

    //
    // painting
    //
    protected:
        /** @brief Paints the form and the intersecting part of its content root.

            Empty dirty rectangles are ignored. The content receives the dirty
            rectangle converted to its local coordinates.
        */
        virtual void onProcessPaintEvent(const PaintEvent& ev);

        /** @brief Paints the form's own view content.

            The default implementation uses the standard %View paint context and
            does not add form-specific drawing.
        */
        virtual void onPaintEvent(const PaintEvent& ev);

    //
    // scaling
    //
    protected:
        /** @brief Updates form scaling and propagates it to the content root.
        */
        virtual void onProcessRescaleEvent(const RescaleEvent& ev);

        /** @brief Handles a scale-change event for the form.
        */
        virtual void onRescaleEvent(const RescaleEvent& ev);

        /** @brief Updates the form's logical-to-device scaling to @a scaling.
        */
        virtual void onRescale(double scaling);

    //
    // enabling
    //
    protected:
        /** @brief Updates form enabled state and propagates it to the content root.
        */
        virtual void onProcessEnableEvent(const EnableEvent& ev);

        /** @brief Handles an enable event for the form.
        */
        virtual void onEnableEvent(const EnableEvent& ev);

        /** @brief Performs behavior associated with effective enabled state @a e.
        */
        virtual void onEnable(bool e);

    //
    // visibility
    //
    protected:
        /** @brief Updates visibility of the form itself.

            Form visibility events do not automatically change content visibility.
        */
        virtual void onProcessShowEvent(const ShowEvent& ev);

        /** @brief Handles a visibility event for the form.
        */
        virtual void onShowEvent(const ShowEvent& ev);

        /** @brief Performs behavior associated with form visibility @a visible.
        */
        virtual void onShow(bool visible);

    //
    // geometry
    //
    protected:
        /** @brief Processes a move event with the standard %View behavior.
        */
        virtual void onProcessMoveEvent(const MoveEvent& ev);

        /** @brief Repaints changed bounds and updates the form position.
        */
        virtual void onMoveEvent(const MoveEvent& ev);

        /** @brief Processes a resize event and schedules relayout when size changes.
        */
        virtual void onProcessResizeEvent(const ResizeEvent& ev);

        /** @brief Repaints changed bounds, updates size, and requests relayout.
        */
        virtual void onResizeEvent(const ResizeEvent& ev);
    //
    // input
    //
    protected:
        /** @brief Routes mouse input to the content under the pointer when possible.
        */
        virtual void onProcessMouseEvent(const MouseEvent& ev);

        /** @brief Routes touch input to the content under the pointer when possible.
        */
        virtual void onProcessTouchEvent(const TouchEvent& ev);

        /** @brief Routes scrolling to the active control, then the focused control.
        */
        virtual void onProcessScrollEvent(const ScrollEvent& sev);

        /** @brief Processes pointer entry with the standard widget responder behavior.
        */
        virtual void onProcessEnterEvent(const EnterEvent& ev);

        /** @brief Processes pointer leave with the standard widget responder behavior.
        */
        virtual void onProcessLeaveEvent(const LeaveEvent& ev);

        /** @brief Routes keyboard input through form-level interaction handling.

            The active control receives the event first. Otherwise, key presses dispatch
            registered shortcuts, Alt mnemonics, and Tab traversal before the focused
            control and finally the responder chain receive the event.
        */
        virtual void onProcessKeyEvent(const KeyEvent& ev);

    //
    // Responder
    //
    protected:
        /** @brief Does not handle mouse input and continues responder dispatch.
        */
        virtual bool onMouseEvent(const MouseEvent& ev);

        /** @brief Does not handle touch input and continues responder dispatch.
        */
        virtual bool onTouchEvent(const TouchEvent& ev);

        /** @brief Does not handle scroll input and continues responder dispatch.
        */
        virtual bool onScrollEvent(const ScrollEvent& ev);

        /** @brief Installs the current cursor and consumes pointer entry.
        */
        virtual bool onEnterEvent(const EnterEvent& ev);

        /** @brief Restores the default cursor and consumes pointer leave.
        */
        virtual bool onLeaveEvent(const LeaveEvent& ev);

        /** @brief Does not handle key input and continues responder dispatch.
        */
        virtual bool onKeyEvent(const KeyEvent& ev);

    private:
        template <typename Iter>
        void moveFocus(Iter begin, Iter end);

    protected:
        /** @brief Returns the shortcut-to-control registrations for this form.
        */
        const std::map<Key, Control*>& shortcuts() const
        {
            return _shortcuts;
        }

        /** @brief Returns the mnemonic-to-control registrations for this form.
        */
        const std::map<Pt::Char, Control*>& mnemonics() const
        {
            return _mnemonics;
        }

    private:
        Control*                      _mainControl;

        int                           _layouts;

        Control*                      _active;

        std::vector<Control*>         _focusList;
        Control*                      _focusControl;
        std::map<Key, Control*>       _shortcuts;
        std::map<Pt::Char, Control*>  _mnemonics;

};

} // namespace

} // namespace

#endif // include guard
