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

#ifndef Pt_Hmi_ProgressBar_H
#define Pt_Hmi_ProgressBar_H

#include <Pt/Hmi/Control.h>
#include <Pt/SmartPtr.h>
#include <Pt/Signal.h>

namespace Pt {

namespace Hmi {

class PT_HMI_API ProgressBar : public Control
{
    public:
        typedef Control Base;

    public:
        ProgressBar();

        virtual ~ProgressBar();

        int maximum() const;

        int minimum() const;

        void setRange(int minpos, int maxpos);

        int value() const;

        void setValue(int n);

        float progress() const;

        void reset();

        Signal<int>& valueChanged();

    public:
        const Gfx::Brush& background() const;

        void setBackground(const Gfx::Brush& b);

        const Gfx::Color& foreground() const;

        void setForeground(const Gfx::Color& b);

        const Gfx::Pen& contour() const;

        void setContour(const Gfx::Pen& p);

        const Gfx::Color& textColor() const;

        void setTextColor(const Gfx::Color& color);

        const std::string& font() const;

        void setFont(const std::string& fontName);

        std::size_t fontSize() const;

        void setFontSize(const std::size_t n);

        const std::string& fontStyle() const;

        void setFontStyle(const std::string& style);

        void setRenderer(ProgressBarRenderer* renderer);

    protected:
        virtual Gfx::SizeF onMeasure(const SizePolicy& policy);

        virtual void onInvalidate();

        virtual void onPaint(Gfx::PaintSurface& surface, const Gfx::RectF& updateRect);

    private:
        Signal<int> _valueChanged;
        int _value;
        int _min;
        int _max;

        AutoPtr<Gfx::Brush>       _background;
        AutoPtr<Gfx::Color>       _foreground;
        AutoPtr<Gfx::Pen>         _contour;
        AutoPtr<Gfx::Color>       _textColor;
        AutoPtr<std::string>      _fontName;
        AutoPtr<std::size_t>      _fontSize;
        AutoPtr<std::string> _fontStyle;
        
        FacetPtr<ProgressBarRenderer> _renderer;
        bool                          _hasRenderer;

        Gfx::Brush  _backgroundBrush;
        Gfx::Brush  _foregroundBrush;
        Gfx::Pen    _contourPen;
        Gfx::Pen    _textPen;
        Gfx::Font   _font;
};

} // namespace

} // namespace

#endif
