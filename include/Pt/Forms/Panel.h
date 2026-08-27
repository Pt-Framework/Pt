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

#ifndef Pt_Forms_Panel_H
#define Pt_Forms_Panel_H

#include <Pt/Forms/Control.h>
#include <Pt/Forms/Alignment.h>
#include <Pt/Forms/PanelStyle.h>
#include <Pt/Forms/Icon.h>
#include <Pt/Forms/PixmapSurface.h>
#include <Pt/Gfx/Color.h>
#include <Pt/Gfx/Brush.h>
#include <Pt/Gfx/Rect.h>
#include <Pt/SmartPtr.h>

namespace Pt {

namespace Forms {

class Painter;

class PT_FORMS_API Panel : public Control
{
    typedef Control Base;

    public:
        Panel();

        virtual ~Panel();

        void setIcon(const Icon& icon, const Gfx::SizeF& iconSize,
                     Alignment align = Alignment::Center);

        Control* content() const;

        void setContent(Control* control);

    public:
        const Gfx::Brush* background() const;

        void setBackground(const Gfx::Brush& b);

        void setBackground(bool b);

        const Gfx::Pen* contour() const;

        void setContour(const Gfx::Pen& pen);

        void setFrame(bool b);

        void setRenderer(PanelRenderer* renderer);

    protected:
        virtual void onRemoveControl(Control& control);

        virtual void onInvalidate();

        virtual void onRescaleEvent(const RescaleEvent& ev);

        virtual Gfx::SizeF onMeasure(const SizePolicy& policy);

        virtual void onLayout(const Gfx::RectF& rect);

        virtual void onPaint(PaintContext& context, const Gfx::RectF& updateRect);

        /** @brief Paints the panel background layer.

            The default implementation does nothing if the panel has no background.
        */
        virtual void onPaintBackground(PaintContext& context,
                                       const Gfx::RectF& rect,
                                       const PanelState& state);

        /** @brief Paints the panel content layer.

            The default implementation paints the optional panel icon.
        */
        virtual void onPaintContent(PaintContext& context,
                                    const Gfx::RectF& contentRect,
                                    const PanelState& state);

        /** @brief Paints the panel frame layer.

            The default implementation does nothing if the panel has no frame.
        */
        virtual void onPaintFrame(PaintContext& context,
                                  const Gfx::RectF& rect,
                                  const PanelState& state);

    private:
        PanelState panelState() const;

    private:
        Control*          _content;
        PanelStyler       _styler;
        bool              _hasBackground;
        bool              _hasFrame;
        Icon              _icon;
        Gfx::SizeF        _iconSize;
        Gfx::RectF        _contentRect;
        PixmapSurface     _picture;
        Alignment         _imageAlignment;
        bool              _iconInvalid;
};

} // namespace

} // namespace

#endif
