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


/** @brief Transient visual state of a list box.

    %ListBoxState is the snapshot a %ListBox passes to measure, layout,
    and paint. It is not the application model. Enabled and focused
    describe the look of one paint pass.

    @ingroup Pt-Forms-Collections
*/
class PT_FORMS_API ListBoxState
{
    public:
        /** @brief Constructs an empty list box state.
        */
        ListBoxState();

        /** @brief Returns true if the list is currently enabled.
        */
        bool isEnabled() const;

        /** @brief Sets whether the list is enabled.
        */
        void setEnabled(bool value);

        /** @brief Returns true if the list currently has focus.
        */
        bool isFocused() const;

        /** @brief Sets whether the list has focus.
        */
        void setFocused(bool value);

    private:
        bool _enabled;
        bool _focused;
};


/** @brief Renders the look of a list box.

    A %ListBoxRenderer is a %Style::Facet for the list-box family.
    Named measure methods run inside-out. Named layout methods run
    outside-in. Named render methods paint prepared rectangles. The
    widget owns geometry and orchestrates those passes. The renderer
    does not mutate widget geometry.

    This renderer paints the list chrome. %ListItemRenderer paints
    each item.

    Derive a renderer to change the look of list boxes. Register it
    on a %Style, or assign it to a widget with %ListBox::setRenderer().

    @ingroup Pt-Forms-Collections
*/
class PT_FORMS_API ListBoxRenderer : public Renderer
{
    public:
        /** @brief Constructs a renderer with reference count @a refs.
        */
        explicit ListBoxRenderer(std::size_t refs = 0);

        /** @brief Destroys the renderer.
        */
        virtual ~ListBoxRenderer();

        /** @brief Creates a new default-constructed instance that the caller owns.
        */
        ListBoxRenderer* create() const;

    public:
        /** @brief Returns the outer size including the frame for @a contentSize.
        */
        Gfx::SizeF measureFrame(PaintSurface& surface,
                                const Gfx::SizeF& contentSize);

        /** @brief Returns the content rectangle within @a rect.
        */
        Gfx::RectF layoutFrame(PaintSurface& surface,
                               const Gfx::RectF& rect);

        /** @brief Paints the background for @a state.
        */
        void renderBackground(PaintContext& context,
                              const Gfx::RectF& rect,
                              const ListBoxState& state);

        /** @brief Paints the frame for @a state.
        */
        void renderChrome(PaintContext& context,
                          const Gfx::RectF& rect,
                          const ListBoxState& state);

    protected:
        /** @brief Creates a new instance of the same concrete type.
        */
        virtual ListBoxRenderer* onCreate() const = 0;

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

        /** @brief Paints the background for @a state.
        */
        virtual void onRenderBackground(PaintContext& context,
                                        const Gfx::RectF& rect,
                                        const ListBoxState& state) = 0;

        /** @brief Paints the frame for @a state.
        */
        virtual void onRenderChrome(PaintContext& context,
                                    const Gfx::RectF& rect,
                                    const ListBoxState& state) = 0;
};


/** @brief Binds a list box to the current style renderer.

    A %ListBox owns a %ListBoxStyler. Applications do not construct
    one. Call %Styler::bind() from %onInvalidate(). When the overlay
    has no local options, bind uses the shared renderer from the style.
    Local options use a private clone. %setRenderer() keeps an assigned
    renderer until it is cleared. A null renderer falls back on the
    next bind.

    Appearance getters return effective tokens after bind. Setters
    write widget-local options. Measure, layout, and paint call the
    typed methods on this styler, not a public renderer accessor.

    @ingroup Pt-Forms-Collections
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

            A null renderer falls back to the current style on the next bind.
        */
        void setRenderer(ListBoxRenderer* renderer = 0);

    protected:
        /** @brief Binds the overlay to @a global and returns it.
        */
        virtual StyleOptions& onBindOptions(const StyleOptions& global);

        /** @brief Returns the shared list-box renderer from @a style, or 0.
        */
        virtual Renderer* onStyleRenderer(const Style& style);

        /** @brief Creates an independent clone of the style renderer, or 0.
        */
        virtual Renderer* onCreateRenderer(const Style& style);

    private:
        FacetPtr<ListBoxRenderer> _renderer;
        StyleOptions              _options;
};


