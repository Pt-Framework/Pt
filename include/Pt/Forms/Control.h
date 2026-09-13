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

#ifndef PT_FORMS_CONTROL_H
#define PT_FORMS_CONTROL_H

#include <Pt/Forms/View.h>
#include <Pt/Forms/SizePolicy.h>
#include <Pt/Forms/Spacing.h>

#include <Pt/Forms/MouseEvent.h>
#include <Pt/Forms/TouchEvent.h>
#include <Pt/Forms/ScrollEvent.h>
#include <Pt/Forms/KeyEvent.h>
#include <Pt/Forms/ResizeEvent.h>
#include <Pt/Forms/MoveEvent.h>
#include <Pt/Forms/PaintEvent.h>
#include <Pt/Forms/EnterEvent.h>
#include <Pt/Forms/LeaveEvent.h>
#include <Pt/Forms/EnableEvent.h>
#include <Pt/Forms/InvalidateEvent.h>
#include <Pt/Forms/LayoutEvent.h>
#include <Pt/Forms/ShowEvent.h>
#include <Pt/Forms/FocusEvent.h>

#include <Pt/Gfx/PaintSurface.h>
#include <Pt/Gfx/Point.h>
#include <Pt/Gfx/Size.h>
#include <Pt/Gfx/Color.h>

#include <Pt/Signal.h>
#include <Pt/Delegate.h>
#include <vector>

namespace Pt {

namespace Forms {

class Form;
class Key;

/** @brief Reusable view used as application content.

    A %Control is the building block of a Forms user interface. Derive from
    it to implement custom content. It can be the content of a %Form or the
    child of another view, and it can attach child controls of its own.
    Attachment is non-owning: a parent stores control pointers but does not
    destroy its children.

    A control's parent is a %View, and its direct children are controls.
    %add() first detaches a child from any old parent; %remove() detaches it
    without destroying it. When the tree joins a connected form, connection,
    form association, responder chain, scaling, and paint surface propagate
    through all descendants. Removing a control reverses those runtime
    relationships. Keep every attached control alive until it has been
    removed or its parent is gone.

    The parent paints children in stacking order and hit-tests them from
    front to back. %raise() moves a direct child to the front. Controls
    forward repaint and relayout requests to their parent, which converts
    the requested local region to its own coordinates. A control measures
    under the supplied %SizePolicy and caches the preferred size until
    content, policy, limits, or scaling invalidate it. Its layout hook
    assigns geometry to its direct children.

    The default measurement returns an empty size and the default layout
    does not assign child geometry. A derived container measures its content
    in %onMeasure() and places each child with %move() and %resize() in
    %onLayout(). Call %relayout() after a content change that affects size
    or geometry and %invalidate() after a visual-only change.

    Once associated with a form, a control may take part in focus traversal,
    action keys, shortcuts, and mnemonics. Override %onMeasure(),
    %onLayout(), %onPaint(), and the protected input hooks as required; use
    state-changing public APIs so changes travel through the parent
    hierarchy.

    @ingroup Pt-Forms-Widgets
*/
class PT_FORMS_API Control : public View
{
    typedef View Base;

    friend class Form;

    public:
        /** @brief Creates an unparented control.

            An %Application must exist before a control is constructed.
        */
        Control();

        /** @brief Detaches this control from its parent and releases its children.

            Attached controls are not destroyed.
        */
        virtual ~Control();

    public:
        /** @brief Attaches this control to @a parent.

            The parent does not take ownership. If this control already has a
            parent, it is detached first. Attachment initializes the runtime
            relationships supplied by the parent, including form association,
            responder chain, screen connection, scaling, and paint surface.
        */
        void setParent(View& parent);

        /** @brief Detaches this control from its current parent.

            Detachment removes the runtime relationships established during
            attachment but does not destroy this control.
        */
        void unparent();

        /** @brief Attaches @a control as a direct child.

            The child remains owned by its caller. Direct children paint in
            attachment order and are hit-tested in reverse order.
        */
        void add(Control& control);

        /** @brief Detaches direct child @a control without destroying it.

            Detachment reverses the runtime relationships established during
            attachment.
        */
        void remove(Control& control);

        /** @brief Returns the non-owning, ordered list of direct children.
        */
        const std::vector<Control*>& controls() const;

    private:
        void setForm(Form* form);

