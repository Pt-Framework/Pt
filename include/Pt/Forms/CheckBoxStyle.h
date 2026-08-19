/* Copyright (C) 2016 Laurentiu-Gheorghe Crisan
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
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.
 
 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, 
 MA 02110-1301 USA
*/

#ifndef Pt_Forms_CheckBoxStyle_h
#define Pt_Forms_CheckBoxStyle_h

#include <Pt/Forms/Styler.h>

namespace Pt {

namespace Forms {

/** @brief Stores widget-local style overrides for a check box.

    Carries only the optional local override tokens that are resolved during
    renderer preparation.
*/
class PT_FORMS_API CheckBoxStyleOptions
{
    public:
        /** @brief Constructs empty local check box style options.
        */
        CheckBoxStyleOptions();

        /** @brief Returns true if any local style override is present.
        */
        bool hasOverrides() const;

        /** @brief Returns the current local override generation.
        */
        std::size_t generation() const;

        /** @brief Returns the local background override or 0 if none is set.
        */
        const Gfx::Brush* background() const;

        /** @brief Sets the local background override.
        */
        void setBackground(const Gfx::Brush& brush);

        /** @brief Returns the local contour override or 0 if none is set.
        */
        const Gfx::Pen* contour() const;

        /** @brief Sets the local contour override.
        */
        void setContour(const Gfx::Pen& pen);

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

        /** @brief Resolves the effective font against the given base font.
        */
        Gfx::Font getFont(const Gfx::Font& base) const;

    private:
        enum StyleOverride
        {
            Background = 0x01,
            Contour    = 0x02,
            TextColor  = 0x04,
            Font       = 0x08
        };

        bool hasOverride(StyleOverride mask) const;

        void setOverride(StyleOverride mask);

    private:
        AutoPtr<Gfx::Brush> _background;
        AutoPtr<Gfx::Pen>   _contour;
        AutoPtr<Gfx::Color> _textColor;
        FontOption          _font;
        std::size_t         _generation;
        unsigned            _overrides;
};

/** @brief Stores the transient render state for a check box widget.

    Carries only widget state that render hooks may observe directly.
*/
class PT_FORMS_API CheckBoxState
{
    public:
        /** @brief Constructs an empty check box state.
        */
        CheckBoxState();

        /** @brief Returns true if the widget is currently enabled.
        */
        bool isEnabled() const;

        /** @brief Sets whether the widget is enabled.
        */
        void setEnabled(bool value);

        /** @brief Returns true if the pointer is currently hovering the widget.
        */
        bool isHovered() const;

        /** @brief Sets whether the pointer is hovering the widget.
        */
        void setHovered(bool value);

        /** @brief Returns true if the widget currently has focus.
        */
        bool isFocused() const;

        /** @brief Sets whether the widget has focus.
        */
        void setFocused(bool value);

        /** @brief Returns true if the check box is currently checked.
        */
        bool isChecked() const;

        /** @brief Sets whether the check box is checked.
        */
        void setChecked(bool value);

    private:
        bool _enabled;
        bool _hovered;
        bool _focused;
        bool _checked;
};

/** @brief Renders the visual appearance of a check box widget.

    Provides rendering primitives for the check indicator, label text,
    and mnemonic underline. Subclasses override the protected virtuals.
*/
class PT_FORMS_API CheckBoxRenderer : public Style::Facet
{
    public:
        explicit CheckBoxRenderer(std::size_t refs = 0);

        virtual ~CheckBoxRenderer();

        /** @brief Creates a new default-constructed instance that the caller owns.
        */
        CheckBoxRenderer* create() const;

        /** @brief Applies the widget-local check box style overrides to this renderer.

            This is the explicit synchronization point for the check box slice.
        */
        void prepare(const StyleOptions& options,
                     const CheckBoxStyleOptions& checkBoxOptions);

    public:
        /** @brief Returns the natural size of the check indicator on surface.
        */
        Gfx::SizeF measureIndicator(PaintSurface& surface);

        /** @brief Returns the combined content size for indicator and text.
        */
        Gfx::SizeF measureContent(PaintSurface& surface,
                                  const Gfx::SizeF& indicatorSize,
                                  const Gfx::SizeF& textSize);

        /** @brief Returns the outer size including frame for the given content size.
        */
        Gfx::SizeF measureFrame(PaintSurface& surface,
                                const Gfx::SizeF& contentSize);

