/* Copyright (C) 2016 Marc Boris Duerner

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

#include <Pt/Forms/ScrollBarStyler.h>
#include <Pt/Forms/StyleOptions.h>
#include <Pt/Forms/Style.h>

namespace Pt {

namespace Forms {

ScrollBarState::ScrollBarState()
: _enabled(false)
, _focused(false)
, _handleHovered(false)
, _handlePressed(false)
, _decreaseHovered(false)
, _decreasePressed(false)
, _increaseHovered(false)
, _increasePressed(false)
{
}


bool ScrollBarState::isEnabled() const
{
    return _enabled;
}


void ScrollBarState::setEnabled(bool value)
{
    _enabled = value;
}


bool ScrollBarState::isFocused() const
{
    return _focused;
}


void ScrollBarState::setFocused(bool value)
{
    _focused = value;
}


bool ScrollBarState::isHandleHovered() const
{
    return _handleHovered;
}


void ScrollBarState::setHandleHovered(bool value)
{
    _handleHovered = value;
}


bool ScrollBarState::isHandlePressed() const
{
    return _handlePressed;
}


void ScrollBarState::setHandlePressed(bool value)
{
    _handlePressed = value;
}


bool ScrollBarState::isDecreaseHovered() const
{
    return _decreaseHovered;
}


void ScrollBarState::setDecreaseHovered(bool value)
{
    _decreaseHovered = value;
}


bool ScrollBarState::isDecreasePressed() const
{
    return _decreasePressed;
}


void ScrollBarState::setDecreasePressed(bool value)
{
    _decreasePressed = value;
}


bool ScrollBarState::isIncreaseHovered() const
{
    return _increaseHovered;
}


void ScrollBarState::setIncreaseHovered(bool value)
{
    _increaseHovered = value;
}


bool ScrollBarState::isIncreasePressed() const
{
    return _increasePressed;
}


void ScrollBarState::setIncreasePressed(bool value)
{
    _increasePressed = value;
}


ScrollBarRenderer::ScrollBarRenderer(std::size_t refs)
: Renderer( typeid(ScrollBarRenderer), refs )
{
}


ScrollBarRenderer::~ScrollBarRenderer()
{
}


ScrollBarRenderer* ScrollBarRenderer::create() const
{
    return onCreate();
}


void ScrollBarRenderer::prepare(const StyleOptions& options)
{
    onPrepare(options);
}


void ScrollBarRenderer::onReset(const StyleOptions& options)
{
    prepare(options);
}


Gfx::SizeF ScrollBarRenderer::measureFrame(PaintSurface& surface,
                                            const Gfx::SizeF& contentSize,
                                            Direction direction)
{
    return onMeasureFrame(surface, contentSize, direction);
}


Gfx::SizeF ScrollBarRenderer::measureTrack(PaintSurface& surface,
                                            Direction direction)
{
    return onMeasureTrack(surface, direction);
}


Gfx::SizeF ScrollBarRenderer::measureHandle(PaintSurface& surface,
                                             Direction direction)
{
    return onMeasureHandle(surface, direction);
}


Gfx::SizeF ScrollBarRenderer::measureButton(PaintSurface& surface,
                                             Direction direction)
{
    return onMeasureButton(surface, direction);
}


void ScrollBarRenderer::layoutChrome(PaintSurface& surface,
                                     const Gfx::RectF& rect,
                                     Direction direction,
                                     const Gfx::SizeF& buttonSize,
                                     Gfx::RectF& trackRect,
                                     Gfx::RectF& decreaseRect,
                                     Gfx::RectF& increaseRect)
{
    onLayoutChrome(surface, rect, direction, buttonSize,
                   trackRect, decreaseRect, increaseRect);
}


void ScrollBarRenderer::layoutHandle(PaintSurface& surface,
                                     const Gfx::RectF& trackRect,
                                     Direction direction,
                                     float fraction,
                                     float viewProportion,
                                     Gfx::RectF& handleRect)
{
    onLayoutHandle(surface, trackRect, direction, fraction,
                   viewProportion, handleRect);
}


void ScrollBarRenderer::renderChrome(PaintContext& context,
                                     const Gfx::RectF& rect,
                                     Direction direction,
                                     const Gfx::RectF& trackRect,
                                     const Gfx::RectF& handleRect,
                                     const Gfx::RectF& decreaseRect,
                                     const Gfx::RectF& increaseRect,
                                     const ScrollBarState& state)
{
    onRenderChrome(context, rect, direction, trackRect, handleRect,
                   decreaseRect, increaseRect, state);
}


void ScrollBarRenderer::renderTrack(PaintContext& context,
                                    const Gfx::RectF& trackRect,
                                    Direction direction,
                                    const ScrollBarState& state)
{
    onRenderTrack(context, trackRect, direction, state);
}


void ScrollBarRenderer::renderHandle(PaintContext& context,
                                     const Gfx::RectF& handleRect,
                                     Direction direction,
                                     const ScrollBarState& state)
{
    onRenderHandle(context, handleRect, direction, state);
}


void ScrollBarRenderer::renderDecreaseButton(PaintContext& context,
                                             const Gfx::RectF& buttonRect,
                                             Direction direction,
                                             const ScrollBarState& state)
{
    onRenderDecreaseButton(context, buttonRect, direction, state);
}


void ScrollBarRenderer::renderIncreaseButton(PaintContext& context,
                                             const Gfx::RectF& buttonRect,
                                             Direction direction,
                                             const ScrollBarState& state)
{
    onRenderIncreaseButton(context, buttonRect, direction, state);
}


void ScrollBarRenderer::onRenderChrome(PaintContext& context,
                                       const Gfx::RectF& /*rect*/,
                                       Direction direction,
                                       const Gfx::RectF& trackRect,
                                       const Gfx::RectF& handleRect,
                                       const Gfx::RectF& decreaseRect,
                                       const Gfx::RectF& increaseRect,
                                       const ScrollBarState& state)
{
    onRenderTrack(context, trackRect, direction, state);
    onRenderHandle(context, handleRect, direction, state);
    onRenderDecreaseButton(context, decreaseRect, direction, state);
    onRenderIncreaseButton(context, increaseRect, direction, state);
}


