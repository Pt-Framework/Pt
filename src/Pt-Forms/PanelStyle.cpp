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

#include <Pt/Forms/PanelStyle.h>
#include <Pt/Forms/StyleOptions.h>

namespace Pt {

namespace Forms {

///////////////////////////////////////////////////////////////////////
// PanelStyleOptions
///////////////////////////////////////////////////////////////////////

PanelStyleOptions::PanelStyleOptions()
{
}


const Gfx::Brush* PanelStyleOptions::background() const
{
    return _background.get();
}


void PanelStyleOptions::setBackground(const Gfx::Brush& brush)
{
    _background.reset( new Gfx::Brush(brush) );
    setOverride(Background);
}


const Gfx::Pen* PanelStyleOptions::contour() const
{
    return _contour.get();
}


void PanelStyleOptions::setContour(const Gfx::Pen& pen)
{
    _contour.reset( new Gfx::Pen(pen) );
    setOverride(Contour);
}


const Gfx::Color* PanelStyleOptions::textColor() const
{
    return _textColor.get();
}


void PanelStyleOptions::setTextColor(const Gfx::Color& color)
{
    _textColor.reset( new Gfx::Color(color) );
    setOverride(TextColor);
}


const Gfx::Font* PanelStyleOptions::font() const
{
    return _font.font();
}


void PanelStyleOptions::setFont(const Gfx::Font& font)
{
    _font.setFont(font);
    setOverride(Font);
}


void PanelStyleOptions::setFontSize(std::size_t size)
{
    _font.setSize(size);
    setOverride(Font);
}


void PanelStyleOptions::setFontWeight(Gfx::Font::Weight weight)
{
    _font.setWeight(weight);
    setOverride(Font);
}


void PanelStyleOptions::setFontSlant(Gfx::Font::Slant slant)
{
    _font.setSlant(slant);
    setOverride(Font);
}


Gfx::Font PanelStyleOptions::getFont(const Gfx::Font& base) const
{
    return _font.getFont(base);
}

///////////////////////////////////////////////////////////////////////
// PanelState
///////////////////////////////////////////////////////////////////////

PanelState::PanelState()
: _enabled(false)
, _focused(false)
{
}


bool PanelState::isEnabled() const
{
    return _enabled;
}


void PanelState::setEnabled(bool value)
{
    _enabled = value;
}


bool PanelState::isFocused() const
{
    return _focused;
}


void PanelState::setFocused(bool value)
{
    _focused = value;
}

///////////////////////////////////////////////////////////////////////
// PanelRenderer
///////////////////////////////////////////////////////////////////////

PanelRenderer::PanelRenderer(std::size_t refs)
: Style::Facet( typeid(PanelRenderer), refs )
{
}


PanelRenderer::~PanelRenderer()
{
}


PanelRenderer* PanelRenderer::create() const
{
    return onCreate();
}


void PanelRenderer::prepare(const StyleOptions& options,
                            const PanelStyleOptions& panelOptions)
{
    onPrepare(options, panelOptions);
}


void PanelRenderer::onReset(const StyleOptions& options)
{
    PanelStyleOptions panelOptions;
    prepare(options, panelOptions);
}


Gfx::SizeF PanelRenderer::measureFrame(PaintSurface& surface,
                                       const Gfx::SizeF& contentSize)
{
    return onMeasureFrame(surface, contentSize);
}


Gfx::RectF PanelRenderer::layoutFrame(PaintSurface& surface,
                                      const Gfx::RectF& frameRect)
{
    return onLayoutFrame(surface, frameRect);
}


const Painter& PanelRenderer::textPainter(PaintSurface& surface)
{
    return onGetTextPainter(surface);
}


void PanelRenderer::renderBackground(PaintContext& context,
                                     const Gfx::RectF& rect,
                                     const PanelState& state)
{
    onRenderBackground(context, rect, state);
}


void PanelRenderer::renderFrame(PaintContext& context,
                                const Gfx::RectF& rect,
                                const PanelState& state)
{
    onRenderFrame(context, rect, state);
}


void PanelRenderer::renderText(PaintContext& context,
                               const Gfx::RectF& rect,
                               const String& text,
                               const Gfx::PointF& pos,
                               const PanelState& state)
{
    onRenderText(context, rect, text, pos, state);
}


void PanelRenderer::renderIcon(PaintContext& context,
                               const Gfx::RectF& rect,
                               const Pixmap& picture,
                               const Gfx::PointF& pos,
                               const PanelState& state)
{
    onRenderIcon(context, rect, picture, pos, state);
}

///////////////////////////////////////////////////////////////////////
// PanelStyle
///////////////////////////////////////////////////////////////////////

PanelStyle::PanelStyle()
{
}

} // namespace

} // namespace
