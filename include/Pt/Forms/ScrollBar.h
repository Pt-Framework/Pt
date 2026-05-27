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
#include <Pt/Forms/StyleFlags.h>
#include <Pt/SmartPtr.h>

namespace Pt {

namespace Forms {

class PT_FORMS_API ScrollBar : public Control
{
    typedef Control Base;

    public:
        enum Orientation
        {
            Horizontal = 0,
            Vertical = 1
        };

        explicit ScrollBar(Orientation o);

        ~ScrollBar();

        void setRange(double minpos, double maxpos);

        void setStepping(double scroll, double page);

        double minimumPosition() const;

        double maximumPosition() const;

        double position() const;

        /** @brief Updates the position without scrolling.
        */
        void setPosition(double pos);

        /** @brief Scrolls to an absolute position.
        */
        void scroll(double pos);

        Signal<double>& changed()
        { return _changed; }

    public:
        const Gfx::Brush& background() const;

        void setBackground(const Gfx::Brush& b);

        const Gfx::Brush& foreground() const;

        void setForeground(const Gfx::Brush& b);

        const Gfx::Pen& contour() const;

        void setContour(const Gfx::Pen& p);

        void setRenderer(ScrollBarRenderer* renderer);

    protected:
        virtual void onInvalidate();

        virtual void onLayout(const Gfx::RectF& rect);

        virtual void onPaint(PaintContext& context, const Gfx::RectF& rect);

        Gfx::SizeF onMeasure(const SizePolicy& s);

        virtual bool onMouseEvent(const MouseEvent& ev);

        virtual bool onTouchEvent(const TouchEvent& ev);

        virtual bool onEnterEvent(const EnterEvent& ev);

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

        ScrollBarStyleFlags scrollBarStyleFlags() const;

        ButtonStyleFlags decreaseButtonFlags() const;

        ButtonStyleFlags increaseButtonFlags() const;

        Direction direction() const;

        float fraction() const;

        float viewProportion() const;

        HotZone hitTest(const Gfx::PointF& pos);

        Gfx::RectF currentHandleRect();

        ScrollBarRenderer* getRenderer();

        void applyRenderer(ScrollBarRenderer* renderer);

    private:
        enum OverrideFlags : unsigned
        {
            OverrideBackground = 0x01,
            OverrideForeground = 0x02,
            OverrideContour    = 0x04
        };

        Orientation    _orientation;
        double         _minPos;
        double         _maxPos;
        double         _pageStep;
        double         _scrollStep;
        double         _position;
        bool           _dragging;
        Signal<double> _changed;

        Gfx::RectF               _trackRect;
        Gfx::RectF               _decreaseRect;
        Gfx::RectF               _increaseRect;
        HotZone                  _hoveredZone;
        HotZone                  _pressedZone;

        FacetPtr<ScrollBarRenderer>  _renderer;
        bool                         _customRenderer;
        std::size_t                  _styleGeneration;

        AutoPtr<Gfx::Brush>          _background;
        AutoPtr<Gfx::Brush>          _foreground;
        AutoPtr<Gfx::Pen>            _contour;
        unsigned                     _overrides;
};

} // namespace

} // namespace

#endif
