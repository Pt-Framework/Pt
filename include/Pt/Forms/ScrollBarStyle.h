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

#include <Pt/Forms/StylerBase.h>
#include <Pt/Forms/Direction.h>

namespace Pt {

namespace Forms {

class PT_FORMS_API ScrollBarState
{
    public:
        ScrollBarState();

        bool isEnabled() const;

        void setEnabled(bool value);

        bool isFocused() const;

        void setFocused(bool value);

        bool isHandleHovered() const;

        void setHandleHovered(bool value);

        bool isHandlePressed() const;

        void setHandlePressed(bool value);

        bool isDecreaseHovered() const;

        void setDecreaseHovered(bool value);

        bool isDecreasePressed() const;

        void setDecreasePressed(bool value);

        bool isIncreaseHovered() const;

        void setIncreaseHovered(bool value);

        bool isIncreasePressed() const;

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


class PT_FORMS_API ScrollBarRenderer : public Renderer
{
    public:
        explicit ScrollBarRenderer(std::size_t refs = 0);

        virtual ~ScrollBarRenderer();

        /** @brief Creates a new default-constructed scroll bar renderer.
        */
        ScrollBarRenderer* create() const;

        /** @brief Applies the effective scroll bar style options to this renderer.
        */
        void prepare(const StyleOptions& options);

    public:
        Gfx::SizeF measureFrame(PaintSurface& surface,
                                const Gfx::SizeF& contentSize,
                                Direction direction);

        Gfx::SizeF measureTrack(PaintSurface& surface,
                                Direction direction);

        Gfx::SizeF measureHandle(PaintSurface& surface,
                                 Direction direction);

        Gfx::SizeF measureButton(PaintSurface& surface,
                                 Direction direction);

        void layoutChrome(PaintSurface& surface,
                          const Gfx::RectF& rect,
                          Direction direction,
                          const Gfx::SizeF& buttonSize,
                          Gfx::RectF& trackRect,
                          Gfx::RectF& decreaseRect,
                          Gfx::RectF& increaseRect);

        void layoutHandle(PaintSurface& surface,
                          const Gfx::RectF& trackRect,
                          Direction direction,
                          float fraction,
                          float viewProportion,
                          Gfx::RectF& handleRect);

        void renderChrome(PaintContext& context,
                          const Gfx::RectF& rect,
                          Direction direction,
                          const Gfx::RectF& trackRect,
                          const Gfx::RectF& handleRect,
                          const Gfx::RectF& decreaseRect,
                          const Gfx::RectF& increaseRect,
                          const ScrollBarState& state);

        void renderTrack(PaintContext& context,
                         const Gfx::RectF& trackRect,
                         Direction direction,
                         const ScrollBarState& state);

        void renderHandle(PaintContext& context,
                          const Gfx::RectF& handleRect,
                          Direction direction,
                          const ScrollBarState& state);

        void renderDecreaseButton(PaintContext& context,
                                  const Gfx::RectF& buttonRect,
                                  Direction direction,
                                  const ScrollBarState& state);

        void renderIncreaseButton(PaintContext& context,
                                  const Gfx::RectF& buttonRect,
                                  Direction direction,
                                  const ScrollBarState& state);

    protected:
        /** @brief Resets the shared scroll bar renderer to global defaults.
        */
        virtual void onReset(const StyleOptions& options);

        virtual ScrollBarRenderer* onCreate() const = 0;

        virtual void onPrepare(const StyleOptions& options) = 0;

        virtual Gfx::SizeF onMeasureFrame(PaintSurface& surface,
                                          const Gfx::SizeF& contentSize,
                                          Direction direction) = 0;

        virtual Gfx::SizeF onMeasureTrack(PaintSurface& surface,
                                          Direction direction) = 0;

        virtual Gfx::SizeF onMeasureHandle(PaintSurface& surface,
                                           Direction direction) = 0;

        virtual Gfx::SizeF onMeasureButton(PaintSurface& surface,
                                           Direction direction) = 0;

        virtual void onLayoutChrome(PaintSurface& surface,
                                    const Gfx::RectF& rect,
                                    Direction direction,
                                    const Gfx::SizeF& buttonSize,
                                    Gfx::RectF& trackRect,
                                    Gfx::RectF& decreaseRect,
                                    Gfx::RectF& increaseRect) = 0;

        virtual void onLayoutHandle(PaintSurface& surface,
                                    const Gfx::RectF& trackRect,
                                    Direction direction,
                                    float fraction,
                                    float viewProportion,
                                    Gfx::RectF& handleRect) = 0;

        virtual void onRenderChrome(PaintContext& context,
                                    const Gfx::RectF& rect,
                                    Direction direction,
                                    const Gfx::RectF& trackRect,
                                    const Gfx::RectF& handleRect,
                                    const Gfx::RectF& decreaseRect,
                                    const Gfx::RectF& increaseRect,
                                    const ScrollBarState& state);

        virtual void onRenderTrack(PaintContext& context,
                                   const Gfx::RectF& trackRect,
                                   Direction direction,
                                   const ScrollBarState& state) = 0;

        virtual void onRenderHandle(PaintContext& context,
                                    const Gfx::RectF& handleRect,
                                    Direction direction,
                                    const ScrollBarState& state) = 0;

        virtual void onRenderDecreaseButton(PaintContext& context,
                                            const Gfx::RectF& buttonRect,
                                            Direction direction,
                                            const ScrollBarState& state) = 0;

        virtual void onRenderIncreaseButton(PaintContext& context,
                                            const Gfx::RectF& buttonRect,
                                            Direction direction,
                                            const ScrollBarState& state) = 0;
};


/** @brief Scroll bar styler.
*/
class PT_FORMS_API ScrollBarStyler : public StylerBase
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
        */
        void setRenderer(ScrollBarRenderer* renderer = 0);

    protected:
        virtual StyleOptions& onBindOptions(const StyleOptions& global);

        virtual Renderer* onStyleRenderer(const Style& style);

        virtual Renderer* onCreateRenderer(const Style& style);

    private:
        FacetPtr<ScrollBarRenderer> _renderer;
        StyleOptions                _options;
};

} // namespace

} // namespace

#endif
