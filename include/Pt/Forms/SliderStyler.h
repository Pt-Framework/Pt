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

#ifndef PT_FORMS_SLIDERSTYLE_H
#define PT_FORMS_SLIDERSTYLE_H

#include <Pt/Forms/Styler.h>

namespace Pt {

namespace Forms {

/** @brief Transient visual state of a slider.

    %SliderState is the snapshot a %Slider passes to measure, layout,
    and paint. It is not the application model. Enabled, hovered, and
    focused describe the look of one paint pass.

    @ingroup Pt-Forms-Editors
*/
class PT_FORMS_API SliderState
{
    public:
        /** @brief Constructs an empty slider state.
        */
        SliderState();

        /** @brief Returns true if the slider is currently enabled.
        */
        bool isEnabled() const;

        /** @brief Sets whether the slider is enabled.
        */
        void setEnabled(bool value);

        /** @brief Returns true if the pointer is currently over the slider.
        */
        bool isHovered() const;

        /** @brief Sets whether the pointer is over the slider.
        */
        void setHovered(bool value);

        /** @brief Returns true if the slider currently has focus.
        */
        bool isFocused() const;

        /** @brief Sets whether the slider has focus.
        */
        void setFocused(bool value);

    private:
        bool _enabled;
        bool _hovered;
        bool _focused;
};


/** @brief Renders the look of a slider.

    A %SliderRenderer is a %Style::Facet for the slider family.
    Named measure methods run inside-out. Named layout methods run
    outside-in. Named render methods paint prepared rectangles. The
    widget owns geometry and orchestrates those passes. The renderer
    does not mutate widget geometry.

    Track and handle are public primitives. Custom widgets such as
    scrubbers, timelines, mixers, range editors, and color controls
    may reuse them. %renderChrome() may paint track and handle
    together. A derived renderer may override %renderTrack() and
    %renderHandle() instead.

    Derive a renderer to change the look of sliders. Register it
    on a %Style, or assign it to a widget with %Slider::setRenderer().

    @ingroup Pt-Forms-Editors
*/
class PT_FORMS_API SliderRenderer : public Renderer
{
    public:
        /** @brief Constructs a renderer with reference count @a refs.
        */
        explicit SliderRenderer(std::size_t refs = 0);

        /** @brief Destroys the renderer.
        */
        virtual ~SliderRenderer();

        /** @brief Creates a new default-constructed instance that the caller owns.
        */
        SliderRenderer* create() const;

    public:
        /** @brief Returns the outer size including the frame for @a contentSize.
        */
        Gfx::SizeF measureFrame(PaintSurface& surface,
                                const Gfx::SizeF& contentSize);

        /** @brief Returns the natural size of the track.
        */
        Gfx::SizeF measureTrack(PaintSurface& surface);

        /** @brief Returns the natural size of the handle.
        */
        Gfx::SizeF measureHandle(PaintSurface& surface);

        /** @brief Places the track and handle in @a rect.
        */
        void layoutChrome(PaintSurface& surface,
                          const Gfx::RectF& rect,
                          const Gfx::SizeF& trackSize,
                          const Gfx::SizeF& handleSize,
                          Gfx::RectF& trackRect,
                          Gfx::RectF& handleRect);

        /** @brief Places the handle for @a fraction along @a trackRect.
        */
        void layoutHandle(PaintSurface& surface,
                          const Gfx::RectF& trackRect,
                          float fraction,
                          Gfx::RectF& handleRect);

        /** @brief Paints track and handle for @a state.
        */
        void renderChrome(PaintContext& context,
                          const Gfx::RectF& rect,
                          const Gfx::RectF& trackRect,
                          const Gfx::RectF& handleRect,
                          const SliderState& state);

        /** @brief Paints the track for @a state.
        */
        void renderTrack(PaintContext& context,
                         const Gfx::RectF& trackRect,
                         const SliderState& state);

        /** @brief Paints the handle for @a state.
        */
        void renderHandle(PaintContext& context,
                          const Gfx::RectF& handleRect,
                          const SliderState& state);

    protected:
        /** @brief Creates a new instance of the same concrete type.
        */
        virtual SliderRenderer* onCreate() const = 0;

        /** @copydoc Style::Facet::onReset
        */
        virtual void onReset(const StyleOptions& options) = 0;

        /** @brief Measures the frame enclosing @a contentSize.
        */
        virtual Gfx::SizeF onMeasureFrame(PaintSurface& surface,
                                          const Gfx::SizeF& contentSize) = 0;

        /** @brief Returns the natural size of the track.
        */
        virtual Gfx::SizeF onMeasureTrack(PaintSurface& surface) = 0;

        /** @brief Returns the natural size of the handle.
        */
        virtual Gfx::SizeF onMeasureHandle(PaintSurface& surface) = 0;