///////////////////////////////////////////////////////////////////////
// ScrollBarStyler
///////////////////////////////////////////////////////////////////////

ScrollBarStyler::ScrollBarStyler()
{
}


const Gfx::Brush& ScrollBarStyler::background() const
{
    return _options.get<BackgroundOption>().value();
}


void ScrollBarStyler::setBackground(const Gfx::Brush& brush)
{
    _options.set( BackgroundOption(brush) );
}


const Gfx::Brush& ScrollBarStyler::foreground() const
{
    return _options.get<ForegroundOption>().value();
}


void ScrollBarStyler::setForeground(const Gfx::Brush& brush)
{
    _options.set( ForegroundOption(brush) );
}


const Gfx::Pen& ScrollBarStyler::contour() const
{
    return _options.get<ContourOption>().value();
}


void ScrollBarStyler::setContour(const Gfx::Pen& pen)
{
    _options.set( ContourOption(pen) );
}


Gfx::SizeF ScrollBarStyler::measureFrame(PaintSurface& surface,
                                         const Gfx::SizeF& contentSize,
                                         Direction direction) const
{
    if( ! _renderer )
    {
        if( direction == Direction::Top || direction == Direction::Bottom )
            return Gfx::SizeF(16.0, contentSize.height());

        return Gfx::SizeF(contentSize.width(), 16.0);
    }

    return _renderer->measureFrame(surface, contentSize, direction);
}


Gfx::SizeF ScrollBarStyler::measureButton(PaintSurface& surface,
                                          Direction direction) const
{
    if( ! _renderer )
        return Gfx::SizeF();

    return _renderer->measureButton(surface, direction);
}


void ScrollBarStyler::layoutChrome(PaintSurface& surface,
                                   const Gfx::RectF& rect,
                                   Direction direction,
                                   const Gfx::SizeF& buttonSize,
                                   Gfx::RectF& trackRect,
                                   Gfx::RectF& decreaseRect,
                                   Gfx::RectF& increaseRect) const
{
    if( ! _renderer )
    {
        trackRect = Gfx::RectF();
        decreaseRect = Gfx::RectF();
        increaseRect = Gfx::RectF();
        return;
    }

    _renderer->layoutChrome(surface,
                            rect,
                            direction,
                            buttonSize,
                            trackRect,
                            decreaseRect,
                            increaseRect);
}


void ScrollBarStyler::layoutHandle(PaintSurface& surface,
                                   const Gfx::RectF& trackRect,
                                   Direction direction,
                                   float fraction,
                                   float viewProportion,
                                   Gfx::RectF& handleRect) const
{
    if( ! _renderer )
    {
        handleRect = Gfx::RectF();
        return;
    }

    _renderer->layoutHandle(surface,
                            trackRect,
                            direction,
                            fraction,
                            viewProportion,
                            handleRect);
}


void ScrollBarStyler::renderChrome(PaintContext& context,
                                   const Gfx::RectF& rect,
                                   Direction direction,
                                   const Gfx::RectF& trackRect,
                                   const Gfx::RectF& handleRect,
                                   const Gfx::RectF& decreaseRect,
                                   const Gfx::RectF& increaseRect,
                                   const ScrollBarState& state) const
{
    if( ! _renderer )
        return;

    _renderer->renderChrome(context,
                            rect,
                            direction,
                            trackRect,
                            handleRect,
                            decreaseRect,
                            increaseRect,
                            state);
}


void ScrollBarStyler::setRenderer(ScrollBarRenderer* renderer)
{
    _renderer.reset(renderer);
    init(renderer);
}


StyleOptions& ScrollBarStyler::onBindOptions(const StyleOptions& global)
{
    _options.bind(&global);
    return _options;
}


Renderer* ScrollBarStyler::onStyleRenderer(const Style& style)
{
    ScrollBarRenderer* renderer = style.get<ScrollBarRenderer>();
    _renderer.reset(renderer);
    return renderer;
}


Renderer* ScrollBarStyler::onCreateRenderer(const Style& style)
{
    ScrollBarRenderer* renderer = style.get<ScrollBarRenderer>();
    _renderer.reset( renderer ? renderer->create() : 0 );
    return _renderer.get();
}

} // namespace

} // namespace
