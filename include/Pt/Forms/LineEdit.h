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
  MA 02110-1301 USA
*/

#ifndef PT_FORMS_LINEEDIT_H
#define PT_FORMS_LINEEDIT_H

#include <Pt/Forms/Control.h>
#include <Pt/Forms/LineEditStyler.h>
#include <Pt/Forms/LineEditor.h>
#include <Pt/Forms/Adjustment.h>
#include <Pt/SmartPtr.h>
#include <Pt/String.h>

namespace Pt {

namespace Forms {

/** @brief Single-line text field.

    A %LineEdit presents one line of text for the user to enter or
    change. %setText() assigns the string and emits %textEdited().
    %setPlaceholderText() is shown while the field is empty and does
    not have focus. %setEchoMode() chooses whether the entered text,
    a mask character, or nothing is shown. %text() remains the entered
    string.

    %setEditable(false) keeps the text from being changed and does not
    take focus. %setAccepted(true) completes an edit: Return emits
    %returnPressed(), and losing focus after a change emits
    %editingFinished().

    The field owns a %LineEditStyler. On invalidate it calls
    %Styler::bind(). Appearance getters and setters overlay the
    application style. %setRenderer() assigns a %LineEditRenderer until
    it is cleared. Measure, layout, and paint call typed methods on the
    styler. %lineEditState() is the snapshot passed to paint layers.

    A %LineEditor stores the text, caret, and scroll. It is not a
    widget.

    @code
    Pt::Forms::LineEdit name;
    name.setPlaceholderText("Full name");
    name.editingFinished() += Pt::slot(*this, &ContactForm::onNameEdited);

    void ContactForm::onNameEdited(const Pt::String& text)
    {
        contact.setName(text);
    }
    @endcode

    @ingroup Pt-Forms-Editors
*/
class PT_FORMS_API LineEdit : public Control
{
    public:
        typedef Control Base;

        /** @brief How entered text is shown.
        */
        enum EchoMode
        {
            /** @brief Shows the entered text.
            */
            Normal = 0,

            /** @brief Does not show the entered text.
            */
            Hidden = 1,

            /** @brief Shows a mask character for each entered character.
            */
            Masked = 2
        };

    public:
        /** @brief Creates an empty line edit.
        */
        LineEdit();

        /** @brief Destroys the line edit.
        */
        ~LineEdit();

        /** @brief Returns true if the user can change the text.
        */
        bool isEditable() const;

        /** @brief Sets whether the user can change the text.

            When @a e is false the field does not take focus.
        */
        void setEditable(bool e);

        /** @brief Returns the entered text.
        */
        const Pt::String& text() const;

        /** @brief Sets the entered text to @a str and emits %textEdited().
        */
        void setText(const Pt::String& str);

        /** @brief Returns true if the entered text is empty.
        */
        bool isEmpty() const;

        /** @brief Returns the text shown in the entry.

            This is the entered text, or a mask string when %echoMode()
            is %Masked.
        */
        const Pt::String& displayText() const;

        /** @brief Returns the placeholder shown while the field is empty.
        */
        const Pt::String& placeholderText() const;

        /** @brief Sets the placeholder shown while the field is empty to @a s.
        */
        void setPlaceholderText(const Pt::String& s);

        /** @brief Returns how entered text is shown.
        */
        EchoMode echoMode() const;

        /** @brief Sets how entered text is shown to @a mode.
        */
        void setEchoMode(EchoMode mode);

        /** @brief Returns the horizontal adjustment of the text.
        */
        Adjustment textAdjustment() const;

        /** @brief Sets the horizontal adjustment of the text to @a a.
        */
        void setTextAdjustment(Adjustment a);

        /** @brief Returns the caret index in the entered text.
        */
        std::size_t cursorPosition() const;

        /** @brief Sets the caret index in the entered text to @a n.
        */
        void setCursorPosition(std::size_t n);

        /** @brief Returns true if Return and focus loss complete an edit.
        */
        bool isAccepted() const;

        /** @brief Sets whether Return and focus loss complete an edit.
        */
        void setAccepted(bool a);

        /** @brief Returns true if the pointer is over the field.
        */
        bool isHighlighted() const;

        /** @brief Returns the signal emitted when the entered text changes.
        */
        Pt::Signal<const Pt::String&>& textEdited();

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
        void setRenderer(LineEditRenderer* renderer);

        /** @brief Returns the transient visual state for the current paint pass.
        */
        LineEditState lineEditState() const;

    protected:
        /** @brief Measures the entry and frame.
        */
        virtual Gfx::SizeF onMeasure(const SizePolicy& policy);

        /** @brief Places the entry, caret, and text in @a rect.
        */
        virtual void onLayout(const Gfx::RectF& rect);

        /** @brief Binds the styler.
        */
        virtual void onInvalidate();

        /** @brief Paints the entry, text, selection, and caret.
        */
        virtual void onPaint(PaintContext& context, const Gfx::RectF& rect);

        /** @brief Paints the line-edit chrome for @a state.
        */
        virtual void onPaintChrome(PaintContext& context,
                                   const Gfx::RectF& rect,
                                   const Gfx::RectF& textRect,
                                   const String& text,
                                   const Gfx::PointF& textPos,
                                   const Gfx::RectF& cursor,
                                   const Gfx::RectF& selection,
                                   const LineEditState& state);

    protected:
        /** @brief Highlights the field when the pointer enters.
        */
        virtual bool onEnterEvent(const EnterEvent& ev);

        /** @brief Clears the highlight when the pointer leaves.
        */
        virtual bool onLeaveEvent(const LeaveEvent& ev);

        /** @brief Inserts, deletes, or moves the caret, or completes an edit.
        */
        virtual bool onKeyEvent(const KeyEvent& ev);

        /** @brief Places the caret from a pointer press when the field is editable.
        */
        virtual bool onMouseEvent(const MouseEvent& ev);

        /** @brief Places the caret from a touch press when the field is editable.
        */
        virtual bool onTouchEvent(const TouchEvent& ev);

        /** @brief Forwards the resize to the base control.
        */
        virtual void onResizeEvent(const ResizeEvent& ev);

        /** @brief Begins text input on focus, or emits %editingFinished() on loss.
        */
        virtual void onFocusEvent(const FocusEvent& ev);

    private:
        Pt::Signal<const Pt::String&> _textEdited;
        Pt::Signal<const Pt::String&> _returnPressed;
        Pt::Signal<const Pt::String&> _editingFinished;

        LineEditor                    _editor;
        TextLine                      _line;
        Pt::String                    _placeholderText;
        bool                          _isEditable;
        bool                          _isAccepted;
        bool                          _isTextChanged;
        bool                          _isHighlighted;
        double                        _pendingCursorX;
        EchoMode                      _echoMode;
        double                        _spacing;

        LineEditStyler                 _styler;

        Gfx::RectF                    _textRect;
        Gfx::RectF                    _cursorRect;
};

} // namespace

} // namespace

#endif
