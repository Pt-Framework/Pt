/* Copyright (C) 2016 Marc Boris Duerner

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

#ifndef PT_FORMS_SCROLLBARSTYLE_H
#define PT_FORMS_SCROLLBARSTYLE_H

#include <Pt/Forms/Styler.h>
#include <Pt/Forms/Direction.h>

namespace Pt {

namespace Forms {

/** @brief Transient visual state of a scroll bar.

    %ScrollBarState is the snapshot a %ScrollBar passes to measure,
    layout, and paint. It is not the application model. Enabled,
    focused, and hover and press of the handle and buttons describe
    the look of one paint pass.

    @ingroup Pt-Forms-Collections
*/
class PT_FORMS_API ScrollBarState
{
    public:
        /** @brief Constructs an empty scroll bar state.
        */
        ScrollBarState();

        /** @brief Returns true if the bar is currently enabled.
        */
        bool isEnabled() const;

        /** @brief Sets whether the bar is enabled.
        */
        void setEnabled(bool value);

        /** @brief Returns true if the bar currently has focus.
        */
        bool isFocused() const;

        /** @brief Sets whether the bar has focus.
        */
        void setFocused(bool value);

        /** @brief Returns true if the pointer is currently over the handle.
        */
        bool isHandleHovered() const;

        /** @brief Sets whether the pointer is over the handle.
        */
        void setHandleHovered(bool value);

        /** @brief Returns true if the handle is currently pressed.
        */
        bool isHandlePressed() const;

        /** @brief Sets whether the handle is pressed.
        */
        void setHandlePressed(bool value);

        /** @brief Returns true if the pointer is currently over the decrease control.
        */
        bool isDecreaseHovered() const;

        /** @brief Sets whether the pointer is over the decrease control.
        */
        void setDecreaseHovered(bool value);

        /** @brief Returns true if the decrease control is currently pressed.
        */
        bool isDecreasePressed() const;

        /** @brief Sets whether the decrease control is pressed.
        */
        void setDecreasePressed(bool value);

        /** @brief Returns true if the pointer is currently over the increase control.
        */
        bool isIncreaseHovered() const;

        /** @brief Sets whether the pointer is over the increase control.
        */
        void setIncreaseHovered(bool value);

        /** @brief Returns true if the increase control is currently pressed.
        */
        bool isIncreasePressed() const;

        /** @brief Sets whether the increase control is pressed.
        */
        void setIncreasePressed(bool value);

    private:
        bool _enabled;
        bool _focused;
        bool _handleHovered;
        bool _handlePressed;
        bool _decreaseHovered;
        bool _decreasePressed;
        bool _increaseHovered;
        bool _increasePressed;
};


/** @brief Renders the look of a scroll bar.

    A %ScrollBarRenderer is a %Style::Facet for the scroll-bar family.
    Named measure methods run inside-out. Named layout methods run
    outside-in. Named render methods paint prepared rectangles. The
    widget owns geometry and orchestrates those passes. The renderer
    does not mutate widget geometry.

    Track, handle, and decrease and increase controls are public
    primitives.

    Derive a renderer to change the look of scroll bars. Register it
    on a %Style, or assign it to a widget with
    %ScrollBar::setRenderer().

    @ingroup Pt-Forms-Collections
*/
class PT_FORMS_API ScrollBarRenderer : public Renderer
{
    public:
        /** @brief Constructs a renderer with reference count @a refs.
        */
        explicit ScrollBarRenderer(std::size_t refs = 0);

        /** @brief Destroys the renderer.
        */
        virtual ~ScrollBarRenderer();

        /** @brief Creates a new default-constructed instance that the caller owns.
        */
        ScrollBarRenderer* create() const;

    public:
        /** @brief Returns the outer size including the frame for @a contentSize.
        */
        Gfx::SizeF measureFrame(PaintSurface& surface,
                                const Gfx::SizeF& contentSize,
                                Direction direction);

        /** @brief Returns the natural size of the track for @a direction.
        */
        Gfx::SizeF measureTrack(PaintSurface& surface,
                                Direction direction);

