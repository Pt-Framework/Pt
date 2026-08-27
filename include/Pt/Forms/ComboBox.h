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
#include <Pt/Forms/ComboBoxStyle.h>
#include <Pt/Forms/Popup.h>
#include <Pt/Forms/ListBox.h>
#include <Pt/Forms/LineEditor.h>
#include <Pt/SmartPtr.h>
#include <Pt/String.h>

namespace Pt {

namespace Forms {

class PT_FORMS_API ComboBox : public Control
{
    typedef Control Base;

	public:
        ComboBox();

        virtual ~ComboBox();

        void addItem(ListBoxItem& item);

        void removeItem(ListBoxItem& item);

        bool isEditable() const;

        void setEditable(bool e);

        bool isAccepted() const;

        void setAccepted(bool a);

        const Pt::String& text() const;

        void setText(const Pt::String& str);

        Adjustment textAdjustment() const;

        void setTextAdjustment(Adjustment a);

        void setScrollBars(bool hasScrollBars);

        void setMaxHeight(double height);

        void showPopup();

        void hidePopup();

        bool isHighlighted() const;

        /** @brief Text has changed programmatically.
        */
        Pt::Signal<const Pt::String&>& textChanged();

        /** @brief Text was edited by usr input.
        */
        Pt::Signal<const Pt::String&>& textEdited();

        Pt::Signal<const Pt::String&>& returnPressed();

        Pt::Signal<const Pt::String&>& editingFinished();

        Pt::Signal<ListBoxItem&>& selected();

    public:
        const Gfx::Brush& background() const;

        void setBackground(const Gfx::Brush& b);

        const Gfx::Pen& contour() const;

        void setContour(const Gfx::Pen& p);

        const Gfx::Brush& foreground() const;

        void setForeground(const Gfx::Brush& b);

        const Gfx::Color& textColor() const;

        void setTextColor(const Gfx::Color& color);

        Gfx::Font font() const;

        void setFont(const Gfx::Font& font);

        void setFontSize(std::size_t size);

        void setFontWeight(Gfx::Font::Weight weight);

        void setFontSlant(Gfx::Font::Slant slant);

        void setRenderer(ComboBoxRenderer* renderer);

    protected:
        virtual Gfx::SizeF onMeasure(const SizePolicy& policy);

        virtual void onLayout(const Gfx::RectF& rect);

        virtual void onInvalidate();

        virtual void onPaint(PaintContext& context, const Gfx::RectF& updateRect);

        virtual void onPaintChrome(PaintContext& context,
                                   const Gfx::RectF& rect,
                                   const Gfx::RectF& entryRect,
                                   const Gfx::RectF& buttonRect,
                                   const ComboBoxState& state,
                                   const ComboBoxButtonState& buttonState);

        virtual void onPaintText(PaintContext& context,
                                 const Gfx::RectF& textRect,
                                 const String& text,
                                 const Gfx::PointF& textPos,
                                 const Gfx::RectF& cursor,
                                 const ComboBoxState& state);

    protected:
        virtual void onProcessMouseEvent(const MouseEvent& ev);

    protected:
        virtual void onResizeEvent(const ResizeEvent& ev);

        virtual bool onKeyEvent(const KeyEvent& ev);

        virtual bool onMouseEvent(const MouseEvent& ev);

        virtual bool onTouchEvent(const TouchEvent& ev);

        virtual bool onEnterEvent(const EnterEvent& ev);

        virtual bool onLeaveEvent(const LeaveEvent& ev);

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