    public:
        /** @brief Returns this control's focus policy.
        */
        FocusPolicy focusPolicy() const;

        /** @brief Sets this control's focus policy.

            When the control belongs to a form, the form updates its focus
            navigation state. Changing the policy to %NoFocus removes focus
            from this control when it currently owns it.
        */
        void setFocusPolicy(FocusPolicy f);

        /** @brief Returns this control's focus-navigation index.
        */
        size_t focusIndex() const;

        /** @brief Sets this control's focus-navigation index.

            A containing form orders its focusable controls by this value.
        */
        void setFocusIndex(size_t index);

        /** @brief Returns true when this control currently has form focus.
        */
        bool hasFocus() const;

        /** @brief Requests focus from the containing form.

            The request has no effect while the control is unparented, outside
            a form, or uses the %NoFocus policy. %hasFocus() changes when the
            form delivers a %FocusEvent.
        */
        void focus();

        /** @brief Returns the key that invokes %onActionKey() while focused.
        */
        Key actionKey() const;

        /** @brief Sets the focused action key to @a ak.

            New controls use %Key::Space.
        */
        void setActionKey(const Key& ak);

        /** @brief Returns the registered form shortcut, or 0 when none is set.
        */
        const Key* shortcut() const;

        /** @brief Registers @a k as this control's form shortcut.

            The key is copied. Pass 0 to remove the shortcut. When multiple
            controls register the same key in a form, its last registration
            receives the shortcut.
        */
        void setShortcut(const Key* k);

        /** @brief Returns the registered mnemonic character, or 0 when none is set.
        */
        const Pt::Char* mnemonic() const;

        /** @brief Registers @a ch as this control's Alt mnemonic.

            When multiple controls register the same character in a form, its
            last registration receives the mnemonic.
        */
        void setMnemonic(const Char& ch);

        /** @brief Extracts and registers a mnemonic marked by an ampersand.

            For example, %setMnemonic("&File") returns "File" and registers
            `F`. A doubled ampersand returns one literal ampersand. A trailing
            unpaired ampersand remains in the returned text.
        */
        String setMnemonic(const String& text);

        /** @brief Forwards mnemonic activation to @a control.

            The target is non-owning. Pass 0 to remove mnemonic forwarding.
        */
        void setMnemonicControl(Control* control);

        /** @brief Invokes this control's shortcut hook for @a key.

            Forms call this after matching a registered shortcut. Applications
            normally register shortcuts with %setShortcut() instead of calling
            this method directly.
        */
        void processShortcut(const Key& key);

        /** @brief Invokes this control's mnemonic hook for @a m.

            Forms call this after matching an Alt mnemonic. Applications
            normally register mnemonics with %setMnemonic() instead of calling
            this method directly.
        */
        void processMnemonic(Pt::Char m);

    public:
        /** @brief Returns true when this control is visible and enabled.

            Parent input routing uses this predicate before delivering pointer,
            touch, scroll, and key input to a child.
        */
        bool acceptsInput() const;

        /** @brief Requests that this control be painted above its siblings.

            The parent moves a direct child to the end of its child order,
            which is painted last and hit-tested first.
        */
        void raise();

    //
    // invalidation
    //
    protected:
        /** @brief Handles the final coalesced invalidation event.

            The base implementation performs the normal widget invalidation
            and requests relayout. Overrides that retain this behavior must
            call the base implementation.
        */
        virtual void onInvalidateEvent(const InvalidateEvent& ev);

        /** @brief Updates derived state after invalidation.

            The default implementation requests relayout because changed
            control content may affect its preferred size.
        */
        virtual void onInvalidate();

    //
    // painting
    //
    protected:
        /** @brief Processes a paint event and paints visible child controls.

            The dirty rectangle is clipped and converted to each child's local
            coordinates. Overrides that retain normal painting must call the
            base implementation.
        */
        virtual void onProcessPaintEvent(const PaintEvent& ev);

        /** @brief Handles this control's own paint event.

            The base implementation creates the paint context and calls
            %onPaint().
        */
        virtual void onPaintEvent(const PaintEvent& ev);

        /** @brief Paints custom content in local coordinates.

            @a rect is the local dirty rectangle. The default implementation
            does not draw.
        */
        virtual void onPaint(PaintContext& context,
                             const Gfx::RectF& rect);

