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

#include <Pt/Forms/SliderStyler.h>
#include <Pt/Forms/StyleOptions.h>
#include <Pt/Forms/Style.h>

namespace Pt {

namespace Forms {

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
: Renderer( typeid(SliderRenderer), refs )
{
}


SliderRenderer::~SliderRenderer()
{
}


SliderRenderer* SliderRenderer::create() const
{
    return onCreate();
}


void SliderRenderer::prepare(const StyleOptions& options)
{
    onPrepare(options);
}


void SliderRenderer::onReset(const StyleOptions& options)
{
    prepare(options);
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


SliderStyler::SliderStyler()
{
}


const Gfx::Brush& SliderStyler::background() const
{
    return _options.get<BackgroundOption>().value();
}


void SliderStyler::setBackground(const Gfx::Brush& brush)
{
    _options.set( BackgroundOption(brush) );
}


const Gfx::Brush& SliderStyler::foreground() const
{
    return _options.get<ForegroundOption>().value();
}


void SliderStyler::setForeground(const Gfx::Brush& brush)
{
    _options.set( ForegroundOption(brush) );
}


const Gfx::Pen& SliderStyler::contour() const
{
    return _options.get<ContourOption>().value();
}


void SliderStyler::setContour(const Gfx::Pen& pen)
{
    _options.set( ContourOption(pen) );
}


const Gfx::Color& SliderStyler::textColor() const
{
    return _options.get<TextColorOption>().value();
}


void SliderStyler::setTextColor(const Gfx::Color& color)
{
    _options.set( TextColorOption(color) );
}


Gfx::Font SliderStyler::font() const
{
    return _options.get<FontOption>().value();
}


void SliderStyler::setFont(const Gfx::Font& font)
{
    FontOption option;
    option.setFont(font);
    _options.set(option);
}


void SliderStyler::setFontSize(std::size_t size)
{
    const FontOption* localFont = _options.findLocal<FontOption>();
    FontOption option = localFont ? *localFont : FontOption();
    option.setSize(size);
    _options.set(option);
}


void SliderStyler::setFontWeight(Gfx::Font::Weight weight)
{
    const FontOption* localFont = _options.findLocal<FontOption>();
    FontOption option = localFont ? *localFont : FontOption();
    option.setWeight(weight);
    _options.set(option);
}


void SliderStyler::setFontSlant(Gfx::Font::Slant slant)
{
    const FontOption* localFont = _options.findLocal<FontOption>();
    FontOption option = localFont ? *localFont : FontOption();
    option.setSlant(slant);
    _options.set(option);
}


Gfx::SizeF SliderStyler::measureFrame(PaintSurface& surface,
                                      const Gfx::SizeF& contentSize) const
{
    if( ! _renderer )
        return Gfx::SizeF();

    return _renderer->measureFrame(surface, contentSize);
}


Gfx::SizeF SliderStyler::measureTrack(PaintSurface& surface) const
{
    if( ! _renderer )
        return Gfx::SizeF();

    return _renderer->measureTrack(surface);
}


Gfx::SizeF SliderStyler::measureHandle(PaintSurface& surface) const
{
    if( ! _renderer )
        return Gfx::SizeF();

    return _renderer->measureHandle(surface);
}


void SliderStyler::layoutChrome(PaintSurface& surface,
                                const Gfx::RectF& rect,
                                const Gfx::SizeF& trackSize,
                                const Gfx::SizeF& handleSize,
                                Gfx::RectF& trackRect,
                                Gfx::RectF& handleRect) const
{
    if( ! _renderer )
    {
        trackRect = Gfx::RectF();
        handleRect = Gfx::RectF();
        return;
    }

    _renderer->layoutChrome(surface,
                            rect,
                            trackSize,
                            handleSize,
                            trackRect,
                            handleRect);
}


void SliderStyler::layoutHandle(PaintSurface& surface,
                                const Gfx::RectF& trackRect,
                                float fraction,
                                Gfx::RectF& handleRect) const
{
    if( ! _renderer )
    {
        handleRect = Gfx::RectF();
        return;
    }

    _renderer->layoutHandle(surface, trackRect, fraction, handleRect);
}


void SliderStyler::renderChrome(PaintContext& context,
                                const Gfx::RectF& rect,
                                const Gfx::RectF& trackRect,
                                const Gfx::RectF& handleRect,
                                const SliderState& state) const
{
    if( ! _renderer )
        return;

    _renderer->renderChrome(context, rect, trackRect, handleRect, state);
}


void SliderStyler::setRenderer(SliderRenderer* renderer)
{
    _renderer.reset(renderer);
    init(renderer);
}


StyleOptions& SliderStyler::options()
{
    return _options;
}


const StyleOptions& SliderStyler::options() const
{
    return _options;
}


StyleOptions& SliderStyler::onBindOptions(const StyleOptions& global)
{
    _options.bind(&global);
    return _options;
}


Renderer* SliderStyler::onStyleRenderer(const Style& style)
{
    SliderRenderer* styleRenderer = style.get<SliderRenderer>();
    _renderer.reset(styleRenderer);
    return _renderer.get();
}


Renderer* SliderStyler::onCreateRenderer(const Style& style)
{
    SliderRenderer* styleRenderer = style.get<SliderRenderer>();
    _renderer.reset( styleRenderer ? styleRenderer->create() : 0 );
    return _renderer.get();
}

} // namespace

} // namespace
