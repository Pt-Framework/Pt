/*
  Copyright (C) 2016 Laurentiu-Gheorghe Crisan
  Copyright (C) 2016 Marc Boris Duerner

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
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the:
  Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
  Boston, MA 02110-1301 USA
*/

#ifndef Pt_Forms_ButtonStyle_h
#define Pt_Forms_ButtonStyle_h

#include <Pt/Forms/StylerBase.h>

namespace Pt {

namespace Forms {

/** @brief Stores the widget-local visual state for a push button.

    Carries only the transient interaction state that render and icon hooks
    may observe directly.
*/
class PT_FORMS_API ButtonState
{
    public:
        /** @brief Constructs an empty button state.
        */
        ButtonState();

        /** @brief Returns true if the button is currently enabled.
        */
        bool isEnabled() const;

        /** @brief Sets whether the button is enabled.
        */
        void setEnabled(bool value);

        /** @brief Returns true if the pointer is currently hovering the button.
        */
        bool isHovered() const;

        /** @brief Sets whether the pointer is currently hovering the button.
        */
        void setHovered(bool value);

        /** @brief Returns true if the button currently has focus.
        */
        bool isFocused() const;

        /** @brief Sets whether the button has focus.
        */
        void setFocused(bool value);

        /** @brief Returns true if the button is currently pressed.
        */
        bool isPressed() const;

        /** @brief Sets whether the button is pressed.
        */
        void setPressed(bool value);

        /** @brief Returns true if the button uses flat rendering.
        */
        bool isFlat() const;

        /** @brief Sets whether the button uses flat rendering.
        */
        void setFlat(bool value);

    private:
        bool _enabled;
        bool _hovered;
        bool _focused;
        bool _pressed;
        bool _flat;
};

/** @brief Renders the visual appearance of a push button.

    Provides rendering primitives for button surfaces, text, mnemonic
    underlines, and icons. Subclasses override the protected virtuals.
*/
class PT_FORMS_API ButtonRenderer : public Renderer
{
    public:
        explicit ButtonRenderer(std::size_t refs = 0);

        virtual ~ButtonRenderer();

        /** @brief Creates a new default-constructed instance that the caller owns.
        */
        ButtonRenderer* create() const;

        /** @brief Applies the button style options to this renderer.

            This is the explicit synchronization point for the button slice.
            The supplied options already include the widget-local overlay via
            the parent lookup of %StyleOptions. Implementations resolve tokens
            with %StyleOptions::get().
        */
        void prepare(const StyleOptions& options);

    public:
        /** @brief Returns the combined content size for icon and text arranged by direction.

            Computes the space needed for icon, text, and spacing between them.
            Does not include surface decoration (borders/padding).
        */
        Gfx::SizeF measureContent(PaintSurface& surface,
                                  Direction direction,
                                  const Gfx::SizeF& iconSize,
                                  const Gfx::SizeF& textSize);

        /** @brief Returns the outer size including button decoration for contentSize on surface.
        */
        Gfx::SizeF measureFrame(PaintSurface& surface,
                                const Gfx::SizeF& contentSize);

        /** @brief Returns the content rectangle within the outer frameRect.
        */
        Gfx::RectF layoutFrame(PaintSurface& surface,
                               const Gfx::RectF& frameRect);

        /** @brief Computes the underline rectangle for the mnemonic character at mnemonicIndex.
        */
        Gfx::RectF layoutMnemonic(PaintSurface& surface,
                                  const String& text,
                                  const Gfx::PointF& textPos,
                                  const Gfx::FontMetrics& fontMetrics,
                                  String::size_type mnemonicIndex);

        /** @brief Returns a painter with the current font and text color applied for surface.
        */
        const Painter& textPainter(PaintSurface& surface);

        /** @brief Computes the icon and text sub-rectangles within the content rect.

            The content rect is the output of layoutFrame (decoration already
            stripped). This method partitions it based on direction and sizes.
        */
        void layoutContent(PaintSurface& surface,
                           const Gfx::RectF& contentRect,
                           Direction direction,
                           const Gfx::SizeF& iconSize,
                           const Gfx::SizeF& textSize,
                           Gfx::RectF& iconRect,
                           Gfx::RectF& textRect);