    //
    // layouting
    //
    public:
        /** @brief Returns the position and size in parent logical coordinates.
        */
        const Gfx::RectF geometry() const;

        /** @brief Returns this control's requested size policy.
        */
        const SizePolicy& sizePolicy() const;

        /** @brief Sets this control's requested size policy and requests relayout.

            The policy size is aligned to the current device scaling.
        */
        void setSizePolicy(const SizePolicy& policy);

        /** @brief Returns the result of the most recent %measure() call.
        */
        Gfx::SizeF preferredSize() const;

        /** @brief Measures the preferred size under parent constraint @a policy.

            The control combines its own policy with the stricter parent modes,
            applies its minimum size, and caches the resulting preferred size.
            Fixed dimensions override the measured result; maximum dimensions
            cap it. The result remains cached until content, policy, limits,
            or scaling invalidates measurement.
        */
        Gfx::SizeF measure(const SizePolicy& policy);

        /** @brief Invalidates this control's measurement and layout.

            The request propagates to the parent and is performed later by the
            containing form or layout sequence.
        */
        void relayout();

    protected:
        /** @brief Measures intrinsic content for effective policy @a policy.

            Return the preferred local content size. The default implementation
            returns `(0, 0)`.
        */
        virtual Gfx::SizeF onMeasure(const SizePolicy& policy);

        /** @brief Processes a pending layout event.

            The base implementation aligns the control geometry to the device
            pixel grid, calls %onLayoutEvent(), and queues layout events for
            direct children. Overrides that retain this sequence must call the
            base implementation.
        */
        virtual void onProcessLayoutEvent(const LayoutEvent& ev);

        /** @brief Handles the aligned layout rectangle and requests repaint.
        */
        virtual void onLayoutEvent(const LayoutEvent& ev);

        /** @brief Assigns geometry to direct children within @a rect.

            Use child %move() and %resize() requests. The default
            implementation does not lay out children.
        */
        virtual void onLayout(const Gfx::RectF& rect);

    public:
        /** @brief Returns the outer spacing requested around this control.
        */
        const Spacing& margin() const;

        /** @brief Sets the outer spacing to @a s and requests relayout.
        */
        void setMargin(const Spacing& s);

        /** @brief Sets equal outer spacing on every side and requests relayout.
        */
        void setMargin(double n);

        /** @brief Sets horizontal and vertical outer spacing and requests relayout.
        */
        void setMargin(double horiz, double vertical);

        /** @brief Returns the inner spacing requested around this control's content.
        */
        const Spacing& padding() const;

        /** @brief Sets the inner spacing to @a p and requests relayout.
        */
        void setPadding(const Spacing& p);

        /** @brief Sets equal inner spacing on every side and requests relayout.
        */
        void setPadding(double n);

        /** @brief Sets horizontal and vertical inner spacing and requests relayout.
        */
        void setPadding(double horiz, double vertical);

    protected:
        /** @brief Requests relayout after local size limits change.
        */
        virtual void onSetSizeLimits(const Gfx::SizeF& minSize,
                                     const Gfx::SizeF& maxSize);

    protected:
        /** @brief Propagates a surface assignment to direct children.

            Overrides that retain standard child painting must call the base
            implementation.
        */
        virtual void onSetSurface(PaintSurface* surface, const Gfx::PointF& pos);

        /** @brief Notifies after @a control becomes a direct child.
        */
        virtual void onAddControl(Control& control);

        /** @brief Notifies after @a control stops being a direct child.
        */
        virtual void onRemoveControl(Control& control);


        /** @brief Handles the configured action key while this control has focus.
        */
        virtual void onActionKey(const KeyEvent& kev);

        /** @brief Handles an activated form shortcut.
        */
        virtual void onShortcut(const Key& kev);

        /** @brief Handles an activated form mnemonic.

            The default implementation forwards @a m to the control selected
            by %setMnemonicControl().
        */
        virtual void onMnemonic(Pt::Char m);

    //
    // Widget
    //
    protected:
        /** @brief Connects this control and all direct descendants to @a screen.
        */
        virtual void onConnect(Screen& screen);

        /** @brief Disconnects this control and all direct descendants from the screen.
        */
        virtual void onDisconnect();


