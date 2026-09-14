/* Copyright (C) 2015 Marc Boris Duerner

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

#ifndef PT_FORMS_LINEEDITSTYLE_H
#define PT_FORMS_LINEEDITSTYLE_H

#include <Pt/Forms/Styler.h>

namespace Pt {

namespace Forms {

/** @brief Transient visual state of a line edit.

    %LineEditState is the snapshot a %LineEdit passes to measure, layout,
    and paint. It is not the application model. Enabled, focused,
    highlighted, editable, and placeholder describe the look of one
    paint pass.

    @ingroup Pt-Forms-Editors
*/
class PT_FORMS_API LineEditState
{
    public:
        /** @brief Constructs an empty line edit state.
        */
        LineEditState();

        /** @brief Returns true if the field is currently enabled.
        */
        bool isEnabled() const;

        /** @brief Sets whether the field is enabled.
        */
        void setEnabled(bool value);

        /** @brief Returns true if the field currently has focus.
        */
        bool isFocused() const;

        /** @brief Sets whether the field has focus.
        */
        void setFocused(bool value);

        /** @brief Returns true if the pointer is currently over the field.
        */
        bool isHighlighted() const;

        /** @brief Sets whether the pointer is over the field.
        */
        void setHighlighted(bool value);

        /** @brief Returns true if the field currently accepts editing.
        */
        bool isEditable() const;

        /** @brief Sets whether the field currently accepts editing.
        */
        void setEditable(bool value);

        /** @brief Returns true if the field currently shows placeholder text.
        */
        bool isPlaceholder() const;

        /** @brief Sets whether the field currently shows placeholder text.
        */
        void setPlaceholder(bool value);

    private:
        bool _enabled;
        bool _focused;
        bool _highlighted;
        bool _editable;
        bool _placeholder;
};


/** @brief Renders the look of a line edit.

    A %LineEditRenderer is a %Style::Facet for the line-edit family.
    Named measure methods run inside-out. Named layout methods run
    outside-in. Named render methods paint prepared rectangles. The
    widget owns geometry and orchestrates those passes. The renderer
    does not mutate widget geometry.

    Derive a renderer to change the look of line edits. Register it
    on a %Style, or assign it to a widget with %LineEdit::setRenderer().

    @ingroup Pt-Forms-Editors
*/
class PT_FORMS_API LineEditRenderer : public Renderer
{
    public:
        /** @brief Constructs a renderer with reference count @a refs.
        */
        explicit LineEditRenderer(std::size_t refs = 0);

        /** @brief Destroys the renderer.
        */
        virtual ~LineEditRenderer();

        /** @brief Creates a new default-constructed instance that the caller owns.
        */
        LineEditRenderer* create() const;

    public:
        /** @brief Returns the outer size including the frame for @a contentSize.
        */
        Gfx::SizeF measureFrame(PaintSurface& surface,
                                const Gfx::SizeF& contentSize);

        /** @brief Returns the content rectangle within @a rect.
        */
        Gfx::RectF layoutFrame(PaintSurface& surface,
                               const Gfx::RectF& rect);

        /** @brief Returns a painter with the current font and text color.
        */
        const Painter& textPainter(PaintSurface& surface);

        /** @brief Paints entry, selection, text, and caret for @a state.
        */
        void renderChrome(PaintContext& context,
                          const Gfx::RectF& rect,
                          const Gfx::RectF& textRect,
                          const String& text,
                          const Gfx::PointF& textPos,
                          const Gfx::RectF& cursor,
                          const Gfx::RectF& selection,
                          const LineEditState& state);

        /** @brief Paints the entry for @a state.
        */
        void renderEntry(PaintContext& context,
                         const Gfx::RectF& rect,
                         const LineEditState& state);

        /** @brief Paints the selection for @a state.
        */
        void renderSelection(PaintContext& context,
                             const Gfx::RectF& textRect,
                             const Gfx::RectF& selection,
                             const LineEditState& state);

        /** @brief Paints @a text at @a textPos for @a state.
        */
        void renderText(PaintContext& context,
                        const Gfx::RectF& textRect,
                        const String& text,
                        const Gfx::PointF& textPos,
                        const LineEditState& state);

        /** @brief Paints the caret for @a state.
        */
        void renderCursor(PaintContext& context,
                          const Gfx::RectF& textRect,
                          const Gfx::RectF& cursor,
                          const LineEditState& state);

