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
#include <Pt/Forms/PanelStyler.h>
#include <Pt/Forms/TextBlock.h>
#include <Pt/Forms/Pixmap.h>
#include <Pt/Forms/Icon.h>
#include <Pt/Gfx/Rect.h>
#include <Pt/SmartPtr.h>
#include <Pt/String.h>

namespace Pt {

namespace Forms {

/** @brief Control that displays text or an icon.

    A %Label presents a caption or a picture. It does not edit text.
    %setText() sets the caption and clears any icon. %setIcon() shows
    an icon instead of text. %setAlignment() places the content in the
    label bounds.

    %setBackground() and %setContour() enable a fill or frame.
    %setBackground(false) and %setFrame(false) turn them off without
    replacing the style. Whether the fill or frame is on is a widget
    setting, not a style option.

    The label owns a %PanelStyler and shares the panel appearance family
    with %Panel. On invalidate it binds that styler. Measure, layout, and
    paint call typed methods on it. %TextBlock lays out wrapped text.
    %Icon supplies pictures. Neither is a widget.

    @code
    Pt::Forms::Label heading;
    heading.setText("Inbox");
    heading.setAlignment(Pt::Forms::Alignment::Left);
    heading.setFontWeight(Pt::Gfx::Font::Weight::Bold);

    Pt::Forms::Label empty;
    empty.setText("No messages");
    empty.setAlignment(Pt::Forms::Alignment::Center);
    empty.setTextColor(Pt::Gfx::Color(120, 120, 120));
    @endcode

    @ingroup Pt-Forms-Displays
*/
class PT_FORMS_API Label : public Control
{
    public:
        typedef Control Base;

    public:
        /** @brief Creates an empty label.
        */
        Label();

        /** @brief Destroys the label.
        */
        virtual ~Label();

        /** @brief Returns the alignment of the caption or icon.
        */
        Alignment alignment() const;

        /** @brief Sets the alignment of the caption or icon to @a a.
        */
        void setAlignment(Alignment a);

        /** @brief Returns the caption.
        */
        const Pt::String& text() const;

        /** @brief Sets the caption to @a text and clears any icon.
        */
        void setText(const Pt::String& text);

        /** @brief Shows @a icon instead of the caption.

            @a iconSize is the logical size requested from @a icon.
        */
        void setIcon(const Icon& icon, const Gfx::SizeF& iconSize);

    public:
        /** @brief Returns the background brush, or 0 if the fill is off.
        */
        const Gfx::Brush* background() const;

        /** @brief Sets the widget-local background brush to @a b and enables the fill.
        */
        void setBackground(const Gfx::Brush& b);

        /** @brief Sets whether the label paints a background fill.
        */
        void setBackground(bool b);

        /** @brief Returns the contour pen, or 0 if the frame is off.
        */
        const Gfx::Pen* contour() const;

        /** @brief Sets the widget-local contour pen to @a p and enables the frame.
        */
        void setContour(const Gfx::Pen& p);

        /** @brief Sets whether the label paints a frame.
        */
        void setFrame(bool b);

        /** @brief Returns the effective text color.
        */
        const Gfx::Color& textColor() const;

        /** @brief Sets the widget-local text color to @a color.
        */
        void setTextColor(const Gfx::Color& color);

        /** @brief Returns the effective font.
        */
        Gfx::Font font() const;

        /** @brief Sets the widget-local font to @a font.
        */
        void setFont(const Gfx::Font& font);

        /** @brief Sets the widget-local font size to @a size.
        */
        void setFontSize(std::size_t size);

        /** @brief Sets the widget-local font weight to @a weight.
        */
        void setFontWeight(Gfx::Font::Weight weight);

        /** @brief Sets the widget-local font slant to @a slant.
        */
        void setFontSlant(Gfx::Font::Slant slant);

        /** @brief Assigns @a renderer as the family renderer.

            Passing 0 uses the renderer from the current style. The
            change takes effect before this function returns.
        */
        void setRenderer(PanelRenderer* renderer);

    protected:
        /** @brief Measures the caption or icon and the frame.
        */
        virtual Gfx::SizeF onMeasure(const SizePolicy& policy);

        /** @brief Places the caption or icon in @a rect.
        */
        virtual void onLayout(const Gfx::RectF& rect);

        /** @brief Invalidates a prepared icon after a scale change.
        */
        virtual void onRescaleEvent(const RescaleEvent& ev);

        /** @brief Forwards the resize to the base control.
        */
        virtual void onResizeEvent(const ResizeEvent& ev);

    protected:
        /** @brief Forwards the screen connection to the base control.
        */
        virtual void onConnect(Screen& screen);

        /** @brief Binds the styler and prepares the icon.

            Calls the base implementation, then %Styler::bind(). When
            bind returns true, refreshes widget-owned caches such as
            icon pixmaps.
        */
        virtual void onInvalidate();

        /** @brief Paints background, frame, icon, and caption.
        */
        virtual void onPaint(PaintContext& context,
                             const Gfx::RectF& rect);

        /** @brief Paints the label background layer.

            The default implementation does nothing if the label has no background.
        */
        virtual void onPaintBackground(PaintContext& context,
                                       const Gfx::RectF& rect,
                                       const PanelState& state);

        /** @brief Paints the label frame layer.

            The default implementation does nothing if the label has no frame.
        */
        virtual void onPaintFrame(PaintContext& context,
                                  const Gfx::RectF& rect,
                                  const PanelState& state);

        /** @brief Paints the label icon layer.

            The default implementation does nothing if no prepared icon pixmap exists.
        */
        virtual void onPaintIcon(PaintContext& context,
                                 const Gfx::RectF& contentRect,
                                 const PanelState& state);

        /** @brief Paints the label text layer.

            The default implementation does nothing while the label is in icon mode.
        */
        virtual void onPaintText(PaintContext& context,
                                 const Gfx::RectF& contentRect,
                                 const PanelState& state);

    private:
        Adjustment adjustment() const;

        PanelState panelState() const;

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

        PanelStyler       _styler;
        bool              _hasBackground;
        bool              _hasFrame;
        Gfx::RectF        _contentRect;
        Pixmap            _pixmap;
};

} // namespace

} // namespace

#endif