        /** @brief Returns the natural size of the handle for @a direction.
        */
        Gfx::SizeF measureHandle(PaintSurface& surface,
                                 Direction direction);

        /** @brief Returns the natural size of a decrease or increase control for @a direction.
        */
        Gfx::SizeF measureButton(PaintSurface& surface,
                                 Direction direction);

        /** @brief Places the track and decrease and increase controls in @a rect.
        */
        void layoutChrome(PaintSurface& surface,
                          const Gfx::RectF& rect,
                          Direction direction,
                          const Gfx::SizeF& buttonSize,
                          Gfx::RectF& trackRect,
                          Gfx::RectF& decreaseRect,
                          Gfx::RectF& increaseRect);

        /** @brief Places the handle in @a trackRect for @a fraction and @a viewProportion.
        */
        void layoutHandle(PaintSurface& surface,
                          const Gfx::RectF& trackRect,
                          Direction direction,
                          float fraction,
                          float viewProportion,
                          Gfx::RectF& handleRect);

        /** @brief Paints track, handle, and buttons for @a state.
        */
        void renderChrome(PaintContext& context,
                          const Gfx::RectF& rect,
                          Direction direction,
                          const Gfx::RectF& trackRect,
                          const Gfx::RectF& handleRect,
                          const Gfx::RectF& decreaseRect,
                          const Gfx::RectF& increaseRect,
                          const ScrollBarState& state);

        /** @brief Paints the track for @a state.
        */
        void renderTrack(PaintContext& context,
                         const Gfx::RectF& trackRect,
                         Direction direction,
                         const ScrollBarState& state);

        /** @brief Paints the handle for @a state.
        */
        void renderHandle(PaintContext& context,
                          const Gfx::RectF& handleRect,
                          Direction direction,
                          const ScrollBarState& state);

        /** @brief Paints the decrease control for @a state.
        */
        void renderDecreaseButton(PaintContext& context,
                                  const Gfx::RectF& buttonRect,
                                  Direction direction,
                                  const ScrollBarState& state);

        /** @brief Paints the increase control for @a state.
        */
        void renderIncreaseButton(PaintContext& context,
                                  const Gfx::RectF& buttonRect,
                                  Direction direction,
                                  const ScrollBarState& state);

    protected:
        /** @brief Creates a new instance of the same concrete type.
        */
        virtual ScrollBarRenderer* onCreate() const = 0;

        /** @copydoc Style::Facet::onReset
        */
        virtual void onReset(const StyleOptions& options) = 0;

        /** @brief Measures the frame enclosing @a contentSize.
        */
        virtual Gfx::SizeF onMeasureFrame(PaintSurface& surface,
                                          const Gfx::SizeF& contentSize,
                                          Direction direction) = 0;

        /** @brief Returns the natural size of the track for @a direction.
        */
        virtual Gfx::SizeF onMeasureTrack(PaintSurface& surface,
                                          Direction direction) = 0;

        /** @brief Returns the natural size of the handle for @a direction.
        */
        virtual Gfx::SizeF onMeasureHandle(PaintSurface& surface,
                                           Direction direction) = 0;

        /** @brief Returns the natural size of a decrease or increase control for @a direction.
        */
        virtual Gfx::SizeF onMeasureButton(PaintSurface& surface,
                                           Direction direction) = 0;

        /** @brief Places the track and decrease and increase controls in @a rect.
        */
        virtual void onLayoutChrome(PaintSurface& surface,
                                    const Gfx::RectF& rect,
                                    Direction direction,
                                    const Gfx::SizeF& buttonSize,
                                    Gfx::RectF& trackRect,
                                    Gfx::RectF& decreaseRect,
                                    Gfx::RectF& increaseRect) = 0;

        /** @brief Places the handle in @a trackRect for @a fraction and @a viewProportion.
        */
        virtual void onLayoutHandle(PaintSurface& surface,
                                    const Gfx::RectF& trackRect,
                                    Direction direction,
                                    float fraction,
                                    float viewProportion,
                                    Gfx::RectF& handleRect) = 0;

