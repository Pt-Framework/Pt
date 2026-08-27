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

#include <Pt/Forms/SpinBoxStyle.h>
#include <Pt/Forms/StyleOptions.h>

namespace Pt {

namespace Forms {

SpinBoxState::SpinBoxState()
: _enabled(false)
, _hovered(false)
, _focused(false)
, _editable(false)
, _upPressed(false)
, _upHovered(false)
, _downPressed(false)
, _downHovered(false)
{
}


bool SpinBoxState::isEnabled() const
{
    return _enabled;
}


void SpinBoxState::setEnabled(bool value)
{
    _enabled = value;
}


bool SpinBoxState::isHovered() const
{
    return _hovered;
}


void SpinBoxState::setHovered(bool value)
{
    _hovered = value;
}


bool SpinBoxState::isFocused() const
{
    return _focused;
}


void SpinBoxState::setFocused(bool value)
{
    _focused = value;
}


bool SpinBoxState::isEditable() const
{
    return _editable;
}


void SpinBoxState::setEditable(bool value)
{
    _editable = value;
}


bool SpinBoxState::isUpPressed() const
{
    return _upPressed;
}


void SpinBoxState::setUpPressed(bool value)
{
    _upPressed = value;
}


bool SpinBoxState::isUpHovered() const
{
    return _upHovered;
}


void SpinBoxState::setUpHovered(bool value)
{
    _upHovered = value;
}


bool SpinBoxState::isDownPressed() const
{
    return _downPressed;
}


void SpinBoxState::setDownPressed(bool value)
{
    _downPressed = value;
}


bool SpinBoxState::isDownHovered() const
{
    return _downHovered;
}


void SpinBoxState::setDownHovered(bool value)
{
    _downHovered = value;
}


SpinBoxRenderer::SpinBoxRenderer(std::size_t refs)
: Renderer( typeid(SpinBoxRenderer), refs )
{
}


SpinBoxRenderer::~SpinBoxRenderer()
{
}


SpinBoxRenderer* SpinBoxRenderer::create() const
{
    return onCreate();
}


void SpinBoxRenderer::prepare(const StyleOptions& options)
{
    onPrepare(options);
}


void SpinBoxRenderer::onReset(const StyleOptions& options)
{
    prepare(options);
}


Gfx::SizeF SpinBoxRenderer::measureFrame(PaintSurface& surface,
                                         const Gfx::SizeF& contentSize)
{
    return onMeasureFrame(surface, contentSize);
}


Gfx::SizeF SpinBoxRenderer::measureEntry(PaintSurface& surface,
                                          const Gfx::SizeF& contentSize)
{
    return onMeasureEntry(surface, contentSize);
}


Gfx::SizeF SpinBoxRenderer::measureIndicator(PaintSurface& surface)
{
    return onMeasureIndicator(surface);
}


void SpinBoxRenderer::layoutChrome(PaintSurface& surface,
                                   const Gfx::RectF& rect,
                                   Gfx::RectF& entryRect,
                                   Gfx::RectF& upButtonRect,
                                   Gfx::RectF& downButtonRect,
                                   Gfx::RectF& textRect)
{
    onLayoutChrome(surface, rect, entryRect, upButtonRect, downButtonRect, textRect);
}


Gfx::RectF SpinBoxRenderer::layoutEntry(PaintSurface& surface,
                                         const Gfx::RectF& entryRect)
{
    return onLayoutEntry(surface, entryRect);
}


const Painter& SpinBoxRenderer::textPainter(PaintSurface& surface)
{
    return onGetTextPainter(surface);
}


void SpinBoxRenderer::renderChrome(PaintContext& context,
                                    const Gfx::RectF& rect,
                                    const Gfx::RectF& entryRect,
                                    const Gfx::RectF& upButtonRect,
                                    const Gfx::RectF& downButtonRect,
                                    const SpinBoxState& state)
{
    onRenderChrome(context, rect, entryRect, upButtonRect, downButtonRect, state);
}


void SpinBoxRenderer::renderText(PaintContext& context,
                                 const Gfx::RectF& textRect,
                                 const String& text,
                                 const Gfx::PointF& textPos,
                                 const Gfx::RectF& cursor,
                                 const SpinBoxState& state)
{
    onRenderText(context, textRect, text, textPos, cursor, state);
}


void SpinBoxRenderer::onRenderChrome(PaintContext& context,
                                      const Gfx::RectF& /*rect*/,
                                      const Gfx::RectF& entryRect,
                                      const Gfx::RectF& upButtonRect,
                                      const Gfx::RectF& downButtonRect,
                                      const SpinBoxState& state)
{
    onRenderEntry(context, entryRect, state);
    onRenderUpButton(context, upButtonRect, state);
    onRenderDownButton(context, downButtonRect, state);
}


///////////////////////////////////////////////////////////////////////
// SpinBoxStyler
///////////////////////////////////////////////////////////////////////

SpinBoxStyler::SpinBoxStyler()
{
}


const Gfx::Brush& SpinBoxStyler::background() const
{
    return _options.get<TextBackgroundOption>().value();
}


void SpinBoxStyler::setBackground(const Gfx::Brush& brush)
{
    _options.set( TextBackgroundOption(brush) );
}


const Gfx::Brush& SpinBoxStyler::foreground() const
{
    return _options.get<ForegroundOption>().value();
}


void SpinBoxStyler::setForeground(const Gfx::Brush& brush)
{
    _options.set( ForegroundOption(brush) );
}


const Gfx::Pen& SpinBoxStyler::contour() const
{
    return _options.get<ContourOption>().value();
}


void SpinBoxStyler::setContour(const Gfx::Pen& pen)
{
    _options.set( ContourOption(pen) );
}


const Gfx::Color& SpinBoxStyler::textColor() const
{
    return _options.get<TextColorOption>().value();
}


void SpinBoxStyler::setTextColor(const Gfx::Color& color)
{
    _options.set( TextColorOption(color) );
}


Gfx::Font SpinBoxStyler::font() const
{
    return _options.get<FontOption>().value();
}


void SpinBoxStyler::setFont(const Gfx::Font& font)
{
    FontOption option;
    option.setFont(font);
    _options.set(option);
}


void SpinBoxStyler::setFontSize(std::size_t size)
{
    const FontOption* localFont = _options.findLocal<FontOption>();
    FontOption option = localFont ? *localFont : FontOption();
    option.setSize(size);
    _options.set(option);
}


void SpinBoxStyler::setFontWeight(Gfx::Font::Weight weight)
{
    const FontOption* localFont = _options.findLocal<FontOption>();
    FontOption option = localFont ? *localFont : FontOption();
    option.setWeight(weight);
    _options.set(option);
}


void SpinBoxStyler::setFontSlant(Gfx::Font::Slant slant)
{
    const FontOption* localFont = _options.findLocal<FontOption>();
    FontOption option = localFont ? *localFont : FontOption();
    option.setSlant(slant);
    _options.set(option);
}


Gfx::SizeF SpinBoxStyler::measureFrame(PaintSurface& surface,
                                        const Gfx::SizeF& contentSize) const
{
    if( ! _renderer )
        return contentSize;

    return _renderer->measureFrame(surface, contentSize);
}


void SpinBoxStyler::layoutChrome(PaintSurface& surface,
                                  const Gfx::RectF& rect,
                                  Gfx::RectF& entryRect,
                                  Gfx::RectF& upButtonRect,
                                  Gfx::RectF& downButtonRect,
                                  Gfx::RectF& textRect) const
{
    if( ! _renderer )
    {
        entryRect = Gfx::RectF();
        upButtonRect = Gfx::RectF();
        downButtonRect = Gfx::RectF();
        textRect = Gfx::RectF();
        return;
    }

    _renderer->layoutChrome(surface, rect, entryRect, upButtonRect,
                            downButtonRect, textRect);
}


const Painter* SpinBoxStyler::textPainter(PaintSurface& surface) const
{
    if( ! _renderer )
        return 0;

    return &_renderer->textPainter(surface);
}


void SpinBoxStyler::renderChrome(PaintContext& context,
                                  const Gfx::RectF& rect,
                                  const Gfx::RectF& entryRect,
                                  const Gfx::RectF& upButtonRect,
                                  const Gfx::RectF& downButtonRect,
                                  const SpinBoxState& state) const
{
    if( ! _renderer )
        return;

    _renderer->renderChrome(context, rect, entryRect, upButtonRect,
                            downButtonRect, state);
}


void SpinBoxStyler::renderText(PaintContext& context,
                                const Gfx::RectF& textRect,
                                const String& text,
                                const Gfx::PointF& textPos,
                                const Gfx::RectF& cursor,
                                const SpinBoxState& state) const
{
    if( ! _renderer )
        return;

    _renderer->renderText(context, textRect, text, textPos, cursor, state);
}


void SpinBoxStyler::setRenderer(SpinBoxRenderer* renderer)
{
    _renderer.reset(renderer);
    init(renderer);
}


StyleOptions& SpinBoxStyler::options()
{
    return _options;
}


const StyleOptions& SpinBoxStyler::options() const
{
    return _options;
}


StyleOptions& SpinBoxStyler::onBindOptions(const StyleOptions& global)
{
    _options.bind(&global);
    return _options;
}


Renderer* SpinBoxStyler::onStyleRenderer(const Style& style)
{
    SpinBoxRenderer* styleRenderer = style.get<SpinBoxRenderer>();
    _renderer.reset(styleRenderer);
    return _renderer.get();
}


Renderer* SpinBoxStyler::onCreateRenderer(const Style& style)
{
    SpinBoxRenderer* styleRenderer = style.get<SpinBoxRenderer>();
    _renderer.reset( styleRenderer ? styleRenderer->create() : 0 );
    return _renderer.get();
}

} // namespace

} // namespace
