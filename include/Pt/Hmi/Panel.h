/* Copyright (C) 2015-2017 Marc Boris Duerner
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

#ifndef Pt_Hmi_Panel_H
#define Pt_Hmi_Panel_H

#include <Pt/Hmi/Control.h>
#include <Pt/Hmi/Alignment.h>
#include <Pt/Hmi/Icon.h>
#include <Pt/Hmi/PixmapSurface.h>
#include <Pt/Gfx/Color.h>
#include <Pt/Gfx/Brush.h>
#include <Pt/SmartPtr.h>

namespace Pt {

namespace Hmi {

class PT_HMI_API Panel : public Control
{
    typedef Control Base;

    public:
        Panel();

        virtual ~Panel();

        void setIcon(const Icon& icon, const Gfx::SizeF& iconSize, Alignment align = Alignment::Center);

        Widget* content() const;

        void setContent(Widget* widget);

    public:
        const Gfx::Brush* background() const;

        void setBackground(const Gfx::Brush& b);

        void setBackground(bool b);

        const Gfx::Pen* contour() const;

        void setContour(const Gfx::Pen& pen);

        void setFrame(bool b);

        void setRenderer(PanelRenderer* renderer);

    protected:
        virtual void onRemoveWidget(Widget& w);

        virtual void onInvalidate();

        virtual Gfx::SizeF onMeasure(const SizePolicy& policy);

        virtual void onLayout(const Gfx::RectF& rect);

        virtual void onPaint(Gfx::PaintSurface& surface, const Gfx::RectF& updateRect);

        virtual void onPaintContent(Gfx::PaintSurface& surface, Gfx::Painter& painter);

    private:
        Widget*                  _content;

        FacetPtr<PanelRenderer> _renderer;
        bool                    _hasRenderer;

        AutoPtr<Gfx::Brush>     _background;
        bool                    _hasBackground;
                                
        AutoPtr<Gfx::Pen>       _contour;
        bool                    _hasFrame;

        Icon                    _icon;
        Gfx::SizeF              _iconSize;
        PixmapSurface           _picture;
        Alignment               _imageAlignment;
};

} // namespace

} // namespace

#endif
