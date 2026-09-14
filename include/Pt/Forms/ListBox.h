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
#include <Pt/Forms/ListBoxStyler.h>
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

/** @brief Item in a list box.

    A %ListBoxItem is a %Control the application creates and adds to a
    %ListBox. %ComboBox uses the same type. %setText() assigns the
    caption. %setIcon() places a picture beside the text.
    %setSelectable() controls whether a completed click toggles
    %isSelected(). %selected() is emitted when the selected state is
    set.

    The item owns a %ListItemStyler. On invalidate it calls
    %Styler::bind(). Appearance getters and setters overlay the
    application style. %setRenderer() assigns a %ListItemRenderer
    until it is cleared. Measure, layout, and paint call typed methods
    on the styler. %ListItemState is the snapshot passed to paint
    layers.

    @ingroup Pt-Forms-Collections
*/
class PT_FORMS_API ListBoxItem : public Control
{
        typedef Control Base;

      public:
        /** @brief Creates an empty list item.
        */
        ListBoxItem();

        /** @brief Destroys the list item.
        */
        virtual ~ListBoxItem();

        /** @brief Returns true if a click can change the selected state.
        */
        bool isSelectable() const;

        /** @brief Sets whether a click can change the selected state.
        */
        void setSelectable(bool b);

        /** @brief Returns true if the item is selected.
        */
        bool isSelected() const;

        /** @brief Sets whether the item is selected and emits %selected().

            When the item is not selectable the selected state does not
            change.
        */
        void setSelected(bool b);

        /** @brief Sets the caption to @a t.
        */
        void setText(const Pt::String& t);

        /** @brief Returns the caption.
        */
        const Pt::String& text() const;

        /** @brief Sets the icon shown beside the caption.

            @a size is the logical size requested from @a icon.
        */
        void setIcon(const Icon& icon, const Gfx::SizeF& size);

        /** @brief Returns the logical icon size.
        */
        const Gfx::SizeF& iconSize() const
        { return _iconSize; }

        /** @brief Returns true if the pointer is over the item.
        */
        bool isHovered() const;

        /** @brief Completes a press and release and toggles selection.
        */
        void click();

        /** @brief Returns the clicked signal.
        */
        Signal<>& clicked();

        /** @brief Returns the signal emitted when the selected state is set.
        */
        Pt::Signal<ListBoxItem&>& selected();

    public:
        /** @brief Sets the widget-local background brush to @a b.
        */
        void setBackground(const Gfx::Brush& b);

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
        void setRenderer(ListItemRenderer* renderer);

    protected:
        /** @brief Completes a press or release from an action key.
        */
        virtual void onActionKey(const KeyEvent& kev);

        /** @brief Completes a press and release from a shortcut.
        */
        virtual void onShortcut(const Key& key);

        /** @brief Completes a press and release from a mnemonic.
        */
        virtual void onMnemonic(Pt::Char m);

        /** @brief Marks the start of a click.
        */
        virtual void onPressed();

        /** @brief Toggles selection at the end of a completed click.
        */
        virtual void onReleased();

        /** @brief Cancels an incomplete click.
        */
        virtual void onCanceled();

        /** @brief Highlights the item when the pointer enters.
        */
        virtual bool onEnterEvent(const EnterEvent& ev);

        /** @brief Clears the highlight when the pointer leaves.
        */
        virtual bool onLeaveEvent(const LeaveEvent& ev);

        /** @brief Completes or cancels a click from a pointer press and release.
        */
        virtual bool onMouseEvent(const MouseEvent& ev);

        /** @brief Completes or cancels a click from a touch press and release.
        */
        virtual bool onTouchEvent(const TouchEvent& ev);

        /** @brief Cancels an incomplete click when the item is scrolled.
        */
        virtual bool onScrollEvent(const ScrollEvent& ev);

    protected:
        /** @brief Binds the styler.
        */
        virtual void onInvalidate();

    protected:
        /** @brief Measures the icon, text, and frame.
        */
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
        /** @brief Places the icon and text in @a rect.
        */
        virtual void onLayout(const Gfx::RectF& rect);

        /** @brief Lays out extra content after icon and text have been positioned.

            Called by %onLayout() after icon and text rectangles are
            prepared. Override to position embedded child controls or
            other extra content. The default implementation does
            nothing.
        */
        virtual void onLayoutContent(const Gfx::RectF& innerRect,
                                     const Gfx::SizeF& iconSz,
                                     const Gfx::SizeF& textSz,
                                     const Gfx::FontMetrics& fm,
                                     Gfx::RectF& iconRect,
                                     Gfx::RectF& textRect);

    protected:
        /** @brief Paints the background, highlight, icon, and text.
        */
        virtual void onPaint(PaintContext& context, const Gfx::RectF& updateRect);

        /** @brief Paints the list item background layer.

            The default implementation delegates to the current %ListItemRenderer.
        */
        virtual void onPaintBackground(PaintContext& context,
                                       const ListItemState& state);

