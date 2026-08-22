/* Copyright (C) 2015 Marc Boris Duerner

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

#include <Pt/Forms/LineEditStyle.h>
#include <Pt/Forms/StyleOptions.h>

namespace Pt {

namespace Forms {

LineEditState::LineEditState()
: _enabled(false)
, _focused(false)
, _highlighted(false)
, _editable(false)
, _placeholder(false)
{
}


bool LineEditState::isEnabled() const
{
    return _enabled;
}


void LineEditState::setEnabled(bool value)
{
    _enabled = value;
}


bool LineEditState::isFocused() const
{
    return _focused;
}


void LineEditState::setFocused(bool value)
{
    _focused = value;
}


bool LineEditState::isHighlighted() const
{
    return _highlighted;
}


void LineEditState::setHighlighted(bool value)
{
    _highlighted = value;
}


bool LineEditState::isEditable() const
{
    return _editable;
}


void LineEditState::setEditable(bool value)
{
    _editable = value;
}


bool LineEditState::isPlaceholder() const
{
    return _placeholder;
}


void LineEditState::setPlaceholder(bool value)
{
    _placeholder = value;
}


LineEditRenderer::LineEditRenderer(std::size_t refs)
: Style::Facet( typeid(LineEditRenderer), refs )
{
}


LineEditRenderer::~LineEditRenderer()
{
}


LineEditRenderer* LineEditRenderer::create() const
{
    return onCreate();
}


void LineEditRenderer::prepare(const StyleOptions& options,
                               const StyleOptions& lineEditOptions)
{
    onPrepare(options, lineEditOptions);
}


void LineEditRenderer::onReset(const StyleOptions& options)
{
    StyleOptions empty;
    onPrepare(options, empty);
}


Gfx::SizeF LineEditRenderer::measureFrame(PaintSurface& surface,
                                          const Gfx::SizeF& contentSize)
{
    return onMeasureFrame(surface, contentSize);
}


Gfx::RectF LineEditRenderer::layoutFrame(PaintSurface& surface,
                                         const Gfx::RectF& rect)
{
    return onLayoutFrame(surface, rect);
}


const Painter& LineEditRenderer::textPainter(PaintSurface& surface)
{
    return onGetTextPainter(surface);
}


void LineEditRenderer::renderChrome(PaintContext& context,
                                    const Gfx::RectF& rect,
                                    const Gfx::RectF& textRect,
                                    const String& text,
                                    const Gfx::PointF& textPos,
                                    const Gfx::RectF& cursor,
                                    const Gfx::RectF& selection,
                                    const LineEditState& state)
{
    onRenderChrome(context, rect, textRect, text, textPos, cursor, selection, state);
}


void LineEditRenderer::renderEntry(PaintContext& context,
                                   const Gfx::RectF& rect,
                                   const LineEditState& state)
{
    onRenderEntry(context, rect, state);
}


void LineEditRenderer::renderSelection(PaintContext& context,
                                       const Gfx::RectF& textRect,
                                       const Gfx::RectF& selection,
                                       const LineEditState& state)
{
    onRenderSelection(context, textRect, selection, state);
}


void LineEditRenderer::renderText(PaintContext& context,
                                  const Gfx::RectF& textRect,
                                  const String& text,
                                  const Gfx::PointF& textPos,
                                  const LineEditState& state)
{
    onRenderText(context, textRect, text, textPos, state);
}


void LineEditRenderer::renderCursor(PaintContext& context,
                                    const Gfx::RectF& textRect,
                                    const Gfx::RectF& cursor,
                                    const LineEditState& state)
{
    onRenderCursor(context, textRect, cursor, state);
}


void LineEditRenderer::onRenderChrome(PaintContext& context,
                                      const Gfx::RectF& rect,
                                      const Gfx::RectF& textRect,
                                      const String& text,
                                      const Gfx::PointF& textPos,
                                      const Gfx::RectF& cursor,
                                      const Gfx::RectF& selection,
                                      const LineEditState& state)
{
    onRenderEntry(context, rect, state);
    onRenderSelection(context, textRect, selection, state);
    onRenderText(context, textRect, text, textPos, state);
    onRenderCursor(context, textRect, cursor, state);
}


LineEditStyle::LineEditStyle()
{
}

} // namespace

} // namespace
