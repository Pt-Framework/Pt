/*
  Copyright (C) 2016 Laurentiu-Gheorghe Crisan
  Copyright (C) 2016 Marc Boris Duerner

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
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the:
  Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
  Boston, MA 02110-1301 USA
*/

#include <Pt/Forms/ButtonStyler.h>
#include <Pt/Forms/StyleOptions.h>
#include <Pt/Forms/Style.h>
#include <Pt/Forms/Painter.h>

namespace Pt {

namespace Forms {

///////////////////////////////////////////////////////////////////////
// ButtonState
///////////////////////////////////////////////////////////////////////

ButtonState::ButtonState()
: _enabled(false)
, _hovered(false)
, _focused(false)
, _pressed(false)
, _flat(false)
{
}


bool ButtonState::isEnabled() const
{
    return _enabled;
}


void ButtonState::setEnabled(bool value)
{
    _enabled = value;
}


bool ButtonState::isHovered() const
{
    return _hovered;
}


void ButtonState::setHovered(bool value)
{
    _hovered = value;
}


bool ButtonState::isFocused() const
{
    return _focused;
}


void ButtonState::setFocused(bool value)
{
    _focused = value;
}


bool ButtonState::isPressed() const
{
    return _pressed;
}


void ButtonState::setPressed(bool value)
{
    _pressed = value;
}


bool ButtonState::isFlat() const
{
    return _flat;
}


void ButtonState::setFlat(bool value)
{
    _flat = value;
}

///////////////////////////////////////////////////////////////////////
// ButtonRenderer
///////////////////////////////////////////////////////////////////////

ButtonRenderer::ButtonRenderer(std::size_t refs)
: Renderer( typeid(ButtonRenderer), refs )
{
}


ButtonRenderer::~ButtonRenderer()
{
}


ButtonRenderer* ButtonRenderer::create() const
{
    return onCreate();
}


Gfx::SizeF ButtonRenderer::measureContent(PaintSurface& surface,
                                          Direction direction,
                                          const Gfx::SizeF& iconSize,
                                          const Gfx::SizeF& textSize)
{
    return onMeasureContent(surface, direction, iconSize, textSize);
}


Gfx::SizeF ButtonRenderer::measureFrame(PaintSurface& surface,
                                        const Gfx::SizeF& contentSize)
{
    return onMeasureFrame(surface, contentSize);
}


Gfx::RectF ButtonRenderer::layoutFrame(PaintSurface& surface,
                                       const Gfx::RectF& frameRect)
{
    return onLayoutFrame(surface, frameRect);
}


Gfx::RectF ButtonRenderer::layoutMnemonic(PaintSurface& surface,
                                          const String& text,
                                          const Gfx::PointF& textPos,
                                          const Gfx::FontMetrics& fontMetrics,
                                          String::size_type mnemonicIndex)
{
    return onLayoutMnemonic(surface, text, textPos, fontMetrics, mnemonicIndex);
}


const Painter& ButtonRenderer::textPainter(PaintSurface& surface)
{
    return onGetTextPainter(surface);
}


void ButtonRenderer::layoutContent(PaintSurface& surface,
                                   const Gfx::RectF& rect,
                                   Direction direction,
                                   const Gfx::SizeF& iconSize,
                                   const Gfx::SizeF& textSize,
                                   Gfx::RectF& iconRect,
                                   Gfx::RectF& textRect)
{
    onLayoutContent(surface, rect, direction, iconSize, textSize, iconRect,
                    textRect);
}


void ButtonRenderer::renderBackground(PaintContext& context,
                                      const Gfx::RectF& rect,
                                      const ButtonState& state)
{
    onRenderBackground(context, rect, state);
}


void ButtonRenderer::prepareIcon(const Gfx::Image& icon,
                                 Pixmap& picture,
                                 const ButtonState& state) const
{
    onPrepareIcon(icon, picture, state);
}


void ButtonRenderer::renderChrome(PaintContext& context,
                                  const Gfx::RectF& rect,
                                  const ButtonState& state)
{
    onRenderChrome(context, rect, state);
}


void ButtonRenderer::renderText(PaintContext& context,
                                const Gfx::RectF& rect,
                                const String& text,
                                const Gfx::PointF& pos,
                                const ButtonState& state)
{
    onRenderText(context, rect, text, pos, state);
}


void ButtonRenderer::renderMnemonic(PaintContext& context,
                                    const Gfx::RectF& rect,
                                    const Gfx::RectF& mnemonic,
                                    const ButtonState& state)
{
    onRenderMnemonic(context, rect, mnemonic, state);
}


void ButtonRenderer::renderIcon(PaintContext& context,
                                const Gfx::RectF& rect,
                                const Pixmap& picture,
                                const Gfx::PointF& pos,
                                const ButtonState& state)
{
    onRenderIcon(context, rect, picture, pos, state);
}

///////////////////////////////////////////////////////////////////////
// ButtonStyler
///////////////////////////////////////////////////////////////////////

ButtonStyler::ButtonStyler()
{
}


const Gfx::Brush& ButtonStyler::foreground() const
{
    return _options.get<ForegroundOption>().value();
}


void ButtonStyler::setForeground(const Gfx::Brush& brush)
{
    _options.set( ForegroundOption(brush) );
}


const Gfx::Pen& ButtonStyler::contour() const
{
    return _options.get<ContourOption>().value();
}


void ButtonStyler::setContour(const Gfx::Pen& pen)
{
    _options.set( ContourOption(pen) );
}


const Gfx::Color& ButtonStyler::accentColor() const
{
    return _options.get<AccentColorOption>().value();
}


void ButtonStyler::setAccentColor(const Gfx::Color& color)
{
    _options.set( AccentColorOption(color) );
}


const Gfx::Color& ButtonStyler::highlightColor() const
{
    return _options.get<HighlightColorOption>().value();
}


void ButtonStyler::setHighlightColor(const Gfx::Color& color)
{
    _options.set( HighlightColorOption(color) );
}


const Gfx::Color& ButtonStyler::textColor() const
{
    return _options.get<TextColorOption>().value();
}


void ButtonStyler::setTextColor(const Gfx::Color& color)
{
    _options.set( TextColorOption(color) );
}


Gfx::Font ButtonStyler::font() const
{
    return _options.get<FontOption>().value();
}


void ButtonStyler::setFont(const Gfx::Font& font)
{
    FontOption option;
    option.setFont(font);
    _options.set(option);
}


void ButtonStyler::setFontSize(std::size_t size)
{
    const FontOption* localFont = _options.findLocal<FontOption>();
    FontOption option = localFont ? *localFont : FontOption();
    option.setSize(size);
    _options.set(option);
}


void ButtonStyler::setFontWeight(Gfx::Font::Weight weight)
{
    const FontOption* localFont = _options.findLocal<FontOption>();
    FontOption option = localFont ? *localFont : FontOption();
    option.setWeight(weight);
    _options.set(option);
}


void ButtonStyler::setFontSlant(Gfx::Font::Slant slant)
{
    const FontOption* localFont = _options.findLocal<FontOption>();
    FontOption option = localFont ? *localFont : FontOption();
    option.setSlant(slant);
    _options.set(option);
}


bool ButtonStyler::prepareIcon(const Gfx::Image& icon,
                               Pixmap& picture,
                               const ButtonState& state) const
{
    if( ! _renderer )
        return false;

    _renderer->prepareIcon(icon, picture, state);
    return true;
}


void ButtonStyler::measureText(PaintSurface& surface,
                               const String& text,
                               Gfx::TextMetrics& textMetrics,
                               Gfx::FontMetrics& fontMetrics) const
{
    if( ! _renderer )
    {
        textMetrics = Gfx::TextMetrics();
        fontMetrics = Gfx::FontMetrics();
        return;
    }

    const Painter& painter = _renderer->textPainter(surface);
    textMetrics = painter.textMetrics(text);
    fontMetrics = painter.fontMetrics();
}


Gfx::SizeF ButtonStyler::measureContent(PaintSurface& surface,
                                        Direction direction,
                                        const Gfx::SizeF& iconSize,
                                        const Gfx::SizeF& textSize) const
{
    if( ! _renderer )
        return Gfx::SizeF();

    return _renderer->measureContent(surface, direction, iconSize, textSize);
}


Gfx::SizeF ButtonStyler::measureFrame(PaintSurface& surface,
                                      const Gfx::SizeF& contentSize) const
{
    if( ! _renderer )
        return Gfx::SizeF();

    return _renderer->measureFrame(surface, contentSize);
}


Gfx::RectF ButtonStyler::layoutFrame(PaintSurface& surface,
                                     const Gfx::RectF& frameRect) const
{
    if( ! _renderer )
        return Gfx::RectF();

    return _renderer->layoutFrame(surface, frameRect);
}


void ButtonStyler::layoutContent(PaintSurface& surface,
                                 const Gfx::RectF& contentRect,
                                 Direction direction,
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

    _renderer->layoutContent(surface,
                             contentRect,
                             direction,
                             iconSize,
                             textSize,
                             iconRect,
                             textRect);
}


Gfx::RectF ButtonStyler::layoutMnemonic(PaintSurface& surface,
                                        const String& text,
                                        const Gfx::PointF& textPos,
                                        const Gfx::FontMetrics& fontMetrics,
                                        String::size_type mnemonicIndex) const
{
    if( ! _renderer )
        return Gfx::RectF();

    return _renderer->layoutMnemonic(surface,
                                     text,
                                     textPos,
                                     fontMetrics,
                                     mnemonicIndex);
}


void ButtonStyler::renderBackground(PaintContext& context,
                                    const Gfx::RectF& rect,
                                    const ButtonState& state) const
{
    if( ! _renderer )
        return;

    _renderer->renderBackground(context, rect, state);
}


void ButtonStyler::renderChrome(PaintContext& context,
                                const Gfx::RectF& rect,
                                const ButtonState& state) const
{
    if( ! _renderer )
        return;

    _renderer->renderChrome(context, rect, state);
}


void ButtonStyler::renderText(PaintContext& context,
                              const Gfx::RectF& rect,
                              const String& text,
                              const Gfx::PointF& pos,
                              const ButtonState& state) const
{
    if( ! _renderer )
        return;

    _renderer->renderText(context, rect, text, pos, state);
}


void ButtonStyler::renderMnemonic(PaintContext& context,
                                  const Gfx::RectF& rect,
                                  const Gfx::RectF& mnemonic,
                                  const ButtonState& state) const
{
    if( ! _renderer )
        return;

    _renderer->renderMnemonic(context, rect, mnemonic, state);
}


void ButtonStyler::renderIcon(PaintContext& context,
                              const Gfx::RectF& rect,
                              const Pixmap& picture,
                              const Gfx::PointF& pos,
                              const ButtonState& state) const
{
    if( ! _renderer )
        return;

    _renderer->renderIcon(context, rect, picture, pos, state);
}


void ButtonStyler::setRenderer(ButtonRenderer* renderer)
{
    _renderer.reset(renderer);
    init(renderer);
}


StyleOptions& ButtonStyler::options()
{
    return _options;
}


const StyleOptions& ButtonStyler::options() const
{
    return _options;
}


StyleOptions& ButtonStyler::onBindOptions(const StyleOptions& global)
{
    _options.bind(&global);
    return _options;
}


Renderer* ButtonStyler::onStyleRenderer(const Style& style)
{
    ButtonRenderer* styleRenderer = style.get<ButtonRenderer>();
    _renderer.reset(styleRenderer);
    return _renderer.get();
}


Renderer* ButtonStyler::onCreateRenderer(const Style& style)
{
    ButtonRenderer* styleRenderer = style.get<ButtonRenderer>();
    _renderer.reset( styleRenderer ? styleRenderer->create() : 0 );
    return _renderer.get();
}

} // namespace

} // namespace
