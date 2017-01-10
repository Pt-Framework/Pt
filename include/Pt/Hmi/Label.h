/* Copyright (C) 2015 Marc Boris Duerner 
   Copyright (C) 2015 Laurentiu-Gheorghe Crisan
  
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

#ifndef Pt_Hmi_Label_H
#define Pt_Hmi_Label_H

#include <Pt/Hmi/Control.h>
#include <Pt/String.h>

namespace Pt {

namespace Hmi {

class PT_HMI_API Label : public Control
{
    public:
        typedef Control Base;

	  public:
        // TODO: separate class
        enum Alignment
        {
            TopLeft,
            TopCenter,
            TopRight,
            MiddleLeft,
            MiddleCenter,
            MiddleRight,
            BottomLeft,
            BottomCenter,
            BottomRight
        };

    public:
        Label();

        virtual ~Label(); 

        const Pt::String& text() const;

        void setText(const Pt::String& text);

        Alignment textAlignment() const;

        void setTextAlignment(Alignment a);

        const Gfx::Brush* background() const;

        void setBackground(const Gfx::Brush& b);

        const Gfx::Pen* frame() const;

        void setFrame(const Gfx::Pen& p);

        const Gfx::Color& textColor() const;

        void setTextColor(const Gfx::Color& color);

        const std::string& font() const;

        void setFont(const std::string& fontName);

        std::size_t fontSize() const;

        void setFontSize(const std::size_t n);

        Gfx::Font::Style fontStyle() const;

        void setFontStyle(Gfx::Font::Style style);

    protected:
        virtual Gfx::SizeF onAutoSize() const;

        virtual void onInvalidate();

        virtual void onPaint(PaintSurface& surface, const Gfx::RectF& updateRect);
        
    private:
        Gfx::PointF textPosition() const;

    private:
        Pt::String  _text;
        Alignment   _textAlignment;
        
        Option<Gfx::Brush>       _background;
        Option<Gfx::Pen>         _frame;
        Option<Gfx::Color>       _textColor;
        Option<std::string>      _fontName;
        Option<std::size_t>      _fontSize;
        Option<Gfx::Font::Style> _fontStyle;
        
        Gfx::Pen    _textPen;
        Gfx::Font   _font;
        
};

} // namespace

} // namespace

#endif