        /** @brief Returns the content rectangle within the outer frame rect.
        */
        Gfx::RectF layoutFrame(PaintSurface& surface,
                               const Gfx::RectF& frameRect);

        /** @brief Partitions the content rect into indicator and text sub-rectangles.
        */
        void layoutContent(PaintSurface& surface,
                           const Gfx::RectF& contentRect,
                           const Gfx::SizeF& indicatorSize,
                           const Gfx::SizeF& textSize,
                           Gfx::RectF& indicatorRect,
                           Gfx::RectF& textRect);

        /** @brief Computes the underline rectangle for the mnemonic character.
        */
        Gfx::RectF layoutMnemonic(PaintSurface& surface,
                                  const String& text,
                                  const Gfx::PointF& textPos,
                                  const Gfx::FontMetrics& fontMetrics,
                                  String::size_type mnemonicIndex);

        /** @brief Returns a painter with the current font and text color applied.
        */
        const Painter& textPainter(PaintSurface& surface);

        /** @brief Draws the check indicator chrome within the widget rect.
        */
        void renderChrome(PaintContext& context,
                          const Gfx::RectF& rect,
                          const Gfx::RectF& boxRect,
                          const CheckBoxState& state);

        /** @brief Draws text at pos, clipped to the text rect.
        */
        void renderText(PaintContext& context,
                        const Gfx::RectF& textRect,
                        const String& text,
                        const Gfx::PointF& pos,
                        const CheckBoxState& state);

        /** @brief Draws the mnemonic underline within mnemonic, clipped to the widget rect.
        */
        void renderMnemonic(PaintContext& context,
                            const Gfx::RectF& rect,
                            const Gfx::RectF& mnemonic,
                            const CheckBoxState& state);

    protected:
        /** @brief Resets the shared check box renderer to global defaults.
        */
        virtual void onReset(const StyleOptions& options);

        virtual CheckBoxRenderer* onCreate() const = 0;

        virtual void onPrepare(const StyleOptions& options,
                               const CheckBoxStyleOptions& checkBoxOptions) = 0;

        virtual Gfx::SizeF onMeasureIndicator(PaintSurface& surface) = 0;

        virtual Gfx::SizeF onMeasureContent(PaintSurface& surface,
                                            const Gfx::SizeF& indicatorSize,
                                            const Gfx::SizeF& textSize) = 0;

        virtual Gfx::SizeF onMeasureFrame(PaintSurface& surface,
                                          const Gfx::SizeF& contentSize) = 0;

        virtual Gfx::RectF onLayoutFrame(PaintSurface& surface,
                                         const Gfx::RectF& frameRect) = 0;

        virtual void onLayoutContent(PaintSurface& surface,
                                     const Gfx::RectF& contentRect,
                                     const Gfx::SizeF& indicatorSize,
                                     const Gfx::SizeF& textSize,
                                     Gfx::RectF& indicatorRect,
                                     Gfx::RectF& textRect) = 0;

        virtual Gfx::RectF onLayoutMnemonic(PaintSurface& surface,
                                            const String& text,
                                            const Gfx::PointF& textPos,
                                            const Gfx::FontMetrics& fontMetrics,
                                            String::size_type mnemonicIndex) = 0;

        virtual const Painter& onGetTextPainter(PaintSurface& surface) = 0;

        virtual void onRenderChrome(PaintContext& context,
                                    const Gfx::RectF& rect,
                                    const Gfx::RectF& boxRect,
                                    const CheckBoxState& state) = 0;

        virtual void onRenderText(PaintContext& context,
                                  const Gfx::RectF& textRect,
                                  const String& text,
                                  const Gfx::PointF& pos,
                                  const CheckBoxState& state) = 0;

        virtual void onRenderMnemonic(PaintContext& context,
                                      const Gfx::RectF& rect,
                                      const Gfx::RectF& mnemonic,
                                      const CheckBoxState& state) = 0;
};

/** @brief Binds a check box widget to the currently active renderer.

    Keeps the active renderer binding for the shared style renderer, a private
    override clone, or an explicitly assigned custom renderer.
*/
class PT_FORMS_API CheckBoxStyle : public Styler<CheckBoxRenderer,
                                                      CheckBoxStyleOptions>
{
    public:
        /** @brief Constructs an unbound check box style controller.
        */
        CheckBoxStyle();
};

} // namespace

} // namespace

#endif
