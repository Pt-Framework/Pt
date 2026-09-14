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

#ifndef Pt_Forms_SpinBoxStyler_h
#define Pt_Forms_SpinBoxStyler_h

#include <Pt/Forms/Styler.h>

namespace Pt {

namespace Forms {

/** @brief Transient visual state of a spin box.

    %SpinBoxState is the snapshot a %SpinBox passes to measure, layout,
    and paint. It is not the application model. Enabled, hovered,
    focused, editable, and the up and down control flags describe the
    look of one paint pass.

    @ingroup Pt-Forms-Editors
*/
class PT_FORMS_API SpinBoxState
{
    public:
        /** @brief Constructs an empty spin box state.
        */
        SpinBoxState();

        /** @brief Returns true if the box is currently enabled.
        */
        bool isEnabled() const;

        /** @brief Sets whether the box is enabled.
        */
        void setEnabled(bool value);

        /** @brief Returns true if the pointer is currently over the box.
        */
        bool isHovered() const;

        /** @brief Sets whether the pointer is over the box.
        */
        void setHovered(bool value);

        /** @brief Returns true if the box currently has focus.
        */
        bool isFocused() const;

        /** @brief Sets whether the box has focus.
        */
        void setFocused(bool value);

        /** @brief Returns true if the box currently accepts typing.
        */
        bool isEditable() const;

        /** @brief Sets whether the box currently accepts typing.
        */
        void setEditable(bool value);

        /** @brief Returns true if the up control is currently pressed.
        */
        bool isUpPressed() const;

        /** @brief Sets whether the up control is pressed.
        */
        void setUpPressed(bool value);

        /** @brief Returns true if the pointer is currently over the up control.
        */
        bool isUpHovered() const;

        /** @brief Sets whether the pointer is over the up control.
        */
        void setUpHovered(bool value);

        /** @brief Returns true if the down control is currently pressed.
        */
        bool isDownPressed() const;

        /** @brief Sets whether the down control is pressed.
        */
        void setDownPressed(bool value);

        /** @brief Returns true if the pointer is currently over the down control.
        */
        bool isDownHovered() const;

        /** @brief Sets whether the pointer is over the down control.
        */
        void setDownHovered(bool value);

    private:
        bool _enabled;
        bool _hovered;
        bool _focused;
        bool _editable;
        bool _upPressed;
        bool _upHovered;
        bool _downPressed;
        bool _downHovered;
};


/** @brief Renders the look of a spin box.

    A %SpinBoxRenderer is a %Style::Facet for the spin-box family.
    Named measure methods run inside-out. Named layout methods run
    outside-in. Named render methods paint prepared rectangles. The
    widget owns geometry and orchestrates those passes. The renderer
    does not mutate widget geometry.

    Up and down controls are integrated subparts. A style may merge
    them with the entry or place them beside it. They are not public
    borrowable primitives.

    Derive a renderer to change the look of spin boxes. Register it
    on a %Style, or assign it to a widget with %SpinBox::setRenderer().

    @ingroup Pt-Forms-Editors
*/
class PT_FORMS_API SpinBoxRenderer : public Renderer
{
    public:
        /** @brief Constructs a renderer with reference count @a refs.
        */
        explicit SpinBoxRenderer(std::size_t refs = 0);

        /** @brief Destroys the renderer.
        */
        virtual ~SpinBoxRenderer();

        /** @brief Creates a new default-constructed instance that the caller owns.
        */
        SpinBoxRenderer* create() const;

    public:
        /** @brief Returns the outer size including the frame for @a contentSize.
        */
        Gfx::SizeF measureFrame(PaintSurface& surface,
                                const Gfx::SizeF& contentSize);

        /** @brief Returns the entry size for @a contentSize.
        */
        Gfx::SizeF measureEntry(PaintSurface& surface,
                                const Gfx::SizeF& contentSize);

        /** @brief Returns the natural size of a step control.
        */
        Gfx::SizeF measureIndicator(PaintSurface& surface);

