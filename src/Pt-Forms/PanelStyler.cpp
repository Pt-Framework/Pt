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

#include <Pt/Forms/PanelStyler.h>
#include <Pt/Forms/StyleOptions.h>
#include <Pt/Forms/Style.h>

namespace Pt {

namespace Forms {

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
: Renderer( typeid(PanelRenderer), refs )
{
}


PanelRenderer::~PanelRenderer()
{
}


PanelRenderer* PanelRenderer::create() const
{
    return onCreate();
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
// PanelStyler
///////////////////////////////////////////////////////////////////////

PanelStyler::PanelStyler()
{
}


const Gfx::Brush& PanelStyler::background() const
{
    return _options.get<BackgroundOption>().value();
}


void PanelStyler::setBackground(const Gfx::Brush& brush)
{
    _options.set( BackgroundOption(brush) );
}


const Gfx::Pen& PanelStyler::contour() const
{
    return _options.get<ContourOption>().value();
}


void PanelStyler::setContour(const Gfx::Pen& pen)
{
    _options.set( ContourOption(pen) );
}


const Gfx::Color& PanelStyler::textColor() const
{
    return _options.get<TextColorOption>().value();
}


void PanelStyler::setTextColor(const Gfx::Color& color)
{
    _options.set( TextColorOption(color) );
}


Gfx::Font PanelStyler::font() const
{
    return _options.get<FontOption>().value();
}


void PanelStyler::setFont(const Gfx::Font& font)
{
    FontOption option;
    option.setFont(font);
    _options.set(option);
}


void PanelStyler::setFontSize(std::size_t size)
{
    const FontOption* localFont = _options.findLocal<FontOption>();
    FontOption option = localFont ? *localFont : FontOption();
    option.setSize(size);
    _options.set(option);
}


void PanelStyler::setFontWeight(Gfx::Font::Weight weight)
{
    const FontOption* localFont = _options.findLocal<FontOption>();
    FontOption option = localFont ? *localFont : FontOption();
    option.setWeight(weight);
    _options.set(option);
}


void PanelStyler::setFontSlant(Gfx::Font::Slant slant)
{
    const FontOption* localFont = _options.findLocal<FontOption>();
    FontOption option = localFont ? *localFont : FontOption();
    option.setSlant(slant);
    _options.set(option);
}


Gfx::SizeF PanelStyler::measureFrame(PaintSurface& surface,
                                     const Gfx::SizeF& contentSize) const
{
    if( ! _renderer )
        return contentSize;

    return _renderer->measureFrame(surface, contentSize);
}


Gfx::RectF PanelStyler::layoutFrame(PaintSurface& surface,
                                    const Gfx::RectF& frameRect) const
{
    if( ! _renderer )
        return frameRect;

    return _renderer->layoutFrame(surface, frameRect);
}


const Painter* PanelStyler::textPainter(PaintSurface& surface) const
{
    if( ! _renderer )
        return 0;

    return &_renderer->textPainter(surface);
}


void PanelStyler::renderBackground(PaintContext& context,
                                   const Gfx::RectF& rect,
                                   const PanelState& state) const
{
    if( ! _renderer )
        return;

    _renderer->renderBackground(context, rect, state);
}


void PanelStyler::renderFrame(PaintContext& context,
                              const Gfx::RectF& rect,
                              const PanelState& state) const
{
    if( ! _renderer )
        return;

    _renderer->renderFrame(context, rect, state);
}


void PanelStyler::renderText(PaintContext& context,
                             const Gfx::RectF& rect,
                             const String& text,
                             const Gfx::PointF& pos,
                             const PanelState& state) const
{
    if( ! _renderer )
        return;

    _renderer->renderText(context, rect, text, pos, state);
}


void PanelStyler::renderIcon(PaintContext& context,
                             const Gfx::RectF& rect,
                             const Pixmap& picture,
                             const Gfx::PointF& pos,
                             const PanelState& state) const
{
    if( ! _renderer )
        return;

    _renderer->renderIcon(context, rect, picture, pos, state);
}


void PanelStyler::setRenderer(PanelRenderer* renderer)
{
    _renderer.reset(renderer);
    init(renderer);
}


StyleOptions& PanelStyler::options()
{
    return _options;
}


const StyleOptions& PanelStyler::options() const
{
    return _options;
}


StyleOptions& PanelStyler::onBindOptions(const StyleOptions& global)
{
    _options.bind(&global);
    return _options;
}


Renderer* PanelStyler::onStyleRenderer(const Style& style)
{
    PanelRenderer* styleRenderer = style.get<PanelRenderer>();
    _renderer.reset(styleRenderer);
    return _renderer.get();
}


Renderer* PanelStyler::onCreateRenderer(const Style& style)
{
    PanelRenderer* styleRenderer = style.get<PanelRenderer>();
    _renderer.reset( styleRenderer ? styleRenderer->create() : 0 );
    return _renderer.get();
}

} // namespace

} // namespace
