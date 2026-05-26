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
#include <Pt/Forms/Direction.h>
#include <Pt/Forms/StyleFlags.h>
#include <Pt/Forms/Style.h>
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

        const Gfx::Color& accentColor() const;

        void setAccentColor(const Gfx::Color& color);

        const Gfx::Color& highlightColor() const;

        void setHighlightColor(const Gfx::Color& c);

        const Gfx::Color& textColor() const;

        void setTextColor(const Gfx::Color& color);

        const Gfx::Font& font() const;

        void setFont(const Gfx::Font& font);

        void setFontSize(std::size_t size);

        void setFontWeight(Gfx::Font::Weight weight);

        void setFontSlant(Gfx::Font::Slant slant);

        void setRenderer(ButtonRenderer* renderer);

        ButtonStyleFlags buttonStyleFlags() const;

    protected:
        virtual void onPressed();

        virtual void onReleased();

        virtual void onCanceled();

    protected:
        virtual Gfx::SizeF onMeasure(const SizePolicy& policy);

        virtual void onLayout(const Gfx::RectF& rect);

        virtual void onInvalidate();

        virtual void onRescaleEvent(const RescaleEvent& ev);

        virtual void onPaint(PaintContext& context, const Gfx::RectF& updateRect);

        /** @brief Paints the button background layer.

            The default implementation delegates to the current %ButtonRenderer.
        */
        virtual void onPaintBackground(PaintContext& context);

        /** @brief Paints the button frame layer.

            The default implementation does nothing for flat buttons.
        */
        virtual void onPaintFrame(PaintContext& context);

        /** @brief Paints the button icon layer.

            The default implementation does nothing if no prepared icon pixmap exists.
        */
        virtual void onPaintIcon(PaintContext& context);

        /** @brief Paints the button text layer.

            The default implementation does nothing if the button text is empty.
        */
        virtual void onPaintText(PaintContext& context);

        /** @brief Paints the button mnemonic layer.

            The default implementation does nothing if the button text is empty.
        */
        virtual void onPaintMnemonic(PaintContext& context);

    private:
        ButtonRenderer* getRenderer();

        void applyRenderer(ButtonRenderer* renderer);

        Gfx::Font getFont() const;

    private:
        enum OverrideFlags : unsigned
        {
            OverrideForeground    = 0x01,
            OverrideContour       = 0x02,
            OverrideAccentColor   = 0x04,
            OverrideHighlightColor= 0x08,
            OverrideTextColor     = 0x10,
            OverrideFontAll       = 0x20,
            OverrideFontSize      = 0x40,
            OverrideFontWeight    = 0x80,
            OverrideFontSlant     = 0x100,
            OverrideFontAny       = OverrideFontAll | OverrideFontSize
                                  | OverrideFontWeight | OverrideFontSlant
        };

    private:
        bool                      _isToggle;
        bool                      _isPressed;
        bool                      _isBeingToggled;
        bool                      _isFlat;
        Direction                 _direction;
        Icon                      _icon;
        Gfx::SizeF                _iconSize;
        Gfx::PointF               _textPos;
        Gfx::PointF               _iconPos;
        Gfx::RectF                _contentRect;
        Gfx::RectF                _iconRect;
        Gfx::RectF                _textRect;
        Gfx::RectF                _mnemonicRect;
        Gfx::TextMetrics          _textMetrics;
        Gfx::FontMetrics          _fontMetrics;
        Gfx::SizeF                _measuredIconSize;
                                  
        FacetPtr<ButtonRenderer>  _renderer;
        bool                      _customRenderer;
        std::size_t               _styleGeneration;
        bool                      _iconInvalid;

        AutoPtr<Gfx::Brush>       _foreground;
        AutoPtr<Gfx::Pen>         _contour;
        AutoPtr<Gfx::Color>       _accentColor;
        AutoPtr<Gfx::Color>       _highlightColor;
        AutoPtr<Gfx::Color>       _textColor;
        Gfx::Font                 _customFont;
        unsigned                  _overrideFlags;

        PixmapSurface    _picture;
};

} // namespace

} // namespace

#endif
