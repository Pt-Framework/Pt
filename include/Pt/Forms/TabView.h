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

/** @brief Item for tab bars.
*/
class TabViewItem
{
    public:
        TabViewItem()
        : _isPressed(false)
        {}

        ~TabViewItem()
        {}

        const String& text() const
        { return _text; }

        void setText(const String& s)
        { _text = s; }

        const Gfx::RectF& geometry() const
        { return _geometry; }

        void setGeometry(const Gfx::RectF& r)
        { _geometry = r; }

        const Gfx::SizeF& measuredSize() const
        { return _measuredSize; }

        void setMeasuredSize(const Gfx::SizeF& s)
        { _measuredSize = s; }

        const Gfx::PointF& textPos() const
        { return _textPos; }

        void setTextPos(const Gfx::PointF& p)
        { _textPos = p; }

        bool isPressed() const
        { return _isPressed; }

        void setPressed(bool b)
        { _isPressed = b; }

    private:
        String      _text;
        Gfx::SizeF  _measuredSize;
        Gfx::RectF  _geometry;
        Gfx::PointF _textPos;
        bool        _isPressed;
};

/** @brief Tabbed view for controls.
*/
class PT_FORMS_API TabView : public Control
{
    public:
        typedef Control Base;

    public:
        TabView();

        virtual ~TabView();

        bool empty() const;

        std::size_t size() const;

        void addTab(Control& control, const Pt::String& title);

        void removeTab(std::size_t n);

        std::size_t current() const;

        void setCurrent(std::size_t n);

        void setText(std::size_t n, const Pt::String& title);

    public:
        void setBackground(const Gfx::Brush& b);

        void setBackground(bool enable);

        void setContour(const Gfx::Pen& p);

        void setFrame(bool enable);

        /** @brief Returns the effective tab view font.
        */
        Gfx::Font font() const;

        void setFont(const Gfx::Font& font);

        void setFontSize(std::size_t size);

        void setFontWeight(Gfx::Font::Weight weight);

        void setFontSlant(Gfx::Font::Slant slant);

        void setTextColor(const Gfx::Color& color);

        /** @brief Sets the local accent color for the active tab.
        */
        void setAccentColor(const Gfx::Color& color);

        void setRenderer(TabViewRenderer* renderer);

    protected:
        virtual void onProcessMouseEvent(const MouseEvent& ev);

        virtual void onProcessTouchEvent(const TouchEvent& ev);

        virtual void onInvalidate();

        virtual Gfx::SizeF onMeasure(const SizePolicy& policy);

        virtual void onLayout(const Gfx::RectF& rect);

        virtual void onPaint(PaintContext& context, const Gfx::RectF& updateRect);

        virtual void onPaintBackground(PaintContext& context,
                                       const Gfx::RectF& contentRect,
                                       const TabViewState& state);

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
