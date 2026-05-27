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

#ifndef Pt_Forms_Label_H
#define Pt_Forms_Label_H

#include <Pt/Forms/Control.h>
#include <Pt/Forms/Alignment.h>
#include <Pt/Forms/Adjustment.h>
#include <Pt/Forms/Style.h>
#include <Pt/Forms/TextBlock.h>
#include <Pt/Forms/PixmapSurface.h>
#include <Pt/Forms/Icon.h>
#include <Pt/SmartPtr.h>
#include <Pt/String.h>

namespace Pt {

namespace Forms {

class PT_FORMS_API Label : public Control
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

        void setBackground(bool b);

        const Gfx::Pen* contour() const;

        void setContour(const Gfx::Pen& p);

        void setFrame(bool b);

        const Gfx::Color& textColor() const;

        void setTextColor(const Gfx::Color& color);

        const Gfx::Font& font() const;

        void setFont(const Gfx::Font& font);

        void setFontSize(std::size_t size);

        void setFontWeight(Gfx::Font::Weight weight);

        void setFontSlant(Gfx::Font::Slant slant);

        void setRenderer(PanelRenderer* renderer);

    protected:
        virtual Gfx::SizeF onMeasure(const SizePolicy& policy);

        virtual void onLayout(const Gfx::RectF& rect);

        virtual void onRescaleEvent(const RescaleEvent& ev);

        virtual void onResizeEvent(const ResizeEvent& ev);

    protected:
        virtual void onConnect(Screen& screen);

        virtual void onInvalidate();

        virtual void onPaint(PaintContext& context, 
                             const Gfx::RectF& rect);

    private:
        PanelRenderer* getRenderer();

        void applyRenderer(PanelRenderer* renderer);

        Adjustment adjustment() const;

        Gfx::Font getFont() const;

    private:
        enum OverrideFlags : unsigned
        {
            OverrideBackground = 0x01,
            OverrideContour    = 0x02,
            OverrideTextColor  = 0x04,
            OverrideFontAll    = 0x08,
            OverrideFontSize   = 0x10,
            OverrideFontWeight = 0x20,
            OverrideFontSlant  = 0x40,
            OverrideFontAny    = OverrideFontAll | OverrideFontSize
                               | OverrideFontWeight | OverrideFontSlant
        };

    private:
        Alignment   _alignment;

        Pt::String  _text;
        Adjustment  _adjustment;
        TextBlock   _textBlock;

        Icon        _icon;
        Gfx::PointF _iconPos;
        Gfx::SizeF  _iconSize;
        Gfx::SizeF  _measuredIconSize;
        bool        _iconInvalid;

        FacetPtr<PanelRenderer>   _renderer;
        bool                      _customRenderer;

        AutoPtr<Gfx::Brush>       _background;
        bool                      _hasBackground;
        AutoPtr<Gfx::Pen>         _contour;
        bool                      _hasFrame;
        AutoPtr<Gfx::Color>       _textColor;
        Gfx::Font                 _customFont;
        unsigned                  _overrides;
        std::size_t               _styleGeneration;
        bool                      _styleInvalid;

        Pixmap         _pixmap;
};

} // namespace

} // namespace

#endif
