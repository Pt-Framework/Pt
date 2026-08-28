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

class PT_FORMS_API SliderState
{
    public:
        SliderState();

        bool isEnabled() const;

        void setEnabled(bool value);

        bool isHovered() const;

        void setHovered(bool value);

        bool isFocused() const;

        void setFocused(bool value);

    private:
        bool _enabled;
        bool _hovered;
        bool _focused;
};


/** @brief Renders the visual appearance of a slider.
*/
class PT_FORMS_API SliderRenderer : public Renderer
{
    public:
        explicit SliderRenderer(std::size_t refs = 0);

        virtual ~SliderRenderer();

        /** @brief Creates a new default-constructed slider renderer.
        */
        SliderRenderer* create() const;

        /** @brief Applies effective slider style options to this renderer.
        */
        void prepare(const StyleOptions& options);

    public:
        Gfx::SizeF measureFrame(PaintSurface& surface,
                                const Gfx::SizeF& contentSize);

        Gfx::SizeF measureTrack(PaintSurface& surface);

        Gfx::SizeF measureHandle(PaintSurface& surface);

        void layoutChrome(PaintSurface& surface,
                          const Gfx::RectF& rect,
                          const Gfx::SizeF& trackSize,
                          const Gfx::SizeF& handleSize,
                          Gfx::RectF& trackRect,
                          Gfx::RectF& handleRect);

        void layoutHandle(PaintSurface& surface,
                          const Gfx::RectF& trackRect,
                          float fraction,
                          Gfx::RectF& handleRect);

        void renderChrome(PaintContext& context,
                          const Gfx::RectF& rect,
                          const Gfx::RectF& trackRect,
                          const Gfx::RectF& handleRect,
                          const SliderState& state);

        void renderTrack(PaintContext& context,
                         const Gfx::RectF& trackRect,
                         const SliderState& state);

        void renderHandle(PaintContext& context,
                          const Gfx::RectF& handleRect,
                          const SliderState& state);

    protected:
        /** @brief Resets the shared slider renderer to global style options.
        */
        virtual void onReset(const StyleOptions& options);

        virtual SliderRenderer* onCreate() const = 0;

        /** @brief Prepares the renderer from effective slider style options.
        */
        virtual void onPrepare(const StyleOptions& options) = 0;

        virtual Gfx::SizeF onMeasureFrame(PaintSurface& surface,
                                          const Gfx::SizeF& contentSize) = 0;

        virtual Gfx::SizeF onMeasureTrack(PaintSurface& surface) = 0;

        virtual Gfx::SizeF onMeasureHandle(PaintSurface& surface) = 0;

        virtual void onLayoutChrome(PaintSurface& surface,
                                    const Gfx::RectF& rect,
                                    const Gfx::SizeF& trackSize,
                                    const Gfx::SizeF& handleSize,
                                    Gfx::RectF& trackRect,
                                    Gfx::RectF& handleRect) = 0;

        virtual void onLayoutHandle(PaintSurface& surface,
                                    const Gfx::RectF& trackRect,
                                    float fraction,
                                    Gfx::RectF& handleRect) = 0;

        virtual void onRenderChrome(PaintContext& context,
                                    const Gfx::RectF& rect,
                                    const Gfx::RectF& trackRect,
                                    const Gfx::RectF& handleRect,
                                    const SliderState& state);

        virtual void onRenderTrack(PaintContext& context,
                                   const Gfx::RectF& trackRect,
                                   const SliderState& state) = 0;

        virtual void onRenderHandle(PaintContext& context,
                                    const Gfx::RectF& handleRect,
                                    const SliderState& state) = 0;
};


/** @brief Slider styler.
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
        */
        void setRenderer(SliderRenderer* renderer = 0);

        /** @brief Returns the bound effective slider options.
        */
        StyleOptions& options();

        /** @brief Returns the bound effective slider options.
        */
        const StyleOptions& options() const;

    protected:
        /** @brief Binds local slider options to @a global and returns them.
        */
        virtual StyleOptions& onBindOptions(const StyleOptions& global);

        /** @brief Resolves the shared slider renderer from @a style.
        */
        virtual Renderer* onStyleRenderer(const Style& style);

        /** @brief Creates an independent slider renderer from @a style.
        */
        virtual Renderer* onCreateRenderer(const Style& style);

    private:
        FacetPtr<SliderRenderer> _renderer;
        StyleOptions             _options;
};

} // namespace

} // namespace

#endif