        /** @brief Places the track and handle in @a rect.
        */
        virtual void onLayoutChrome(PaintSurface& surface,
                                    const Gfx::RectF& rect,
                                    const Gfx::SizeF& trackSize,
                                    const Gfx::SizeF& handleSize,
                                    Gfx::RectF& trackRect,
                                    Gfx::RectF& handleRect) = 0;

        /** @brief Places the handle for @a fraction along @a trackRect.
        */
        virtual void onLayoutHandle(PaintSurface& surface,
                                    const Gfx::RectF& trackRect,
                                    float fraction,
                                    Gfx::RectF& handleRect) = 0;

        /** @brief Paints track and handle for @a state.
        */
        virtual void onRenderChrome(PaintContext& context,
                                    const Gfx::RectF& rect,
                                    const Gfx::RectF& trackRect,
                                    const Gfx::RectF& handleRect,
                                    const SliderState& state);

        /** @brief Paints the track for @a state.
        */
        virtual void onRenderTrack(PaintContext& context,
                                   const Gfx::RectF& trackRect,
                                   const SliderState& state) = 0;

        /** @brief Paints the handle for @a state.
        */
        virtual void onRenderHandle(PaintContext& context,
                                    const Gfx::RectF& handleRect,
                                    const SliderState& state) = 0;
};


/** @brief Binds a slider to the current style renderer.

    A %Slider owns a %SliderStyler. Applications do not construct
    one. Call %Styler::bind() from %onInvalidate(). When the overlay
    has no local options, bind uses the shared renderer from the style.
    Local options use a private clone. %setRenderer() keeps an assigned
    renderer until it is cleared. A null renderer falls back on the
    next bind.

    Appearance getters return effective tokens after bind. Setters
    write widget-local options. Measure, layout, and paint call the
    typed methods on this styler, not a public renderer accessor.

    @ingroup Pt-Forms-Editors
*/
class PT_FORMS_API SliderStyler : public Styler
{
    public:
        /** @brief Constructs an unbound slider styler.
        */
        SliderStyler();

        /** @brief Returns the effective background brush.
        */
        const Gfx::Brush& background() const;

        /** @brief Sets the widget-local background brush to @a brush.
        */
        void setBackground(const Gfx::Brush& brush);

        /** @brief Returns the effective foreground brush.
        */
        const Gfx::Brush& foreground() const;

        /** @brief Sets the widget-local foreground brush to @a brush.
        */
        void setForeground(const Gfx::Brush& brush);

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

        /** @brief Measures the frame enclosing @a contentSize.
        */
        Gfx::SizeF measureFrame(PaintSurface& surface,
                                const Gfx::SizeF& contentSize) const;

        /** @brief Measures the slider track.
        */
        Gfx::SizeF measureTrack(PaintSurface& surface) const;

        /** @brief Measures the slider handle.
        */
        Gfx::SizeF measureHandle(PaintSurface& surface) const;

        /** @brief Lays out the slider track and handle rectangles.
        */
        void layoutChrome(PaintSurface& surface,
                          const Gfx::RectF& rect,
                          const Gfx::SizeF& trackSize,
                          const Gfx::SizeF& handleSize,
                          Gfx::RectF& trackRect,
                          Gfx::RectF& handleRect) const;

        /** @brief Lays out the handle for @a fraction along @a trackRect.
        */
        void layoutHandle(PaintSurface& surface,
                          const Gfx::RectF& trackRect,
                          float fraction,
                          Gfx::RectF& handleRect) const;

        /** @brief Renders the slider chrome within @a rect for @a state.
        */
        void renderChrome(PaintContext& context,
                          const Gfx::RectF& rect,
                          const Gfx::RectF& trackRect,
                          const Gfx::RectF& handleRect,
                          const SliderState& state) const;

        /** @brief Assigns a specific slider renderer.

            A null renderer falls back to the current style on the next bind.
        */
        void setRenderer(SliderRenderer* renderer = 0);

        /** @brief Returns the bound effective slider options.
        */
        StyleOptions& options();

        /** @brief Returns the bound effective slider options.
        */
        const StyleOptions& options() const;

    protected:
        /** @brief Binds the overlay to @a global and returns it.
        */
        virtual StyleOptions& onBindOptions(const StyleOptions& global);

        /** @brief Returns the shared slider renderer from @a style, or 0.
        */
        virtual Renderer* onStyleRenderer(const Style& style);

        /** @brief Creates an independent clone of the style renderer, or 0.
        */
        virtual Renderer* onCreateRenderer(const Style& style);

    private:
        FacetPtr<SliderRenderer> _renderer;
        StyleOptions             _options;
};

} // namespace

} // namespace

#endif
