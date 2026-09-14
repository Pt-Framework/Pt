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
#include <Pt/Forms/PanelStyler.h>
#include <Pt/Forms/Icon.h>
#include <Pt/Forms/PixmapSurface.h>
#include <Pt/Gfx/Color.h>
#include <Pt/Gfx/Brush.h>
#include <Pt/Gfx/Rect.h>
#include <Pt/SmartPtr.h>

namespace Pt {

namespace Forms {

class Painter;

/** @brief Surface that groups one child behind an optional fill and frame.

    A %Panel presents one content control with an optional background,
    frame, and icon. %setContent() attaches the child without taking
    ownership. Keep the child alive until it is detached. %content()
    returns the attached control, or 0.

    %setBackground() and %setContour() enable a fill or frame.
    %setBackground(false) and %setFrame(false) turn them off.
    Whether the fill or frame is on is a widget setting, not a style
    option. %setIcon() paints a picture aligned with the given
    %Alignment.

    The panel owns a %PanelStyler and shares that appearance family with
    %Label. On invalidate it binds the styler. Measure, layout, and paint
    call typed methods on it. The content control is laid out in the inner
    rectangle after the frame.

    @code
    Pt::Forms::Label status;
    status.setText("Ready");

    Pt::Forms::Panel banner;
    banner.setBackground(Pt::Gfx::Brush(Pt::Gfx::Color(240, 240, 240)));
    banner.setContent(&status);
    window.setContent(&banner);
    @endcode

    @ingroup Pt-Forms-Displays
*/
class PT_FORMS_API Panel : public Control
{
    typedef Control Base;

    public:
        /** @brief Creates an empty panel.
        */
        Panel();

        /** @brief Destroys the panel. The content control is not destroyed.
        */
        virtual ~Panel();

        /** @brief Sets the icon painted in the panel.

            @a iconSize is the logical size requested from @a icon.
            @a align places the icon in the inner rectangle.
        */
        void setIcon(const Icon& icon, const Gfx::SizeF& iconSize,
                     Alignment align = Alignment::Center);

        /** @brief Returns the attached content control, or 0.
        */
        Control* content() const;

        /** @brief Attaches @a control as the content.

            The panel does not take ownership. Passing 0 detaches the
            current content without destroying it.
        */
        void setContent(Control* control);

    public:
        /** @brief Returns the background brush, or 0 if the fill is off.
        */
        const Gfx::Brush* background() const;

        /** @brief Sets the widget-local background brush to @a b and enables the fill.
        */
        void setBackground(const Gfx::Brush& b);

        /** @brief Sets whether the panel paints a background fill.
        */
        void setBackground(bool b);

        /** @brief Returns the contour pen, or 0 if the frame is off.
        */
        const Gfx::Pen* contour() const;

        /** @brief Sets the widget-local contour pen to @a pen and enables the frame.
        */
        void setContour(const Gfx::Pen& pen);

        /** @brief Sets whether the panel paints a frame.
        */
        void setFrame(bool b);

        /** @brief Assigns @a renderer as the family renderer.

            Passing 0 uses the renderer from the current style. The
            change takes effect before this function returns.
        */
        void setRenderer(PanelRenderer* renderer);

    protected:
        /** @brief Clears the content pointer when @a control is detached.
        */
        virtual void onRemoveControl(Control& control);

        /** @brief Binds the styler and prepares the icon.

            Calls the base implementation, then %Styler::bind(). When
            bind returns true, refreshes widget-owned caches such as
            icon pixmaps.
        */
        virtual void onInvalidate();

        /** @brief Invalidates a prepared icon after a scale change.
        */
        virtual void onRescaleEvent(const RescaleEvent& ev);

        /** @brief Measures the content or icon and the frame.
        */
        virtual Gfx::SizeF onMeasure(const SizePolicy& policy);

        /** @brief Places the content control in the inner rectangle.
        */
        virtual void onLayout(const Gfx::RectF& rect);

        /** @brief Paints background, content, and frame.
        */
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
