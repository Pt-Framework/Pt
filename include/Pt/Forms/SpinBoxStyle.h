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

#ifndef Pt_Forms_SpinBoxStyle_h
#define Pt_Forms_SpinBoxStyle_h

#include <Pt/Forms/Styler.h>

namespace Pt {

namespace Forms {

/** @brief Stores widget-local style overrides for a spin box.

    Carries only the optional local override tokens that are resolved during
    renderer preparation.
*/
class PT_FORMS_API SpinBoxStyleOptions
{
    public:
        SpinBoxStyleOptions();

        bool hasOverrides() const;

        std::size_t generation() const;

        const Gfx::Brush* background() const;

        void setBackground(const Gfx::Brush& brush);

        const Gfx::Pen* contour() const;

        void setContour(const Gfx::Pen& pen);

        const Gfx::Brush* foreground() const;

        void setForeground(const Gfx::Brush& brush);

        const Gfx::Color* textColor() const;

        void setTextColor(const Gfx::Color& color);

        const Gfx::Font* font() const;

        void setFont(const Gfx::Font& font);

        void setFontSize(std::size_t size);

        void setFontWeight(Gfx::Font::Weight weight);

        void setFontSlant(Gfx::Font::Slant slant);

        Gfx::Font getFont(const Gfx::Font& base) const;

    private:
        enum StyleOverride
        {
            Background = 0x01,
            Contour    = 0x02,
            Foreground = 0x04,
            TextColor  = 0x08,
            Font       = 0x10
        };

        bool hasOverride(StyleOverride mask) const;

        void setOverride(StyleOverride mask);

    private:
        AutoPtr<Gfx::Brush> _background;
        AutoPtr<Gfx::Pen>   _contour;
        AutoPtr<Gfx::Brush> _foreground;
        AutoPtr<Gfx::Color> _textColor;
        FontOption          _font;
        std::size_t         _generation;
        unsigned            _overrides;
};


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
class PT_FORMS_API SpinBoxRenderer : public Style::Facet
{
    public:
        explicit SpinBoxRenderer(std::size_t refs = 0);

        virtual ~SpinBoxRenderer();

        SpinBoxRenderer* create() const;

        void prepare(const StyleOptions& options,
                     const SpinBoxStyleOptions& spinBoxOptions);

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
        virtual void onReset(const StyleOptions& options);

        virtual SpinBoxRenderer* onCreate() const = 0;

        virtual void onPrepare(const StyleOptions& options,
                               const SpinBoxStyleOptions& spinBoxOptions) = 0;

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


/** @brief Binds a spin box widget to the currently active renderer.
*/
class PT_FORMS_API SpinBoxStyle : public Styler<SpinBoxRenderer,
                                                     SpinBoxStyleOptions>
{
    public:
        SpinBoxStyle();
};

} // namespace

} // namespace

#endif
