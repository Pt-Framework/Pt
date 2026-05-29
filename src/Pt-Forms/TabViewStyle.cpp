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
// TabViewStyleOptions
///////////////////////////////////////////////////////////////////////////////

TabViewStyleOptions::TabViewStyleOptions()
: _generation(0)
, _overrides(0)
{
}


bool TabViewStyleOptions::hasOverrides() const
{
    return _overrides != 0;
}


std::size_t TabViewStyleOptions::generation() const
{
    return _generation;
}


bool TabViewStyleOptions::hasOverride(StyleOverride mask) const
{
    return (_overrides & mask) != 0;
}


void TabViewStyleOptions::setOverride(StyleOverride mask)
{
    _overrides |= mask;
    ++_generation;
}


const Gfx::Brush* TabViewStyleOptions::background() const
{
    return _background.get();
}


void TabViewStyleOptions::setBackground(const Gfx::Brush& brush)
{
    _background.reset( new Gfx::Brush(brush) );
    setOverride(Background);
}


const Gfx::Pen* TabViewStyleOptions::contour() const
{
    return _contour.get();
}


void TabViewStyleOptions::setContour(const Gfx::Pen& pen)
{
    _contour.reset( new Gfx::Pen(pen) );
    setOverride(Contour);
}


const Gfx::Color* TabViewStyleOptions::textColor() const
{
    return _textColor.get();
}


void TabViewStyleOptions::setTextColor(const Gfx::Color& color)
{
    _textColor.reset( new Gfx::Color(color) );
    setOverride(TextColor);
}


const Gfx::Font* TabViewStyleOptions::font() const
{
    return _font.font();
}


void TabViewStyleOptions::setFont(const Gfx::Font& font)
{
    _font.setFont(font);
    setOverride(Font);
}


void TabViewStyleOptions::setFontSize(std::size_t size)
{
    _font.setSize(size);
    setOverride(Font);
}


void TabViewStyleOptions::setFontWeight(Gfx::Font::Weight weight)
{
    _font.setWeight(weight);
    setOverride(Font);
}


void TabViewStyleOptions::setFontSlant(Gfx::Font::Slant slant)
{
    _font.setSlant(slant);
    setOverride(Font);
}


Gfx::Font TabViewStyleOptions::getFont(const Gfx::Font& base) const
{
    return _font.getFont(base);
}


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
// TabItemState
///////////////////////////////////////////////////////////////////////////////

TabItemState::TabItemState()
: _enabled(false)
, _active(false)
, _highlighted(false)
, _pressed(false)
{
}


bool TabItemState::isEnabled() const
{
    return _enabled;
}


void TabItemState::setEnabled(bool v)
{
    _enabled = v;
}


bool TabItemState::isActive() const
{
    return _active;
}


void TabItemState::setActive(bool v)
{
    _active = v;
}


bool TabItemState::isHighlighted() const
{
    return _highlighted;
}


void TabItemState::setHighlighted(bool v)
{
    _highlighted = v;
}


bool TabItemState::isPressed() const
{
    return _pressed;
}


void TabItemState::setPressed(bool v)
{
    _pressed = v;
}


///////////////////////////////////////////////////////////////////////////////
// TabViewRenderer
///////////////////////////////////////////////////////////////////////////////

TabViewRenderer::TabViewRenderer(std::size_t refs)
: Style::Facet( typeid(TabViewRenderer), refs )
{
}


TabViewRenderer::~TabViewRenderer()
{
}


TabViewRenderer* TabViewRenderer::create() const
{
    return onCreate();
}


void TabViewRenderer::prepare(const StyleOptions& options,
                              const TabViewStyleOptions& tabViewOptions)
{
    onPrepare(options, tabViewOptions);
}


void TabViewRenderer::onReset(const StyleOptions& options)
{
    TabViewStyleOptions empty;
    onPrepare(options, empty);
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
                                const TabItemState& state)
{
    onRenderTab(context, tabRect, text, textPos, state);
}


TabViewStyle::TabViewStyle()
{
}

} // namespace

} // namespace
