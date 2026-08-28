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

#include <Pt/Forms/ComboBoxStyler.h>
#include <Pt/Forms/StyleOptions.h>

namespace Pt {

namespace Forms {

ComboBoxState::ComboBoxState()
: _enabled(false)
, _focused(false)
, _highlighted(false)
, _editable(false)
, _popupVisible(false)
{
}


bool ComboBoxState::isEnabled() const
{
    return _enabled;
}


void ComboBoxState::setEnabled(bool v)
{
    _enabled = v;
}


bool ComboBoxState::isFocused() const
{
    return _focused;
}


void ComboBoxState::setFocused(bool v)
{
    _focused = v;
}


bool ComboBoxState::isHighlighted() const
{
    return _highlighted;
}


void ComboBoxState::setHighlighted(bool v)
{
    _highlighted = v;
}


bool ComboBoxState::isEditable() const
{
    return _editable;
}


void ComboBoxState::setEditable(bool v)
{
    _editable = v;
}


bool ComboBoxState::isPopupVisible() const
{
    return _popupVisible;
}


void ComboBoxState::setPopupVisible(bool v)
{
    _popupVisible = v;
}


ComboBoxButtonState::ComboBoxButtonState()
: _highlighted(false)
, _pressed(false)
{
}


bool ComboBoxButtonState::isHighlighted() const
{
    return _highlighted;
}


void ComboBoxButtonState::setHighlighted(bool v)
{
    _highlighted = v;
}


bool ComboBoxButtonState::isPressed() const
{
    return _pressed;
}


void ComboBoxButtonState::setPressed(bool v)
{
    _pressed = v;
}


ComboBoxRenderer::ComboBoxRenderer(std::size_t refs)
: Renderer( typeid(ComboBoxRenderer), refs )
{
}


ComboBoxRenderer::~ComboBoxRenderer()
{
}


ComboBoxRenderer* ComboBoxRenderer::create() const
{
    return onCreate();
}


void ComboBoxRenderer::prepare(const StyleOptions& options)
{
    onPrepare(options);
}


void ComboBoxRenderer::onReset(const StyleOptions& options)
{
    prepare(options);
}


Gfx::SizeF ComboBoxRenderer::measureFrame(PaintSurface& surface,
                                           const Gfx::SizeF& contentSize)
{
    return onMeasureFrame(surface, contentSize);
}


Gfx::SizeF ComboBoxRenderer::measureButton(PaintSurface& surface)
{
    return onMeasureButton(surface);
}


void ComboBoxRenderer::layoutChrome(PaintSurface& surface,
                                    const Gfx::RectF& rect,
                                    Gfx::RectF& entryRect,
                                    Gfx::RectF& buttonRect,
                                    Gfx::RectF& textRect)
{
    onLayoutChrome(surface, rect, entryRect, buttonRect, textRect);
}


const Painter& ComboBoxRenderer::textPainter(PaintSurface& surface)
{
    return onGetTextPainter(surface);
}


void ComboBoxRenderer::renderChrome(PaintContext& context,
                                    const Gfx::RectF& rect,
                                    const Gfx::RectF& entryRect,
                                    const Gfx::RectF& buttonRect,
                                    const ComboBoxState& state,
                                    const ComboBoxButtonState& buttonState)
{
    onRenderChrome(context, rect, entryRect, buttonRect, state, buttonState);
}


void ComboBoxRenderer::renderButton(PaintContext& context,
                                    const Gfx::RectF& buttonRect,
                                    const ComboBoxState& state,
                                    const ComboBoxButtonState& buttonState)
{
    onRenderButton(context, buttonRect, state, buttonState);
}


void ComboBoxRenderer::renderText(PaintContext& context,
                                  const Gfx::RectF& textRect,
                                  const String& text,
                                  const Gfx::PointF& textPos,
                                  const Gfx::RectF& cursor,
                                  const ComboBoxState& state)
{
    onRenderText(context, textRect, text, textPos, cursor, state);
}


void ComboBoxRenderer::onRenderChrome(PaintContext& context,
                                      const Gfx::RectF& rect,
                                      const Gfx::RectF& entryRect,
                                      const Gfx::RectF& buttonRect,
                                      const ComboBoxState& state,
                                      const ComboBoxButtonState& buttonState)
{
    onRenderEntry(context, entryRect, state);
    onRenderButton(context, buttonRect, state, buttonState);
}


ComboBoxStyler::ComboBoxStyler()
{
}


const Gfx::Brush& ComboBoxStyler::background() const
{
    return _options.get<TextBackgroundOption>().value();
}


void ComboBoxStyler::setBackground(const Gfx::Brush& brush)
{
    _options.set( TextBackgroundOption(brush) );
}


const Gfx::Brush& ComboBoxStyler::foreground() const
{
    return _options.get<ForegroundOption>().value();
}


void ComboBoxStyler::setForeground(const Gfx::Brush& brush)
{
    _options.set( ForegroundOption(brush) );
}


const Gfx::Pen& ComboBoxStyler::contour() const
{
    return _options.get<ContourOption>().value();
}


void ComboBoxStyler::setContour(const Gfx::Pen& pen)
{
    _options.set( ContourOption(pen) );
}


const Gfx::Color& ComboBoxStyler::textColor() const
{
    return _options.get<TextColorOption>().value();
}


void ComboBoxStyler::setTextColor(const Gfx::Color& color)
{
    _options.set( TextColorOption(color) );
}


Gfx::Font ComboBoxStyler::font() const
{
    return _options.get<FontOption>().value();
}


void ComboBoxStyler::setFont(const Gfx::Font& font)
{
    FontOption option;
    option.setFont(font);
    _options.set(option);
}


void ComboBoxStyler::setFontSize(std::size_t size)
{
    const FontOption* localFont = _options.findLocal<FontOption>();
    FontOption option = localFont ? *localFont : FontOption();
    option.setSize(size);
    _options.set(option);
}


void ComboBoxStyler::setFontWeight(Gfx::Font::Weight weight)
{
    const FontOption* localFont = _options.findLocal<FontOption>();
    FontOption option = localFont ? *localFont : FontOption();
    option.setWeight(weight);
    _options.set(option);
}


void ComboBoxStyler::setFontSlant(Gfx::Font::Slant slant)
{
    const FontOption* localFont = _options.findLocal<FontOption>();
    FontOption option = localFont ? *localFont : FontOption();
    option.setSlant(slant);
    _options.set(option);
}


Gfx::SizeF ComboBoxStyler::measureFrame(PaintSurface& surface,
                                         const Gfx::SizeF& contentSize) const
{
    if( ! _renderer )
        return contentSize;

    return _renderer->measureFrame(surface, contentSize);
}


void ComboBoxStyler::layoutChrome(PaintSurface& surface,
                                  const Gfx::RectF& rect,
                                  Gfx::RectF& entryRect,
                                  Gfx::RectF& buttonRect,
                                  Gfx::RectF& textRect) const
{
    if( ! _renderer )
    {
        entryRect = Gfx::RectF();
        buttonRect = Gfx::RectF();
        textRect = Gfx::RectF();
        return;
    }

    _renderer->layoutChrome(surface, rect, entryRect, buttonRect, textRect);
}


const Painter* ComboBoxStyler::textPainter(PaintSurface& surface)
{
    if( ! _renderer )
        return 0;

    return &_renderer->textPainter(surface);
}


void ComboBoxStyler::renderChrome(PaintContext& context,
                                  const Gfx::RectF& rect,
                                  const Gfx::RectF& entryRect,
                                  const Gfx::RectF& buttonRect,
                                  const ComboBoxState& state,
                                  const ComboBoxButtonState& buttonState) const
{
    if( ! _renderer )
        return;

    _renderer->renderChrome(context, rect, entryRect, buttonRect,
                            state, buttonState);
}


void ComboBoxStyler::renderText(PaintContext& context,
                                const Gfx::RectF& textRect,
                                const String& text,
                                const Gfx::PointF& textPos,
                                const Gfx::RectF& cursor,
                                const ComboBoxState& state) const
{
    if( ! _renderer )
        return;

    _renderer->renderText(context, textRect, text, textPos, cursor, state);
}


void ComboBoxStyler::setRenderer(ComboBoxRenderer* renderer)
{
    _renderer.reset(renderer);
    init(renderer);
}


StyleOptions& ComboBoxStyler::options()
{
    return _options;
}


const StyleOptions& ComboBoxStyler::options() const
{
    return _options;
}


StyleOptions& ComboBoxStyler::onBindOptions(const StyleOptions& global)
{
    _options.bind(&global);
    return _options;
}


Renderer* ComboBoxStyler::onStyleRenderer(const Style& style)
{
    ComboBoxRenderer* styleRenderer = style.get<ComboBoxRenderer>();
    _renderer.reset(styleRenderer);
    return _renderer.get();
}


Renderer* ComboBoxStyler::onCreateRenderer(const Style& style)
{
    ComboBoxRenderer* styleRenderer = style.get<ComboBoxRenderer>();
    _renderer.reset( styleRenderer ? styleRenderer->create() : 0 );
    return _renderer.get();
}

} // namespace

} // namespace