        /** @brief Returns the frontmost descendant hit at local position @a p.

            The control itself is returned when @a p is in bounds and no child
            is hit.
        */
        virtual Widget* onHitTest(const Gfx::PointF& p);

        /** @brief Converts local coordinates to the containing view.
        */
        virtual Gfx::PointF onToParent(const Gfx::PointF& pos) const;

        /** @brief Converts containing-view coordinates to local coordinates.
        */
        virtual Gfx::PointF onFromParent(const Gfx::PointF& pos) const;


        /** @brief Forwards a local repaint request to the parent view.
        */
        virtual void onRequestRepaint(const Gfx::RectF& rect);

        /** @brief Records and forwards a visibility request to the parent view.
        */
        virtual void onRequestShow(bool isShown);

        /** @brief Records and forwards an enable request to the parent view.
        */
        virtual void onRequestEnable(bool isEnable);

        /** @brief Forwards an activation request to the parent view.
        */
        virtual void onRequestActivate(bool active);

        /** @brief Records and forwards a move request to the parent view.
        */
        virtual void onRequestMove(const Gfx::PointF& pos);

        /** @brief Records and forwards a resize request to the parent view.
        */
        virtual void onRequestResize(const Gfx::SizeF& s);


        /** @brief Dispatches @a ev through the inherited widget event dispatcher.
        */
        virtual void onProcessEvent(const Pt::Event& ev);


        /** @brief Dispatches effective enabled state to this control and its children.

            A disabled parent keeps every descendant effectively disabled.
        */
        virtual void onProcessEnableEvent(const EnableEvent& ev);

        /** @brief Updates enabled state and handles an enable event.
        */
        virtual void onEnableEvent(const EnableEvent& ev);

        /** @brief Performs behavior associated with effective enabled state @a isEnable.
        */
        virtual void onEnable(bool isEnable);


        /** @brief Dispatches a visibility event.
        */
        virtual void onProcessShowEvent(const ShowEvent& ev);

        /** @brief Handles a visibility event and requests relayout.
        */
        virtual void onShowEvent(const ShowEvent& ev);

        /** @brief Performs behavior associated with visibility state @a visible.
        */
        virtual void onShow(bool visible);


        /** @brief Dispatches a focus event.
        */
        virtual void onProcessFocusEvent(const FocusEvent& ev);

        /** @brief Updates focus state and requests invalidation.
        */
        virtual void onFocusEvent(const FocusEvent& ev);


        /** @brief Dispatches a scale change to this control and all children.
        */
        virtual void onProcessRescaleEvent(const RescaleEvent& ev);

        /** @brief Handles a scale change.
        */
        virtual void onRescaleEvent(const RescaleEvent& ev);

        /** @brief Aligns local spacing and size-policy values to @a scaling.
        */
        virtual void onRescale(double scaling);


        /** @brief Dispatches a move event.
        */
        virtual void onProcessMoveEvent(const MoveEvent& ev);

        /** @brief Updates parent-local position from a move event.
        */
        virtual void onMoveEvent(const MoveEvent& ev);


        /** @brief Dispatches a resize event.
        */
        virtual void onProcessResizeEvent(const ResizeEvent& ev);

        /** @brief Updates local size and bounds from a resize event.
        */
        virtual void onResizeEvent(const ResizeEvent& ev);


        /** @brief Routes mouse input to the frontmost eligible child or this control.

            A press delivered to this control requests pointer capture until
            release. Overrides that retain child routing must call the base
            implementation.
        */
        virtual void onProcessMouseEvent(const MouseEvent& ev);

        /** @brief Routes touch input to the frontmost eligible child or this control.

            A press delivered to this control requests pointer capture until
            release. Overrides that retain child routing must call the base
            implementation.
        */
        virtual void onProcessTouchEvent(const TouchEvent& ev);

        /** @brief Delivers scroll input only while this control accepts input.
        */
        virtual void onProcessScrollEvent(const ScrollEvent& ev);

        /** @brief Dispatches a pointer-entry event.
        */
        virtual void onProcessEnterEvent(const EnterEvent& ev);

        /** @brief Dispatches a pointer-leave event.
        */
        virtual void onProcessLeaveEvent(const LeaveEvent& ev);

        /** @brief Delivers key input only while this control accepts input.
        */
        virtual void onProcessKeyEvent(const KeyEvent& ev);

