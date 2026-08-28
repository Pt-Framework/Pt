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

#ifndef PT_FORMS_TABVIEWSTYLE_H
#define PT_FORMS_TABVIEWSTYLE_H

#include <Pt/Forms/Styler.h>

namespace Pt {

namespace Forms {

class PT_FORMS_API TabViewState
{
    public:
        TabViewState();

        bool isEnabled() const;

        void setEnabled(bool value);

        bool isFocused() const;

        void setFocused(bool value);

    private:
        bool _enabled;
        bool _focused;
};


class PT_FORMS_API TabViewItemState
{
    public:
        TabViewItemState();

        bool isEnabled() const;

        void setEnabled(bool value);

        bool isActive() const;

        void setActive(bool value);

        bool isHighlighted() const;

        void setHighlighted(bool value);

        bool isPressed() const;

        void setPressed(bool value);

    private:
        bool _enabled;
        bool _active;
        bool _highlighted;
        bool _pressed;
};


/** @brief Renders tab view chrome and tab items.
*/
class PT_FORMS_API TabViewRenderer : public Renderer
{
    public:
        /** @brief Constructs a tab view renderer.
        */
        explicit TabViewRenderer(std::size_t refs = 0);

        virtual ~TabViewRenderer();

        /** @brief Creates a new default-constructed renderer instance.
        */
        TabViewRenderer* create() const;

        /** @brief Applies the resolved tab view style options.
        */
        void prepare(const StyleOptions& options);

    public:
        Gfx::SizeF measureTab(PaintSurface& surface,
                              const Pt::String& text);

        Gfx::RectF layoutTab(PaintSurface& surface,
                             const Gfx::RectF& tabRect);

        const Painter& textPainter(PaintSurface& surface);

        void renderBackground(PaintContext& context,
                              const Gfx::RectF& contentRect,
                              const TabViewState& state);

        void renderChrome(PaintContext& context,
                          const Gfx::RectF& contentRect,
                          const Gfx::RectF& activeTabRect,
                          const TabViewState& state);

        void renderTab(PaintContext& context,
                       const Gfx::RectF& tabRect,
                       const Pt::String& text,
                       const Gfx::PointF& textPos,
                       const TabViewItemState& state);

    protected:
        /** @brief Resets the shared renderer to global style options.
        */
        virtual void onReset(const StyleOptions& options);

        virtual TabViewRenderer* onCreate() const = 0;

        /** @brief Prepares this renderer from resolved tab view style options.
        */
        virtual void onPrepare(const StyleOptions& options) = 0;

        virtual Gfx::SizeF onMeasureTab(PaintSurface& surface,
                                        const Pt::String& text) = 0;

        virtual Gfx::RectF onLayoutTab(PaintSurface& surface,
                                       const Gfx::RectF& tabRect) = 0;

        virtual const Painter& onGetTextPainter(PaintSurface& surface) = 0;

        virtual void onRenderBackground(PaintContext& context,
                                        const Gfx::RectF& contentRect,
                                        const TabViewState& state) = 0;

        virtual void onRenderChrome(PaintContext& context,
                                    const Gfx::RectF& contentRect,
                                    const Gfx::RectF& activeTabRect,
                                    const TabViewState& state) = 0;

        virtual void onRenderTab(PaintContext& context,
                                 const Gfx::RectF& tabRect,
                                 const Pt::String& text,
                                 const Gfx::PointF& textPos,
                                 const TabViewItemState& state) = 0;
};


/** @brief Binds tab view renderers and local style options.
*/
class PT_FORMS_API TabViewStyler : public Styler
{
    public:
        /** @brief Constructs an unbound tab view styler.
        */
        TabViewStyler();

        /** @brief Returns the effective background brush.
        */
        const Gfx::Brush& background() const;

        /** @brief Sets the widget-local background brush to @a brush.
        */
        void setBackground(const Gfx::Brush& brush);

        /** @brief Returns the effective contour pen.
        */
        const Gfx::Pen& contour() const;

        /** @brief Sets the widget-local contour pen to @a pen.
        */
        void setContour(const Gfx::Pen& pen);

        /** @brief Returns the effective text color.
        */
        const Gfx::Color& textColor() const;

        /** @brief Sets the widget-local text color to @a color.
        */
        void setTextColor(const Gfx::Color& color);

        /** @brief Returns the effective accent color.
        */
        const Gfx::Color& accentColor() const;

        /** @brief Sets the widget-local accent color to @a color.
        */
        void setAccentColor(const Gfx::Color& color);

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

        /** @brief Measures @a text as a tab label.
        */
        Gfx::SizeF measureTab(PaintSurface& surface,
                               const Pt::String& text) const;

        /** @brief Returns the tab-label rectangle within @a tabRect.
        */
        Gfx::RectF layoutTab(PaintSurface& surface,
                             const Gfx::RectF& tabRect) const;

        /** @brief Returns a tab-label painter for @a surface, or 0 when unavailable.
        */
        const Painter* textPainter(PaintSurface& surface) const;

        /** @brief Renders the tab view background for @a state.
        */
        void renderBackground(PaintContext& context,
                              const Gfx::RectF& contentRect,
                              const TabViewState& state) const;

        /** @brief Renders the tab view frame for @a state.
        */
        void renderChrome(PaintContext& context,
                          const Gfx::RectF& contentRect,
                          const Gfx::RectF& activeTabRect,
                          const TabViewState& state) const;

        /** @brief Renders a tab item for @a state.
        */
        void renderTab(PaintContext& context,
                       const Gfx::RectF& tabRect,
                       const Pt::String& text,
                       const Gfx::PointF& textPos,
                       const TabViewItemState& state) const;

        /** @brief Assigns a specific tab view renderer.
        */
        void setRenderer(TabViewRenderer* renderer = 0);

        /** @brief Returns the effective tab view options.
        */
        StyleOptions& options();

        /** @brief Returns the effective tab view options.
        */
        const StyleOptions& options() const;

    protected:
        /** @brief Binds the local tab view options to @a global.
        */
        virtual StyleOptions& onBindOptions(const StyleOptions& global);

        /** @brief Resolves the shared tab view renderer from @a style.
        */
        virtual Renderer* onStyleRenderer(const Style& style);

        /** @brief Creates a tab view renderer clone from @a style.
        */
        virtual Renderer* onCreateRenderer(const Style& style);

    private:
        FacetPtr<TabViewRenderer> _renderer;
        StyleOptions              _options;
};

} // namespace

} // namespace

#endif