        /** @brief Converts icon to a state-appropriate pixmap written to picture.
        */
        void prepareIcon(const Gfx::Image& icon,
                         Pixmap& picture,
                         const ButtonState& state) const;

        /** @brief Paints the background fill within the widget rect.

            Only fills if an explicit background is needed.
            Otherwise the call is a no-op, leaving the widget transparent.
        */
        void renderBackground(PaintContext& context,
                              const Gfx::RectF& rect,
                              const ButtonState& state);

        /** @brief Paints the button background and border within the button bounding rect.
        */
        void renderChrome(PaintContext& context,
                          const Gfx::RectF& rect,
                          const ButtonState& state);

        /** @brief Draws text at pos, clipped to the button bounding rect.
        */
        void renderText(PaintContext& context,
                        const Gfx::RectF& rect,
                        const String& text,
                        const Gfx::PointF& pos,
                        const ButtonState& state);

        /** @brief Draws the mnemonic underline within mnemonic, clipped to the button bounding rect.
        */
        void renderMnemonic(PaintContext& context,
                            const Gfx::RectF& rect,
                            const Gfx::RectF& mnemonic,
                            const ButtonState& state);

        /** @brief Draws picture at pos, clipped to the button bounding rect.
        */
        void renderIcon(PaintContext& context,
                        const Gfx::RectF& rect,
                        const Pixmap& picture,
                        const Gfx::PointF& pos,
                        const ButtonState& state);

    protected:
        /** @brief Resets the shared button renderer to global defaults.
        */
        virtual void onReset(const StyleOptions& options);

        virtual ButtonRenderer* onCreate() const = 0;

        virtual void onPrepare(const StyleOptions& options) = 0;

        virtual Gfx::SizeF onMeasureFrame(PaintSurface& surface,
                                          const Gfx::SizeF& contentSize) = 0;

        virtual Gfx::SizeF onMeasureContent(PaintSurface& surface,
                                            Direction direction,
                                            const Gfx::SizeF& iconSize,
                                            const Gfx::SizeF& textSize) = 0;

        virtual Gfx::RectF onLayoutFrame(PaintSurface& surface,
                                         const Gfx::RectF& frameRect) = 0;

        virtual Gfx::RectF onLayoutMnemonic(PaintSurface& surface,
                                            const String& text,
                                            const Gfx::PointF& textPos,
                                            const Gfx::FontMetrics& fontMetrics,
                                            String::size_type mnemonicIndex) = 0;

        virtual const Painter& onGetTextPainter(PaintSurface& surface) = 0;

        virtual void onLayoutContent(PaintSurface& surface,
                                     const Gfx::RectF& contentRect,
                                     Direction direction,
                                     const Gfx::SizeF& iconSize,
                                     const Gfx::SizeF& textSize,
                                     Gfx::RectF& iconRect,
                                     Gfx::RectF& textRect) = 0;

        virtual void onRenderBackground(PaintContext& context,
                                        const Gfx::RectF& rect,
                                        const ButtonState& state) = 0;

        virtual void onPrepareIcon(const Gfx::Image& icon,
                                   Pixmap& picture,
                                   const ButtonState& state) const = 0;

        virtual void onRenderChrome(PaintContext& context,
                                    const Gfx::RectF& rect,
                                    const ButtonState& state) = 0;

        virtual void onRenderText(PaintContext& context,
                                  const Gfx::RectF& rect,
                                  const String& text,
                                  const Gfx::PointF& pos,
                                  const ButtonState& state) = 0;

        virtual void onRenderMnemonic(PaintContext& context,
                                      const Gfx::RectF& rect,
                                      const Gfx::RectF& mnemonic,
                                      const ButtonState& state) = 0;

        virtual void onRenderIcon(PaintContext& context,
                                  const Gfx::RectF& rect,
                                  const Pixmap& picture,
                                  const Gfx::PointF& pos,
                                  const ButtonState& state) = 0;
};

/** @brief Button styler.
*/
class PT_FORMS_API ButtonStyler : public StylerBase
{
    public:
        /** @brief Constructs an unbound button styler.
        */
        ButtonStyler();

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