    //
    // Responder
    //
    protected:
        /** @brief Handles mouse input in local coordinates.

            The default implementation requests focus on a left-button press
            and returns false to continue responder dispatch.
        */
        virtual bool onMouseEvent(const MouseEvent& ev);

        /** @brief Handles touch input in local coordinates.

            The default implementation requests focus on press and returns
            false to continue responder dispatch.
        */
        virtual bool onTouchEvent(const TouchEvent& ev);

        /** @brief Handles scroll input or returns false to continue dispatch.
        */
        virtual bool onScrollEvent( const ScrollEvent& ev);

        /** @brief Handles focused action keys or continues responder dispatch.
        */
        virtual bool onKeyEvent(const KeyEvent& ev);

        /** @brief Handles pointer entry or continues responder dispatch.
        */
        virtual bool onEnterEvent( const EnterEvent& ev);

        /** @brief Handles pointer leave or continues responder dispatch.
        */
        virtual bool onLeaveEvent(const LeaveEvent& ev);

    //
    // View
    //
    protected:
        /** @brief Converts this control coordinates to direct child coordinates.
        */
        virtual Gfx::PointF onToControl(const Control& control,
                                       const Gfx::PointF& pos) const;

        /** @brief Converts direct child coordinates to this control coordinates.
        */
        virtual Gfx::PointF onFromControl(const Control& control,
                                         const Gfx::PointF& pos) const;

        /** @brief Stores @a control as a direct child and invokes %onAddControl().
        */
        virtual void onAttach(Control& control);

        /** @brief Removes @a control as a direct child and invokes %onRemoveControl().
        */
        virtual void onDetach(Control& control);

        /** @brief Initializes runtime relationships for newly attached @a control.

            The base implementation connects it when necessary, sets its next
            responder and form association, propagates scaling, and requests
            relayout.
        */
        virtual void onInit(Control& control);

        /** @brief Releases runtime relationships for detached @a control.

            The base implementation removes its form association and next
            responder, disconnects it when necessary, and requests relayout.
        */
        virtual void onRelease(Control& control);

    protected:
        /** @brief Converts and forwards @a rect from @a control for repainting.
        */
        virtual void onRepaintRequest(Control& control, const Gfx::RectF& rect);

        /** @brief Requests relayout after a direct child changes its layout.
        */
        virtual void onRelayoutRequest(Control& control);

        /** @brief Delivers an effective enable request to direct child @a control.
        */
        virtual void onEnableRequest(Control& control, bool isEnable);

        /** @brief Forwards a direct child's activation request to the parent view.
        */
        virtual void onActivateRequest(Control& control, bool active);

        /** @brief Delivers a visibility request to direct child @a control.
        */
        virtual void onShowRequest(Control& control, bool isShown);

        /** @brief Forwards a direct child's move request to the parent view.
        */
        virtual void onMoveRequest(Control& control, const Gfx::PointF& pos);

        /** @brief Forwards a direct child's resize request to the parent view.
        */
        virtual void onResizeRequest(Control& control, const Gfx::SizeF& size);

        /** @brief Moves direct child @a control to the front of child order.
        */
        virtual void onRaiseRequest(Control& control);

        /** @brief Returns shortcuts registered by this control.
        */
        virtual const std::vector<Key> onGetShortcuts();

        /** @brief Returns mnemonics registered by this control.
        */
        virtual const std::vector<Char> onGetMnemonics();

    private:
        View*                        _parent;
        std::vector<Control*>        _children;

        Form*                        _form;

        bool                         _isCapture;
        bool                         _isLayoutInvalid;

        bool                         _show;
        bool                         _enabled;

        Gfx::PointF                  _requestedPosition;
        Gfx::SizeF                   _requestedSize;

        bool                         _isMeasureInvalid;

        SizePolicy                   _sizePolicy;
        SizePolicy                   _lastPolicy;
        Gfx::SizeF                   _preferredSize;

        bool                         _hasFocus;
        FocusPolicy                  _focusPolicy;
        size_t                       _focusIndex;

        Key                          _actionKey;
        Key                          _shortcutKey;
        Pt::Char                     _mnemonic;
        Pt::Signal<Pt::Char>         _mnemonicEntered;

        Spacing                      _padding;
        Spacing                      _margin;
};

} // namespace

} // namespace

#endif // include guard
