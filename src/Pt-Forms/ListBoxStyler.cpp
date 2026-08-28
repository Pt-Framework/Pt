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

#include <Pt/Forms/ListBoxStyler.h>
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
: Renderer( typeid(ListBoxRenderer), refs )
{
}


ListBoxRenderer::~ListBoxRenderer()
{
}


ListBoxRenderer* ListBoxRenderer::create() const
{
    return onCreate();
}


void ListBoxRenderer::prepare(const StyleOptions& options)
{
    onPrepare(options);
}


void ListBoxRenderer::onReset(const StyleOptions& options)
{
    prepare(options);
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


ListBoxStyler::ListBoxStyler()
{
}


const Gfx::Brush& ListBoxStyler::background() const
{
    return _options.get<ViewBackgroundOption>().value();
}


void ListBoxStyler::setBackground(const Gfx::Brush& brush)
{
    _options.set( ViewBackgroundOption(brush) );
}


const Gfx::Pen& ListBoxStyler::contour() const
{
    return _options.get<ContourOption>().value();
}


void ListBoxStyler::setContour(const Gfx::Pen& pen)
{
    _options.set( ContourOption(pen) );
}


Gfx::SizeF ListBoxStyler::measureFrame(PaintSurface& surface,
                                       const Gfx::SizeF& contentSize) const
{
    if( ! _renderer )
        return contentSize;

    return _renderer->measureFrame(surface, contentSize);
}


Gfx::RectF ListBoxStyler::layoutFrame(PaintSurface& surface,
                                      const Gfx::RectF& frameRect) const
{
    if( ! _renderer )
        return frameRect;

    return _renderer->layoutFrame(surface, frameRect);
}


void ListBoxStyler::renderBackground(PaintContext& context,
                                     const Gfx::RectF& rect,
                                     const ListBoxState& state) const
{
    if( ! _renderer )
        return;

    _renderer->renderBackground(context, rect, state);
}


void ListBoxStyler::renderChrome(PaintContext& context,
                                 const Gfx::RectF& rect,
                                 const ListBoxState& state) const
{
    if( ! _renderer )
        return;

    _renderer->renderChrome(context, rect, state);
}


void ListBoxStyler::setRenderer(ListBoxRenderer* renderer)
{
    _renderer.reset(renderer);
    init(renderer);
}


StyleOptions& ListBoxStyler::onBindOptions(const StyleOptions& global)
{
    _options.bind(&global);
    return _options;
}


Renderer* ListBoxStyler::onStyleRenderer(const Style& style)
{
    ListBoxRenderer* styleRenderer = style.get<ListBoxRenderer>();
    _renderer.reset(styleRenderer);
    return _renderer.get();
}


Renderer* ListBoxStyler::onCreateRenderer(const Style& style)
{
    ListBoxRenderer* styleRenderer = style.get<ListBoxRenderer>();
    _renderer.reset( styleRenderer ? styleRenderer->create() : 0 );
    return _renderer.get();
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
: Renderer( typeid(ListItemRenderer), refs )
{
}


ListItemRenderer::~ListItemRenderer()
{
}


ListItemRenderer* ListItemRenderer::create() const
{
    return onCreate();
}


void ListItemRenderer::prepare(const StyleOptions& options)
{
    onPrepare(options);
}


void ListItemRenderer::onReset(const StyleOptions& options)
{
    prepare(options);
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


void ListItemRenderer::renderHighlight(PaintContext& context,
                                       const Gfx::RectF& rect,
                                       const ListItemState& state)
{
    onRenderHighlight(context, rect, state);
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


ListItemStyler::ListItemStyler()
{
}


void ListItemStyler::setBackground(const Gfx::Brush& brush)
{
    _options.set( BackgroundOption(brush) );
}


const Gfx::Color& ListItemStyler::textColor() const
{
    return _options.get<TextColorOption>().value();
}


void ListItemStyler::setTextColor(const Gfx::Color& color)
{
    _options.set( TextColorOption(color) );
}


Gfx::Font ListItemStyler::font() const
{
    return _options.get<FontOption>().value();
}


void ListItemStyler::setFont(const Gfx::Font& font)
{
    FontOption option;
    option.setFont(font);
    _options.set(option);
}


void ListItemStyler::setFontSize(std::size_t size)
{
    const FontOption* localFont = _options.findLocal<FontOption>();
    FontOption option = localFont ? *localFont : FontOption();
    option.setSize(size);
    _options.set(option);
}


void ListItemStyler::setFontWeight(Gfx::Font::Weight weight)
{
    const FontOption* localFont = _options.findLocal<FontOption>();
    FontOption option = localFont ? *localFont : FontOption();
    option.setWeight(weight);
    _options.set(option);
}


void ListItemStyler::setFontSlant(Gfx::Font::Slant slant)
{
    const FontOption* localFont = _options.findLocal<FontOption>();
    FontOption option = localFont ? *localFont : FontOption();
    option.setSlant(slant);
    _options.set(option);
}


Gfx::SizeF ListItemStyler::measureContent(PaintSurface& surface,
                                          const Gfx::SizeF& iconSize,
                                          const Gfx::SizeF& textSize) const
{
    if( ! _renderer )
        return Gfx::SizeF();

    return _renderer->measureContent(surface, iconSize, textSize);
}


Gfx::SizeF ListItemStyler::measureFrame(PaintSurface& surface,
                                        const Gfx::SizeF& contentSize) const
{
    if( ! _renderer )
        return Gfx::SizeF();

    return _renderer->measureFrame(surface, contentSize);
}


const Painter* ListItemStyler::textPainter(PaintSurface& surface) const
{
    if( ! _renderer )
        return 0;

    return &_renderer->textPainter(surface);
}


Gfx::RectF ListItemStyler::layoutFrame(PaintSurface& surface,
                                       const Gfx::RectF& frameRect) const
{
    if( ! _renderer )
        return frameRect;

    return _renderer->layoutFrame(surface, frameRect);
}


void ListItemStyler::layoutContent(PaintSurface& surface,
                                   const Gfx::RectF& contentRect,
                                   const Gfx::SizeF& iconSize,
                                   const Gfx::SizeF& textSize,
                                   Gfx::RectF& iconRect,
                                   Gfx::RectF& textRect) const
{
    if( ! _renderer )
    {
        iconRect = Gfx::RectF();
        textRect = Gfx::RectF();
        return;
    }

    _renderer->layoutContent(surface, contentRect, iconSize, textSize, iconRect, textRect);
}


void ListItemStyler::renderBackground(PaintContext& context,
                                      const Gfx::RectF& rect,
                                      const ListItemState& state) const
{
    if( ! _renderer )
        return;

    _renderer->renderBackground(context, rect, state);
}


void ListItemStyler::renderHighlight(PaintContext& context,
                                     const Gfx::RectF& rect,
                                     const ListItemState& state) const
{
    if( ! _renderer )
        return;

    _renderer->renderHighlight(context, rect, state);
}


void ListItemStyler::renderText(PaintContext& context,
                                const Gfx::RectF& textRect,
                                const String& text,
                                const Gfx::PointF& pos,
                                const ListItemState& state) const
{
    if( ! _renderer )
        return;

    _renderer->renderText(context, textRect, text, pos, state);
}


void ListItemStyler::renderIcon(PaintContext& context,
                                const Gfx::RectF& iconRect,
                                const Pixmap& picture,
                                const Gfx::PointF& pos,
                                const ListItemState& state) const
{
    if( ! _renderer )
        return;

    _renderer->renderIcon(context, iconRect, picture, pos, state);
}


void ListItemStyler::setRenderer(ListItemRenderer* renderer)
{
    _renderer.reset(renderer);
    init(renderer);
}


StyleOptions& ListItemStyler::options()
{
    return _options;
}


const StyleOptions& ListItemStyler::options() const
{
    return _options;
}


StyleOptions& ListItemStyler::onBindOptions(const StyleOptions& global)
{
    _options.bind(&global);
    return _options;
}


Renderer* ListItemStyler::onStyleRenderer(const Style& style)
{
    ListItemRenderer* styleRenderer = style.get<ListItemRenderer>();
    _renderer.reset(styleRenderer);
    return _renderer.get();
}


Renderer* ListItemStyler::onCreateRenderer(const Style& style)
{
    ListItemRenderer* styleRenderer = style.get<ListItemRenderer>();
    _renderer.reset( styleRenderer ? styleRenderer->create() : 0 );
    return _renderer.get();
}

} // namespace

} // namespace
