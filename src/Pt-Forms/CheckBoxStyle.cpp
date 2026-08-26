/* Copyright (C) 2016 Laurentiu-Gheorghe Crisan
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
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 MA 02110-1301 USA
*/

#include <Pt/Forms/CheckBoxStyle.h>
#include <Pt/Forms/StyleOptions.h>
#include <Pt/Forms/Style.h>

namespace Pt {

namespace Forms {

CheckBoxState::CheckBoxState()
: _enabled(false)
, _hovered(false)
, _focused(false)
, _checked(false)
{
}


bool CheckBoxState::isEnabled() const
{
    return _enabled;
}


void CheckBoxState::setEnabled(bool value)
{
    _enabled = value;
}


bool CheckBoxState::isHovered() const
{
    return _hovered;
}


void CheckBoxState::setHovered(bool value)
{
    _hovered = value;
}


bool CheckBoxState::isFocused() const
{
    return _focused;
}


void CheckBoxState::setFocused(bool value)
{
    _focused = value;
}


bool CheckBoxState::isChecked() const
{
    return _checked;
}


void CheckBoxState::setChecked(bool value)
{
    _checked = value;
}


CheckBoxRenderer::CheckBoxRenderer(std::size_t refs)
: Renderer( typeid(CheckBoxRenderer), refs )
{
}


CheckBoxRenderer::~CheckBoxRenderer()
{
}


CheckBoxRenderer* CheckBoxRenderer::create() const
{
    return onCreate();
}


void CheckBoxRenderer::prepare(const StyleOptions& options)
{
    onPrepare(options);
}


void CheckBoxRenderer::onReset(const StyleOptions& options)
{
    prepare(options);
}


Gfx::SizeF CheckBoxRenderer::measureIndicator(PaintSurface& surface)
{
    return onMeasureIndicator(surface);
}


Gfx::SizeF CheckBoxRenderer::measureContent(PaintSurface& surface,
                                            const Gfx::SizeF& indicatorSize,
                                            const Gfx::SizeF& textSize)
{
    return onMeasureContent(surface, indicatorSize, textSize);
}


Gfx::SizeF CheckBoxRenderer::measureFrame(PaintSurface& surface,
                                          const Gfx::SizeF& contentSize)
{
    return onMeasureFrame(surface, contentSize);
}


Gfx::RectF CheckBoxRenderer::layoutFrame(PaintSurface& surface,
                                         const Gfx::RectF& frameRect)
{
    return onLayoutFrame(surface, frameRect);
}


void CheckBoxRenderer::layoutContent(PaintSurface& surface,
                                     const Gfx::RectF& contentRect,
                                     const Gfx::SizeF& indicatorSize,
                                     const Gfx::SizeF& textSize,
                                     Gfx::RectF& indicatorRect,
                                     Gfx::RectF& textRect)
{
    onLayoutContent(surface, contentRect, indicatorSize, textSize,
                    indicatorRect, textRect);
}


Gfx::RectF CheckBoxRenderer::layoutMnemonic(PaintSurface& surface,
                                            const String& text,
                                            const Gfx::PointF& textPos,
                                            const Gfx::FontMetrics& fontMetrics,
                                            String::size_type mnemonicIndex)
{
    return onLayoutMnemonic(surface, text, textPos, fontMetrics, mnemonicIndex);
}


const Painter& CheckBoxRenderer::textPainter(PaintSurface& surface)
{
    return onGetTextPainter(surface);
}


void CheckBoxRenderer::renderChrome(PaintContext& context,
                                    const Gfx::RectF& rect,
                                    const Gfx::RectF& boxRect,
                                    const CheckBoxState& state)
{
    onRenderChrome(context, rect, boxRect, state);
}


void CheckBoxRenderer::renderText(PaintContext& context,
                                  const Gfx::RectF& textRect,
                                  const String& text,
                                  const Gfx::PointF& pos,
                                  const CheckBoxState& state)
{
    onRenderText(context, textRect, text, pos, state);
}


void CheckBoxRenderer::renderMnemonic(PaintContext& context,
                                      const Gfx::RectF& rect,
                                      const Gfx::RectF& mnemonic,
                                      const CheckBoxState& state)
{
    onRenderMnemonic(context, rect, mnemonic, state);
}


CheckBoxStyler::CheckBoxStyler()
{
}


const Gfx::Brush& CheckBoxStyler::background() const
{
    return _options.get<TextBackgroundOption>().value();
}


void CheckBoxStyler::setBackground(const Gfx::Brush& brush)
{
    _options.set( TextBackgroundOption(brush) );
}


const Gfx::Pen& CheckBoxStyler::contour() const
{
    return _options.get<ContourOption>().value();
}


void CheckBoxStyler::setContour(const Gfx::Pen& pen)
{
    _options.set( ContourOption(pen) );
}


const Gfx::Color& CheckBoxStyler::textColor() const
{
    return _options.get<TextColorOption>().value();
}


void CheckBoxStyler::setTextColor(const Gfx::Color& color)
{
    _options.set( TextColorOption(color) );
}


Gfx::Font CheckBoxStyler::font() const
{
    return _options.get<FontOption>().value();
}


void CheckBoxStyler::setFont(const Gfx::Font& font)
{
    FontOption option;
    option.setFont(font);
    _options.set(option);
}


void CheckBoxStyler::setFontSize(std::size_t size)
{
    const FontOption* localFont = _options.findLocal<FontOption>();
    FontOption option = localFont ? *localFont : FontOption();
    option.setSize(size);
    _options.set(option);
}


void CheckBoxStyler::setFontWeight(Gfx::Font::Weight weight)
{
    const FontOption* localFont = _options.findLocal<FontOption>();
    FontOption option = localFont ? *localFont : FontOption();
    option.setWeight(weight);
    _options.set(option);
}


void CheckBoxStyler::setFontSlant(Gfx::Font::Slant slant)
{
    const FontOption* localFont = _options.findLocal<FontOption>();
    FontOption option = localFont ? *localFont : FontOption();
    option.setSlant(slant);
    _options.set(option);
}


void CheckBoxStyler::setRenderer(CheckBoxRenderer* renderer)
{
    _renderer.reset(renderer);
    init(renderer);
}


CheckBoxRenderer* CheckBoxStyler::renderer()
{
    return _renderer.get();
}


StyleOptions& CheckBoxStyler::options()
{
    return _options;
}


const StyleOptions& CheckBoxStyler::options() const
{
    return _options;
}


StyleOptions& CheckBoxStyler::onBindOptions(const StyleOptions& global)
{
    _options.bind(&global);
    return _options;
}


Renderer* CheckBoxStyler::onStyleRenderer(const Style& style)
{
    CheckBoxRenderer* styleRenderer = style.get<CheckBoxRenderer>();
    _renderer.reset(styleRenderer);
    return _renderer.get();
}


Renderer* CheckBoxStyler::onCreateRenderer(const Style& style)
{
    CheckBoxRenderer* styleRenderer = style.get<CheckBoxRenderer>();
    _renderer.reset( styleRenderer ? styleRenderer->create() : 0 );
    return _renderer.get();
}

} // namespace

} // namespace
