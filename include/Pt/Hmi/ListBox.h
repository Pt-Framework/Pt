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

#ifndef Pt_Hmi_ListBox_H
#define Pt_Hmi_ListBox_H

#include <Pt/Hmi/Control.h>
#include <Pt/Hmi/Button.h>
#include <Pt/Hmi/ScrollView.h>
#include <Pt/Hmi/FlowLayout.h>
#include <Pt/Hmi/Icon.h>
#include <Pt/Hmi/PixmapSurface.h>
#include <Pt/Gfx/Color.h>
#include <Pt/Gfx/Image.h>
#include <Pt/SmartPtr.h>
#include <cstddef>

namespace Pt {

namespace Hmi {

class PT_HMI_API ListBoxItem : public Button
{
    typedef Button Base;

      public:
        ListBoxItem();
        
        virtual ~ListBoxItem();

        bool isSelectable() const;

        void setSelectable(bool b);

        bool isSelected() const;

        void setSelected(bool b);
        
        void setText(const Pt::String& t);

        const Pt::String& text() const;

        void setIcon(const Icon& icon, const Gfx::SizeF& size);

        const Gfx::SizeF& iconSize() const
        { return _iconSize; }

        Pt::Signal<ListBoxItem&>& selected();

    public:
        const Gfx::Brush& background() const;

        void setBackground(const Gfx::Brush& b);

        const Gfx::Pen& contour() const;

        void setContour(const Gfx::Pen& p);

        const Gfx::Color& textColor() const;

        void setTextColor(const Gfx::Color& color);

        const std::string& font() const;

        void setFont(const std::string& fontName);

        std::size_t fontSize() const;

        void setFontSize(const std::size_t n);

        const std::string& fontStyle() const;

        void setFontStyle(const std::string& style);

        void setRenderer(ListBoxRenderer* renderer);

    protected:
        const PixmapSurface& picture() const
        { return _picture; }

        const Gfx::Font& currentFont() const
        { return _font; }

    protected:
        virtual void onPressed();

        virtual void onReleased();

        virtual void onCanceled();

    protected:
        virtual Gfx::SizeF onMeasure(const SizePolicy& p);

        virtual void onInvalidate();
    
        virtual void onPaint(Gfx::PaintSurface& surface, const Gfx::RectF& updateRect);

        // TODO: why pass PaintSurface?
        virtual void onPaintContent(Gfx::PaintSurface& surface, Gfx::Painter& painter);

    private:
        Pt::Signal<ListBoxItem&> _selected;
        bool                     _isSelectable;
        bool                     _isSelected;
        String                   _text;
        
        Icon                     _icon;
        Gfx::SizeF               _iconSize;

        FacetPtr<ListBoxRenderer> _renderer;
        bool                      _hasRenderer;

        AutoPtr<Gfx::Brush>       _background;
        AutoPtr<Gfx::Pen>         _contour;
        AutoPtr<Gfx::Color>       _textColor;
        AutoPtr<std::string>      _fontName;
        AutoPtr<std::size_t>      _fontSize;
        AutoPtr<std::string> _fontStyle;

        Gfx::Pen   _textPen;
        Gfx::Font  _font;
        Gfx::Brush _brush;
        Gfx::Pen   _pen;
        PixmapSurface    _picture;
};


class ListBoxLayout : public FlowLayout
{
    friend class ListBox;

    public:
        ListBoxLayout();

        const std::vector<ListBoxItem*>& selectedItems() const;

        Pt::Signal<ListBoxItem&>& selected();

    protected:
        virtual void onAddWidget(Widget& w);
        
        virtual void onRemoveWidget(Widget& w);

    private:
        void onItemSelected(ListBoxItem& item);

    private:
        Pt::Signal<ListBoxItem&>  _selected;
        std::vector<ListBoxItem*> _selectedItems;
};


class PT_HMI_API ListBox : public Control
{
    typedef Control Base;

    public:
        ListBox();
        
        virtual ~ListBox();

        void setScrollBars(bool hasScrollBars);

        void addItem(ListBoxItem& item);

        void removeItem(ListBoxItem& item);

        const std::vector<ListBoxItem*>& selectedItems() const;

        Pt::Signal<ListBoxItem&>& selected();

        void scrollX(int xpos);

        void scrollY(int ypos);

        int maximumX() const;

        int maximumY() const;

    public:
        const Gfx::Brush* background() const;

        void setBackground(const Gfx::Brush& b);

        void setBackground(bool b);

        const Gfx::Pen* contour() const;

        void setContour(const Gfx::Pen& pen);

        void setFrame(bool b);

        void setRenderer(ListBoxRenderer* renderer);

    protected:
        virtual void onInvalidate();
    
        virtual void onPaint(Gfx::PaintSurface& surface, const Gfx::RectF& updateRect);

        virtual Gfx::SizeF onMeasure(const SizePolicy& policy);

        virtual void onLayout(const Gfx::RectF& rect);
    
    private:
        ScrollView                _scrollView;
        ListBoxLayout             _layout;        
        FacetPtr<ListBoxRenderer> _renderer;
        bool                      _hasRenderer;
        AutoPtr<Gfx::Brush>       _background;
        bool                      _hasBackground;       
        AutoPtr<Gfx::Pen>         _contour;
        bool                      _hasFrame;
                        
        Spacing    _frameSize;          
        Gfx::Brush _brush;
        Gfx::Pen   _pen;
};

} // namespace

} // namespace

#endif