        /** @brief Places the entry, step controls, and text in @a rect.
        */
        void layoutChrome(PaintSurface& surface,
                          const Gfx::RectF& rect,
                          Gfx::RectF& entryRect,
                          Gfx::RectF& upButtonRect,
                          Gfx::RectF& downButtonRect,
                          Gfx::RectF& textRect);

        /** @brief Returns the text rectangle within @a entryRect.
        */
        Gfx::RectF layoutEntry(PaintSurface& surface,
                               const Gfx::RectF& entryRect);

        /** @brief Returns a painter with the current font and text color.
        */
        const Painter& textPainter(PaintSurface& surface);

        /** @brief Paints entry and step controls for @a state.
        */
        void renderChrome(PaintContext& context,
                          const Gfx::RectF& rect,
                          const Gfx::RectF& entryRect,
                          const Gfx::RectF& upButtonRect,
                          const Gfx::RectF& downButtonRect,
                          const SpinBoxState& state);

        /** @brief Paints @a text at @a textPos for @a state.
        */
        void renderText(PaintContext& context,
                        const Gfx::RectF& textRect,
                        const String& text,
                        const Gfx::PointF& textPos,
                        const Gfx::RectF& cursor,
                        const SpinBoxState& state);

    protected:
        /** @brief Creates a new instance of the same concrete type.
        */
        virtual SpinBoxRenderer* onCreate() const = 0;

        /** @copydoc Style::Facet::onReset
        */
        virtual void onReset(const StyleOptions& options) = 0;

        /** @brief Measures the frame enclosing @a contentSize.
        */
        virtual Gfx::SizeF onMeasureFrame(PaintSurface& surface,
                                          const Gfx::SizeF& contentSize) = 0;

        /** @brief Measures the entry enclosing @a contentSize.
        */
        virtual Gfx::SizeF onMeasureEntry(PaintSurface& surface,
                                          const Gfx::SizeF& contentSize) = 0;

        /** @brief Returns the natural size of a step control.
        */
        virtual Gfx::SizeF onMeasureIndicator(PaintSurface& surface) = 0;

        /** @brief Places the entry, step controls, and text in @a rect.
        */
        virtual void onLayoutChrome(PaintSurface& surface,
                                    const Gfx::RectF& rect,
                                    Gfx::RectF& entryRect,
                                    Gfx::RectF& upButtonRect,
                                    Gfx::RectF& downButtonRect,
                                    Gfx::RectF& textRect) = 0;

        /** @brief Returns the text rectangle within @a entryRect.
        */
        virtual Gfx::RectF onLayoutEntry(PaintSurface& surface,
                                         const Gfx::RectF& entryRect) = 0;

        /** @brief Returns a painter with the current font and text color.
        */
        virtual const Painter& onGetTextPainter(PaintSurface& surface) = 0;

        /** @brief Paints entry and step controls for @a state.
        */
        virtual void onRenderChrome(PaintContext& context,
                                    const Gfx::RectF& rect,
                                    const Gfx::RectF& entryRect,
                                    const Gfx::RectF& upButtonRect,
                                    const Gfx::RectF& downButtonRect,
                                    const SpinBoxState& state);

        /** @brief Paints the entry for @a state.
        */
        virtual void onRenderEntry(PaintContext& context,
                                   const Gfx::RectF& entryRect,
                                   const SpinBoxState& state) = 0;

        /** @brief Paints the up control for @a state.
        */
        virtual void onRenderUpButton(PaintContext& context,
                                      const Gfx::RectF& buttonRect,
                                      const SpinBoxState& state) = 0;

        /** @brief Paints the down control for @a state.
        */
        virtual void onRenderDownButton(PaintContext& context,
                                        const Gfx::RectF& buttonRect,
                                        const SpinBoxState& state) = 0;

