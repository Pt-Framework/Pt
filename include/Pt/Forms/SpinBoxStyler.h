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

/** @brief Stores the transient render state for a spin box widget.

    Carries only widget state that render hooks may observe directly.
*/
class PT_FORMS_API SpinBoxState
{
    public:
        SpinBoxState();

        bool isEnabled() const;

        void setEnabled(bool value);

        bool isHovered() const;

        void setHovered(bool value);

        bool isFocused() const;

        void setFocused(bool value);

        bool isEditable() const;

        void setEditable(bool value);

        bool isUpPressed() const;

        void setUpPressed(bool value);

        bool isUpHovered() const;

        void setUpHovered(bool value);

        bool isDownPressed() const;

        void setDownPressed(bool value);

        bool isDownHovered() const;

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


/** @brief Renders the visual appearance of a spin box widget.

    Provides rendering primitives for the entry area, up/down buttons,
    indicators, and text. Subclasses override the protected virtuals.
*/
class PT_FORMS_API SpinBoxRenderer : public Renderer
{
    public:
        explicit SpinBoxRenderer(std::size_t refs = 0);

        virtual ~SpinBoxRenderer();

        /** @brief Creates a new default-constructed spin box renderer.
        */
        SpinBoxRenderer* create() const;

    public:
        Gfx::SizeF measureFrame(PaintSurface& surface,
                                const Gfx::SizeF& contentSize);

        Gfx::SizeF measureEntry(PaintSurface& surface,
                                const Gfx::SizeF& contentSize);

        Gfx::SizeF measureIndicator(PaintSurface& surface);

        void layoutChrome(PaintSurface& surface,
                          const Gfx::RectF& rect,
                          Gfx::RectF& entryRect,
                          Gfx::RectF& upButtonRect,
                          Gfx::RectF& downButtonRect,
                          Gfx::RectF& textRect);

        Gfx::RectF layoutEntry(PaintSurface& surface,
                               const Gfx::RectF& entryRect);

        const Painter& textPainter(PaintSurface& surface);

        void renderChrome(PaintContext& context,
                          const Gfx::RectF& rect,
                          const Gfx::RectF& entryRect,
                          const Gfx::RectF& upButtonRect,
                          const Gfx::RectF& downButtonRect,
                          const SpinBoxState& state);

        void renderText(PaintContext& context,
                        const Gfx::RectF& textRect,
                        const String& text,
                        const Gfx::PointF& textPos,
                        const Gfx::RectF& cursor,
                        const SpinBoxState& state);

    protected:
        virtual SpinBoxRenderer* onCreate() const = 0;

        /** @copydoc Style::Facet::onReset
        */
        virtual void onReset(const StyleOptions& options) = 0;

        virtual Gfx::SizeF onMeasureFrame(PaintSurface& surface,
                                          const Gfx::SizeF& contentSize) = 0;

        virtual Gfx::SizeF onMeasureEntry(PaintSurface& surface,
                                          const Gfx::SizeF& contentSize) = 0;

        virtual Gfx::SizeF onMeasureIndicator(PaintSurface& surface) = 0;

        virtual void onLayoutChrome(PaintSurface& surface,
                                    const Gfx::RectF& rect,
                                    Gfx::RectF& entryRect,
                                    Gfx::RectF& upButtonRect,
                                    Gfx::RectF& downButtonRect,
                                    Gfx::RectF& textRect) = 0;

        virtual Gfx::RectF onLayoutEntry(PaintSurface& surface,
                                         const Gfx::RectF& entryRect) = 0;

        virtual const Painter& onGetTextPainter(PaintSurface& surface) = 0;

        virtual void onRenderChrome(PaintContext& context,
                                    const Gfx::RectF& rect,
                                    const Gfx::RectF& entryRect,
                                    const Gfx::RectF& upButtonRect,
                                    const Gfx::RectF& downButtonRect,
                                    const SpinBoxState& state);

        virtual void onRenderEntry(PaintContext& context,
                                   const Gfx::RectF& entryRect,
                                   const SpinBoxState& state) = 0;

        virtual void onRenderUpButton(PaintContext& context,
                                      const Gfx::RectF& buttonRect,
                                      const SpinBoxState& state) = 0;

        virtual void onRenderDownButton(PaintContext& context,
                                        const Gfx::RectF& buttonRect,
                                        const SpinBoxState& state) = 0;

        virtual void onRenderText(PaintContext& context,
                                  const Gfx::RectF& textRect,
                                  const String& text,
                                  const Gfx::PointF& textPos,
                                  const Gfx::RectF& cursor,
                                  const SpinBoxState& state) = 0;
};


/** @brief Binds spin box renderers and widget-local style options.
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
        */
        void setRenderer(SpinBoxRenderer* renderer = 0);

        /** @brief Returns the bound effective spin box options.
        */
        StyleOptions& options();

        /** @brief Returns the bound effective spin box options.
        */
        const StyleOptions& options() const;

    protected:
        virtual StyleOptions& onBindOptions(const StyleOptions& global);

        virtual Renderer* onStyleRenderer(const Style& style);

        virtual Renderer* onCreateRenderer(const Style& style);

    private:
        FacetPtr<SpinBoxRenderer> _renderer;
        StyleOptions              _options;
};

} // namespace

} // namespace

#endif
