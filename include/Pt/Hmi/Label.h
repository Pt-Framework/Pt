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
#include <Pt/Hmi/Alignment.h>
#include <Pt/Hmi/Adjustment.h>
#include <Pt/Hmi/TextBlock.h>
#include <Pt/Hmi/PixmapSurface.h>
#include <Pt/Hmi/Icon.h>
#include <Pt/SmartPtr.h>
#include <Pt/String.h>

namespace Pt {

namespace Hmi {

class PT_HMI_API Label : public Control
{
    public:
        typedef Control Base;

    public:
        Label();

        virtual ~Label(); 

        Alignment alignment() const;

        void setAlignment(Alignment a);

        const Pt::String& text() const;

        void setText(const Pt::String& text);

        void setIcon(const Icon& icon, const Gfx::SizeF& iconSize);

    public:
        const Gfx::Brush* background() const;

        void setBackground(const Gfx::Brush& b);

        const Gfx::Pen* contour() const;

        void setContour(const Gfx::Pen& p);

        const Gfx::Color& textColor() const;

        void setTextColor(const Gfx::Color& color);

        const std::string& font() const;

        void setFont(const std::string& fontName);

        std::size_t fontSize() const;

        void setFontSize(const std::size_t n);

        const std::string& fontStyle() const;

        void setFontStyle(const std::string& style);

        void setRenderer(LabelRenderer* renderer);

    protected:
        virtual Gfx::SizeF onMeasure(const SizePolicy& policy);

        virtual void onLayout(const Gfx::RectF& rect);

        virtual void onRescaleEvent(const RescaleEvent& ev);

        virtual void onResizeEvent(const ResizeEvent& ev);

    protected:
        virtual void onInvalidate();

        virtual void onPaint(Gfx::PaintSurface& surface, 
                             const Gfx::RectF& rect);

    private:
        Adjustment adjustment() const;

        void layoutText();

        void layoutImage();

    private:
        Alignment   _alignment;

        Pt::String  _text;
        Adjustment  _adjustment;
        TextBlock   _textBlock;

        Icon        _icon;
        Gfx::PointF _iconPos;
        Gfx::SizeF  _iconSize;
        bool        _iconInvalid;

        FacetPtr<LabelRenderer>   _renderer;
        bool                      _hasRenderer;

        AutoPtr<Gfx::Brush>       _background;
        AutoPtr<Gfx::Pen>         _contour;
        AutoPtr<Gfx::Color>       _textColor;
        AutoPtr<std::string>      _fontName;
        AutoPtr<std::size_t>      _fontSize;
        AutoPtr<std::string>      _fontStyle;
        bool                      _styleInvalid;
        
        Gfx::Pen       _textPen;
        Gfx::Pen       _pen;
        Gfx::Font      _font;
        PixmapSurface  _picture;
};

} // namespace

} // namespace

#endif
