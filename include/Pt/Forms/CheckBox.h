
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

#ifndef Pt_Forms_CheckBox_h
#define Pt_Forms_CheckBox_h

#include <Pt/Forms/Button.h>
#include <Pt/Forms/CheckBoxStyler.h>
#include <Pt/Gfx/FontMetrics.h>
#include <Pt/Gfx/TextMetrics.h>

namespace Pt {

namespace Forms {

/** @brief Button that presents a boolean choice.

    A %CheckBox is a %Button whose value is %Unspecified, %Checked, or
    %Unchecked. %setState() assigns the value. %isChecked() is true only
    for %Checked. A completed click toggles %Checked and %Unchecked, then
    emits %clicked(). %Unspecified becomes %Checked on that click.

    The box owns a %CheckBoxStyler. On invalidate it calls %Styler::bind().
    Appearance getters and setters overlay the application style.
    %setRenderer() assigns a %CheckBoxRenderer. %CheckBoxState is the
    snapshot passed to paint layers.

    @code
    Pt::Forms::CheckBox remember;
    remember.setText("Remember me");
    remember.clicked() += Pt::slot(*this, &LoginDialog::onRememberToggled);

    void LoginDialog::onRememberToggled()
    {
        if( remember.isChecked() )
            storeCredentials();
    }
    @endcode

    @ingroup Pt-Forms-Buttons
*/
class PT_FORMS_API CheckBox : public Button
{
    public:
        typedef Button Base;

        /** @brief Value of the choice.
        */
        enum State
        {
            /** @brief Neither on nor off.
            */
            Unspecified = 0,

            /** @brief The choice is on.
            */
            Checked = 1,

            /** @brief The choice is off.
            */
            Unchecked = 2
            // Partial
        };

    public:
        /** @brief Creates an unchecked check box.
        */
        CheckBox();

        /** @brief Destroys the check box.
        */
        virtual ~CheckBox();

        /** @brief Returns the current choice.
        */
        State state() const;

        /** @brief Sets the choice to @a s.
        */
        void setState(State s);

        /** @brief Returns true if the choice is %Checked.
        */
        bool isChecked() const;

    public:
        /** @brief Returns the effective background brush.
        */
        const Gfx::Brush& background() const;

        /** @brief Sets the widget-local background brush to @a b.
        */
        void setBackground(const Gfx::Brush& b);

        /** @brief Returns the effective contour pen.
        */
        const Gfx::Pen& contour() const;

        /** @brief Sets the widget-local contour pen to @a p.
        */
        void setContour(const Gfx::Pen& p);

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

            A null renderer falls back to the current style on the next bind.
        */
        void setRenderer(CheckBoxRenderer* renderer);

    protected:
        /** @brief Measures the checkable area, caption, and frame.
        */
        virtual Gfx::SizeF onMeasure(const SizePolicy& policy);

        /** @brief Places the checkable area, caption, and mnemonic in @a rect.
        */
        virtual void onLayout(const Gfx::RectF& rect);

        /** @brief Binds the styler and requests relayout.
        */
        virtual void onInvalidate();

        /** @brief Forwards press to %Button.
        */
        virtual void onPressed();

        /** @brief Toggles %Checked and %Unchecked, then emits %clicked().
        */
        virtual void onReleased();

        /** @brief Forwards cancel to %Button.
        */
        virtual void onCanceled();

    protected:
        /** @brief Paints chrome, caption, and mnemonic.
        */
        virtual void onPaint(PaintContext& context, const Gfx::RectF& updateRect);

        /** @brief Paints the checkable area for @a state.
        */
        virtual void onPaintChrome(PaintContext& context,
                                   const Gfx::RectF& rect,
                                   const Gfx::RectF& boxRect,
                                   const CheckBoxState& state);

        /** @brief Paints the caption for @a state.
        */
        virtual void onPaintText(PaintContext& context,
                                 const Gfx::RectF& textRect,
                                 const String& text,
                                 const Gfx::PointF& pos,
                                 const CheckBoxState& state);

        /** @brief Paints the mnemonic underline for @a state.
        */
        virtual void onPaintMnemonic(PaintContext& context,
                                    const Gfx::RectF& rect,
                                    const Gfx::RectF& mnemonic,
                                    const CheckBoxState& state);

    private:
        CheckBoxState checkBoxState() const;

    private:
        State                      _state;
        CheckBoxStyler             _styler;

        Gfx::RectF                 _boxRect;
        Gfx::RectF                 _textRect;
        Gfx::PointF                _textPos;
        Gfx::RectF                 _mnemonicRect;
};

} // namespace

} // namespace

#endif