        /** @brief Paints @a text at @a textPos for @a state.
        */
        virtual void onRenderText(PaintContext& context,
                                  const Gfx::RectF& textRect,
                                  const String& text,
                                  const Gfx::PointF& textPos,
                                  const Gfx::RectF& cursor,
                                  const SpinBoxState& state) = 0;
};


/** @brief Binds a spin box to the current style renderer.

    A %SpinBox owns a %SpinBoxStyler. Applications do not construct
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
class PT_FORMS_API SpinBoxStyler : public Styler
{
    public:
        /** @brief Constructs an unbound spin box styler.
        */
        SpinBoxStyler();

        /** @brief Returns the effective spin box text background brush.
        */
        const Gfx::Brush& background() const;

        /** @brief Sets the widget-local spin box text background brush.
        */
        void setBackground(const Gfx::Brush& brush);

        /** @brief Returns the effective foreground brush.
        */
        const Gfx::Brush& foreground() const;

        /** @brief Sets the widget-local foreground brush.
        */
        void setForeground(const Gfx::Brush& brush);

        /** @brief Returns the effective contour pen.
        */
        const Gfx::Pen& contour() const;

        /** @brief Sets the widget-local contour pen.
        */
        void setContour(const Gfx::Pen& pen);

        /** @brief Returns the effective text color.
        */
        const Gfx::Color& textColor() const;

        /** @brief Sets the widget-local text color.
        */
        void setTextColor(const Gfx::Color& color);

        /** @brief Returns the effective font.
        */
        Gfx::Font font() const;

        /** @brief Sets the widget-local font.
        */
        void setFont(const Gfx::Font& font);

        /** @brief Sets the widget-local font size.
        */
        void setFontSize(std::size_t size);

        /** @brief Sets the widget-local font weight.
        */
        void setFontWeight(Gfx::Font::Weight weight);

        /** @brief Sets the widget-local font slant.
        */
        void setFontSlant(Gfx::Font::Slant slant);

        /** @brief Measures the frame enclosing @a contentSize.
        */
        Gfx::SizeF measureFrame(PaintSurface& surface,
                                const Gfx::SizeF& contentSize) const;

        /** @brief Lays out spin box chrome and clears the output rectangles when unavailable.
        */
        void layoutChrome(PaintSurface& surface,
                          const Gfx::RectF& rect,
                          Gfx::RectF& entryRect,
                          Gfx::RectF& upButtonRect,
                          Gfx::RectF& downButtonRect,
                          Gfx::RectF& textRect) const;

        /** @brief Returns the prepared text painter for @a surface, or 0 when unavailable.
        */
        const Painter* textPainter(PaintSurface& surface) const;

        /** @brief Renders spin box chrome within the supplied rectangles.
        */
        void renderChrome(PaintContext& context,
                          const Gfx::RectF& rect,
                          const Gfx::RectF& entryRect,
                          const Gfx::RectF& upButtonRect,
                          const Gfx::RectF& downButtonRect,
                          const SpinBoxState& state) const;

        /** @brief Renders spin box text and cursor data within @a textRect.
        */
        void renderText(PaintContext& context,
                        const Gfx::RectF& textRect,
                        const String& text,
                        const Gfx::PointF& textPos,
                        const Gfx::RectF& cursor,
                        const SpinBoxState& state) const;

        /** @brief Assigns a specific spin box renderer.

            A null renderer falls back to the current style on the next bind.
        */
        void setRenderer(SpinBoxRenderer* renderer = 0);

        /** @brief Returns the bound effective spin box options.
        */
        StyleOptions& options();

        /** @brief Returns the bound effective spin box options.
        */
        const StyleOptions& options() const;

    protected:
        /** @brief Binds the overlay to @a global and returns it.
        */
        virtual StyleOptions& onBindOptions(const StyleOptions& global);

        /** @brief Returns the shared spin-box renderer from @a style, or 0.
        */
        virtual Renderer* onStyleRenderer(const Style& style);

        /** @brief Creates an independent clone of the style renderer, or 0.
        */
        virtual Renderer* onCreateRenderer(const Style& style);

    private:
        FacetPtr<SpinBoxRenderer> _renderer;
        StyleOptions              _options;
};

} // namespace

} // namespace

#endif
