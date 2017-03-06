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

#ifndef Pt_Hmi_ComboBox_H
#define Pt_Hmi_ComboBox_H

#include <Pt/Hmi/Control.h>
#include <Pt/Hmi/Window.h>
#include <Pt/Hmi/ListBox.h>
#include <Pt/Hmi/LineEditor.h>
#include <Pt/SmartPtr.h>
#include <Pt/String.h>

namespace Pt {

namespace Hmi {

class PT_HMI_API ComboBoxMenu : public Window
{
    typedef Window Base;

    public:
        ComboBoxMenu();
		
        virtual ~ComboBoxMenu();

        void addItem(ListBoxItem& item);

        void removeItem(ListBoxItem& item);

        void setScrollBars(bool hasScrollBars);

        Pt::Signal<ListBoxItem&>& selected();

        Gfx::SizeF preferredSize(const SizePolicy& policy) const;

    protected:
        void onShowEvent(const ShowEvent& ev);

        bool onMouseEvent(const MouseEvent& ev);

        void onTouchEvent(const TouchEvent& ev);

    private:
        void onItemSelected(ListBoxItem& item);

    private:
        ListBox _items;
};


class PT_HMI_API ComboBox : public Control
{
    typedef Control Base;

	public:
        ComboBox();
		
        virtual ~ComboBox();

        void addItem(ListBoxItem& item);

        void removeItem(ListBoxItem& item);

        bool isEditable() const;

        void setEditable(bool e);

        const Pt::String& text() const;

        void setText(const Pt::String& str);

        Adjustment textAdjustment() const;

        void setTextAdjustment(Adjustment a);

        void setScrollBars(bool hasScrollBars);

        void setMaxHeight(double height);

        void showPopup();

        void hidePopup();

        Pt::Signal<const Pt::String&>& textEdited();

        Pt::Signal<const Pt::String&>& returnPressed();

        Pt::Signal<const Pt::String&>& editingFinished();

        Pt::Signal<ListBoxItem&>& selected();

    public:
        const Gfx::Brush& background() const;

        void setBackground(const Gfx::Brush& b);

        const Gfx::Brush& foreground() const;

        void setForeground(const Gfx::Brush& b);

        const Gfx::Pen& contour() const;

        void setContour(const Gfx::Pen& p);

        const Gfx::Color& textColor() const;

        void setTextColor(const Gfx::Color& color);

        const std::string& font() const;

        void setFont(const std::string& fontName);

        std::size_t fontSize() const;

        void setFontSize(const std::size_t n);

        Gfx::Font::Style fontStyle() const;

        void setFontStyle(Gfx::Font::Style style);

        void setRenderer(ComboBoxRenderer* renderer);

    protected:
        virtual void onInvalidate();
	
        virtual void onPaint(PaintSurface& surface, const Gfx::RectF& updateRect);

    protected:
        virtual void onResizeEvent(const ResizeEvent& ev);

        virtual void onKeyEvent(const KeyEvent& ev);

        virtual bool onMouseEvent(const MouseEvent& ev);

        virtual void onTouchEvent(const TouchEvent& ev);

        virtual void onFocusEvent(const FocusEvent& ev);

    private:
        void onItemSelected(ListBoxItem& item);
        
        void processKeyEvent(const KeyEvent& ev);

    private:
        Pt::Signal<const Pt::String&> _textEdited;
        Pt::Signal<const Pt::String&> _returnPressed;
        Pt::Signal<const Pt::String&> _editingFinished;

        LineEditor   _editor;
        TextLine     _line;
        ComboBoxMenu _popup;
        Gfx::SizeF   _buttonSize;
        double       _maxHeight;
        double       _spacing;
        bool         _isEditable;
        
        AutoPtr<Gfx::Brush>       _background;
        AutoPtr<Gfx::Brush>       _foreground;
        AutoPtr<Gfx::Pen>         _contour;
        AutoPtr<Gfx::Color>       _textColor;
        AutoPtr<std::string>      _fontName;
        AutoPtr<std::size_t>      _fontSize;
        AutoPtr<Gfx::Font::Style> _fontStyle;

        FacetPtr<ComboBoxRenderer> _renderer;
        bool                       _hasRenderer;

        Gfx::Brush _backgroundBrush;
        Gfx::Brush _foregroundBrush;
        Gfx::Pen   _pen;
        Gfx::Pen   _textPen;
        Gfx::Font  _font;

        //ListBoxItem _itemX;
};

} // namespace

} // namespace

#endif
