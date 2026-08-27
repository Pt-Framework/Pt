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

#ifndef Pt_Forms_ListBox_H
#define Pt_Forms_ListBox_H

#include <Pt/Forms/Control.h>
#include <Pt/Forms/ListBoxStyle.h>
#include <Pt/Forms/ScrollView.h>
#include <Pt/Forms/FlowLayout.h>
#include <Pt/Forms/Icon.h>
#include <Pt/Forms/PixmapSurface.h>
#include <Pt/Gfx/Color.h>
#include <Pt/Gfx/FontMetrics.h>
#include <Pt/Gfx/Image.h>
#include <Pt/SmartPtr.h>
#include <cstddef>

#include <Pt/Forms/ProgressBar.h> // XXX

namespace Pt {

namespace Forms {

class Painter;

class PT_FORMS_API ListBoxItem : public Control
{
        typedef Control Base;

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

        bool isHovered() const;

        void click();

        Signal<>& clicked();

        Pt::Signal<ListBoxItem&>& selected();

    public:
        void setBackground(const Gfx::Brush& b);

        const Gfx::Color& textColor() const;

        void setTextColor(const Gfx::Color& color);

        Gfx::Font font() const;

        void setFont(const Gfx::Font& font);

        void setFontSize(std::size_t size);

        void setFontWeight(Gfx::Font::Weight weight);

        void setFontSlant(Gfx::Font::Slant slant);

        void setRenderer(ListItemRenderer* renderer);

    protected:
        virtual void onActionKey(const KeyEvent& kev);

        virtual void onShortcut(const Key& key);

        virtual void onMnemonic(Pt::Char m);

        virtual void onPressed();

        virtual void onReleased();

        virtual void onCanceled();

        virtual bool onEnterEvent(const EnterEvent& ev);

        virtual bool onLeaveEvent(const LeaveEvent& ev);

        virtual bool onMouseEvent(const MouseEvent& ev);

        virtual bool onTouchEvent(const TouchEvent& ev);

        virtual bool onScrollEvent(const ScrollEvent& ev);

    protected:
        virtual void onInvalidate();

    protected:
        virtual Gfx::SizeF onMeasure(const SizePolicy& p);

        /** @brief Measures the icon content size.

            The default implementation returns the logical picture size
            or the configured icon size.
        */
        virtual Gfx::SizeF onMeasureIcon();

        /** @brief Measures the text content size.

            The default implementation returns the text advance and font
            height.
        */
        virtual Gfx::SizeF onMeasureText(const String& text);

        /** @brief Aggregates icon and text sizes into total content size.

            Override to provide a custom aggregation when icon and text are
            arranged differently.
        */
        virtual Gfx::SizeF onMeasureContent(const SizePolicy& policy,
                                            const Gfx::SizeF& iconSz,
                                            const Gfx::SizeF& textSz);

    protected:
        virtual void onLayout(const Gfx::RectF& rect);

        /** @brief Lays out extra content after icon and text have been positioned.

            Called by the base onLayout after onLayoutIcon and onLayoutText have
            populated their private caches. Override to position embedded child
            controls or other extra content using the pre-computed rects.
            The default implementation does nothing.
        */
        virtual void onLayoutContent(const Gfx::RectF& innerRect,
                                     const Gfx::SizeF& iconSz,
                                     const Gfx::SizeF& textSz,
                                     const Gfx::FontMetrics& fm,
                                     Gfx::RectF& iconRect,
                                     Gfx::RectF& textRect);

    protected:
        virtual void onPaint(PaintContext& context, const Gfx::RectF& updateRect);

        /** @brief Paints the list item background layer.

            The default implementation delegates to the current %ListItemRenderer.
        */
        virtual void onPaintBackground(PaintContext& context,
                                         const ListItemState& state);

        /** @brief Paints the list item content layers.

            The default implementation sequences icon and text painting.
        */
        virtual void onPaintContent(PaintContext& context,
                                    const ListItemState& state);

        /** @brief Paints the list item icon layer.

            The default implementation does nothing if no prepared icon pixmap exists.
        */
        virtual void onPaintIcon(PaintContext& context,
                                 const Gfx::RectF& iconRect,
                                 const PixmapSurface& picture,
                                 const Gfx::PointF& iconPos,
                                 const ListItemState& state);

        /** @brief Paints the list item text layer.

            The default implementation does nothing if the item text is empty.
        */
        virtual void onPaintText(PaintContext& context,
                                 const Gfx::RectF& textRect,
                                 const String& text,
                                 const Gfx::PointF& textPos,
                                 const Gfx::FontMetrics& fm,
                                 const ListItemState& state);

    protected:
        ListItemRenderer* renderer();

    private:
        ListItemState getState() const;

        Signal<>                 _clicked;
        Pt::Signal<ListBoxItem&> _selected;
        bool                     _onClickBegin;
        bool                     _isHovered;
        bool                     _isSelectable;
        bool                     _isSelected;
        String                   _text;

        Icon                     _icon;
        Gfx::SizeF               _iconSize;

        ListItemStyle        _listItemStyle;
        StyleOptions         _listItemOptions;

        PixmapSurface        _picture;

        Gfx::SizeF           _measuredIconSz;
        Gfx::SizeF           _measuredTextSz;

        Gfx::RectF           _iconRect;
        Gfx::RectF           _textRect;
        Gfx::PointF          _iconPos;
        Gfx::PointF          _textPos;
        Gfx::FontMetrics     _fontMetrics;
};


class ListBoxLayout : public FlowLayout
{
    friend class ListBox;

    public:
        ListBoxLayout();

        const std::vector<ListBoxItem*>& selectedItems() const;

        Pt::Signal<ListBoxItem&>& selected();

    protected:
        virtual void onAddControl(Control& control);

        virtual void onRemoveControl(Control& control);

    private:
        void onItemSelected(ListBoxItem& item);

    private:
        Pt::Signal<ListBoxItem&>  _selected;
        std::vector<ListBoxItem*> _selectedItems;
};


class PT_FORMS_API ListBox : public Control
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

        virtual Gfx::SizeF onMeasure(const SizePolicy& policy);

        virtual void onLayout(const Gfx::RectF& rect);

        virtual void onPaint(PaintContext& context, const Gfx::RectF& updateRect);

    private:
        ScrollView                _scrollView;
        ListBoxLayout             _layout;
        ListBoxStyler             _styler;
        bool                      _hasBackground;
        bool                      _hasFrame;
};

} // namespace

} // namespace

#endif
