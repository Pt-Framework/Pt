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

#ifndef Pt_Hmi_PushButton_H
#define Pt_Hmi_PushButton_H

#include <Pt/Hmi/Button.h>
#include <Pt/Hmi/Picture.h>
#include <Pt/Gfx/Image.h>
#include <Pt/Gfx/Brush.h>
#include <Pt/Gfx/Pen.h>
#include <Pt/SmartPtr.h>

namespace Pt {

namespace Hmi {

class ButtonRenderer;

class PT_HMI_API PushButton : public Button
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

        bool isToggle() const;

        void setToggle(bool toggle);

        void setIcon(const Gfx::Image& image);

        void setIconSize(const Gfx::SizeF& size);

        bool isFlat() const;

        void setFlat(bool f);

        void setLayout(Direction d);

    public:
        const Gfx::Brush& foreground() const;

        void setForeground(const Gfx::Brush& b);

        const Gfx::Pen& contour() const;

        void setContour(const Gfx::Pen& p);

        const Gfx::Color& textColor() const;

        void setTextColor(const Gfx::Color& color);

        const std::string& font() const;

        void setFont(const std::string& fontName);

        std::size_t fontSize() const;

        void setFontSize(const std::size_t n);

        Gfx::Font::Style fontStyle() const;

        void setFontStyle(Gfx::Font::Style style);

        void setRenderer(ButtonRenderer* renderer);

    protected:
        virtual void onPressed();

        virtual void onReleased();

        virtual void onCanceled();

    protected:
        virtual void onSetStyleOptions(const StyleOptions& options);

        virtual Pt::Gfx::SizeF onAutoSize(const SizePolicy& policy) const;

        virtual void onLayout();

        virtual void onInvalidate();

        virtual void onPaint(PaintSurface& surface, const Gfx::RectF& updateRect);

    protected:
        virtual void onEnableEvent(const EnableEvent& ev);

    private:
        void layoutContent();

    private:
        bool                      _isToggle;
        bool                      _isBeingToggled;
        bool                      _isFlat;
        Direction                 _direction;
        Gfx::Image                _icon;
        Gfx::SizeF                _iconSize;
        Gfx::PointF               _iconPos;
        Gfx::PointF               _textPos;
                                  
        FacetPtr<ButtonRenderer>  _renderer;
        bool                      _hasRenderer;

        AutoPtr<Gfx::Brush>       _foreground;
        AutoPtr<Gfx::Pen>         _contour;
        AutoPtr<Gfx::Color>       _textColor;
        AutoPtr<std::string>      _fontName;
        AutoPtr<std::size_t>      _fontSize;
        AutoPtr<Gfx::Font::Style> _fontStyle;

        Gfx::Brush _brush;
        Gfx::Pen   _pen;
        Gfx::Pen   _textPen;
        Gfx::Font  _font;
        Picture    _picture;
};

} // namespace

} // namespace

#endif
