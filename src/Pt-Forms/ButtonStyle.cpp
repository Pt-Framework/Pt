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

#include <Pt/Forms/ButtonStyle.h>
#include <Pt/Forms/StyleOptions.h>

namespace Pt {

namespace Forms {

///////////////////////////////////////////////////////////////////////
// ButtonStyleOptions
///////////////////////////////////////////////////////////////////////

ButtonStyleOptions::ButtonStyleOptions()
: _generation(0)
, _overrides(0)
{
}


bool ButtonStyleOptions::hasOverrides() const
{
    return _overrides != 0;
}


std::size_t ButtonStyleOptions::generation() const
{
    return _generation;
}


bool ButtonStyleOptions::hasOverride(StyleOverride mask) const
{
    return (_overrides & mask) != 0;
}


void ButtonStyleOptions::setOverride(StyleOverride mask)
{
    _overrides |= mask;
    ++_generation;
}


const Gfx::Brush* ButtonStyleOptions::foreground() const
{
    if( _foreground )
        return _foreground.get();

    return 0;
}


void ButtonStyleOptions::setForeground(const Gfx::Brush& brush)
{
    _foreground.reset( new Gfx::Brush(brush) );
    setOverride(ButtonStyleOptions::Foreground);
}


const Gfx::Pen* ButtonStyleOptions::contour() const
{
    if( _contour )
        return _contour.get();

    return 0;
}


void ButtonStyleOptions::setContour(const Gfx::Pen& pen)
{
    _contour.reset( new Gfx::Pen(pen) );
    setOverride(ButtonStyleOptions::Contour);
}


const Gfx::Color* ButtonStyleOptions::accentColor() const
{
    if( _accentColor )
        return _accentColor.get();

    return 0;
}


void ButtonStyleOptions::setAccentColor(const Gfx::Color& color)
{
    _accentColor.reset( new Gfx::Color(color) );
    setOverride(ButtonStyleOptions::AccentColor);
}


const Gfx::Color* ButtonStyleOptions::highlightColor() const
{
    if( _highlightColor )
        return _highlightColor.get();

    return 0;
}


void ButtonStyleOptions::setHighlightColor(const Gfx::Color& color)
{
    _highlightColor.reset( new Gfx::Color(color) );
    setOverride(ButtonStyleOptions::HighlightColor);
}


const Gfx::Color* ButtonStyleOptions::textColor() const
{
    if( _textColor )
        return _textColor.get();

    return 0;
}


void ButtonStyleOptions::setTextColor(const Gfx::Color& color)
{
    _textColor.reset( new Gfx::Color(color) );
    setOverride(ButtonStyleOptions::TextColor);
}


const Gfx::Font* ButtonStyleOptions::font() const
{
    return _font.font();
}


void ButtonStyleOptions::setFont(const Gfx::Font& font)
{
    _font.setFont(font);
    setOverride(Font);
}


void ButtonStyleOptions::setFontSize(std::size_t size)
{
    _font.setSize(size);
    setOverride(Font);
}


void ButtonStyleOptions::setFontWeight(Gfx::Font::Weight weight)
{
    _font.setWeight(weight);
    setOverride(Font);
}


void ButtonStyleOptions::setFontSlant(Gfx::Font::Slant slant)
{
    _font.setSlant(slant);
    setOverride(Font);
}


Gfx::Font ButtonStyleOptions::getFont(const Gfx::Font& base) const
{
    return _font.getFont(base);
}

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
: Style::Facet( typeid(ButtonRenderer), refs )
{
}


ButtonRenderer::~ButtonRenderer()
{
}


ButtonRenderer* ButtonRenderer::create() const
{
    return onCreate();
}


void ButtonRenderer::prepare(const StyleOptions& options,
                             const ButtonStyleOptions& buttonOptions)
{
    onPrepare(options, buttonOptions);
}


void ButtonRenderer::onReset(const StyleOptions& options)
{
    ButtonStyleOptions buttonOptions;
    prepare(options, buttonOptions);
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

} // namespace

} // namespace
