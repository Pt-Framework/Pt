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

#include <Pt/Forms/ScrollBarStyle.h>
#include <Pt/Forms/StyleOptions.h>

namespace Pt {

namespace Forms {

ScrollBarStyleOptions::ScrollBarStyleOptions()
{
}


const Gfx::Brush* ScrollBarStyleOptions::background() const
{
    return _background.get();
}


void ScrollBarStyleOptions::setBackground(const Gfx::Brush& brush)
{
    _background.reset( new Gfx::Brush(brush) );
    setOverride(Background);
}


const Gfx::Pen* ScrollBarStyleOptions::contour() const
{
    return _contour.get();
}


void ScrollBarStyleOptions::setContour(const Gfx::Pen& pen)
{
    _contour.reset( new Gfx::Pen(pen) );
    setOverride(Contour);
}


const Gfx::Brush* ScrollBarStyleOptions::foreground() const
{
    return _foreground.get();
}


void ScrollBarStyleOptions::setForeground(const Gfx::Brush& brush)
{
    _foreground.reset( new Gfx::Brush(brush) );
    setOverride(Foreground);
}


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
: Style::Facet( typeid(ScrollBarRenderer), refs )
{
}


ScrollBarRenderer::~ScrollBarRenderer()
{
}


ScrollBarRenderer* ScrollBarRenderer::create() const
{
    return onCreate();
}


void ScrollBarRenderer::prepare(const StyleOptions& options,
                                const ScrollBarStyleOptions& scrollBarOptions)
{
    onPrepare(options, scrollBarOptions);
}


void ScrollBarRenderer::onReset(const StyleOptions& options)
{
    ScrollBarStyleOptions empty;
    onPrepare(options, empty);
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


ScrollBarStyle::ScrollBarStyle()
{
}

} // namespace

} // namespace
