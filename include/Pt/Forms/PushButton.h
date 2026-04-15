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
   MA  02110-1301  USA
*/

#ifndef Pt_Forms_PushButton_H
#define Pt_Forms_PushButton_H

#include <Pt/Forms/Button.h>
#include <Pt/Forms/Icon.h>
#include <Pt/Forms/PixmapSurface.h>
#include <Pt/Gfx/Image.h>
#include <Pt/Gfx/Brush.h>
#include <Pt/Gfx/Pen.h>
#include <Pt/SmartPtr.h>

namespace Pt {

namespace Forms {

class ButtonRenderer;

class PT_FORMS_API PushButton : public Button
{
    public:
        typedef Button Base;

        enum Direction
        {
            Left,
            Right,
            Top,
            Bottom
        };

    public:
        PushButton();

        virtual ~PushButton();

        bool isPressed() const;

        void setPressed(bool pressed);

        bool isToggle() const;

        void setToggle(bool toggle);

        void setIcon(const Icon& icon, const Gfx::SizeF& iconSize);

        bool isFlat() const;

        void setFlat(bool f);

        void setLayout(Direction d);

    public:
        const Gfx::Brush& foreground() const;

        void setForeground(const Gfx::Brush& b);

        const Gfx::Pen& contour() const;

        void setContour(const Gfx::Pen& p);

        const Gfx::ColorF& accentColor() const;
        
        void setAccentColor(const Gfx::ColorF& color);

        const Gfx::ColorF& highlightColor() const;

        void setHighlightColor(const Gfx::ColorF& c);

        const Gfx::ColorF& textColor() const;

        void setTextColor(const Gfx::ColorF& color);

        const Gfx::Font& font() const;

        void setFont(const Gfx::Font& font);

        void setFontSize(std::size_t size);

        void setFontWeight(Gfx::Font::Weight weight);

        void setFontSlant(Gfx::Font::Slant slant);

        void setRenderer(ButtonRenderer* renderer);

    protected:
        virtual void onPressed();

        virtual void onReleased();

        virtual void onCanceled();

    protected:
        virtual void onSetStyleOptions(const StyleOptions& options);

        virtual Gfx::SizeF onMeasure(const SizePolicy& policy);

        virtual void onLayout(const Gfx::RectF& rect);

        virtual void onInvalidate();

        virtual void onPaint(PaintSurface& surface, const Gfx::RectF& updateRect);

    private:
        void onIconChanged();

        Gfx::Font getFont() const;

    private:
        enum FontOverride : unsigned
        {
            OverrideSize   = 0x01,
            OverrideWeight = 0x02,
            OverrideSlant  = 0x04,
            OverrideAll    = 0xFF
        };

    private:
        bool                      _isToggle;
        bool                      _isPressed;
        bool                      _isBeingToggled;
        bool                      _isFlat;
        Direction                 _direction;
        Icon                      _icon;
        Gfx::SizeF                _iconSize;
        Gfx::PointF               _iconPos;
        Gfx::PointF               _textPos;
        Gfx::TextMetrics          _textMetrics;
        Gfx::FontMetrics          _fontMetrics;
                                  
        FacetPtr<ButtonRenderer>  _renderer;
        bool                      _hasRenderer;

        AutoPtr<Gfx::Brush>       _foreground;
        AutoPtr<Gfx::Pen>         _contour;
        AutoPtr<Gfx::ColorF>      _accentColor;
        AutoPtr<Gfx::ColorF>      _highlightColor;
        AutoPtr<Gfx::ColorF>      _textColor;
        Gfx::Font                 _customFont;
        unsigned                  _fontOverride;
        std::size_t               _styleGeneration;

        Gfx::Brush _brush;
        Gfx::Pen   _pen;
        Gfx::Pen   _textPen;
        Gfx::Font  _font;
        PixmapSurface    _picture;
};

} // namespace

} // namespace

#endif
