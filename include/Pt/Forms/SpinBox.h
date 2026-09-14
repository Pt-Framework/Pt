/* Copyright (C) 2017 Marc Boris Duerner

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

#ifndef PT_FORMS_SPINBOX_H
#define PT_FORMS_SPINBOX_H

#include <Pt/Forms/Control.h>
#include <Pt/Forms/Button.h>
#include <Pt/Forms/PushButton.h>
#include <Pt/Forms/LineEditor.h>
#include <Pt/Forms/Adjustment.h>
#include <Pt/Forms/SpinBoxStyler.h>
#include <Pt/SmartPtr.h>
#include <Pt/String.h>

namespace Pt {

namespace Forms {

/** @brief Up or down control used by a spin box.

    A %SpinBoxButton is a %Button that steps a %SpinBox. It is an
    integrated subpart. Applications do not construct or place it.

    @ingroup Pt-Forms-Editors
*/
class PT_FORMS_API SpinBoxButton : public Button
{
    public:
        typedef Button Base;

        /** @brief Direction of the step control.
        */
        enum Type
        {
          /** @brief Increases the value.
          */
          Up,

          /** @brief Decreases the value.
          */
          Down
        };

    public:
        /** @brief Creates an up or down control of @a type.
        */
        SpinBoxButton(Type type);

        /** @brief Destroys the step control.
        */
        ~SpinBoxButton();

        /** @brief Returns whether the control increases or decreases the value.
        */
        Type type() const;

        /** @brief Returns true if the control is currently pressed.
        */
        bool isPressed() const;

    protected:
        /** @brief Marks the control pressed.
        */
        virtual void onPressed();

        /** @brief Marks the control released.
        */
        virtual void onReleased();

        /** @brief Restores the pressed state after an abandoned click.
        */
        virtual void onCanceled();

    protected:
        /** @brief Forwards invalidate to the base button.
        */
        virtual void onInvalidate();

        /** @brief Paints the step control.
        */
        virtual void onPaint(PaintContext& context, const Gfx::RectF& rect);

    private:
        Type _type;
        bool _isPressed;
};


/** @brief Integer editor with a range and step controls.

    A %SpinBox presents an integer between %minimum() and %maximum().
    %setRange() sets the bounds. %setValue() clamps the value, updates
    the entry, and emits %valueEdited(). Up and down controls step the
    value. %setEditable(false) keeps the number from being typed and
    does not take focus. %setAccepted(true) completes an edit: Return
    emits %returnPressed(), and losing focus after a change emits
    %editingFinished().

    Override %toText() and %toValue() to change how the integer is
    shown and parsed. %onInput() accepts partial text while the user
    types. %onStep() applies an up or down step.

    The box owns a %SpinBoxStyler. On invalidate it calls
    %Styler::bind(). Appearance getters and setters overlay the
    application style. %setRenderer() assigns a %SpinBoxRenderer until
    it is cleared. Measure, layout, and paint call typed methods on the
    styler.

    A %LineEditor stores the entry text and caret. It is not a widget.
    %SpinBoxButton is an integrated subpart, not a control the
    application places.

    @code
    Pt::Forms::SpinBox quantity;
    quantity.setRange(1, 99);
    quantity.setValue(1);
    quantity.valueEdited() += Pt::slot(*this, &OrderForm::onQuantityEdited);

    void OrderForm::onQuantityEdited(int n)
    {
        order.setQuantity(n);
    }
    @endcode

    @ingroup Pt-Forms-Editors
*/
class PT_FORMS_API SpinBox : public Control
{
    public:
        typedef Control Base;

    public:
        /** @brief Creates a spin box.
        */
        SpinBox();

        /** @brief Destroys the spin box.
        */
        ~SpinBox();

        /** @brief Returns true if the user can type the number.
        */
        bool isEditable() const;

        /** @brief Sets whether the user can type the number.

            When @a e is false the box does not take focus.
        */
        void setEditable(bool e);

        /** @brief Returns the lower bound of the range.
        */
        int minimum() const;

        /** @brief Returns the upper bound of the range.
        */
        int maximum() const;

        /** @brief Sets the range to @a min through @a max and clamps the value.
        */
        void setRange(int min, int max);

        /** @brief Returns the current value.
        */
        int value() const;

        /** @brief Sets the value to @a n, clamped to the range, and emits %valueEdited().
        */
        void setValue(int n);

        /** @brief Returns the text shown in the entry.
        */
        const Pt::String& text() const;

        /** @brief Returns true if the entry text is empty.
        */
        bool isEmpty() const;

        /** @brief Returns the horizontal adjustment of the text.
        */
        Adjustment textAdjustment() const;

        /** @brief Sets the horizontal adjustment of the text to @a a.
        */
        void setTextAdjustment(Adjustment a);

        /** @brief Returns the caret index in the entry text.
        */
        std::size_t cursorPosition() const;

        /** @brief Sets the caret index in the entry text to @a n.
        */
        void setCursorPosition(std::size_t n);

        /** @brief Returns true if Return and focus loss complete an edit.
        */
        bool isAccepted() const;

        /** @brief Sets whether Return and focus loss complete an edit.
        */
        void setAccepted(bool a);

        /** @brief Returns true if the pointer is over the box.
        */
        bool isHighlighted() const;

        /** @brief Returns the signal emitted when the value changes.
        */
        Pt::Signal<int>& valueEdited();

