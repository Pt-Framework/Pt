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

#include <Pt/Forms/ComboBoxStyle.h>
#include <Pt/Forms/StyleOptions.h>

namespace Pt {

namespace Forms {

ComboBoxStyleOptions::ComboBoxStyleOptions()
: _generation(0)
, _overrides(0)
{
}


bool ComboBoxStyleOptions::hasOverrides() const
{
    return _overrides != 0;
}


std::size_t ComboBoxStyleOptions::generation() const
{
    return _generation;
}


bool ComboBoxStyleOptions::hasOverride(StyleOverride mask) const
{
    return (_overrides & mask) != 0;
}


void ComboBoxStyleOptions::setOverride(StyleOverride mask)
{
    _overrides |= mask;
    ++_generation;
}


const Gfx::Brush* ComboBoxStyleOptions::background() const
{
    return _background.get();
}


void ComboBoxStyleOptions::setBackground(const Gfx::Brush& brush)
{
    _background.reset( new Gfx::Brush(brush) );
    setOverride(Background);
}


const Gfx::Pen* ComboBoxStyleOptions::contour() const
{
    return _contour.get();
}


void ComboBoxStyleOptions::setContour(const Gfx::Pen& pen)
{
    _contour.reset( new Gfx::Pen(pen) );
    setOverride(Contour);
}


const Gfx::Brush* ComboBoxStyleOptions::foreground() const
{
    return _foreground.get();
}


void ComboBoxStyleOptions::setForeground(const Gfx::Brush& brush)
{
    _foreground.reset( new Gfx::Brush(brush) );
    setOverride(Foreground);
}


const Gfx::Color* ComboBoxStyleOptions::textColor() const
{
    return _textColor.get();
}


void ComboBoxStyleOptions::setTextColor(const Gfx::Color& color)
{
    _textColor.reset( new Gfx::Color(color) );
    setOverride(TextColor);
}


const Gfx::Font* ComboBoxStyleOptions::font() const
{
    return _font.font();
}


void ComboBoxStyleOptions::setFont(const Gfx::Font& font)
{
    _font.setFont(font);
    setOverride(Font);
}


void ComboBoxStyleOptions::setFontSize(std::size_t size)
{
    _font.setSize(size);
    setOverride(Font);
}


void ComboBoxStyleOptions::setFontWeight(Gfx::Font::Weight weight)
{
    _font.setWeight(weight);
    setOverride(Font);
}


void ComboBoxStyleOptions::setFontSlant(Gfx::Font::Slant slant)
{
    _font.setSlant(slant);
    setOverride(Font);
}


Gfx::Font ComboBoxStyleOptions::getFont(const Gfx::Font& base) const
{
    return _font.getFont(base);
}


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
: Style::Facet( typeid(ComboBoxRenderer), refs )
{
}


ComboBoxRenderer::~ComboBoxRenderer()
{
}


ComboBoxRenderer* ComboBoxRenderer::create() const
{
    return onCreate();
}


void ComboBoxRenderer::prepare(const StyleOptions& options,
                               const ComboBoxStyleOptions& comboBoxOptions)
{
    onPrepare(options, comboBoxOptions);
}


void ComboBoxRenderer::onReset(const StyleOptions& options)
{
    ComboBoxStyleOptions empty;
    onPrepare(options, empty);
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


ComboBoxStyle::ComboBoxStyle()
{
}

} // namespace

} // namespace
