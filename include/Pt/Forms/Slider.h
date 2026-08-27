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
#include <Pt/Forms/SliderStyle.h>
#include <Pt/Signal.h>

namespace Pt {

namespace Forms {

class PT_FORMS_API Slider : public Control
{
    public:
        typedef Control Base;

    public:
        Slider();

        virtual ~Slider();

        int position() const;

        void setPosition(int pos);

        int minimum() const;

        int maximum() const;

        void setRange(int min, int max);

        bool isHighlighted() const;

        Signal<int>& positionChanged();

    public:
        const Gfx::Brush& background() const;

        void setBackground(const Gfx::Brush& b);

        const Gfx::Brush& foreground() const;

        void setForeground(const Gfx::Brush& b);

        const Gfx::Pen& contour() const;

        void setContour(const Gfx::Pen& p);

        const Gfx::Color& textColor() const;

        void setTextColor(const Gfx::Color& color);

        Gfx::Font font() const;

        void setFont(const Gfx::Font& font);

        void setFontSize(std::size_t size);

        void setFontWeight(Gfx::Font::Weight weight);

        void setFontSlant(Gfx::Font::Slant slant);

        void setRenderer(SliderRenderer* renderer);

        SliderState sliderState() const;

    protected:
        virtual Gfx::SizeF onMeasure(const SizePolicy& policy);

        virtual void onInvalidate();

        virtual void onLayout(const Gfx::RectF& rect);

        virtual void onPaint(PaintContext& context, const Gfx::RectF& updateRect);

        virtual void onPaintChrome(PaintContext& context,
                                   const Gfx::RectF& rect,
                                   const Gfx::RectF& trackRect,
                                   const Gfx::RectF& handleRect,
                                   const SliderState& state);

        virtual bool onMouseEvent(const MouseEvent& ev);

        virtual bool onTouchEvent(const TouchEvent& ev);

        virtual bool onEnterEvent(const EnterEvent& ev);

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
