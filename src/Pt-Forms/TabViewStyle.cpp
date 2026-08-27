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

#include <Pt/Forms/TabViewStyle.h>
#include <Pt/Forms/StyleOptions.h>

namespace Pt {

namespace Forms {

///////////////////////////////////////////////////////////////////////////////
// TabViewState
///////////////////////////////////////////////////////////////////////////////

TabViewState::TabViewState()
: _enabled(false)
, _focused(false)
{
}


bool TabViewState::isEnabled() const
{
    return _enabled;
}


void TabViewState::setEnabled(bool v)
{
    _enabled = v;
}


bool TabViewState::isFocused() const
{
    return _focused;
}


void TabViewState::setFocused(bool v)
{
    _focused = v;
}


///////////////////////////////////////////////////////////////////////////////
// TabViewItemState
///////////////////////////////////////////////////////////////////////////////

TabViewItemState::TabViewItemState()
: _enabled(false)
, _active(false)
, _highlighted(false)
, _pressed(false)
{
}


bool TabViewItemState::isEnabled() const
{
    return _enabled;
}


void TabViewItemState::setEnabled(bool v)
{
    _enabled = v;
}


bool TabViewItemState::isActive() const
{
    return _active;
}


void TabViewItemState::setActive(bool v)
{
    _active = v;
}


bool TabViewItemState::isHighlighted() const
{
    return _highlighted;
}


void TabViewItemState::setHighlighted(bool v)
{
    _highlighted = v;
}


bool TabViewItemState::isPressed() const
{
    return _pressed;
}


void TabViewItemState::setPressed(bool v)
{
    _pressed = v;
}


///////////////////////////////////////////////////////////////////////////////
// TabViewRenderer
///////////////////////////////////////////////////////////////////////////////

TabViewRenderer::TabViewRenderer(std::size_t refs)
: Renderer( typeid(TabViewRenderer), refs )
{
}


TabViewRenderer::~TabViewRenderer()
{
}


TabViewRenderer* TabViewRenderer::create() const
{
    return onCreate();
}


void TabViewRenderer::prepare(const StyleOptions& options)
{
    onPrepare(options);
}


void TabViewRenderer::onReset(const StyleOptions& options)
{
    prepare(options);
}


Gfx::SizeF TabViewRenderer::measureTab(PaintSurface& surface,
                                        const Pt::String& text)
{
    return onMeasureTab(surface, text);
}


Gfx::RectF TabViewRenderer::layoutTab(PaintSurface& surface,
                                       const Gfx::RectF& tabRect)
{
    return onLayoutTab(surface, tabRect);
}


const Painter& TabViewRenderer::textPainter(PaintSurface& surface)
{
    return onGetTextPainter(surface);
}


void TabViewRenderer::renderBackground(PaintContext& context,
                                       const Gfx::RectF& contentRect,
                                       const TabViewState& state)
{
    onRenderBackground(context, contentRect, state);
}


void TabViewRenderer::renderChrome(PaintContext& context,
                                   const Gfx::RectF& contentRect,
                                   const Gfx::RectF& activeTabRect,
                                   const TabViewState& state)
{
    onRenderChrome(context, contentRect, activeTabRect, state);
}


void TabViewRenderer::renderTab(PaintContext& context,
                                const Gfx::RectF& tabRect,
                                const Pt::String& text,
                                const Gfx::PointF& textPos,
                                const TabViewItemState& state)
{
    onRenderTab(context, tabRect, text, textPos, state);
}


TabViewStyler::TabViewStyler()
{
}


const Gfx::Brush& TabViewStyler::background() const
{
    return _options.get<BackgroundOption>().value();
}


void TabViewStyler::setBackground(const Gfx::Brush& brush)
{
    _options.set( BackgroundOption(brush) );
}


const Gfx::Pen& TabViewStyler::contour() const
{
    return _options.get<ContourOption>().value();
}


void TabViewStyler::setContour(const Gfx::Pen& pen)
{
    _options.set( ContourOption(pen) );
}


const Gfx::Color& TabViewStyler::textColor() const
{
    return _options.get<TextColorOption>().value();
}


void TabViewStyler::setTextColor(const Gfx::Color& color)
{
    _options.set( TextColorOption(color) );
}


const Gfx::Color& TabViewStyler::accentColor() const
{
    return _options.get<AccentColorOption>().value();
}


void TabViewStyler::setAccentColor(const Gfx::Color& color)
{
    _options.set( AccentColorOption(color) );
}


Gfx::Font TabViewStyler::font() const
{
    return _options.get<FontOption>().value();
}


void TabViewStyler::setFont(const Gfx::Font& font)
{
    FontOption option;
    option.setFont(font);
    _options.set(option);
}


void TabViewStyler::setFontSize(std::size_t size)
{
    const FontOption* localFont = _options.findLocal<FontOption>();
    FontOption option = localFont ? *localFont : FontOption();
    option.setSize(size);
    _options.set(option);
}


void TabViewStyler::setFontWeight(Gfx::Font::Weight weight)
{
    const FontOption* localFont = _options.findLocal<FontOption>();
    FontOption option = localFont ? *localFont : FontOption();
    option.setWeight(weight);
    _options.set(option);
}


void TabViewStyler::setFontSlant(Gfx::Font::Slant slant)
{
    const FontOption* localFont = _options.findLocal<FontOption>();
    FontOption option = localFont ? *localFont : FontOption();
    option.setSlant(slant);
    _options.set(option);
}


Gfx::SizeF TabViewStyler::measureTab(PaintSurface& surface,
                                      const Pt::String& text) const
{
    if( ! _renderer )
        return Gfx::SizeF();

    return _renderer->measureTab(surface, text);
}


Gfx::RectF TabViewStyler::layoutTab(PaintSurface& surface,
                                    const Gfx::RectF& tabRect) const
{
    if( ! _renderer )
        return tabRect;

    return _renderer->layoutTab(surface, tabRect);
}


const Painter* TabViewStyler::textPainter(PaintSurface& surface) const
{
    if( ! _renderer )
        return 0;

    return &_renderer->textPainter(surface);
}


void TabViewStyler::renderBackground(PaintContext& context,
                                     const Gfx::RectF& contentRect,
                                     const TabViewState& state) const
{
    if( ! _renderer )
        return;

    _renderer->renderBackground(context, contentRect, state);
}


void TabViewStyler::renderChrome(PaintContext& context,
                                 const Gfx::RectF& contentRect,
                                 const Gfx::RectF& activeTabRect,
                                 const TabViewState& state) const
{
    if( ! _renderer )
        return;

    _renderer->renderChrome(context, contentRect, activeTabRect, state);
}


void TabViewStyler::renderTab(PaintContext& context,
                              const Gfx::RectF& tabRect,
                              const Pt::String& text,
                              const Gfx::PointF& textPos,
                              const TabViewItemState& state) const
{
    if( ! _renderer )
        return;

    _renderer->renderTab(context, tabRect, text, textPos, state);
}


void TabViewStyler::setRenderer(TabViewRenderer* renderer)
{
    _renderer.reset(renderer);
    init(renderer);
}


StyleOptions& TabViewStyler::options()
{
    return _options;
}


const StyleOptions& TabViewStyler::options() const
{
    return _options;
}


StyleOptions& TabViewStyler::onBindOptions(const StyleOptions& global)
{
    _options.bind(&global);
    return _options;
}


Renderer* TabViewStyler::onStyleRenderer(const Style& style)
{
    _renderer.reset( style.get<TabViewRenderer>() );
    return _renderer.get();
}


Renderer* TabViewStyler::onCreateRenderer(const Style& style)
{
    TabViewRenderer* styleRenderer = style.get<TabViewRenderer>();
    _renderer.reset( styleRenderer ? styleRenderer->create() : 0 );
    return _renderer.get();
}

} // namespace

} // namespace
