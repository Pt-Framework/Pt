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

/** @brief Transient visual state of a tab view.

    %TabViewState is the snapshot a %TabView passes to measure, layout,
    and paint. It is not the application model. Enabled and focused
    describe the look of one paint pass.

    @ingroup Pt-Forms-Collections
*/
class PT_FORMS_API TabViewState
{
    public:
        /** @brief Constructs an empty tab view state.
        */
        TabViewState();

        /** @brief Returns true if the view is currently enabled.
        */
        bool isEnabled() const;

        /** @brief Sets whether the view is enabled.
        */
        void setEnabled(bool value);

        /** @brief Returns true if the view currently has focus.
        */
        bool isFocused() const;

        /** @brief Sets whether the view has focus.
        */
        void setFocused(bool value);

    private:
        bool _enabled;
        bool _focused;
};


/** @brief Transient visual state of a tab.

    %TabViewItemState is the snapshot a %TabView passes for one tab.
    It is not the application model. Enabled, active, highlighted, and
    pressed describe the look of one paint pass.

    @ingroup Pt-Forms-Collections
*/
class PT_FORMS_API TabViewItemState
{
    public:
        /** @brief Constructs an empty tab state.
        */
        TabViewItemState();

        /** @brief Returns true if the tab is currently enabled.
        */
        bool isEnabled() const;

        /** @brief Sets whether the tab is enabled.
        */
        void setEnabled(bool value);

        /** @brief Returns true if the tab is the current tab.
        */
        bool isActive() const;

        /** @brief Sets whether the tab is the current tab.
        */
        void setActive(bool value);

        /** @brief Returns true if the pointer is currently over the tab.
        */
        bool isHighlighted() const;

        /** @brief Sets whether the pointer is over the tab.
        */
        void setHighlighted(bool value);

        /** @brief Returns true if the tab is currently pressed.
        */
        bool isPressed() const;

        /** @brief Sets whether the tab is pressed.
        */
        void setPressed(bool value);

    private:
        bool _enabled;
        bool _active;
        bool _highlighted;
        bool _pressed;
};


/** @brief Renders the look of a tab view.

    A %TabViewRenderer is a %Style::Facet for the tab-view family.
    Named measure methods run inside-out. Named layout methods run
    outside-in. Named render methods paint prepared rectangles. The
    widget owns geometry and orchestrates those passes. The renderer
    does not mutate widget geometry.

    View chrome uses %TabViewState. Each tab uses %TabViewItemState.
    %renderTab() takes tab state, not view state.

    Derive a renderer to change the look of tab views. Register it
    on a %Style, or assign it to a widget with %TabView::setRenderer().

    @ingroup Pt-Forms-Collections
*/
class PT_FORMS_API TabViewRenderer : public Renderer
{
    public:
        /** @brief Constructs a renderer with reference count @a refs.
        */
        explicit TabViewRenderer(std::size_t refs = 0);

        /** @brief Destroys the renderer.
        */
        virtual ~TabViewRenderer();

        /** @brief Creates a new default-constructed instance that the caller owns.
        */
        TabViewRenderer* create() const;

    public:
        /** @brief Returns the natural size of a tab labeled @a text.
        */
        Gfx::SizeF measureTab(PaintSurface& surface,
                              const Pt::String& text);

        /** @brief Returns the label rectangle within @a tabRect.
        */
        Gfx::RectF layoutTab(PaintSurface& surface,
                             const Gfx::RectF& tabRect);

        /** @brief Returns a painter with the current font and text color.
        */
        const Painter& textPainter(PaintSurface& surface);

        /** @brief Paints the background for @a state.
        */
        void renderBackground(PaintContext& context,
                              const Gfx::RectF& contentRect,
                              const TabViewState& state);

        /** @brief Paints the frame for @a state.
        */
        void renderChrome(PaintContext& context,
                          const Gfx::RectF& contentRect,
                          const Gfx::RectF& activeTabRect,
                          const TabViewState& state);

        /** @brief Paints a tab labeled @a text for @a state.
        */
        void renderTab(PaintContext& context,
                       const Gfx::RectF& tabRect,
                       const Pt::String& text,
                       const Gfx::PointF& textPos,
                       const TabViewItemState& state);

    protected:
        /** @brief Creates a new instance of the same concrete type.
        */
        virtual TabViewRenderer* onCreate() const = 0;

        /** @copydoc Style::Facet::onReset
        */
        virtual void onReset(const StyleOptions& options) = 0;

        /** @brief Returns the natural size of a tab labeled @a text.
        */
        virtual Gfx::SizeF onMeasureTab(PaintSurface& surface,
                                        const Pt::String& text) = 0;

        /** @brief Returns the label rectangle within @a tabRect.
        */
        virtual Gfx::RectF onLayoutTab(PaintSurface& surface,
                                       const Gfx::RectF& tabRect) = 0;

        /** @brief Returns a painter with the current font and text color.
        */
        virtual const Painter& onGetTextPainter(PaintSurface& surface) = 0;

        /** @brief Paints the background for @a state.
        */
        virtual void onRenderBackground(PaintContext& context,
                                        const Gfx::RectF& contentRect,
                                        const TabViewState& state) = 0;

        /** @brief Paints the frame for @a state.
        */
        virtual void onRenderChrome(PaintContext& context,
                                    const Gfx::RectF& contentRect,
                                    const Gfx::RectF& activeTabRect,
                                    const TabViewState& state) = 0;

        /** @brief Paints a tab labeled @a text for @a state.
        */
        virtual void onRenderTab(PaintContext& context,
                                 const Gfx::RectF& tabRect,
                                 const Pt::String& text,
                                 const Gfx::PointF& textPos,
                                 const TabViewItemState& state) = 0;
};


/** @brief Binds a tab view to the current style renderer.

    A %TabView owns a %TabViewStyler. Applications do not construct
    one. Call %Styler::bind() from %onInvalidate(). When the overlay
    has no local options, bind uses the shared renderer from the style.
    Local options use a private clone. %setRenderer() keeps an assigned
    renderer until it is cleared. A null renderer falls back on the
    next bind.

    Appearance getters return effective tokens after bind. Setters
    write widget-local options. Measure, layout, and paint call the
    typed methods on this styler, not a public renderer accessor.

    @ingroup Pt-Forms-Collections
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

            A null renderer falls back to the current style on the next bind.
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
