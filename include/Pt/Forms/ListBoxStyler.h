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

#ifndef PT_FORMS_LISTBOXSTYLE_H
#define PT_FORMS_LISTBOXSTYLE_H

#include <Pt/Forms/Styler.h>

namespace Pt {

namespace Forms {

class Pixmap;


class PT_FORMS_API ListBoxState
{
    public:
        ListBoxState();

        bool isEnabled() const;

        void setEnabled(bool value);

        bool isFocused() const;

        void setFocused(bool value);

    private:
        bool _enabled;
        bool _focused;
};


/** @brief Renders the visual appearance of a list box container.
*/
class PT_FORMS_API ListBoxRenderer : public Renderer
{
    public:
        explicit ListBoxRenderer(std::size_t refs = 0);

        virtual ~ListBoxRenderer();

        /** @brief Creates a new default-constructed renderer instance.
        */
        ListBoxRenderer* create() const;

    public:
        Gfx::SizeF measureFrame(PaintSurface& surface,
                                const Gfx::SizeF& contentSize);

        Gfx::RectF layoutFrame(PaintSurface& surface,
                               const Gfx::RectF& rect);

        void renderBackground(PaintContext& context,
                              const Gfx::RectF& rect,
                              const ListBoxState& state);

        void renderChrome(PaintContext& context,
                          const Gfx::RectF& rect,
                          const ListBoxState& state);

    protected:
        virtual ListBoxRenderer* onCreate() const = 0;

        /** @copydoc Style::Facet::onReset
        */
        virtual void onReset(const StyleOptions& options) = 0;

        virtual Gfx::SizeF onMeasureFrame(PaintSurface& surface,
                                          const Gfx::SizeF& contentSize) = 0;

        virtual Gfx::RectF onLayoutFrame(PaintSurface& surface,
                                         const Gfx::RectF& rect) = 0;

        virtual void onRenderBackground(PaintContext& context,
                                        const Gfx::RectF& rect,
                                        const ListBoxState& state) = 0;

        virtual void onRenderChrome(PaintContext& context,
                                    const Gfx::RectF& rect,
                                    const ListBoxState& state) = 0;
};


/** @brief Binds list box renderers and their local style options.
*/
class PT_FORMS_API ListBoxStyler : public Styler
{
    public:
        /** @brief Constructs an unbound list box styler.
        */
        ListBoxStyler();

        /** @brief Returns the effective list box background brush.
        */
        const Gfx::Brush& background() const;

        /** @brief Sets the widget-local list box background brush to @a brush.
        */
        void setBackground(const Gfx::Brush& brush);

        /** @brief Returns the effective list box contour pen.
        */
        const Gfx::Pen& contour() const;

        /** @brief Sets the widget-local list box contour pen to @a pen.
        */
        void setContour(const Gfx::Pen& pen);

        /** @brief Measures the frame enclosing @a contentSize.
        */
        Gfx::SizeF measureFrame(PaintSurface& surface,
                                const Gfx::SizeF& contentSize) const;

        /** @brief Returns the frame content rectangle within @a frameRect.
        */
        Gfx::RectF layoutFrame(PaintSurface& surface,
                               const Gfx::RectF& frameRect) const;

        /** @brief Renders the list box background within @a rect for @a state.
        */
        void renderBackground(PaintContext& context,
                              const Gfx::RectF& rect,
                              const ListBoxState& state) const;

        /** @brief Renders the list box chrome within @a rect for @a state.
        */
        void renderChrome(PaintContext& context,
                          const Gfx::RectF& rect,
                          const ListBoxState& state) const;

        /** @brief Assigns a specific list box renderer.
        */
        void setRenderer(ListBoxRenderer* renderer = 0);

    protected:
        virtual StyleOptions& onBindOptions(const StyleOptions& global);

        virtual Renderer* onStyleRenderer(const Style& style);

        virtual Renderer* onCreateRenderer(const Style& style);

