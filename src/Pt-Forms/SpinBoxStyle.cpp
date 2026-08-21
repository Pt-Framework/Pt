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

SpinBoxStyleOptions::SpinBoxStyleOptions()
{
}


const Gfx::Brush* SpinBoxStyleOptions::background() const
{
    return _background.get();
}


void SpinBoxStyleOptions::setBackground(const Gfx::Brush& brush)
{
    _background.reset( new Gfx::Brush(brush) );
    setOverride(Background);
}


const Gfx::Pen* SpinBoxStyleOptions::contour() const
{
    return _contour.get();
}


void SpinBoxStyleOptions::setContour(const Gfx::Pen& pen)
{
    _contour.reset( new Gfx::Pen(pen) );
    setOverride(Contour);
}


const Gfx::Brush* SpinBoxStyleOptions::foreground() const
{
    return _foreground.get();
}


void SpinBoxStyleOptions::setForeground(const Gfx::Brush& brush)
{
    _foreground.reset( new Gfx::Brush(brush) );
    setOverride(Foreground);
}


const Gfx::Color* SpinBoxStyleOptions::textColor() const
{
    return _textColor.get();
}


void SpinBoxStyleOptions::setTextColor(const Gfx::Color& color)
{
    _textColor.reset( new Gfx::Color(color) );
    setOverride(TextColor);
}


const Gfx::Font* SpinBoxStyleOptions::font() const
{
    return _font.font();
}


void SpinBoxStyleOptions::setFont(const Gfx::Font& font)
{
    _font.setFont(font);
    setOverride(Font);
}


void SpinBoxStyleOptions::setFontSize(std::size_t size)
{
    _font.setSize(size);
    setOverride(Font);
}


void SpinBoxStyleOptions::setFontWeight(Gfx::Font::Weight weight)
{
    _font.setWeight(weight);
    setOverride(Font);
}


void SpinBoxStyleOptions::setFontSlant(Gfx::Font::Slant slant)
{
    _font.setSlant(slant);
    setOverride(Font);
}


Gfx::Font SpinBoxStyleOptions::getFont(const Gfx::Font& base) const
{
    return _font.getFont(base);
}


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
: Style::Facet( typeid(SpinBoxRenderer), refs )
{
}


SpinBoxRenderer::~SpinBoxRenderer()
{
}


SpinBoxRenderer* SpinBoxRenderer::create() const
{
    return onCreate();
}


void SpinBoxRenderer::prepare(const StyleOptions& options,
                              const SpinBoxStyleOptions& spinBoxOptions)
{
    onPrepare(options, spinBoxOptions);
}


void SpinBoxRenderer::onReset(const StyleOptions& options)
{
    SpinBoxStyleOptions empty;
    onPrepare(options, empty);
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


SpinBoxStyle::SpinBoxStyle()
{
}

} // namespace

} // namespace
