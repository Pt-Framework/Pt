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

/** @brief Transient visual state of a combo box.

    %ComboBoxState is the snapshot a %ComboBox passes to measure, layout,
    and paint. It is not the application model. Enabled, focused,
    highlighted, editable, and popup visibility describe the look of one
    paint pass.

    @ingroup Pt-Forms-Editors
*/
class PT_FORMS_API ComboBoxState
{
    public:
        /** @brief Constructs an empty combo box state.
        */
        ComboBoxState();

        /** @brief Returns true if the box is currently enabled.
        */
        bool isEnabled() const;

        /** @brief Sets whether the box is enabled.
        */
        void setEnabled(bool value);

        /** @brief Returns true if the box currently has focus.
        */
        bool isFocused() const;

        /** @brief Sets whether the box has focus.
        */
        void setFocused(bool value);

        /** @brief Returns true if the pointer is currently over the box.
        */
        bool isHighlighted() const;

        /** @brief Sets whether the pointer is over the box.
        */
        void setHighlighted(bool value);

        /** @brief Returns true if the box currently accepts typing.
        */
        bool isEditable() const;

        /** @brief Sets whether the box currently accepts typing.
        */
        void setEditable(bool value);

        /** @brief Returns true if the popup list is currently shown.
        */
        bool isPopupVisible() const;

        /** @brief Sets whether the popup list is shown.
        */
        void setPopupVisible(bool value);

    private:
        bool _enabled;
        bool _focused;
        bool _highlighted;
        bool _editable;
        bool _popupVisible;
};


/** @brief Transient visual state of a combo-box drop button.

    %ComboBoxButtonState is the snapshot a %ComboBox passes for the
    drop button. It is not the application model. Highlighted and
    pressed describe the look of one paint pass.

    @ingroup Pt-Forms-Editors
*/
class PT_FORMS_API ComboBoxButtonState
{
    public:
        /** @brief Constructs an empty combo-box button state.
        */
        ComboBoxButtonState();

        /** @brief Returns true if the pointer is currently over the button.
        */
        bool isHighlighted() const;

        /** @brief Sets whether the pointer is over the button.
        */
        void setHighlighted(bool value);

        /** @brief Returns true if the button is currently pressed.
        */
        bool isPressed() const;

        /** @brief Sets whether the button is pressed.
        */
        void setPressed(bool value);

    private:
        bool _highlighted;
        bool _pressed;
};


/** @brief Renders the look of a combo box.

    A %ComboBoxRenderer is a %Style::Facet for the combo-box family.
    Named measure methods run inside-out. Named layout methods run
    outside-in. Named render methods paint prepared rectangles. The
    widget owns geometry and orchestrates those passes. The renderer
    does not mutate widget geometry.

    Derive a renderer to change the look of combo boxes. Register it
    on a %Style, or assign it to a widget with %ComboBox::setRenderer().

    @ingroup Pt-Forms-Editors
*/
class PT_FORMS_API ComboBoxRenderer : public Renderer
{
    public:
        /** @brief Constructs a renderer with reference count @a refs.
        */
        explicit ComboBoxRenderer(std::size_t refs = 0);

        /** @brief Destroys the renderer.
        */
        virtual ~ComboBoxRenderer();

        /** @brief Creates a new default-constructed instance that the caller owns.
        */
        ComboBoxRenderer* create() const;

    public:
        /** @brief Returns the outer size including the frame for @a contentSize.
        */
        Gfx::SizeF measureFrame(PaintSurface& surface,
                                const Gfx::SizeF& contentSize);

        /** @brief Returns the natural size of the drop button.
        */
        Gfx::SizeF measureButton(PaintSurface& surface);

        /** @brief Places the entry, drop button, and text in @a rect.
        */
        void layoutChrome(PaintSurface& surface,
                          const Gfx::RectF& rect,
                          Gfx::RectF& entryRect,
                          Gfx::RectF& buttonRect,
                          Gfx::RectF& textRect);

        /** @brief Returns a painter with the current font and text color.
        */
        const Painter& textPainter(PaintSurface& surface);

        /** @brief Paints entry and drop button for @a state.
        */
        void renderChrome(PaintContext& context,
                          const Gfx::RectF& rect,
                          const Gfx::RectF& entryRect,
                          const Gfx::RectF& buttonRect,
                          const ComboBoxState& state,
                          const ComboBoxButtonState& buttonState);

