/* Copyright (C) 2017 Marc Boris Duerner
   Copyright (C) 2017 Ilja Maier

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

#ifndef Pt_Forms_Slider_H
#define Pt_Forms_Slider_H

#include <Pt/Forms/Control.h>
#include <Pt/Forms/SliderStyler.h>
#include <Pt/Signal.h>

namespace Pt {

namespace Forms {

/** @brief Value chosen along a range by dragging a handle on a track.

    A %Slider maps an integer between %minimum() and %maximum() to a
    handle on a track. %setRange() sets the bounds. %setPosition()
    clamps the value and emits %positionChanged() when it changes.
    Dragging the handle updates the position.

    The slider owns a %SliderStyler. On invalidate it calls
    %Styler::bind(). Appearance getters and setters overlay the
    application style. %setRenderer() assigns a %SliderRenderer until
    it is cleared. Measure, layout, and paint call typed methods on the
    styler. %sliderState() is the snapshot passed to paint layers.

    Track and handle are borrowable renderer primitives. The slider
    still orchestrates measure, layout, and paint through the styler.

    @code
    Pt::Forms::Slider volume;
    volume.setRange(0, 100);
    volume.setPosition(50);
    volume.positionChanged() += Pt::slot(*this, &Mixer::onVolumeChanged);

    void Mixer::onVolumeChanged(int pos)
    {
        output.setVolume(pos);
    }
    @endcode

    @ingroup Pt-Forms-Editors
*/
class PT_FORMS_API Slider : public Control
{
    public:
        typedef Control Base;

    public:
        /** @brief Creates a slider.
        */
        Slider();

        /** @brief Destroys the slider.
        */
        virtual ~Slider();

        /** @brief Returns the current position.
        */
        int position() const;

        /** @brief Sets the position to @a pos, clamped to the range, and emits %positionChanged() when it changes.
        */
        void setPosition(int pos);

        /** @brief Returns the lower bound of the range.
        */
        int minimum() const;

        /** @brief Returns the upper bound of the range.
        */
        int maximum() const;

        /** @brief Sets the range to @a min through @a max.
        */
        void setRange(int min, int max);

        /** @brief Returns true if the pointer is over the slider.
        */
        bool isHighlighted() const;

        /** @brief Returns the signal emitted when the position changes.
        */
        Signal<int>& positionChanged();

    public:
        /** @brief Returns the effective background brush.
        */
        const Gfx::Brush& background() const;

        /** @brief Sets the widget-local background brush to @a b.
        */
        void setBackground(const Gfx::Brush& b);

        /** @brief Returns the effective foreground brush.
        */
        const Gfx::Brush& foreground() const;

        /** @brief Sets the widget-local foreground brush to @a b.
        */
        void setForeground(const Gfx::Brush& b);

        /** @brief Returns the effective contour pen.
        */
        const Gfx::Pen& contour() const;

        /** @brief Sets the widget-local contour pen to @a p.
        */
        void setContour(const Gfx::Pen& p);

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
        void setRenderer(SliderRenderer* renderer);

        /** @brief Returns the transient visual state for the current paint pass.
        */
        SliderState sliderState() const;

    protected:
        /** @brief Measures the track, handle, and frame.
        */
        virtual Gfx::SizeF onMeasure(const SizePolicy& policy);

        /** @brief Binds the styler.
        */
        virtual void onInvalidate();

        /** @brief Places the track and handle in @a rect.
        */
        virtual void onLayout(const Gfx::RectF& rect);

        /** @brief Paints the track and handle.
        */
        virtual void onPaint(PaintContext& context, const Gfx::RectF& updateRect);

        /** @brief Paints the slider chrome for @a state.
        */
        virtual void onPaintChrome(PaintContext& context,
                                   const Gfx::RectF& rect,
                                   const Gfx::RectF& trackRect,
                                   const Gfx::RectF& handleRect,
                                   const SliderState& state);

        /** @brief Updates the position from a pointer drag.
        */
        virtual bool onMouseEvent(const MouseEvent& ev);

        /** @brief Updates the position from a touch drag.
        */
        virtual bool onTouchEvent(const TouchEvent& ev);

        /** @brief Highlights the slider when the pointer enters.
        */
        virtual bool onEnterEvent(const EnterEvent& ev);

        /** @brief Clears the highlight when the pointer leaves.
        */
        virtual bool onLeaveEvent(const LeaveEvent& ev);

    private:
        float toFraction() const;

        int toPosition(double x) const;

    private:
        Signal<int>           _positionChanged;
        SliderStyler          _sliderStyler;
        int                   _position;
        int                   _min;
        int                   _max;
        bool                  _isHighlighted;
        Gfx::RectF           _trackRect;
        Gfx::RectF           _handleRect;
};

} // namespace

} // namespace

#endif