        /** @brief Returns the effective accent color.
        */
        const Gfx::Color& accentColor() const;

        /** @brief Sets the widget-local accent color to @a color.
        */
        void setAccentColor(const Gfx::Color& color);

        /** @brief Returns the effective highlight color.
        */
        const Gfx::Color& highlightColor() const;

        /** @brief Sets the widget-local highlight color to @a color.
        */
        void setHighlightColor(const Gfx::Color& color);

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

        /** @brief Prepares @a picture from @a icon for @a state.

            Returns true when a bound renderer prepared @a picture.
        */
        bool prepareIcon(const Gfx::Image& icon,
                         Pixmap& picture,
                         const ButtonState& state) const;

        /** @brief Measures @a text and stores its metrics for @a surface.
        */
        void measureText(PaintSurface& surface,
                         const String& text,
                         Gfx::TextMetrics& textMetrics,
                         Gfx::FontMetrics& fontMetrics) const;

        /** @brief Measures icon and text content arranged by @a direction.
        */
        Gfx::SizeF measureContent(PaintSurface& surface,
                                  Direction direction,
                                  const Gfx::SizeF& iconSize,
                                  const Gfx::SizeF& textSize) const;

        /** @brief Measures the frame enclosing @a contentSize.
        */
        Gfx::SizeF measureFrame(PaintSurface& surface,
                                const Gfx::SizeF& contentSize) const;

        /** @brief Returns the frame content rectangle within @a frameRect.
        */
        Gfx::RectF layoutFrame(PaintSurface& surface,
                               const Gfx::RectF& frameRect) const;

        /** @brief Lays out icon and text rectangles within @a contentRect.
        */
        void layoutContent(PaintSurface& surface,
                           const Gfx::RectF& contentRect,
                           Direction direction,
                           const Gfx::SizeF& iconSize,
                           const Gfx::SizeF& textSize,
                           Gfx::RectF& iconRect,
                           Gfx::RectF& textRect) const;

        /** @brief Returns the mnemonic underline rectangle for @a mnemonicIndex.
        */
        Gfx::RectF layoutMnemonic(PaintSurface& surface,
                                  const String& text,
                                  const Gfx::PointF& textPos,
                                  const Gfx::FontMetrics& fontMetrics,
                                  String::size_type mnemonicIndex) const;

        /** @brief Renders the button background within @a rect for @a state.
        */
        void renderBackground(PaintContext& context,
                              const Gfx::RectF& rect,
                              const ButtonState& state) const;

        /** @brief Renders the button chrome within @a rect for @a state.
        */
        void renderChrome(PaintContext& context,
                          const Gfx::RectF& rect,
                          const ButtonState& state) const;

        /** @brief Renders @a text at @a pos within @a rect for @a state.
        */
        void renderText(PaintContext& context,
                        const Gfx::RectF& rect,
                        const String& text,
                        const Gfx::PointF& pos,
                        const ButtonState& state) const;

        /** @brief Renders the mnemonic underline within @a mnemonic for @a state.
        */
        void renderMnemonic(PaintContext& context,
                            const Gfx::RectF& rect,
                            const Gfx::RectF& mnemonic,
                            const ButtonState& state) const;

        /** @brief Renders @a picture at @a pos within @a rect for @a state.
        */
        void renderIcon(PaintContext& context,
                        const Gfx::RectF& rect,
                        const Pixmap& picture,
                        const Gfx::PointF& pos,
                        const ButtonState& state) const;

        /** @brief Assigns a specific button renderer.
        */
        void setRenderer(ButtonRenderer* renderer = 0);

        /** @brief Returns the bound effective button options.

            Use %StyleOptions::findLocal() to query an explicit widget-local token.
        */
        StyleOptions& options();

        /** @brief Returns the bound effective button options.
        */
        const StyleOptions& options() const;

    protected:
        virtual StyleOptions& onBindOptions(const StyleOptions& styleOptions);

        virtual Renderer* onStyleRenderer(const Style& style);

        virtual Renderer* onCreateRenderer(const Style& style);

    private:
        FacetPtr<ButtonRenderer> _renderer;
        StyleOptions             _options;
};

} // namespace

} // namespace

#endif
