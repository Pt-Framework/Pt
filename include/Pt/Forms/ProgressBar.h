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

#ifndef Pt_Forms_ProgressBar_H
#define Pt_Forms_ProgressBar_H

#include <Pt/Forms/Control.h>
#include <Pt/Forms/ProgressBarStyler.h>
#include <Pt/Signal.h>

namespace Pt {

namespace Forms {

/** @brief Control that shows a value within a range.

    A %ProgressBar maps an integer value between %minimum() and
    %maximum() to a ratio from 0 to 1. %setRange() sets the bounds.
    %setValue() clamps the value and emits %valueChanged(). %progress()
    returns the ratio used to fill the track. %reset() returns to the
    minimum.

    The bar owns a %ProgressBarStyler. On invalidate it binds that
    styler. Appearance getters and setters overlay the application
    style. %setRenderer() assigns a %ProgressBarRenderer.
    %progressBarState() is the snapshot passed to paint layers.

    @code
    Pt::Forms::Label caption;
    caption.setText("Copying photos...");

    Pt::Forms::ProgressBar copy;
    copy.setRange(0, photoCount);
    copy.setValue(0);

    void Album::onPhotoCopied()
    {
        copy.setValue(copy.value() + 1);
    }
    @endcode

    @ingroup Pt-Forms-Displays
*/
class PT_FORMS_API ProgressBar : public Control
{
    public:
        typedef Control Base;

    public:
        /** @brief Creates a progress bar with range 0 to 100 and value 50.
        */
        ProgressBar();

        /** @brief Destroys the progress bar.
        */
        virtual ~ProgressBar();

        /** @brief Returns the upper bound of the range.
        */
        int maximum() const;

        /** @brief Returns the lower bound of the range.
        */
        int minimum() const;

        /** @brief Sets the range to @a minpos through @a maxpos.
        */
        void setRange(int minpos, int maxpos);

        /** @brief Returns the current value.
        */
        int value() const;

        /** @brief Sets the value to @a n, clamped to the range, and emits %valueChanged().
        */
        void setValue(int n);

        /** @brief Returns the filled ratio from 0 to 1.
        */
        float progress() const;

        /** @brief Sets the value to the minimum.
        */
        void reset();

        /** @brief Returns the signal emitted after %setValue() changes the value.
        */
        Signal<int>& valueChanged();

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
        void setRenderer(ProgressBarRenderer* renderer);

        /** @brief Returns the snapshot passed to paint layers.
        */
        ProgressBarState progressBarState() const;

    protected:
        /** @brief Binds the styler and requests relayout.
        */
        virtual void onInvalidate();

        /** @brief Measures the track and frame.
        */
        virtual Gfx::SizeF onMeasure(const SizePolicy& policy);

        /** @brief Places the track and filled portion in @a rect.
        */
        virtual void onLayout(const Gfx::RectF& rect);

        /** @brief Paints the chrome for the current progress.
        */
        virtual void onPaint(PaintContext& context,
                            const Gfx::RectF& updateRect);

        /** @brief Paints track, filled portion, and text for @a state.
        */
        virtual void onPaintChrome(PaintContext& context,
                                   const Gfx::RectF& rect,
                                   const Gfx::RectF& trackRect,
                                   const Gfx::RectF& chunkRect,
                                   const Gfx::RectF& textRect,
                                   const String& text,
                                   const Gfx::PointF& textPos,
                                   const ProgressBarState& state);

    private:
        Signal<int> _valueChanged;
        int _value;
        int _min;
        int _max;

        Gfx::RectF _barRect;
        Gfx::RectF _textRect;
        Gfx::RectF _trackRect;
        Gfx::RectF _chunkRect;

        ProgressBarStyler       _styler;
};

} // namespace

} // namespace

#endif
