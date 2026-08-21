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

#include <Pt/Forms/SliderStyle.h>
#include <Pt/Forms/StyleOptions.h>

namespace Pt {

namespace Forms {

SliderStyleOptions::SliderStyleOptions()
{
}


const Gfx::Brush* SliderStyleOptions::background() const
{
    return _background.get();
}


void SliderStyleOptions::setBackground(const Gfx::Brush& brush)
{
    _background.reset( new Gfx::Brush(brush) );
    setOverride(Background);
}


const Gfx::Pen* SliderStyleOptions::contour() const
{
    return _contour.get();
}


void SliderStyleOptions::setContour(const Gfx::Pen& pen)
{
    _contour.reset( new Gfx::Pen(pen) );
    setOverride(Contour);
}


const Gfx::Color* SliderStyleOptions::foreground() const
{
    return _foreground.get();
}


void SliderStyleOptions::setForeground(const Gfx::Color& color)
{
    _foreground.reset( new Gfx::Color(color) );
    setOverride(Foreground);
}


const Gfx::Color* SliderStyleOptions::textColor() const
{
    return _textColor.get();
}


void SliderStyleOptions::setTextColor(const Gfx::Color& color)
{
    _textColor.reset( new Gfx::Color(color) );
    setOverride(TextColor);
}


const Gfx::Font* SliderStyleOptions::font() const
{
    return _font.font();
}


void SliderStyleOptions::setFont(const Gfx::Font& font)
{
    _font.setFont(font);
    setOverride(Font);
}


void SliderStyleOptions::setFontSize(std::size_t size)
{
    _font.setSize(size);
    setOverride(Font);
}


void SliderStyleOptions::setFontWeight(Gfx::Font::Weight weight)
{
    _font.setWeight(weight);
    setOverride(Font);
}


void SliderStyleOptions::setFontSlant(Gfx::Font::Slant slant)
{
    _font.setSlant(slant);
    setOverride(Font);
}


Gfx::Font SliderStyleOptions::getFont(const Gfx::Font& base) const
{
    return _font.getFont(base);
}


SliderState::SliderState()
: _enabled(false)
, _hovered(false)
, _focused(false)
{
}


bool SliderState::isEnabled() const
{
    return _enabled;
}


void SliderState::setEnabled(bool value)
{
    _enabled = value;
}


bool SliderState::isHovered() const
{
    return _hovered;
}


void SliderState::setHovered(bool value)
{
    _hovered = value;
}


bool SliderState::isFocused() const
{
    return _focused;
}


void SliderState::setFocused(bool value)
{
    _focused = value;
}


SliderRenderer::SliderRenderer(std::size_t refs)
: Style::Facet( typeid(SliderRenderer), refs )
{
}


SliderRenderer::~SliderRenderer()
{
}


SliderRenderer* SliderRenderer::create() const
{
    return onCreate();
}


void SliderRenderer::prepare(const StyleOptions& options,
                             const SliderStyleOptions& sliderOptions)
{
    onPrepare(options, sliderOptions);
}


void SliderRenderer::onReset(const StyleOptions& options)
{
    SliderStyleOptions empty;
    onPrepare(options, empty);
}


Gfx::SizeF SliderRenderer::measureFrame(PaintSurface& surface,
                                         const Gfx::SizeF& contentSize)
{
    return onMeasureFrame(surface, contentSize);
}


Gfx::SizeF SliderRenderer::measureTrack(PaintSurface& surface)
{
    return onMeasureTrack(surface);
}


Gfx::SizeF SliderRenderer::measureHandle(PaintSurface& surface)
{
    return onMeasureHandle(surface);
}


void SliderRenderer::layoutChrome(PaintSurface& surface,
                                  const Gfx::RectF& rect,
                                  const Gfx::SizeF& trackSize,
                                  const Gfx::SizeF& handleSize,
                                  Gfx::RectF& trackRect,
                                  Gfx::RectF& handleRect)
{
    onLayoutChrome(surface, rect, trackSize, handleSize, trackRect, handleRect);
}


void SliderRenderer::layoutHandle(PaintSurface& surface,
                                  const Gfx::RectF& trackRect,
                                  float fraction,
                                  Gfx::RectF& handleRect)
{
    onLayoutHandle(surface, trackRect, fraction, handleRect);
}


void SliderRenderer::renderChrome(PaintContext& context,
                                  const Gfx::RectF& rect,
                                  const Gfx::RectF& trackRect,
                                  const Gfx::RectF& handleRect,
                                  const SliderState& state)
{
    onRenderChrome(context, rect, trackRect, handleRect, state);
}


void SliderRenderer::renderTrack(PaintContext& context,
                                 const Gfx::RectF& trackRect,
                                 const SliderState& state)
{
    onRenderTrack(context, trackRect, state);
}


void SliderRenderer::renderHandle(PaintContext& context,
                                  const Gfx::RectF& handleRect,
                                  const SliderState& state)
{
    onRenderHandle(context, handleRect, state);
}


void SliderRenderer::onRenderChrome(PaintContext& context,
                                    const Gfx::RectF& /*rect*/,
                                    const Gfx::RectF& trackRect,
                                    const Gfx::RectF& handleRect,
                                    const SliderState& state)
{
    onRenderTrack(context, trackRect, state);
    onRenderHandle(context, handleRect, state);
}


SliderStyle::SliderStyle()
{
}

} // namespace

} // namespace