    protected:
        /** @brief Creates a new instance of the same concrete type.
        */
        virtual LineEditRenderer* onCreate() const = 0;

        /** @copydoc Style::Facet::onReset
        */
        virtual void onReset(const StyleOptions& options) = 0;

        /** @brief Measures the frame enclosing @a contentSize.
        */
        virtual Gfx::SizeF onMeasureFrame(PaintSurface& surface,
                                          const Gfx::SizeF& contentSize) = 0;

        /** @brief Returns the content rectangle within @a rect.
        */
        virtual Gfx::RectF onLayoutFrame(PaintSurface& surface,
                                         const Gfx::RectF& rect) = 0;

        /** @brief Returns a painter with the current font and text color.
        */
        virtual const Painter& onGetTextPainter(PaintSurface& surface) = 0;

        /** @brief Paints entry, selection, text, and caret for @a state.
        */
        virtual void onRenderChrome(PaintContext& context,
                                    const Gfx::RectF& rect,
                                    const Gfx::RectF& textRect,
                                    const String& text,
                                    const Gfx::PointF& textPos,
                                    const Gfx::RectF& cursor,
                                    const Gfx::RectF& selection,
                                    const LineEditState& state);

        /** @brief Paints the entry for @a state.
        */
        virtual void onRenderEntry(PaintContext& context,
                                   const Gfx::RectF& rect,
                                   const LineEditState& state) = 0;

        /** @brief Paints the selection for @a state.
        */
        virtual void onRenderSelection(PaintContext& context,
                                       const Gfx::RectF& textRect,
                                       const Gfx::RectF& selection,
                                       const LineEditState& state) = 0;

        /** @brief Paints @a text at @a textPos for @a state.
        */
        virtual void onRenderText(PaintContext& context,
                                  const Gfx::RectF& textRect,
                                  const String& text,
                                  const Gfx::PointF& textPos,
                                  const LineEditState& state) = 0;

        /** @brief Paints the caret for @a state.
        */
        virtual void onRenderCursor(PaintContext& context,
                                    const Gfx::RectF& textRect,
                                    const Gfx::RectF& cursor,
                                    const LineEditState& state) = 0;
};


/** @brief Binds a line edit to the current style renderer.

    A %LineEdit owns a %LineEditStyler. Applications do not construct
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
class PT_FORMS_API LineEditStyler : public Styler
{
    public:
        /** @brief Constructs an unbound line edit styler.
        */
        LineEditStyler();

        /** @brief Returns the effective text background brush.
        */
        const Gfx::Brush& background() const;

        /** @brief Sets the widget-local text background brush to @a brush.
        */
        void setBackground(const Gfx::Brush& brush);

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

        /** @brief Measures the frame enclosing @a contentSize, or returns an empty size.
        */
        Gfx::SizeF measureFrame(PaintSurface& surface,
                                const Gfx::SizeF& contentSize) const;

        /** @brief Returns the frame content rectangle, or an empty rectangle when unavailable.
        */
        Gfx::RectF layoutFrame(PaintSurface& surface,
                               const Gfx::RectF& rect) const;

        /** @brief Returns the prepared text painter, or 0 when no renderer is bound.
        */
        const Painter* textPainter(PaintSurface& surface) const;

        /** @brief Renders line edit chrome when a renderer is bound.
        */
        void renderChrome(PaintContext& context,
                          const Gfx::RectF& rect,
                          const Gfx::RectF& textRect,
                          const String& text,
                          const Gfx::PointF& textPos,
                          const Gfx::RectF& cursor,
                          const Gfx::RectF& selection,
                          const LineEditState& state) const;

        /** @brief Assigns a specific line edit renderer or restores style fallback.
        */
        void setRenderer(LineEditRenderer* renderer = 0);

        /** @brief Returns the bound effective line edit options.
        */
        StyleOptions& options();

        /** @brief Returns the bound effective line edit options.
        */
        const StyleOptions& options() const;

    protected:
        /** @brief Binds the overlay to @a styleOptions and returns it.
        */
        virtual StyleOptions& onBindOptions(const StyleOptions& styleOptions);

        /** @brief Returns the shared line-edit renderer from @a style, or 0.
        */
        virtual Renderer* onStyleRenderer(const Style& style);

        /** @brief Creates an independent clone of the style renderer, or 0.
        */
        virtual Renderer* onCreateRenderer(const Style& style);

    private:
        FacetPtr<LineEditRenderer> _renderer;
        StyleOptions               _options;
};

} // namespace

} // namespace

#endif