        /** @brief Paints the drop button for @a state.
        */
        void renderButton(PaintContext& context,
                          const Gfx::RectF& buttonRect,
                          const ComboBoxState& state,
                          const ComboBoxButtonState& buttonState);

        /** @brief Paints @a text at @a textPos for @a state.
        */
        void renderText(PaintContext& context,
                        const Gfx::RectF& textRect,
                        const String& text,
                        const Gfx::PointF& textPos,
                        const Gfx::RectF& cursor,
                        const ComboBoxState& state);

    protected:
        /** @brief Creates a new instance of the same concrete type.
        */
        virtual ComboBoxRenderer* onCreate() const = 0;

        /** @copydoc Style::Facet::onReset
        */
        virtual void onReset(const StyleOptions& options) = 0;

        /** @brief Measures the frame enclosing @a contentSize.
        */
        virtual Gfx::SizeF onMeasureFrame(PaintSurface& surface,
                                          const Gfx::SizeF& contentSize) = 0;

        /** @brief Returns the natural size of the drop button.
        */
        virtual Gfx::SizeF onMeasureButton(PaintSurface& surface) = 0;

        /** @brief Places the entry, drop button, and text in @a rect.
        */
        virtual void onLayoutChrome(PaintSurface& surface,
                                    const Gfx::RectF& rect,
                                    Gfx::RectF& entryRect,
                                    Gfx::RectF& buttonRect,
                                    Gfx::RectF& textRect) = 0;

        /** @brief Returns a painter with the current font and text color.
        */
        virtual const Painter& onGetTextPainter(PaintSurface& surface) = 0;

        /** @brief Paints entry and drop button for @a state.
        */
        virtual void onRenderChrome(PaintContext& context,
                                    const Gfx::RectF& rect,
                                    const Gfx::RectF& entryRect,
                                    const Gfx::RectF& buttonRect,
                                    const ComboBoxState& state,
                                    const ComboBoxButtonState& buttonState);

        /** @brief Paints the entry for @a state.
        */
        virtual void onRenderEntry(PaintContext& context,
                                   const Gfx::RectF& entryRect,
                                   const ComboBoxState& state) = 0;

        /** @brief Paints the drop button for @a state.
        */
        virtual void onRenderButton(PaintContext& context,
                                    const Gfx::RectF& buttonRect,
                                    const ComboBoxState& state,
                                    const ComboBoxButtonState& buttonState) = 0;

        /** @brief Paints @a text at @a textPos for @a state.
        */
        virtual void onRenderText(PaintContext& context,
                                  const Gfx::RectF& textRect,
                                  const String& text,
                                  const Gfx::PointF& textPos,
                                  const Gfx::RectF& cursor,
                                  const ComboBoxState& state) = 0;
};


/** @brief Binds a combo box to the current style renderer.

    A %ComboBox owns a %ComboBoxStyler. Applications do not construct
    one. Call %Styler::bind() from %onInvalidate(). When the overlay
    has no local options, bind uses the shared renderer from the style.
    Local options use a private clone. %setRenderer() keeps an assigned
    renderer until it is cleared. A null renderer falls back on the
    next bind.

    Appearance getters return effective tokens after bind. Setters
    write widget-local options. Measure, layout, and paint call the
    typed methods on this styler, not a public renderer accessor.

    @ingroup Pt-Forms-Editors
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

            A null renderer falls back to the current style on the next bind.
        */
        void setRenderer(ComboBoxRenderer* renderer = 0);

        /** @brief Returns the effective combo box options.
        */
        StyleOptions& options();

        /** @brief Returns the effective combo box options.
        */
        const StyleOptions& options() const;

    protected:
        /** @brief Binds the overlay to @a global and returns it.
        */
        virtual StyleOptions& onBindOptions(const StyleOptions& global);

        /** @brief Returns the shared combo-box renderer from @a style, or 0.
        */
        virtual Renderer* onStyleRenderer(const Style& style);

        /** @brief Creates an independent clone of the style renderer, or 0.
        */
        virtual Renderer* onCreateRenderer(const Style& style);

    private:
        FacetPtr<ComboBoxRenderer> _renderer;
        StyleOptions               _options;
};

} // namespace

} // namespace

#endif
