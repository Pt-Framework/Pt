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
#include <Pt/Forms/ButtonStyler.h>
#include <Pt/Forms/Icon.h>
#include <Pt/Forms/Pixmap.h>
#include <Pt/Forms/Direction.h>
#include <Pt/Gfx/Image.h>
#include <Pt/Gfx/Brush.h>
#include <Pt/Gfx/Pen.h>
#include <Pt/SmartPtr.h>

namespace Pt {

namespace Forms {

/** @brief Command button with optional toggle, icon, and caption.

    A %PushButton is a %Button that presents a command. A completed click
    emits %clicked() and, unless it is a toggle, returns to the unpressed
    look. %setToggle() keeps the pressed state after release so the button
    can show an on/off command such as Bold.

    %setIcon() places a picture next to the caption. %setLayout() chooses
    the %Direction of icon and text. %setFlat() drops the framed face.

    The button owns a %ButtonStyler. On invalidate it calls %Styler::bind().
    Appearance getters and setters read and write widget-local options.
    %setRenderer() assigns a %ButtonRenderer until it is cleared. Measure,
    layout, and paint call typed methods on the styler. %ButtonState is the
    snapshot passed to paint layers.

    @code
    Pt::Forms::PushButton send;
    send.setText("&Send");
    send.clicked() += Pt::slot(*this, &MailComposer::onSend);

    void MailComposer::onSend()
    {
        mailer.send(message);
    }
    @endcode

    @ingroup Pt-Forms-Buttons
*/
class PT_FORMS_API PushButton : public Button
{
    public:
        typedef Button Base;

    public:
        /** @brief Creates a command button.
        */
        PushButton();

        /** @brief Destroys the button.
        */
        virtual ~PushButton();

        /** @brief Returns true if the button is visually pressed.
        */
        bool isPressed() const;

        /** @brief Sets whether the button is visually pressed.
        */
        void setPressed(bool pressed);

        /** @brief Returns true if the button stays pressed after a click.
        */
        bool isToggle() const;

        /** @brief Sets whether the button stays pressed after a click.
        */
        void setToggle(bool toggle);

        /** @brief Sets the icon shown beside the caption.

            @a iconSize is the logical size requested from @a icon.
        */
        void setIcon(const Icon& icon, const Gfx::SizeF& iconSize);

        /** @brief Returns true if the button uses a flat face.
        */
        bool isFlat() const;

        /** @brief Sets whether the button uses a flat face.
        */
        void setFlat(bool f);

        /** @brief Sets the direction of icon and caption to @a d.
        */
        void setLayout(Direction d);

    public:
        /** @brief Returns the effective foreground brush.
        */
        const Gfx::Brush& foreground() const;

        /** @brief Sets the widget-local foreground brush to @a b.
        */
        void setForeground(const Gfx::Brush& b);

        /** @brief Returns the effective contour pen.
        */
        const Gfx::Pen& contour() const;

        /** @brief Sets the widget-local contour pen to @a p.
        */
        void setContour(const Gfx::Pen& p);

        /** @brief Returns the effective accent color.
        */
        const Gfx::Color& accentColor() const;

        /** @brief Sets the widget-local accent color to @a color.
        */
        void setAccentColor(const Gfx::Color& color);

        /** @brief Returns the effective highlight color.
        */
        const Gfx::Color& highlightColor() const;

        /** @brief Sets the widget-local highlight color to @a c.
        */
        void setHighlightColor(const Gfx::Color& c);

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
        void setRenderer(ButtonRenderer* renderer);

    protected:
        /** @brief Presses the button, or toggles it when %isToggle() is true.
        */
        virtual void onPressed();

        /** @brief Completes the click and emits %clicked().
        */
        virtual void onReleased();

        /** @brief Restores the pressed state after an abandoned click.
        */
        virtual void onCanceled();

    protected:
        /** @brief Invalidates a prepared icon after a scale change.
        */
        virtual void onRescaleEvent(const RescaleEvent& ev);

    protected:
        /** @brief Binds the styler and prepares the icon.

            Calls the base implementation, then %Styler::bind(). When
            bind returns true, refreshes widget-owned caches such as
            icon pixmaps.
        */
        virtual void onInvalidate();

        /** @brief Measures icon, caption, and frame.
        */
        virtual Gfx::SizeF onMeasure(const SizePolicy& policy);

        /** @brief Places icon, caption, and mnemonic in @a rect.
        */
        virtual void onLayout(const Gfx::RectF& rect);

        /** @brief Paints background, frame, icon, caption, and mnemonic.
        */
        virtual void onPaint(PaintContext& context, const Gfx::RectF& updateRect);

        /** @brief Paints the button background layer.

            The default implementation delegates to the current %ButtonRenderer.
        */
        virtual void onPaintBackground(PaintContext& context,
                                       const Gfx::RectF& rect,
                                       const ButtonState& state);

        /** @brief Paints the button frame layer.

            The default implementation does nothing for flat buttons.
        */
        virtual void onPaintFrame(PaintContext& context,
                                  const Gfx::RectF& rect,
                                  const ButtonState& state);

        /** @brief Paints the button icon layer.

            The default implementation does nothing if no prepared icon pixmap exists.
        */
        virtual void onPaintIcon(PaintContext& context,
                                 const Gfx::RectF& rect,
                                 const Pixmap& picture,
                                 const Gfx::PointF& pos,
                                 const ButtonState& state);

        /** @brief Paints the button text layer.

            The default implementation does nothing if the button text is empty.
        */
        virtual void onPaintText(PaintContext& context,
                                 const Gfx::RectF& rect,
                                 const String& text,
                                 const Gfx::PointF& pos,
                                 const ButtonState& state);

        /** @brief Paints the button mnemonic layer.

            The default implementation does nothing if the button text is empty.
        */
        virtual void onPaintMnemonic(PaintContext& context,
                                    const Gfx::RectF& rect,
                                    const Gfx::RectF& mnemonic,
                                    const ButtonState& state);

    private:
        const ButtonState& buttonState();

    private:
        bool                      _isToggle;
        bool                      _isBeingToggled;
        Direction                 _direction;
        Icon                      _icon;
        bool                      _iconInvalid;
        Pixmap                    _picture;

        ButtonState               _buttonState;
        ButtonStyler              _styler;

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
};

} // namespace

} // namespace

#endif