        /** @brief Paints track, handle, and buttons for @a state.
        */
        virtual void onRenderChrome(PaintContext& context,
                                    const Gfx::RectF& rect,
                                    Direction direction,
                                    const Gfx::RectF& trackRect,
                                    const Gfx::RectF& handleRect,
                                    const Gfx::RectF& decreaseRect,
                                    const Gfx::RectF& increaseRect,
                                    const ScrollBarState& state);

        /** @brief Paints the track for @a state.
        */
        virtual void onRenderTrack(PaintContext& context,
                                   const Gfx::RectF& trackRect,
                                   Direction direction,
                                   const ScrollBarState& state) = 0;

        /** @brief Paints the handle for @a state.
        */
        virtual void onRenderHandle(PaintContext& context,
                                    const Gfx::RectF& handleRect,
                                    Direction direction,
                                    const ScrollBarState& state) = 0;

        /** @brief Paints the decrease control for @a state.
        */
        virtual void onRenderDecreaseButton(PaintContext& context,
                                            const Gfx::RectF& buttonRect,
                                            Direction direction,
                                            const ScrollBarState& state) = 0;

        /** @brief Paints the increase control for @a state.
        */
        virtual void onRenderIncreaseButton(PaintContext& context,
                                            const Gfx::RectF& buttonRect,
                                            Direction direction,
                                            const ScrollBarState& state) = 0;
};


/** @brief Binds a scroll bar to the current style renderer.

    A %ScrollBar owns a %ScrollBarStyler. Applications do not construct
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
class PT_FORMS_API ScrollBarStyler : public Styler
{
    public:
        /** @brief Constructs an unbound scroll bar styler.
        */
        ScrollBarStyler();

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

        /** @brief Measures the frame enclosing @a contentSize.
        */
        Gfx::SizeF measureFrame(PaintSurface& surface,
                                const Gfx::SizeF& contentSize,
                                Direction direction) const;

        /** @brief Measures a decrease or increase button for @a direction.
        */
        Gfx::SizeF measureButton(PaintSurface& surface,
                                 Direction direction) const;

        /** @brief Lays out the track and decrease and increase buttons within @a rect.
        */
        void layoutChrome(PaintSurface& surface,
                          const Gfx::RectF& rect,
                          Direction direction,
                          const Gfx::SizeF& buttonSize,
                          Gfx::RectF& trackRect,
                          Gfx::RectF& decreaseRect,
                          Gfx::RectF& increaseRect) const;

        /** @brief Lays out the handle within @a trackRect.
        */
        void layoutHandle(PaintSurface& surface,
                          const Gfx::RectF& trackRect,
                          Direction direction,
                          float fraction,
                          float viewProportion,
                          Gfx::RectF& handleRect) const;

        /** @brief Renders the scroll bar chrome within @a rect for @a state.
        */
        void renderChrome(PaintContext& context,
                          const Gfx::RectF& rect,
                          Direction direction,
                          const Gfx::RectF& trackRect,
                          const Gfx::RectF& handleRect,
                          const Gfx::RectF& decreaseRect,
                          const Gfx::RectF& increaseRect,
                          const ScrollBarState& state) const;

        /** @brief Assigns a specific scroll bar renderer.

            A null renderer falls back to the current style on the next bind.
        */
        void setRenderer(ScrollBarRenderer* renderer = 0);

    protected:
        /** @brief Binds the overlay to @a global and returns it.
        */
        virtual StyleOptions& onBindOptions(const StyleOptions& global);

        /** @brief Returns the shared scroll-bar renderer from @a style, or 0.
        */
        virtual Renderer* onStyleRenderer(const Style& style);

        /** @brief Creates an independent clone of the style renderer, or 0.
        */
        virtual Renderer* onCreateRenderer(const Style& style);

    private:
        FacetPtr<ScrollBarRenderer> _renderer;
        StyleOptions                _options;
};

} // namespace

} // namespace

#endif
