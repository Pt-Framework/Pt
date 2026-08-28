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
#include <Pt/Forms/Painter.h>

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


/** @brief Renders the visual appearance of a combo box.
*/
class PT_FORMS_API ComboBoxRenderer : public Renderer
{
    public:
        /** @brief Constructs a combo box renderer.
        */
        explicit ComboBoxRenderer(std::size_t refs = 0);

        virtual ~ComboBoxRenderer();

        /** @brief Creates a new default-constructed renderer instance.
        */
        ComboBoxRenderer* create() const;

        /** @brief Applies the resolved combo box style options to this renderer.
        */
        void prepare(const StyleOptions& options);

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

        /** @brief Prepares the concrete renderer from resolved style options.
        */
        virtual void onPrepare(const StyleOptions& options) = 0;

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


/** @brief Binds ComboBox renderers and widget-local style options.
*/
class PT_FORMS_API ComboBoxStyler : public Styler
{
    public:
        /** @brief Constructs an unbound combo box styler.
        */
        ComboBoxStyler();

        /** @brief Returns the effective text background brush.
        */
        const Gfx::Brush& background() const;

        /** @brief Sets the widget-local text background brush to @a brush.
        */
        void setBackground(const Gfx::Brush& brush);

        /** @brief Returns the effective foreground brush.
        */
        const Gfx::Brush& foreground() const;

        /** @brief Sets the widget-local foreground brush to @a brush.
        */
        void setForeground(const Gfx::Brush& brush);

        /** @brief Returns the effective contour pen.
        */
        const Gfx::Pen& contour() const;

        /** @brief Sets the widget-local contour pen to @a pen.
        */
        void setContour(const Gfx::Pen& pen);

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

        /** @brief Measures the frame enclosing @a contentSize.
        */
        Gfx::SizeF measureFrame(PaintSurface& surface,
                                const Gfx::SizeF& contentSize) const;

        /** @brief Lays out combo box chrome within @a rect.
        */
        void layoutChrome(PaintSurface& surface,
                          const Gfx::RectF& rect,
                          Gfx::RectF& entryRect,
                          Gfx::RectF& buttonRect,
                          Gfx::RectF& textRect) const;

        /** @brief Returns the painter configured for combo box text, or 0 if the styler is not bound.
        */
        const Painter* textPainter(PaintSurface& surface);

        /** @brief Renders combo box chrome for the supplied states.
        */
        void renderChrome(PaintContext& context,
                          const Gfx::RectF& rect,
                          const Gfx::RectF& entryRect,
                          const Gfx::RectF& buttonRect,
                          const ComboBoxState& state,
                          const ComboBoxButtonState& buttonState) const;

        /** @brief Renders @a text at @a textPos for @a state.
        */
        void renderText(PaintContext& context,
                        const Gfx::RectF& textRect,
                        const String& text,
                        const Gfx::PointF& textPos,
                        const Gfx::RectF& cursor,
                        const ComboBoxState& state) const;

        /** @brief Assigns a specific combo box renderer.
        */
        void setRenderer(ComboBoxRenderer* renderer = 0);

        /** @brief Returns the effective combo box options.
        */
        StyleOptions& options();

        /** @brief Returns the effective combo box options.
        */
        const StyleOptions& options() const;

    protected:
        virtual StyleOptions& onBindOptions(const StyleOptions& global);

        virtual Renderer* onStyleRenderer(const Style& style);

        virtual Renderer* onCreateRenderer(const Style& style);

    private:
        FacetPtr<ComboBoxRenderer> _renderer;
        StyleOptions               _options;
};

} // namespace

} // namespace

#endif
