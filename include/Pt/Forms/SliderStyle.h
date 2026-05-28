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

#ifndef PT_FORMS_SLIDERSTYLE_H
#define PT_FORMS_SLIDERSTYLE_H

#include <Pt/Forms/Style.h>

namespace Pt {

namespace Forms {

class PT_FORMS_API SliderStyleOptions
{
    public:
        SliderStyleOptions();

        bool hasOverrides() const;

        std::size_t generation() const;

        const Gfx::Brush* background() const;

        void setBackground(const Gfx::Brush& brush);

        const Gfx::Pen* contour() const;

        void setContour(const Gfx::Pen& pen);

        const Gfx::Color* foreground() const;

        void setForeground(const Gfx::Color& color);

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
        AutoPtr<Gfx::Color> _foreground;
        AutoPtr<Gfx::Color> _textColor;
        FontOption          _font;
        std::size_t         _generation;
        unsigned            _overrides;
};


class PT_FORMS_API SliderState
{
    public:
        SliderState();

        bool isEnabled() const;

        void setEnabled(bool value);

        bool isHovered() const;

        void setHovered(bool value);

        bool isFocused() const;

        void setFocused(bool value);

    private:
        bool _enabled;
        bool _hovered;
        bool _focused;
};


class PT_FORMS_API SliderRenderer : public Style::Facet
{
    public:
        explicit SliderRenderer(std::size_t refs = 0);

        virtual ~SliderRenderer();

        SliderRenderer* create() const;

        void prepare(const StyleOptions& options,
                     const SliderStyleOptions& sliderOptions);

    public:
        Gfx::SizeF measureFrame(PaintSurface& surface,
                                const Gfx::SizeF& contentSize);

        Gfx::SizeF measureTrack(PaintSurface& surface);

        Gfx::SizeF measureHandle(PaintSurface& surface);

        void layoutChrome(PaintSurface& surface,
                          const Gfx::RectF& rect,
                          const Gfx::SizeF& trackSize,
                          const Gfx::SizeF& handleSize,
                          Gfx::RectF& trackRect,
                          Gfx::RectF& handleRect);

        void layoutHandle(PaintSurface& surface,
                          const Gfx::RectF& trackRect,
                          float fraction,
                          Gfx::RectF& handleRect);

        void renderChrome(PaintContext& context,
                          const Gfx::RectF& rect,
                          const Gfx::RectF& trackRect,
                          const Gfx::RectF& handleRect,
                          const SliderState& state);

        void renderTrack(PaintContext& context,
                         const Gfx::RectF& trackRect,
                         const SliderState& state);

        void renderHandle(PaintContext& context,
                          const Gfx::RectF& handleRect,
                          const SliderState& state);

    protected:
        virtual void onReset(const StyleOptions& options);

        virtual SliderRenderer* onCreate() const = 0;

        virtual void onPrepare(const StyleOptions& options,
                               const SliderStyleOptions& sliderOptions) = 0;

        virtual Gfx::SizeF onMeasureFrame(PaintSurface& surface,
                                          const Gfx::SizeF& contentSize) = 0;

        virtual Gfx::SizeF onMeasureTrack(PaintSurface& surface) = 0;

        virtual Gfx::SizeF onMeasureHandle(PaintSurface& surface) = 0;

        virtual void onLayoutChrome(PaintSurface& surface,
                                    const Gfx::RectF& rect,
                                    const Gfx::SizeF& trackSize,
                                    const Gfx::SizeF& handleSize,
                                    Gfx::RectF& trackRect,
                                    Gfx::RectF& handleRect) = 0;

        virtual void onLayoutHandle(PaintSurface& surface,
                                    const Gfx::RectF& trackRect,
                                    float fraction,
                                    Gfx::RectF& handleRect) = 0;

        virtual void onRenderChrome(PaintContext& context,
                                    const Gfx::RectF& rect,
                                    const Gfx::RectF& trackRect,
                                    const Gfx::RectF& handleRect,
                                    const SliderState& state);

        virtual void onRenderTrack(PaintContext& context,
                                   const Gfx::RectF& trackRect,
                                   const SliderState& state) = 0;

        virtual void onRenderHandle(PaintContext& context,
                                    const Gfx::RectF& handleRect,
                                    const SliderState& state) = 0;
};


class PT_FORMS_API SliderStyle : public StyleBinder<SliderRenderer,
                                                    SliderStyleOptions>
{
    public:
        SliderStyle();
};

} // namespace

} // namespace

#endif
