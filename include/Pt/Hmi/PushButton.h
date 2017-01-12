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

namespace Pt {

namespace Hmi {

class ButtonRenderer;

class PT_HMI_API PushButton : public Button
{
    public:
        typedef Button Base;

        // TODO: use Direction to layout image and text

    public:
        PushButton();

        virtual ~PushButton();

        bool isToggle() const;

        void setToggle(bool toggle);

        void setImage(const Gfx::Image& image);

        bool isFlat() const;

        void setFlat(bool f);

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

    protected:
        virtual void onPressed();

        virtual void onReleased();

    protected:
        virtual void onInvalidate();

        virtual void onPaint(PaintSurface& surface, const Gfx::RectF& updateRect);

    protected:
        virtual void onEnableEvent(const EnableEvent& ev);

    private:
        bool       _isToggle;
        bool       _isFlat;
        Gfx::Image _image;

        FacetPtr<ButtonRenderer> _renderer;

        Option<Gfx::Brush>       _foreground;
        Option<Gfx::Pen>         _contour;
        Option<Gfx::Color>       _textColor;
        Option<std::string>      _fontName;
        Option<std::size_t>      _fontSize;
        Option<Gfx::Font::Style> _fontStyle;

        Gfx::Brush _brush;
        Gfx::Pen   _pen;
        Gfx::Pen   _textPen;
        Gfx::Font  _font;
        Picture    _picture;
};

} // namespace

} // namespace

#endif
