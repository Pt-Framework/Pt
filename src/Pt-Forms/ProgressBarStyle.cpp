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

#include <Pt/Forms/ProgressBarStyle.h>
#include <Pt/Forms/StyleOptions.h>

namespace Pt {

namespace Forms {

ProgressBarState::ProgressBarState()
: _enabled(false)
, _focused(false)
{
}


bool ProgressBarState::isEnabled() const
{
    return _enabled;
}


void ProgressBarState::setEnabled(bool value)
{
    _enabled = value;
}


bool ProgressBarState::isFocused() const
{
    return _focused;
}


void ProgressBarState::setFocused(bool value)
{
    _focused = value;
}


ProgressBarRenderer::ProgressBarRenderer(std::size_t refs)
: Style::Facet( typeid(ProgressBarRenderer), refs )
{
}


ProgressBarRenderer::~ProgressBarRenderer()
{
}


ProgressBarRenderer* ProgressBarRenderer::create() const
{
    return onCreate();
}


void ProgressBarRenderer::prepare(const StyleOptions& options,
                                  const StyleOptions& progressBarOptions)
{
    onPrepare(options, progressBarOptions);
}


void ProgressBarRenderer::onReset(const StyleOptions& options)
{
    StyleOptions empty;
    onPrepare(options, empty);
}


Gfx::SizeF ProgressBarRenderer::measureFrame(PaintSurface& surface,
                                             const Gfx::SizeF& contentSize)
{
    return onMeasureFrame(surface, contentSize);
}


Gfx::SizeF ProgressBarRenderer::measureBar(PaintSurface& surface)
{
    return onMeasureBar(surface);
}


void ProgressBarRenderer::layoutChrome(PaintSurface& surface,
                                       const Gfx::RectF& rect,
                                       const Gfx::SizeF& barSize,
                                       const Gfx::SizeF& textSize,
                                       Gfx::RectF& barRect,
                                       Gfx::RectF& textRect)
{
    onLayoutChrome(surface, rect, barSize, textSize, barRect, textRect);
}


void ProgressBarRenderer::layoutBar(PaintSurface& surface,
                                    const Gfx::RectF& barRect,
                                    float progressRatio,
                                    Gfx::RectF& trackRect,
                                    Gfx::RectF& chunkRect)
{
    onLayoutBar(surface, barRect, progressRatio, trackRect, chunkRect);
}


const Painter& ProgressBarRenderer::textPainter(PaintSurface& surface)
{
    return onGetTextPainter(surface);
}


void ProgressBarRenderer::renderChrome(PaintContext& context,
                                       const Gfx::RectF& rect,
                                       const Gfx::RectF& trackRect,
                                       const Gfx::RectF& chunkRect,
                                       const Gfx::RectF& textRect,
                                       const String& text,
                                       const Gfx::PointF& textPos,
                                       const ProgressBarState& state)
{
    onRenderChrome(context, rect, trackRect, chunkRect, textRect, text, textPos, state);
}


void ProgressBarRenderer::renderBar(PaintContext& context,
                                    const Gfx::RectF& trackRect,
                                    const Gfx::RectF& chunkRect,
                                    const ProgressBarState& state)
{
    onRenderBar(context, trackRect, chunkRect, state);
}


void ProgressBarRenderer::renderTrack(PaintContext& context,
                                      const Gfx::RectF& trackRect,
                                      const ProgressBarState& state)
{
    onRenderTrack(context, trackRect, state);
}


void ProgressBarRenderer::renderChunk(PaintContext& context,
                                      const Gfx::RectF& chunkRect,
                                      const ProgressBarState& state)
{
    onRenderChunk(context, chunkRect, state);
}


void ProgressBarRenderer::renderText(PaintContext& context,
                                     const Gfx::RectF& textRect,
                                     const Gfx::RectF& chunkRect,
                                     const String& text,
                                     const Gfx::PointF& textPos,
                                     const ProgressBarState& state)
{
    onRenderText(context, textRect, chunkRect, text, textPos, state);
}


void ProgressBarRenderer::onRenderChrome(PaintContext& context,
                                         const Gfx::RectF& /*rect*/,
                                         const Gfx::RectF& trackRect,
                                         const Gfx::RectF& chunkRect,
                                         const Gfx::RectF& textRect,
                                         const String& text,
                                         const Gfx::PointF& textPos,
                                         const ProgressBarState& state)
{
    onRenderBar(context, trackRect, chunkRect, state);
    onRenderText(context, textRect, chunkRect, text, textPos, state);
}


void ProgressBarRenderer::onRenderBar(PaintContext& context,
                                      const Gfx::RectF& trackRect,
                                      const Gfx::RectF& chunkRect,
                                      const ProgressBarState& state)
{
    onRenderTrack(context, trackRect, state);
    onRenderChunk(context, chunkRect, state);
}


ProgressBarStyle::ProgressBarStyle()
{
}

} // namespace

} // namespace
