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

#ifndef Pt_Forms_ComboBox_H
#define Pt_Forms_ComboBox_H

#include <Pt/Forms/Control.h>
#include <Pt/Forms/ComboBoxStyler.h>
#include <Pt/Forms/Popup.h>
#include <Pt/Forms/ListBox.h>
#include <Pt/Forms/LineEditor.h>
#include <Pt/SmartPtr.h>
#include <Pt/String.h>

namespace Pt {

namespace Forms {

/** @brief Provides a text entry field with a drop-down list.

    A %ComboBox presents a current text in an entry and a button that
    opens a popup list. %addItem() adds a caller-owned %ListBoxItem.
    Choosing an item emits %selected() and sets the text.
    %setText() assigns the string and emits %textChanged(). User typing
    emits %textEdited() when the box is editable.

    %setEditable(false) keeps the text from being typed; the user still
    picks from the list. %setAccepted(true) completes an edit: Return
    emits %returnPressed(), and losing focus after a change emits
    %editingFinished(). %setMaxHeight() limits the popup. %setScrollBars()
    shows scroll bars on the list.

    The box owns a %ComboBoxStyler. On invalidate it calls
    %Styler::bind(). Appearance getters and setters overlay the
    application style. %setRenderer() assigns a %ComboBoxRenderer until
    it is cleared. Measure, layout, and paint call typed methods on the
    styler.

    A %LineEditor stores the entry text and caret. It is not a widget.
    The popup list is a %Popup that hosts a %ListBox.

    @code
    Pt::Forms::ListBoxItem berlin;
    berlin.setText("Berlin");

    Pt::Forms::ComboBox city;
    city.addItem(berlin);
    city.selected() += Pt::slot(*this, &AddressForm::onCitySelected);

    void AddressForm::onCitySelected(Pt::Forms::ListBoxItem& item)
    {
        address.setCity(item.text());
    }
    @endcode

    @ingroup Pt-Forms-Editors
*/
class PT_FORMS_API ComboBox : public Control
{
    typedef Control Base;

    public:
        /** @brief Creates a combo box.
        */
        ComboBox();

        /** @brief Destroys the combo box.
        */
        virtual ~ComboBox();

        /** @brief Adds caller-owned @a item to the popup list.
        */
        void addItem(ListBoxItem& item);

        /** @brief Removes @a item from the popup list.
        */
        void removeItem(ListBoxItem& item);

        /** @brief Returns true if the user can type the text.
        */
        bool isEditable() const;

        /** @brief Sets whether the user can type the text.
        */
        void setEditable(bool e);

        /** @brief Returns true if Return and focus loss complete an edit.
        */
        bool isAccepted() const;

        /** @brief Sets whether Return and focus loss complete an edit.
        */
        void setAccepted(bool a);

        /** @brief Returns the current text.
        */
        const Pt::String& text() const;

        /** @brief Sets the current text to @a str and emits %textChanged().
        */
        void setText(const Pt::String& str);

        /** @brief Returns the horizontal adjustment of the text.
        */
        Adjustment textAdjustment() const;

        /** @brief Sets the horizontal adjustment of the text to @a a.
        */
        void setTextAdjustment(Adjustment a);

        /** @brief Sets whether the popup list shows scroll bars.
        */
        void setScrollBars(bool hasScrollBars);

        /** @brief Sets the maximum height of the popup to @a height.
        */
        void setMaxHeight(double height);

        /** @brief Shows the popup list.
        */
        void showPopup();

        /** @brief Hides the popup list.
        */
        void hidePopup();

        /** @brief Returns true if the pointer is over the box.
        */
        bool isHighlighted() const;

        /** @brief Returns the signal emitted when %setText() changes the text.
        */
        Pt::Signal<const Pt::String&>& textChanged();

        /** @brief Returns the signal emitted when the user changes the text.
        */
        Pt::Signal<const Pt::String&>& textEdited();

        /** @brief Returns the signal emitted when Return completes an edit.
        */
        Pt::Signal<const Pt::String&>& returnPressed();

        /** @brief Returns the signal emitted when focus loss completes an edit.
        */
        Pt::Signal<const Pt::String&>& editingFinished();

        /** @brief Returns the signal emitted when a list item is chosen.
        */
        Pt::Signal<ListBoxItem&>& selected();

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

        /** @brief Returns the effective foreground brush.
        */
        const Gfx::Brush& foreground() const;

        /** @brief Sets the widget-local foreground brush to @a b.
        */
        void setForeground(const Gfx::Brush& b);

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
        void setRenderer(ComboBoxRenderer* renderer);

    protected:
        /** @brief Measures the entry, button, and frame.
        */
        virtual Gfx::SizeF onMeasure(const SizePolicy& policy);

        /** @brief Places the entry, button, and text in @a rect.
        */
        virtual void onLayout(const Gfx::RectF& rect);

        /** @brief Binds the styler.
        */
        virtual void onInvalidate();

        /** @brief Paints the entry, button, text, and caret.
        */
        virtual void onPaint(PaintContext& context, const Gfx::RectF& updateRect);

        /** @brief Paints the combo-box chrome for @a state.
        */
        virtual void onPaintChrome(PaintContext& context,
                                   const Gfx::RectF& rect,
                                   const Gfx::RectF& entryRect,
                                   const Gfx::RectF& buttonRect,
                                   const ComboBoxState& state,
                                   const ComboBoxButtonState& buttonState);

        /** @brief Paints the entry text and caret for @a state.
        */
        virtual void onPaintText(PaintContext& context,
                                 const Gfx::RectF& textRect,
                                 const String& text,
                                 const Gfx::PointF& textPos,
                                 const Gfx::RectF& cursor,
                                 const ComboBoxState& state);

    protected:
        /** @brief Forwards the pointer event to the base control.
        */
        virtual void onProcessMouseEvent(const MouseEvent& ev);

    protected:
        /** @brief Forwards the resize to the base control.
        */
        virtual void onResizeEvent(const ResizeEvent& ev);

        /** @brief Inserts, deletes, or moves the caret, or completes an edit.
        */
        virtual bool onKeyEvent(const KeyEvent& ev);

        /** @brief Places the caret or opens the popup from a pointer press.
        */
        virtual bool onMouseEvent(const MouseEvent& ev);

        /** @brief Places the caret or opens the popup from a touch press.
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
        void onItemSelected(ListBoxItem& item);

        void processKeyEvent(const KeyEvent& ev);

        ComboBoxState comboBoxState() const;

    private:
        Pt::Signal<const Pt::String&> _textChanged;
        Pt::Signal<const Pt::String&> _textEdited;
        Pt::Signal<const Pt::String&> _returnPressed;
        Pt::Signal<const Pt::String&> _editingFinished;

        LineEditor    _editor;
        TextLine      _line;
        Popup         _popup;
        ListBox       _items;
        Gfx::RectF    _entryRect;
        Gfx::RectF    _buttonRect;
        Gfx::RectF    _textRect;
        Gfx::RectF    _cursorRect;
        double        _maxHeight;
        bool          _isEditable;
        bool          _isAccepted;
        bool          _isTextChanged;
        bool          _isHighlighted;
        bool          _isButtonHighlighted;
        double        _pendingCursorX;

        ComboBoxStyler       _styler;
};

} // namespace

} // namespace

#endif
