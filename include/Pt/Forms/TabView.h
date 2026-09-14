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

#ifndef Pt_Forms_TabView_H
#define Pt_Forms_TabView_H

#include <Pt/Forms/Control.h>
#include <Pt/Forms/TabViewStyler.h>
#include <Pt/Forms/StackLayout.h>
#include <Pt/Signal.h>

#include <vector>

namespace Pt {

namespace Forms {

/** @brief Item used by a tab view.

    A %TabViewItem stores the title and geometry of one tab. It is not
    a %Control. Applications do not construct or place it. %TabView
    creates tab labels from %addTab titles.

    @ingroup Pt-Forms-Collections
*/
class TabViewItem
{
    public:
        /** @brief Constructs an empty item.
        */
        TabViewItem()
        : _isPressed(false)
        {}

        /** @brief Destroys the item.
        */
        ~TabViewItem()
        {}

        /** @brief Returns the tab title.
        */
        const String& text() const
        { return _text; }

        /** @brief Sets the tab title to @a s.
        */
        void setText(const String& s)
        { _text = s; }

        /** @brief Returns the tab rectangle.
        */
        const Gfx::RectF& geometry() const
        { return _geometry; }

        /** @brief Sets the tab rectangle to @a r.
        */
        void setGeometry(const Gfx::RectF& r)
        { _geometry = r; }

        /** @brief Returns the measured size of the tab label.
        */
        const Gfx::SizeF& measuredSize() const
        { return _measuredSize; }

        /** @brief Sets the measured size of the tab label to @a s.
        */
        void setMeasuredSize(const Gfx::SizeF& s)
        { _measuredSize = s; }

        /** @brief Returns the position of the tab title.
        */
        const Gfx::PointF& textPos() const
        { return _textPos; }

        /** @brief Sets the position of the tab title to @a p.
        */
        void setTextPos(const Gfx::PointF& p)
        { _textPos = p; }

        /** @brief Returns true if the tab is the current tab.
        */
        bool isPressed() const
        { return _isPressed; }

        /** @brief Sets whether the tab is the current tab.
        */
        void setPressed(bool b)
        { _isPressed = b; }

    private:
        String      _text;
        Gfx::SizeF  _measuredSize;
        Gfx::RectF  _geometry;
        Gfx::PointF _textPos;
        bool        _isPressed;
};

/** @brief Pages selected by tabs.

    A %TabView presents several caller-owned content controls, one at
    a time. Use %addTab() to add a control with a title. Keep each
    control alive until %removeTab() detaches it. %setCurrent() shows
    that page. Choosing a tab shows its page. %setText() changes a
    tab title.

    The view owns a %TabViewStyler. On invalidate it calls
    %Styler::bind(). Appearance getters and setters overlay the
    application style. Use %setRenderer() to assign a
    %TabViewRenderer until it is cleared. Measure, layout, and paint
    operations call typed methods on the styler.

    Pages live in a %StackLayout.

    @code
    Pt::Forms::Label general;
    general.setText("General settings");

    Pt::Forms::Label network;
    network.setText("Network settings");

    Pt::Forms::TabView settings;
    settings.addTab(general, "General");
    settings.addTab(network, "Network");
    @endcode

    @ingroup Pt-Forms-Collections
*/
class PT_FORMS_API TabView : public Control
{
    public:
        typedef Control Base;

    public:
        /** @brief Creates an empty tab view.
        */
        TabView();

        /** @brief Destroys the tab view. Attached controls are not destroyed.
        */
        virtual ~TabView();

        /** @brief Returns true if the view has no tabs.
        */
        bool empty() const;

        /** @brief Returns the number of tabs.
        */
        std::size_t size() const;

        /** @brief Adds caller-owned @a control as a tab with @a title.
        */
        void addTab(Control& control, const Pt::String& title);

        /** @brief Removes the tab at index @a n.
        */
        void removeTab(std::size_t n);

        /** @brief Returns the index of the current tab.
        */
        std::size_t current() const;

        /** @brief Shows the tab at index @a n.
        */
        void setCurrent(std::size_t n);

        /** @brief Sets the title of the tab at index @a n to @a title.
        */
        void setText(std::size_t n, const Pt::String& title);

    public:
        /** @brief Sets the widget-local background brush to @a b.
        */
        void setBackground(const Gfx::Brush& b);

        /** @brief Sets whether the view paints a background.
        */
        void setBackground(bool enable);

        /** @brief Sets the widget-local contour pen to @a p.
        */
        void setContour(const Gfx::Pen& p);

        /** @brief Sets whether the view paints a frame.
        */
        void setFrame(bool enable);

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

        /** @brief Sets the widget-local text color to @a color.
        */
        void setTextColor(const Gfx::Color& color);

        /** @brief Sets the widget-local accent color to @a color.
        */
        void setAccentColor(const Gfx::Color& color);

        /** @brief Assigns @a renderer as the family renderer.

            A null renderer falls back to the current style on the next bind.
        */
        void setRenderer(TabViewRenderer* renderer);

    protected:
        /** @brief Selects a tab from a pointer press.
        */
        virtual void onProcessMouseEvent(const MouseEvent& ev);

        /** @brief Selects a tab from a touch press.
        */
        virtual void onProcessTouchEvent(const TouchEvent& ev);

        /** @brief Binds the styler.
        */
        virtual void onInvalidate();

        /** @brief Measures the tabs and current page.
        */
        virtual Gfx::SizeF onMeasure(const SizePolicy& policy);

        /** @brief Places the tabs and current page in @a rect.
        */
        virtual void onLayout(const Gfx::RectF& rect);

        /** @brief Paints the background, frame, and tabs.
        */
        virtual void onPaint(PaintContext& context, const Gfx::RectF& updateRect);

        /** @brief Paints the tab view background for @a state.
        */
        virtual void onPaintBackground(PaintContext& context,
                                       const Gfx::RectF& contentRect,
                                       const TabViewState& state);

        /** @brief Paints the tab view frame for @a state.
        */
        virtual void onPaintChrome(PaintContext& context,
                                   const Gfx::RectF& contentRect,
                                   const Gfx::RectF& activeTabRect,
                                   const TabViewState& state);

    private:
        std::size_t hitTab(const Gfx::PointF& pos) const;

        /** @brief Measures all tab labels on @a surface.
        */
        Gfx::SizeF measureTabs(PaintSurface& surface);

        /** @brief Lays out all tab labels within @a rect.
        */
        void layoutTabs(PaintSurface& surface, const Gfx::RectF& rect);

        /** @brief Renders all tab labels with @a enabled state.
        */
        void renderTabs(PaintContext& context, bool enabled);

        /** @brief Renders one tab label for @a state.
        */
        void renderTab(PaintContext& context,
                       const Gfx::RectF& tabRect,
                       const Pt::String& text,
                       const Gfx::PointF& textPos,
                       const TabViewItemState& state);

        const Gfx::RectF& currentTabRect() const;

        void onControlRemoved(std::size_t n);

    private:
        StackLayout              _stack;
        std::vector<TabViewItem> _tabs;
        std::size_t              _current;
        Gfx::RectF               _tabBarRect;

        TabViewStyler            _tabViewStyler;
        bool                     _hasBackground;
        bool                     _hasFrame;
};

} // namespace

} // namespace

#endif