        /** @brief Returns the signal emitted when Return completes an edit.
        */
        Pt::Signal<const Pt::String&>& returnPressed();

        /** @brief Returns the signal emitted when focus loss completes an edit.
        */
        Pt::Signal<const Pt::String&>& editingFinished();

    public:
        /** @brief Returns the effective background brush.
        */
        const Gfx::Brush& background() const;

        /** @brief Sets the widget-local background brush to @a b.
        */
        void setBackground(const Gfx::Brush& b);

        /** @brief Returns the effective foreground brush.
        */
        const Gfx::Brush& foreground() const;

        /** @brief Sets the widget-local foreground brush to @a b.
        */
        void setForeground(const Gfx::Brush& b);

        /** @brief Returns the effective contour pen.
        */
        const Gfx::Pen& contour() const;

        /** @brief Sets the widget-local contour pen to @a p.
        */
        void setContour(const Gfx::Pen& p);

        /** @brief Returns the effective text color.
        */
        const Gfx::Color& textColor() const;

        /** @brief Sets the widget-local text color to @a color.
        */
        void setTextColor(const Gfx::Color& color);

        /** @brief Returns the effective font.
        */
        Gfx::Font font() const;

        /** @brief Sets the widget-local font to @a font.
        */
        void setFont(const Gfx::Font& font);

        /** @brief Sets the widget-local font size to @a size.
        */
        void setFontSize(std::size_t size);

        /** @brief Sets the widget-local font weight to @a weight.
        */
        void setFontWeight(Gfx::Font::Weight weight);

        /** @brief Sets the widget-local font slant to @a slant.
        */
        void setFontSlant(Gfx::Font::Slant slant);

        /** @brief Assigns @a renderer as the family renderer.

            A null renderer falls back to the current style on the next bind.
        */
        void setRenderer(SpinBoxRenderer* renderer);

    protected:
        /** @brief Returns the text shown for @a n.
        */
        virtual Pt::String toText(int n) const;

        /** @brief Parses @a str into @a n and returns true when the whole string is an integer.
        */
        virtual bool toValue(const Pt::String& str, int& n) const;

        /** @brief Returns true if @a str is accepted as typed input.

            The default implementation accepts an empty string, a sign,
            or a complete integer.
        */
        virtual bool onInput(const Pt::String& str) const;

        /** @brief Adds @a n to the value, clamped to the range.
        */
        virtual void onStep(int n);

    private:
        bool setInput(const Pt::String& str);

        void onUp();

        void onDown();

    protected:
        /** @brief Measures the entry, step controls, and frame.
        */
        virtual Gfx::SizeF onMeasure(const SizePolicy& policy);

        /** @brief Places the entry and step controls in @a rect.
        */
        virtual void onLayout(const Gfx::RectF& rect);

        /** @brief Binds the styler.
        */
        virtual void onInvalidate();

        /** @brief Paints the entry, step controls, text, and caret.
        */
        virtual void onPaint(PaintContext& context, const Gfx::RectF& rect);

        /** @brief Paints the spin-box chrome for @a state.
        */
        virtual void onPaintChrome(PaintContext& context,
                                   const Gfx::RectF& rect,
                                   const Gfx::RectF& entryRect,
                                   const Gfx::RectF& upButtonRect,
                                   const Gfx::RectF& downButtonRect,
                                   const SpinBoxState& state);

        /** @brief Paints the entry text and caret for @a state.
        */
        virtual void onPaintText(PaintContext& context,
                                 const Gfx::RectF& textRect,
                                 const String& text,
                                 const Gfx::PointF& textPos,
                                 const Gfx::RectF& cursor,
                                 const SpinBoxState& state);

    protected:
        /** @brief Inserts, deletes, or moves the caret, or completes an edit.
        */
        virtual bool onKeyEvent(const KeyEvent& ev);

        /** @brief Places the caret from a pointer press when the box is editable.
        */
        virtual bool onMouseEvent(const MouseEvent& ev);

        /** @brief Places the caret from a touch press when the box is editable.
        */
        virtual bool onTouchEvent(const TouchEvent& ev);

        /** @brief Highlights the box when the pointer enters.
        */
        virtual bool onEnterEvent(const EnterEvent& ev);

        /** @brief Clears the highlight when the pointer leaves.
        */
        virtual bool onLeaveEvent(const LeaveEvent& ev);

        /** @brief Begins text input on focus, or emits %editingFinished() on loss.
        */
        virtual void onFocusEvent(const FocusEvent& ev);

    private:
        SpinBoxState spinBoxState() const;

    private:
        Pt::Signal<int>               _valueEdited;
        Pt::Signal<const Pt::String&> _returnPressed;
        Pt::Signal<const Pt::String&> _editingFinished;

        LineEditor                    _editor;
        TextLine                      _line;
        bool                          _isEditable;
        bool                          _isAccepted;
        bool                          _isTextChanged;
        bool                          _isHighlighted;
        double                        _pendingCursorX;
        int                           _value;
        int                           _minimum;
        int                           _maximum;

        SpinBoxButton                 _downButton;
        SpinBoxButton                 _upButton;
        Gfx::RectF                    _entryRect;
        Gfx::RectF                    _upButtonRect;
        Gfx::RectF                    _downButtonRect;
        Gfx::RectF                    _textRect;

        SpinBoxStyler                 _styler;
};

} // namespace

} // namespace

#endif
