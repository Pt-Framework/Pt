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

#ifndef Pt_Hmi_SCROLLBAR_H
#define Pt_Hmi_SCROLLBAR_H

#include <Pt/Hmi/Api.h>
#include <Pt/Hmi/Control.h>
#include <Pt/SmartPtr.h>

namespace Pt {

namespace Hmi {

class PT_HMI_API ScrollBar : public Control
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

        const Pt::Gfx::RectF& handleRect() const
        {
          return _handleRect;
        }

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

        virtual void onPaint(Gfx::PaintSurface& surface, const Gfx::RectF& rect);

    protected:

        Gfx::SizeF onMeasure(const SizePolicy& s);;

        virtual bool onMouseEvent(const MouseEvent& ev);

        virtual bool onTouchEvent(const TouchEvent& ev);

        virtual void onResizeEvent(const ResizeEvent& ev);

    private:
        double pixelToPosition(double pix);
        
        double positionToPixel(double pos);

        void updateScroll();

    private:
        Orientation    _orientation;
        double         _minPos;
        double         _maxPos;
        double         _pageStep;
        double         _scrollStep;
        double         _position;
        bool           _dragging;
        double         _factorPixel;
        double         _offsetPixel;
        double         _factorPosition;
        double         _offsetPosition;
        Gfx::RectF     _handleRect;
        Signal<double> _changed;

        FacetPtr<ScrollBarRenderer>  _renderer;
        bool                         _hasRenderer;

        AutoPtr<Gfx::Brush>          _background;
        AutoPtr<Gfx::Brush>          _foreground;
        AutoPtr<Gfx::Pen>            _contour;
        
        Gfx::Brush                   _backgroundBrush;
        Gfx::Brush                   _foregroundBrush;
        Gfx::Pen                     _contourPen;
};

} // namespace

} // namespace

#endif