        /** @brief Paints the list item highlight layer.

            The default implementation delegates to the current %ListItemRenderer.
        */
        virtual void onPaintHighlight(PaintContext& context,
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

    private:
        ListItemState getState() const;

    private:
        Signal<>                 _clicked;
        Pt::Signal<ListBoxItem&> _selected;
        bool                     _onClickBegin;
        bool                     _isHovered;
        bool                     _isSelectable;
        bool                     _isSelected;
        bool                     _hasBackground;
        String                   _text;

        Icon                     _icon;
        Gfx::SizeF               _iconSize;

        ListItemStyler           _listItemStyle;

        PixmapSurface            _picture;

        Gfx::SizeF               _measuredIconSz;
        Gfx::SizeF               _measuredTextSz;

        Gfx::RectF               _iconRect;
        Gfx::RectF               _textRect;
        Gfx::PointF              _iconPos;
        Gfx::PointF              _textPos;
        Gfx::FontMetrics         _fontMetrics;
};


/** @brief Layout that tracks selected list items.

    A %ListBoxLayout is a %FlowLayout used by a %ListBox. It is an
    integrated subpart. Applications do not construct or place it.

    @ingroup Pt-Forms-Collections
*/
class ListBoxLayout : public FlowLayout
{
    friend class ListBox;

    public:
        /** @brief Creates an empty list layout.
        */
        ListBoxLayout();

        /** @brief Returns the currently selected items.
        */
        const std::vector<ListBoxItem*>& selectedItems() const;

        /** @brief Returns the signal emitted when an item's selected state is set.
        */
        Pt::Signal<ListBoxItem&>& selected();

    protected:
        /** @brief Forwards the added control to the base layout.
        */
        virtual void onAddControl(Control& control);

        /** @brief Removes @a control from the selected items.
        */
        virtual void onRemoveControl(Control& control);

    private:
        void onItemSelected(ListBoxItem& item);

    private:
        Pt::Signal<ListBoxItem&>  _selected;
        std::vector<ListBoxItem*> _selectedItems;
};


/** @brief Scrollable list of items.

    A %ListBox presents caller-owned %ListBoxItem objects. Use
    %addItem() to add an item. Keep each item alive until
    %removeItem() detaches it. A completed click on a selectable item
    toggles %ListBoxItem::isSelected(). %selected() emits that item.
    %selectedItems() returns the items that are currently selected.
    Calling %setScrollBars() shows scroll bars when the list does not
    fit.

    The list owns a %ListBoxStyler. On invalidate it calls
    %Styler::bind(). Appearance getters and setters overlay the
    application style. Use %setRenderer() to assign a
    %ListBoxRenderer until it is cleared. Measure, layout, and paint
    operations call typed methods on the styler.

    Items are hosted in a %ScrollView.

    @code
    Pt::Forms::ListBoxItem berlin;
    berlin.setText("Berlin");

    Pt::Forms::ListBox cities;
    cities.addItem(berlin);
    cities.selected() += Pt::slot(*this, &AddressForm::onCitySelected);

    void AddressForm::onCitySelected(Pt::Forms::ListBoxItem& item)
    {
        address.setCity(item.text());
    }
    @endcode

    @ingroup Pt-Forms-Collections
*/
class PT_FORMS_API ListBox : public Control
{
    typedef Control Base;

    public:
        /** @brief Creates an empty list box.
        */
        ListBox();

        /** @brief Destroys the list box. Attached items are not destroyed.
        */
        virtual ~ListBox();

        /** @brief Sets whether the list shows scroll bars.
        */
        void setScrollBars(bool hasScrollBars);

        /** @brief Adds caller-owned @a item to the list.
        */
        void addItem(ListBoxItem& item);

        /** @brief Removes @a item from the list.
        */
        void removeItem(ListBoxItem& item);

        /** @brief Returns the currently selected items.
        */
        const std::vector<ListBoxItem*>& selectedItems() const;

        /** @brief Returns the signal emitted when an item's selected state is set.
        */
        Pt::Signal<ListBoxItem&>& selected();

        /** @brief Scrolls the list horizontally to @a xpos.
        */
        void scrollX(int xpos);

        /** @brief Scrolls the list vertically to @a ypos.
        */
        void scrollY(int ypos);

        /** @brief Returns the maximum horizontal scroll offset.
        */
        int maximumX() const;

        /** @brief Returns the maximum vertical scroll offset.
        */
        int maximumY() const;

    public:
        /** @brief Returns the effective background brush, or 0 when disabled.
        */
        const Gfx::Brush* background() const;

        /** @brief Sets the widget-local background brush to @a b.
        */
        void setBackground(const Gfx::Brush& b);

        /** @brief Sets whether the list paints a background.
        */
        void setBackground(bool b);

        /** @brief Returns the effective contour pen, or 0 when the frame is off.
        */
        const Gfx::Pen* contour() const;

        /** @brief Sets the widget-local contour pen to @a pen.
        */
        void setContour(const Gfx::Pen& pen);

        /** @brief Sets whether the list paints a frame.
        */
        void setFrame(bool b);

        /** @brief Assigns @a renderer as the family renderer.

            A null renderer falls back to the current style on the next bind.
        */
        void setRenderer(ListBoxRenderer* renderer);

    protected:
        /** @brief Binds the styler.
        */
        virtual void onInvalidate();

        /** @brief Measures the items and frame.
        */
        virtual Gfx::SizeF onMeasure(const SizePolicy& policy);

        /** @brief Places the scroll view in @a rect.
        */
        virtual void onLayout(const Gfx::RectF& rect);

        /** @brief Paints the background and frame.
        */
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