/** @brief Transient visual state of a list item.

    %ListItemState is the snapshot a %ListBoxItem passes to measure,
    layout, and paint. It is not the application model. Enabled,
    highlighted, focused, and selected describe the look of one paint
    pass.

    @ingroup Pt-Forms-Collections
*/
class PT_FORMS_API ListItemState
{
    public:
        /** @brief Constructs an empty list item state.
        */
        ListItemState();

        /** @brief Returns true if the item is currently enabled.
        */
        bool isEnabled() const;

        /** @brief Sets whether the item is enabled.
        */
        void setEnabled(bool value);

        /** @brief Returns true if the pointer is currently over the item.
        */
        bool isHighlighted() const;

        /** @brief Sets whether the pointer is over the item.
        */
        void setHighlighted(bool value);

        /** @brief Returns true if the item currently has focus.
        */
        bool isFocused() const;

        /** @brief Sets whether the item has focus.
        */
        void setFocused(bool value);

        /** @brief Returns true if the item is currently selected.
        */
        bool isSelected() const;

        /** @brief Sets whether the item is selected.
        */
        void setSelected(bool value);

    private:
        bool _enabled;
        bool _highlighted;
        bool _focused;
        bool _selected;
};


/** @brief Renders the look of a list item.

    A %ListItemRenderer is a %Style::Facet for the list-item family.
    Named measure methods run inside-out. Named layout methods run
    outside-in. Named render methods paint prepared rectangles. The
    widget owns geometry and orchestrates those passes. The renderer
    does not mutate widget geometry.

    Item methods take rectangles, text, icons, and item state. They
    do not take the list.

    Derive a renderer to change the look of list items. Register it
    on a %Style, or assign it to a widget with
    %ListBoxItem::setRenderer().

    @ingroup Pt-Forms-Collections
*/
class PT_FORMS_API ListItemRenderer : public Renderer
{
    public:
        /** @brief Constructs a renderer with reference count @a refs.
        */
        explicit ListItemRenderer(std::size_t refs = 0);

        /** @brief Destroys the renderer.
        */
        virtual ~ListItemRenderer();

        /** @brief Creates a new default-constructed instance that the caller owns.
        */
        ListItemRenderer* create() const;

    public:
        /** @brief Returns the size of content with @a iconSize and @a textSize.
        */
        Gfx::SizeF measureContent(PaintSurface& surface,
                                  const Gfx::SizeF& iconSize,
                                  const Gfx::SizeF& textSize);

        /** @brief Returns the outer size including the frame for @a contentSize.
        */
        Gfx::SizeF measureFrame(PaintSurface& surface,
                                const Gfx::SizeF& contentSize);

        /** @brief Returns a painter with the current font and text color.
        */
        const Painter& textPainter(PaintSurface& surface);

        /** @brief Returns the content rectangle within @a rect.
        */
        Gfx::RectF layoutFrame(PaintSurface& surface,
                               const Gfx::RectF& rect);

        /** @brief Places the icon and text in @a contentRect.
        */
        void layoutContent(PaintSurface& surface,
                           const Gfx::RectF& contentRect,
                           const Gfx::SizeF& iconSize,
                           const Gfx::SizeF& textSize,
                           Gfx::RectF& iconRect,
                           Gfx::RectF& textRect);

        /** @brief Paints the background for @a state.
        */
        void renderBackground(PaintContext& context,
                              const Gfx::RectF& rect,
                              const ListItemState& state);

        /** @brief Paints the highlight for @a state.
        */
        void renderHighlight(PaintContext& context,
                             const Gfx::RectF& rect,
                             const ListItemState& state);

        /** @brief Paints @a text at @a pos for @a state.
        */
        void renderText(PaintContext& context,
                        const Gfx::RectF& textRect,
                        const String& text,
                        const Gfx::PointF& pos,
                        const ListItemState& state);

