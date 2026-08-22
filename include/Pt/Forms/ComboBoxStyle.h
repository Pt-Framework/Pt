/* Copyright (C) 2017 Marc Boris Duerner

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

#ifndef PT_FORMS_COMBOBOXSTYLE_H
#define PT_FORMS_COMBOBOXSTYLE_H

#include <Pt/Forms/Styler.h>

namespace Pt {

namespace Forms {

class PT_FORMS_API ComboBoxState
{
    public:
        ComboBoxState();

        bool isEnabled() const;

        void setEnabled(bool value);

        bool isFocused() const;

        void setFocused(bool value);

        bool isHighlighted() const;

        void setHighlighted(bool value);

        bool isEditable() const;

        void setEditable(bool value);

        bool isPopupVisible() const;

        void setPopupVisible(bool value);

    private:
        bool _enabled;
        bool _focused;
        bool _highlighted;
        bool _editable;
        bool _popupVisible;
};


class PT_FORMS_API ComboBoxButtonState
{
    public:
        ComboBoxButtonState();

        bool isHighlighted() const;

        void setHighlighted(bool value);

        bool isPressed() const;

        void setPressed(bool value);

    private:
        bool _highlighted;
        bool _pressed;
};


class PT_FORMS_API ComboBoxRenderer : public Style::Facet
{
    public:
        explicit ComboBoxRenderer(std::size_t refs = 0);

        virtual ~ComboBoxRenderer();

        ComboBoxRenderer* create() const;

        void prepare(const StyleOptions& options,
                     const StyleOptions& comboBoxOptions);

    public:
        Gfx::SizeF measureFrame(PaintSurface& surface,
                                const Gfx::SizeF& contentSize);

        Gfx::SizeF measureButton(PaintSurface& surface);

        void layoutChrome(PaintSurface& surface,
                          const Gfx::RectF& rect,
                          Gfx::RectF& entryRect,
                          Gfx::RectF& buttonRect,
                          Gfx::RectF& textRect);

        const Painter& textPainter(PaintSurface& surface);

        void renderChrome(PaintContext& context,
                          const Gfx::RectF& rect,
                          const Gfx::RectF& entryRect,
                          const Gfx::RectF& buttonRect,
                          const ComboBoxState& state,
                          const ComboBoxButtonState& buttonState);

        void renderButton(PaintContext& context,
                          const Gfx::RectF& buttonRect,
                          const ComboBoxState& state,
                          const ComboBoxButtonState& buttonState);

        void renderText(PaintContext& context,
                        const Gfx::RectF& textRect,
                        const String& text,
                        const Gfx::PointF& textPos,
                        const Gfx::RectF& cursor,
                        const ComboBoxState& state);

    protected:
        virtual void onReset(const StyleOptions& options);

        virtual ComboBoxRenderer* onCreate() const = 0;

        virtual void onPrepare(const StyleOptions& options,
                               const StyleOptions& comboBoxOptions) = 0;

        virtual Gfx::SizeF onMeasureFrame(PaintSurface& surface,
                                          const Gfx::SizeF& contentSize) = 0;

        virtual Gfx::SizeF onMeasureButton(PaintSurface& surface) = 0;

        virtual void onLayoutChrome(PaintSurface& surface,
                                    const Gfx::RectF& rect,
                                    Gfx::RectF& entryRect,
                                    Gfx::RectF& buttonRect,
                                    Gfx::RectF& textRect) = 0;

        virtual const Painter& onGetTextPainter(PaintSurface& surface) = 0;

        virtual void onRenderChrome(PaintContext& context,
                                    const Gfx::RectF& rect,
                                    const Gfx::RectF& entryRect,
                                    const Gfx::RectF& buttonRect,
                                    const ComboBoxState& state,
                                    const ComboBoxButtonState& buttonState);

        virtual void onRenderEntry(PaintContext& context,
                                   const Gfx::RectF& entryRect,
                                   const ComboBoxState& state) = 0;

        virtual void onRenderButton(PaintContext& context,
                                    const Gfx::RectF& buttonRect,
                                    const ComboBoxState& state,
                                    const ComboBoxButtonState& buttonState) = 0;

        virtual void onRenderText(PaintContext& context,
                                  const Gfx::RectF& textRect,
                                  const String& text,
                                  const Gfx::PointF& textPos,
                                  const Gfx::RectF& cursor,
                                  const ComboBoxState& state) = 0;
};


class PT_FORMS_API ComboBoxStyle : public Styler<ComboBoxRenderer,
                                                      StyleOptions>
{
    public:
        ComboBoxStyle();
};

} // namespace

} // namespace

#endif