    private:
        FacetPtr<ListBoxRenderer> _renderer;
        StyleOptions              _options;
};


class PT_FORMS_API ListItemState
{
    public:
        ListItemState();

        bool isEnabled() const;

        void setEnabled(bool value);

        bool isHighlighted() const;

        void setHighlighted(bool value);

        bool isFocused() const;

        void setFocused(bool value);

        bool isSelected() const;

        void setSelected(bool value);

    private:
        bool _enabled;
        bool _highlighted;
        bool _focused;
        bool _selected;
};


/** @brief Renders the visual appearance of a list item.
*/
class PT_FORMS_API ListItemRenderer : public Renderer
{
    public:
        explicit ListItemRenderer(std::size_t refs = 0);

        virtual ~ListItemRenderer();

        ListItemRenderer* create() const;

    public:
        Gfx::SizeF measureContent(PaintSurface& surface,
                                  const Gfx::SizeF& iconSize,
                                  const Gfx::SizeF& textSize);

        Gfx::SizeF measureFrame(PaintSurface& surface,
                                const Gfx::SizeF& contentSize);

        const Painter& textPainter(PaintSurface& surface);

        Gfx::RectF layoutFrame(PaintSurface& surface,
                               const Gfx::RectF& rect);

        void layoutContent(PaintSurface& surface,
                           const Gfx::RectF& contentRect,
                           const Gfx::SizeF& iconSize,
                           const Gfx::SizeF& textSize,
                           Gfx::RectF& iconRect,
                           Gfx::RectF& textRect);

        void renderBackground(PaintContext& context,
                              const Gfx::RectF& rect,
                              const ListItemState& state);

        /** @brief Renders the list item highlight background within @a rect for @a state.
        */
        void renderHighlight(PaintContext& context,
                             const Gfx::RectF& rect,
                             const ListItemState& state);

        void renderText(PaintContext& context,
                        const Gfx::RectF& textRect,
                        const String& text,
                        const Gfx::PointF& pos,
                        const ListItemState& state);

        void renderIcon(PaintContext& context,
                        const Gfx::RectF& iconRect,
                        const Pixmap& picture,
                        const Gfx::PointF& pos,
                        const ListItemState& state);

    protected:
        virtual ListItemRenderer* onCreate() const = 0;

        /** @copydoc Style::Facet::onReset
        */
        virtual void onReset(const StyleOptions& options) = 0;

        virtual Gfx::SizeF onMeasureContent(PaintSurface& surface,
                                            const Gfx::SizeF& iconSize,
                                            const Gfx::SizeF& textSize) = 0;

        virtual Gfx::SizeF onMeasureFrame(PaintSurface& surface,
                                          const Gfx::SizeF& contentSize) = 0;

        virtual const Painter& onGetTextPainter(PaintSurface& surface) = 0;

        virtual Gfx::RectF onLayoutFrame(PaintSurface& surface,
                                         const Gfx::RectF& rect) = 0;

        virtual void onLayoutContent(PaintSurface& surface,
                                     const Gfx::RectF& contentRect,
                                     const Gfx::SizeF& iconSize,
                                     const Gfx::SizeF& textSize,
                                     Gfx::RectF& iconRect,
                                     Gfx::RectF& textRect) = 0;

        virtual void onRenderBackground(PaintContext& context,
                                        const Gfx::RectF& rect,
                                        const ListItemState& state) = 0;

        /** @brief Renders the selected or highlighted list item background.
        */
        virtual void onRenderHighlight(PaintContext& context,
                                       const Gfx::RectF& rect,
                                       const ListItemState& state) = 0;

        virtual void onRenderText(PaintContext& context,
                                  const Gfx::RectF& textRect,
                                  const String& text,
                                  const Gfx::PointF& pos,
                                  const ListItemState& state) = 0;

