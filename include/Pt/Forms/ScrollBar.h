/* Copyright (C) 2016 Marc Boris Duerner
   Copyright (C) 2016 Laurentiu-Gheorghe Crisan

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

#ifndef Pt_Forms_SCROLLBAR_H
#define Pt_Forms_SCROLLBAR_H

#include <Pt/Forms/Api.h>
#include <Pt/Forms/Control.h>
#include <Pt/Forms/Direction.h>
#include <Pt/Forms/ScrollBarStyler.h>

namespace Pt {

namespace Forms {

/** @brief Position in a range shown by a handle on a track.

    A %ScrollBar maps a value between %minimumPosition() and
    %maximumPosition() to a handle on a track. %setRange() sets the
    bounds. %setPosition() clamps the value. %scroll() clamps the
    value and emits %changed(). Decrease and increase controls step
    by %setStepping(). The constructor sets %Orientation.

    The bar owns a %ScrollBarStyler. On invalidate it calls
    %Styler::bind(). Appearance getters and setters overlay the
    application style. %setRenderer() assigns a %ScrollBarRenderer
    until it is cleared. Measure, layout, and paint call typed methods
    on the styler. %scrollBarState() is the snapshot passed to paint
    layers.

    Track, handle, and decrease and increase controls are renderer
    primitives. The bar still orchestrates measure, layout, and paint
    through the styler.

    @code
    Pt::Forms::ScrollBar volume(Pt::Forms::ScrollBar::Horizontal);
    volume.setRange(0, 100);
    volume.setPosition(50);
    volume.changed() += Pt::slot(*this, &Mixer::onVolume);

    void Mixer::onVolume(double pos)
    {
        output.setVolume(pos);
    }
    @endcode

    @ingroup Pt-Forms-Collections
*/
class PT_FORMS_API ScrollBar : public Control
{
    typedef Control Base;

    public:
        /** @brief Direction of the track.
        */
        enum Orientation
        {
            /** @brief Track runs left to right.
            */
            Horizontal = 0,

            /** @brief Track runs top to bottom.
            */
            Vertical = 1
        };

        /** @brief Creates a scroll bar with @a o orientation.
        */
        explicit ScrollBar(Orientation o);

        /** @brief Destroys the scroll bar.
        */
        ~ScrollBar();

        /** @brief Sets the range to @a minpos through @a maxpos.
        */
        void setRange(double minpos, double maxpos);

        /** @brief Sets the step to @a scroll and the page step to @a page.
        */
        void setStepping(double scroll, double page);

        /** @brief Returns the lower bound of the range.
        */
        double minimumPosition() const;

        /** @brief Returns the upper bound of the range.
        */
        double maximumPosition() const;

        /** @brief Returns the current position.
        */
        double position() const;

        /** @brief Sets the position to @a pos, clamped to the range.
        */
        void setPosition(double pos);

        /** @brief Sets the position to @a pos, clamped to the range, and emits %changed().
        */
        void scroll(double pos);

        /** @brief Returns the signal emitted when %scroll() changes the position.
        */
        Signal<double>& changed()
        { return _changed; }

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

        /** @brief Assigns @a renderer as the family renderer.

            A null renderer falls back to the current style on the next bind.
        */
        void setRenderer(ScrollBarRenderer* renderer);

        /** @brief Returns the transient visual state for the current paint pass.
        */
        ScrollBarState scrollBarState() const;

    protected:
        /** @brief Binds the styler.
        */
        virtual void onInvalidate();

        /** @brief Places the track, handle, and buttons in @a rect.
        */
        virtual void onLayout(const Gfx::RectF& rect);

        /** @brief Paints the track, handle, and buttons.
        */
        virtual void onPaint(PaintContext& context, const Gfx::RectF& rect);

        /** @brief Paints the scroll bar chrome for @a state.
        */
        virtual void onPaintChrome(PaintContext& context,
                                   const Gfx::RectF& rect,
                                   Direction direction,
                                   const Gfx::RectF& trackRect,
                                   const Gfx::RectF& handleRect,
                                   const Gfx::RectF& decreaseRect,
                                   const Gfx::RectF& increaseRect,
                                   const ScrollBarState& state);

        /** @brief Measures the track, handle, and buttons.
        */
        Gfx::SizeF onMeasure(const SizePolicy& s);

        /** @brief Updates the position from a pointer press or drag.
        */
        virtual bool onMouseEvent(const MouseEvent& ev);

        /** @brief Updates the position from a touch press or drag.
        */
        virtual bool onTouchEvent(const TouchEvent& ev);

        /** @brief Highlights a hot zone when the pointer enters.
        */
        virtual bool onEnterEvent(const EnterEvent& ev);

        /** @brief Clears the highlight when the pointer leaves.
        */
        virtual bool onLeaveEvent(const LeaveEvent& ev);

    private:
        enum HotZone
        {
            NoZone = 0,
            TrackZone,
            HandleZone,
            DecreaseZone,
            IncreaseZone
        };

        Direction direction() const;

        float fraction() const;

        float viewProportion() const;

        HotZone hitTest(const Gfx::PointF& pos);

        Gfx::RectF currentHandleRect();

    private:
        Orientation              _orientation;
        double                   _minPos;
        double                   _maxPos;
        double                   _pageStep;
        double                   _scrollStep;
        double                   _position;
        bool                     _dragging;
        Signal<double>           _changed;

        ScrollBarStyler          _styler;
        Gfx::RectF               _trackRect;
        Gfx::RectF               _decreaseRect;
        Gfx::RectF               _increaseRect;
        HotZone                  _hoveredZone;
        HotZone                  _pressedZone;
};

} // namespace

} // namespace

#endif