        /** @brief Paints @a picture at @a pos for @a state.
        */
        void renderIcon(PaintContext& context,
                        const Gfx::RectF& iconRect,
                        const Pixmap& picture,
                        const Gfx::PointF& pos,
                        const ListItemState& state);

    protected:
        /** @brief Creates a new instance of the same concrete type.
        */
        virtual ListItemRenderer* onCreate() const = 0;

        /** @copydoc Style::Facet::onReset
        */
        virtual void onReset(const StyleOptions& options) = 0;

        /** @brief Returns the size of content with @a iconSize and @a textSize.
        */
        virtual Gfx::SizeF onMeasureContent(PaintSurface& surface,
                                            const Gfx::SizeF& iconSize,
                                            const Gfx::SizeF& textSize) = 0;

        /** @brief Measures the frame enclosing @a contentSize.
        */
        virtual Gfx::SizeF onMeasureFrame(PaintSurface& surface,
                                          const Gfx::SizeF& contentSize) = 0;

        /** @brief Returns a painter with the current font and text color.
        */
        virtual const Painter& onGetTextPainter(PaintSurface& surface) = 0;

        /** @brief Returns the content rectangle within @a rect.
        */
        virtual Gfx::RectF onLayoutFrame(PaintSurface& surface,
                                         const Gfx::RectF& rect) = 0;

        /** @brief Places the icon and text in @a contentRect.
        */
        virtual void onLayoutContent(PaintSurface& surface,
                                     const Gfx::RectF& contentRect,
                                     const Gfx::SizeF& iconSize,
                                     const Gfx::SizeF& textSize,
                                     Gfx::RectF& iconRect,
                                     Gfx::RectF& textRect) = 0;

        /** @brief Paints the background for @a state.
        */
        virtual void onRenderBackground(PaintContext& context,
                                        const Gfx::RectF& rect,
                                        const ListItemState& state) = 0;

        /** @brief Paints the highlight for @a state.
        */
        virtual void onRenderHighlight(PaintContext& context,
                                       const Gfx::RectF& rect,
                                       const ListItemState& state) = 0;

        /** @brief Paints @a text at @a pos for @a state.
        */
        virtual void onRenderText(PaintContext& context,
                                  const Gfx::RectF& textRect,
                                  const String& text,
                                  const Gfx::PointF& pos,
                                  const ListItemState& state) = 0;

        /** @brief Paints @a picture at @a pos for @a state.
        */
        virtual void onRenderIcon(PaintContext& context,
                                  const Gfx::RectF& iconRect,
                                  const Pixmap& picture,
                                  const Gfx::PointF& pos,
                                  const ListItemState& state) = 0;
};


/** @brief Binds a list item to the current style renderer.

    A %ListBoxItem owns a %ListItemStyler. Applications do not
    construct one. Call %Styler::bind() from %onInvalidate(). When the
    overlay has no local options, bind uses the shared renderer from
    the style. Local options use a private clone. %setRenderer() keeps
    an assigned renderer until it is cleared. A null renderer falls
    back on the next bind.

    Appearance getters return effective tokens after bind. Setters
    write widget-local options. Measure, layout, and paint call the
    typed methods on this styler, not a public renderer accessor.

    @ingroup Pt-Forms-Collections
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

            A null renderer falls back to the current style on the next bind.
        */
        void setRenderer(ListItemRenderer* renderer = 0);

        /** @brief Returns the bound effective list item options.
        */
        StyleOptions& options();

        /** @brief Returns the bound effective list item options.
        */
        const StyleOptions& options() const;

    protected:
        /** @brief Binds the overlay to @a global and returns it.
        */
        virtual StyleOptions& onBindOptions(const StyleOptions& global);

        /** @brief Returns the shared list-item renderer from @a style, or 0.
        */
        virtual Renderer* onStyleRenderer(const Style& style);

        /** @brief Creates an independent clone of the style renderer, or 0.
        */
        virtual Renderer* onCreateRenderer(const Style& style);

    private:
        FacetPtr<ListItemRenderer> _renderer;
        StyleOptions               _options;
};

} // namespace

} // namespace

#endif
