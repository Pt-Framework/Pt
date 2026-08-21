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

/** @brief Stores the widget-local style overrides for a push button.

    Carries optional local tokens that are resolved during the prepare step.
    Render and icon hooks never receive these overrides directly.
*/
class PT_FORMS_API ButtonStyleOptions : public StyleOptionsBase
{
    public:
        /** @brief Constructs empty local button style options.
        */
        ButtonStyleOptions();

        /** @brief Returns the local foreground override or 0 if none is set.
        */
        const Gfx::Brush* foreground() const;

        /** @brief Sets the local foreground override.
        */
        void setForeground(const Gfx::Brush& brush);

        /** @brief Returns the local contour override or 0 if none is set.
        */
        const Gfx::Pen* contour() const;

        /** @brief Sets the local contour override.
        */
        void setContour(const Gfx::Pen& pen);

        /** @brief Returns the local accent color override or 0 if none is
            set.
        */
        const Gfx::Color* accentColor() const;

        /** @brief Sets the local accent color override.
        */
        void setAccentColor(const Gfx::Color& color);

        /** @brief Returns the local highlight color override or 0 if none is
            set.
        */
        const Gfx::Color* highlightColor() const;

        /** @brief Sets the local highlight color override.
        */
        void setHighlightColor(const Gfx::Color& color);

        /** @brief Returns the local text color override or 0 if none is set.
        */
        const Gfx::Color* textColor() const;

        /** @brief Sets the local text color override.
        */
        void setTextColor(const Gfx::Color& color);

        /** @brief Returns the local font override data or 0 if none is set.
        */
        const Gfx::Font* font() const;

        /** @brief Sets the complete local font override.
        */
        void setFont(const Gfx::Font& font);

        /** @brief Sets the local font size override.
        */
        void setFontSize(std::size_t size);

        /** @brief Sets the local font weight override.
        */
        void setFontWeight(Gfx::Font::Weight weight);

        /** @brief Sets the local font slant override.
        */
        void setFontSlant(Gfx::Font::Slant slant);

        /** @brief Resolves the effective font against the given style default.
        */
        Gfx::Font getFont(const Gfx::Font& base) const;

    private:
        enum StyleOverride
        {
            Foreground     = 0x01,
            Contour        = 0x02,
            AccentColor    = 0x04,
            HighlightColor = 0x08,
            TextColor      = 0x10,
            Font           = 0x20
        };

    private:
        AutoPtr<Gfx::Brush> _foreground;
        AutoPtr<Gfx::Pen>   _contour;
        AutoPtr<Gfx::Color> _accentColor;
        AutoPtr<Gfx::Color> _highlightColor;
        AutoPtr<Gfx::Color> _textColor;
        FontOption          _font;
};

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

        /** @brief Applies the widget-local button style overrides to this renderer.

            This is the explicit synchronization point for the button slice.
            Implementations prepare all renderer-local resources from the
            supplied %StyleOptions and %ButtonStyleOptions.
        */
        void prepare(const StyleOptions& options,
                     const ButtonStyleOptions& buttonOptions);

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

        virtual void onPrepare(const StyleOptions& options,
                               const ButtonStyleOptions& buttonOptions) = 0;

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

/** @brief Binds a push button to the currently active renderer.

    Owns the widget-local button overrides and caches a typed renderer
    pointer for the three supported button cases: shared style renderer,
    private override clone, and externally assigned custom renderer.
    Custom-renderer ownership and bind-case selection live on %StylerBase.
    %onStyleRenderer writes the typed pointer at each creation site.
*/
class PT_FORMS_API ButtonStyler : public StylerBase
{
    public:
        /** @brief Constructs an unbound button style controller.
        */
        ButtonStyler();

        /** @brief Assigns an externally owned custom renderer.

            Stores the custom source and marks it dirty so the next %bind
            call reacquires it. A null renderer falls back to the current
            style. Binding and prepare happen in %bind.
        */
        void setRenderer(ButtonRenderer* renderer);

        /** @brief Returns the currently bound button renderer or 0.
        */
        ButtonRenderer* renderer();

        /** @brief Returns the currently bound button renderer or 0.
        */
        const ButtonRenderer* renderer() const;

        /** @brief Returns the widget-local button style options.
        */
        ButtonStyleOptions& options();

        /** @brief Returns the widget-local button style options.
        */
        const ButtonStyleOptions& options() const;

    protected:
        virtual const StyleOptionsBase& onLocalOptions() const;

        virtual Renderer* onStyleRenderer(const Style& style);

        virtual Renderer* onCreateRenderer(const Style& style);

        virtual void onBindOptions(const StyleOptions& options);

    private:
        ButtonStyleOptions _options;
        ButtonRenderer*    _renderer;
};

} // namespace

} // namespace

#endif
