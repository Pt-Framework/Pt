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

class PT_FORMS_API LineEditState
{
    public:
        LineEditState();

        bool isEnabled() const;

        void setEnabled(bool value);

        bool isFocused() const;

        void setFocused(bool value);

        bool isHighlighted() const;

        void setHighlighted(bool value);

        bool isEditable() const;

        void setEditable(bool value);

        bool isPlaceholder() const;

        void setPlaceholder(bool value);

    private:
        bool _enabled;
        bool _focused;
        bool _highlighted;
        bool _editable;
        bool _placeholder;
};


/** @brief Renders the visual appearance of a line edit.
*/
class PT_FORMS_API LineEditRenderer : public Renderer
{
    public:
        explicit LineEditRenderer(std::size_t refs = 0);

        virtual ~LineEditRenderer();

        LineEditRenderer* create() const;

    public:
        Gfx::SizeF measureFrame(PaintSurface& surface,
                                const Gfx::SizeF& contentSize);

        Gfx::RectF layoutFrame(PaintSurface& surface,
                               const Gfx::RectF& rect);

        /** @brief Returns the prepared text painter for @a surface.
        */
        const Painter& textPainter(PaintSurface& surface);

        void renderChrome(PaintContext& context,
                          const Gfx::RectF& rect,
                          const Gfx::RectF& textRect,
                          const String& text,
                          const Gfx::PointF& textPos,
                          const Gfx::RectF& cursor,
                          const Gfx::RectF& selection,
                          const LineEditState& state);

        void renderEntry(PaintContext& context,
                         const Gfx::RectF& rect,
                         const LineEditState& state);

        void renderSelection(PaintContext& context,
                             const Gfx::RectF& textRect,
                             const Gfx::RectF& selection,
                             const LineEditState& state);

        void renderText(PaintContext& context,
                        const Gfx::RectF& textRect,
                        const String& text,
                        const Gfx::PointF& textPos,
                        const LineEditState& state);

        void renderCursor(PaintContext& context,
                          const Gfx::RectF& textRect,
                          const Gfx::RectF& cursor,
                          const LineEditState& state);

    protected:
        virtual LineEditRenderer* onCreate() const = 0;

        /** @copydoc Style::Facet::onReset
        */
        virtual void onReset(const StyleOptions& options) = 0;

        virtual Gfx::SizeF onMeasureFrame(PaintSurface& surface,
                                          const Gfx::SizeF& contentSize) = 0;

        virtual Gfx::RectF onLayoutFrame(PaintSurface& surface,
                                         const Gfx::RectF& rect) = 0;

        virtual const Painter& onGetTextPainter(PaintSurface& surface) = 0;

        virtual void onRenderChrome(PaintContext& context,
                                    const Gfx::RectF& rect,
                                    const Gfx::RectF& textRect,
                                    const String& text,
                                    const Gfx::PointF& textPos,
                                    const Gfx::RectF& cursor,
                                    const Gfx::RectF& selection,
                                    const LineEditState& state);

        virtual void onRenderEntry(PaintContext& context,
                                   const Gfx::RectF& rect,
                                   const LineEditState& state) = 0;

        virtual void onRenderSelection(PaintContext& context,
                                       const Gfx::RectF& textRect,
                                       const Gfx::RectF& selection,
                                       const LineEditState& state) = 0;

        virtual void onRenderText(PaintContext& context,
                                  const Gfx::RectF& textRect,
                                  const String& text,
                                  const Gfx::PointF& textPos,
                                  const LineEditState& state) = 0;

        virtual void onRenderCursor(PaintContext& context,
                                    const Gfx::RectF& textRect,
                                    const Gfx::RectF& cursor,
                                    const LineEditState& state) = 0;
};


/** @brief Binds line edit renderers and widget-local style options.
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
        virtual StyleOptions& onBindOptions(const StyleOptions& styleOptions);

        virtual Renderer* onStyleRenderer(const Style& style);

        virtual Renderer* onCreateRenderer(const Style& style);

    private:
        FacetPtr<LineEditRenderer> _renderer;
        StyleOptions               _options;
};

} // namespace

} // namespace

#endif
