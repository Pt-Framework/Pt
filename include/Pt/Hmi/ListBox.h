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
#include <Pt/Hmi/Picture.h>
#include <Pt/Gfx/Color.h>
#include <Pt/Gfx/Image.h>
#include <Pt/SmartPtr.h>

namespace Pt {

namespace Hmi {

//
// TODO: auto-sizing
//
class PT_HMI_API ListBoxItem : public Button
{
    typedef Button Base;

	  public:
        ListBoxItem();
		
        virtual ~ListBoxItem();	
        
        void setText(const Pt::String& t);

        const Pt::String& text() const;
        
        void setIcon(const Gfx::Image& image);

        void setIconSize(const Gfx::SizeF& size);

        Pt::Signal<ListBoxItem&>& selected();

    public:
        const Gfx::Color& textColor() const;

        void setTextColor(const Gfx::Color& color);

        const std::string& font() const;

        void setFont(const std::string& fontName);

        std::size_t fontSize() const;

        void setFontSize(const std::size_t n);

        Gfx::Font::Style fontStyle() const;

        void setFontStyle(Gfx::Font::Style style);

    protected:
        virtual void onPressed();

        virtual void onReleased();

        virtual void onCanceled();

    protected:
        virtual void onInvalidate();
	
        virtual void onPaint(PaintSurface& surface, const Gfx::RectF& updateRect);

        virtual void onPaintContent(Painter& painter);

    protected:
        virtual void onResizeEvent(const ResizeEvent& ev);

    private:
        Pt::Signal<ListBoxItem&> _selected;
        String     _text;
        Gfx::Image _image;
        Gfx::SizeF _iconSize;

        AutoPtr<Gfx::Color>       _textColor;
        AutoPtr<std::string>      _fontName;
        AutoPtr<std::size_t>      _fontSize;
        AutoPtr<Gfx::Font::Style> _fontStyle;

        Gfx::Pen   _textPen;
        Gfx::Font  _font;
        Picture    _picture;
};


class PT_HMI_API ListBoxLayout : public FlowLayout
{
    typedef FlowLayout Base;

	  public:
        ListBoxLayout();
		
        virtual ~ListBoxLayout();

    protected:
        virtual void onAddWidget(Widget& w);

        virtual void onRemoveWidget(Widget& w);

    private:
        void onContentChanged();
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

        Pt::Signal<ListBoxItem&>& selected();

    protected:
        virtual void onInvalidate();
	
        virtual void onPaint(PaintSurface& surface, const Gfx::RectF& updateRect);
    
    protected:
        virtual void onResizeEvent(const ResizeEvent& ev);

    private:
        void onItemSelected(ListBoxItem& item);
    
    private:
        Pt::Signal<ListBoxItem&> _selected;
        ListBoxLayout _layout;
        ScrollView    _scrollView;
};

} // namespace

} // namespace

#endif