        virtual void onRenderIcon(PaintContext& context,
                                  const Gfx::RectF& iconRect,
                                  const Pixmap& picture,
                                  const Gfx::PointF& pos,
                                  const ListItemState& state) = 0;
};


/** @brief Binds list item renderers and their local style options.
*/
class PT_FORMS_API ListItemStyler : public Styler
{
    public:
        /** @brief Constructs an unbound list item styler.
        */
        ListItemStyler();

        /** @brief Sets the widget-local list item background brush to @a brush.
        */
        void setBackground(const Gfx::Brush& brush);

        /** @brief Returns the effective list item text color.
        */
        const Gfx::Color& textColor() const;

        /** @brief Sets the widget-local list item text color to @a color.
        */
        void setTextColor(const Gfx::Color& color);

        /** @brief Returns the effective list item font.
        */
        Gfx::Font font() const;

        /** @brief Sets the widget-local list item font to @a font.
        */
        void setFont(const Gfx::Font& font);

        /** @brief Sets the widget-local list item font size to @a size.
        */
        void setFontSize(std::size_t size);

        /** @brief Sets the widget-local list item font weight to @a weight.
        */
        void setFontWeight(Gfx::Font::Weight weight);

        /** @brief Sets the widget-local list item font slant to @a slant.
        */
        void setFontSlant(Gfx::Font::Slant slant);

        /** @brief Measures content containing @a iconSize and @a textSize.
        */
        Gfx::SizeF measureContent(PaintSurface& surface,
                                  const Gfx::SizeF& iconSize,
                                  const Gfx::SizeF& textSize) const;

        /** @brief Measures the frame enclosing @a contentSize.
        */
        Gfx::SizeF measureFrame(PaintSurface& surface,
                                const Gfx::SizeF& contentSize) const;

        /** @brief Returns the text painter for @a surface, or 0 when unavailable.
        */
        const Painter* textPainter(PaintSurface& surface) const;

        /** @brief Returns the frame content rectangle within @a frameRect.
        */
        Gfx::RectF layoutFrame(PaintSurface& surface,
                               const Gfx::RectF& frameRect) const;

        /** @brief Lays out icon and text rectangles within @a contentRect.
        */
        void layoutContent(PaintSurface& surface,
                           const Gfx::RectF& contentRect,
                           const Gfx::SizeF& iconSize,
                           const Gfx::SizeF& textSize,
                           Gfx::RectF& iconRect,
                           Gfx::RectF& textRect) const;

        /** @brief Renders the list item background within @a rect for @a state.
        */
        void renderBackground(PaintContext& context,
                              const Gfx::RectF& rect,
                              const ListItemState& state) const;

        /** @brief Renders the list item highlight background within @a rect for @a state.
        */
        void renderHighlight(PaintContext& context,
                             const Gfx::RectF& rect,
                             const ListItemState& state) const;

        /** @brief Renders @a text at @a pos within @a textRect for @a state.
        */
        void renderText(PaintContext& context,
                        const Gfx::RectF& textRect,
                        const String& text,
                        const Gfx::PointF& pos,
                        const ListItemState& state) const;

        /** @brief Renders @a picture at @a pos within @a iconRect for @a state.
        */
        void renderIcon(PaintContext& context,
                        const Gfx::RectF& iconRect,
                        const Pixmap& picture,
                        const Gfx::PointF& pos,
                        const ListItemState& state) const;

        /** @brief Assigns a specific list item renderer.
        */
        void setRenderer(ListItemRenderer* renderer = 0);

        /** @brief Returns the bound effective list item options.
        */
        StyleOptions& options();

        /** @brief Returns the bound effective list item options.
        */
        const StyleOptions& options() const;

    protected:
        virtual StyleOptions& onBindOptions(const StyleOptions& global);

        virtual Renderer* onStyleRenderer(const Style& style);

        virtual Renderer* onCreateRenderer(const Style& style);

    private:
        FacetPtr<ListItemRenderer> _renderer;
        StyleOptions               _options;
};

} // namespace

} // namespace

#endif
