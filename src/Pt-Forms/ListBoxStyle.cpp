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

#include <Pt/Forms/ListBoxStyle.h>
#include <Pt/Forms/StyleOptions.h>

namespace Pt {

namespace Forms {

///////////////////////////////////////////////////////////////////////////////
// ListBoxState
///////////////////////////////////////////////////////////////////////////////

ListBoxState::ListBoxState()
: _enabled(false)
, _focused(false)
{
}


bool ListBoxState::isEnabled() const
{
    return _enabled;
}


void ListBoxState::setEnabled(bool v)
{
    _enabled = v;
}


bool ListBoxState::isFocused() const
{
    return _focused;
}


void ListBoxState::setFocused(bool v)
{
    _focused = v;
}


///////////////////////////////////////////////////////////////////////////////
// ListBoxRenderer
///////////////////////////////////////////////////////////////////////////////

ListBoxRenderer::ListBoxRenderer(std::size_t refs)
: Style::Facet( typeid(ListBoxRenderer), refs )
{
}


ListBoxRenderer::~ListBoxRenderer()
{
}


ListBoxRenderer* ListBoxRenderer::create() const
{
    return onCreate();
}


void ListBoxRenderer::prepare(const StyleOptions& options,
                              const StyleOptions& listBoxOptions)
{
    onPrepare(options, listBoxOptions);
}


void ListBoxRenderer::onReset(const StyleOptions& options)
{
    StyleOptions empty;
    onPrepare(options, empty);
}


Gfx::SizeF ListBoxRenderer::measureFrame(PaintSurface& surface,
                                          const Gfx::SizeF& contentSize)
{
    return onMeasureFrame(surface, contentSize);
}


Gfx::RectF ListBoxRenderer::layoutFrame(PaintSurface& surface,
                                         const Gfx::RectF& rect)
{
    return onLayoutFrame(surface, rect);
}


void ListBoxRenderer::renderBackground(PaintContext& context,
                                       const Gfx::RectF& rect,
                                       const ListBoxState& state)
{
    onRenderBackground(context, rect, state);
}


void ListBoxRenderer::renderChrome(PaintContext& context,
                                   const Gfx::RectF& rect,
                                   const ListBoxState& state)
{
    onRenderChrome(context, rect, state);
}


ListBoxStyle::ListBoxStyle()
{
}


///////////////////////////////////////////////////////////////////////////////
// ListItemState
///////////////////////////////////////////////////////////////////////////////

ListItemState::ListItemState()
: _enabled(false)
, _highlighted(false)
, _focused(false)
, _selected(false)
{
}


bool ListItemState::isEnabled() const
{
    return _enabled;
}


void ListItemState::setEnabled(bool v)
{
    _enabled = v;
}


bool ListItemState::isHighlighted() const
{
    return _highlighted;
}


void ListItemState::setHighlighted(bool v)
{
    _highlighted = v;
}


bool ListItemState::isFocused() const
{
    return _focused;
}


void ListItemState::setFocused(bool v)
{
    _focused = v;
}


bool ListItemState::isSelected() const
{
    return _selected;
}


void ListItemState::setSelected(bool v)
{
    _selected = v;
}


///////////////////////////////////////////////////////////////////////////////
// ListItemRenderer
///////////////////////////////////////////////////////////////////////////////

ListItemRenderer::ListItemRenderer(std::size_t refs)
: Style::Facet( typeid(ListItemRenderer), refs )
{
}


ListItemRenderer::~ListItemRenderer()
{
}


ListItemRenderer* ListItemRenderer::create() const
{
    return onCreate();
}


void ListItemRenderer::prepare(const StyleOptions& options,
                               const StyleOptions& listItemOptions)
{
    onPrepare(options, listItemOptions);
}


void ListItemRenderer::onReset(const StyleOptions& options)
{
    StyleOptions empty;
    onPrepare(options, empty);
}


Gfx::SizeF ListItemRenderer::measureContent(PaintSurface& surface,
                                            const Gfx::SizeF& iconSize,
                                            const Gfx::SizeF& textSize)
{
    return onMeasureContent(surface, iconSize, textSize);
}


Gfx::SizeF ListItemRenderer::measureFrame(PaintSurface& surface,
                                           const Gfx::SizeF& contentSize)
{
    return onMeasureFrame(surface, contentSize);
}


const Painter& ListItemRenderer::textPainter(PaintSurface& surface)
{
    return onGetTextPainter(surface);
}


Gfx::RectF ListItemRenderer::layoutFrame(PaintSurface& surface,
                                          const Gfx::RectF& rect)
{
    return onLayoutFrame(surface, rect);
}


void ListItemRenderer::layoutContent(PaintSurface& surface,
                                     const Gfx::RectF& contentRect,
                                     const Gfx::SizeF& iconSize,
                                     const Gfx::SizeF& textSize,
                                     Gfx::RectF& iconRect,
                                     Gfx::RectF& textRect)
{
    onLayoutContent(surface, contentRect, iconSize, textSize, iconRect, textRect);
}


void ListItemRenderer::renderBackground(PaintContext& context,
                                        const Gfx::RectF& rect,
                                        const ListItemState& state)
{
    onRenderBackground(context, rect, state);
}


void ListItemRenderer::renderText(PaintContext& context,
                                  const Gfx::RectF& textRect,
                                  const String& text,
                                  const Gfx::PointF& pos,
                                  const ListItemState& state)
{
    onRenderText(context, textRect, text, pos, state);
}


void ListItemRenderer::renderIcon(PaintContext& context,
                                  const Gfx::RectF& iconRect,
                                  const Pixmap& picture,
                                  const Gfx::PointF& pos,
                                  const ListItemState& state)
{
    onRenderIcon(context, iconRect, picture, pos, state);
}


ListItemStyle::ListItemStyle()
{
}

} // namespace

} // namespace
