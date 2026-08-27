
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
#include <Pt/Forms/CheckBoxStyle.h>
#include <Pt/Gfx/FontMetrics.h>
#include <Pt/Gfx/TextMetrics.h>

namespace Pt {

namespace Forms {

class PT_FORMS_API CheckBox : public Button
{
    public:
        typedef Button Base;

        enum State
        {
            Unspecified = 0,
            Checked = 1,
            Unchecked = 2
            // Partial
        };

    public:
        CheckBox();

        virtual ~CheckBox();

        State state() const;

        void setState(State s);

        bool isChecked() const;

    public:
        const Gfx::Brush& background() const;

        void setBackground(const Gfx::Brush& b);

        const Gfx::Pen& contour() const;

        void setContour(const Gfx::Pen& p);

        const Gfx::Color& textColor() const;

        void setTextColor(const Gfx::Color& color);

        Gfx::Font font() const;

        void setFont(const Gfx::Font& font);

        void setFontSize(std::size_t size);

        void setFontWeight(Gfx::Font::Weight weight);

        void setFontSlant(Gfx::Font::Slant slant);

        void setRenderer(CheckBoxRenderer* renderer);

    protected:
        virtual Gfx::SizeF onMeasure(const SizePolicy& policy);

        virtual void onLayout(const Gfx::RectF& rect);

        virtual void onInvalidate();

        virtual void onPressed();

        virtual void onReleased();

        virtual void onCanceled();

    protected:
        virtual void onPaint(PaintContext& context, const Gfx::RectF& updateRect);

        virtual void onPaintChrome(PaintContext& context,
                                   const Gfx::RectF& rect,
                                   const Gfx::RectF& boxRect,
                                   const CheckBoxState& state);

        virtual void onPaintText(PaintContext& context,
                                 const Gfx::RectF& textRect,
                                 const String& text,
                                 const Gfx::PointF& pos,
                                 const CheckBoxState& state);

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
