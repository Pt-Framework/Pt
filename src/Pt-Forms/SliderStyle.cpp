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
                             const StyleOptions& sliderOptions)
{
    onPrepare(options, sliderOptions);
}


void SliderRenderer::onReset(const StyleOptions& options)
{
    StyleOptions empty;
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
