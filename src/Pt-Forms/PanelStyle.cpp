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

PanelStyleOptions::PanelStyleOptions()
: _generation(0)
, _overrides(0)
{
}


bool PanelStyleOptions::hasOverrides() const
{
    return _overrides != 0;
}


std::size_t PanelStyleOptions::generation() const
{
    return _generation;
}


bool PanelStyleOptions::hasOverride(StyleOverride mask) const
{
    return (_overrides & mask) != 0;
}


void PanelStyleOptions::setOverride(StyleOverride mask)
{
    _overrides |= mask;
    ++_generation;
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
    return _font.get();
}


void PanelStyleOptions::setFont(const Gfx::Font& font)
{
    _font.reset( new Gfx::Font(font) );
    setOverride(FontAll);
}


void PanelStyleOptions::setFontSize(std::size_t size)
{
    if( ! _font )
        _font.reset( new Gfx::Font );

    *_font = _font->withSize(size);
    setOverride(FontSize);
}


void PanelStyleOptions::setFontWeight(Gfx::Font::Weight weight)
{
    if( ! _font )
        _font.reset( new Gfx::Font );

    *_font = _font->withWeight(weight);
    setOverride(FontWeight);
}


void PanelStyleOptions::setFontSlant(Gfx::Font::Slant slant)
{
    if( ! _font )
        _font.reset( new Gfx::Font );

    *_font = _font->withSlant(slant);
    setOverride(FontSlant);
}


Gfx::Font PanelStyleOptions::getFont(const Gfx::Font& base) const
{
    if( ! _font )
        return base;

    if( hasOverride(FontAll) )
        return *_font;

    Gfx::Font font(base);

    if( hasOverride(FontSize) )
        font = font.withSize(_font->size());

    if( hasOverride(FontWeight) )
        font = font.withWeight(_font->weight());

    if( hasOverride(FontSlant) )
        font = font.withSlant(_font->slant());

    return font;
}


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
PanelStyle::PanelStyle()
: _binding(Style)
, _boundStyleGeneration( std::size_t(-1) )
, _styleOptionsGeneration( std::size_t(-1) )
, _panelOptionsGeneration( std::size_t(-1) )
{
}


bool PanelStyle::isBound() const
{
    return _renderer != 0;
}


bool PanelStyle::isCustom() const
{
    return _binding == Custom;
}


PanelRenderer* PanelStyle::bind(const Pt::Forms::Style& style,
                                const StyleOptions& options,
                                const PanelStyleOptions& panelOptions)
{
    _styleOptionsGeneration = std::size_t(-1);
    _panelOptionsGeneration = std::size_t(-1);

    if( panelOptions.hasOverrides() )
    {
        PanelRenderer* renderer = style.get<PanelRenderer>();
        if( renderer )
            renderer = renderer->create();

        _renderer.reset(renderer);
        _binding = Override;

        if( _renderer )
        {
            _renderer->prepare(options, panelOptions);
            _styleOptionsGeneration = options.generation();
            _panelOptionsGeneration = panelOptions.generation();
        }
    }
    else
    {
        _renderer.reset( style.get<PanelRenderer>() );
        _binding = Style;
    }

    _boundStyleGeneration = style.generation();

    return _renderer.get();
}


PanelRenderer* PanelStyle::bind(PanelRenderer& renderer,
                                const StyleOptions& options,
                                const PanelStyleOptions& panelOptions)
{
    _renderer.reset(&renderer);

    _binding = Custom;
    _boundStyleGeneration = std::size_t(-1);
    _styleOptionsGeneration = std::size_t(-1);
    _panelOptionsGeneration = std::size_t(-1);

    _renderer->prepare(options, panelOptions);

    _styleOptionsGeneration = options.generation();
    _panelOptionsGeneration = panelOptions.generation();

    return _renderer.get();
}


PanelRenderer* PanelStyle::rebind(const Pt::Forms::Style& style,
                                  const StyleOptions& options,
                                  const PanelStyleOptions& panelOptions)
{
    if( ! _renderer || _binding != Custom )
        return bind(style, options, panelOptions);

    bool needsPrepare = _styleOptionsGeneration != options.generation() ||
                        _panelOptionsGeneration != panelOptions.generation();

    if( needsPrepare )
        _renderer->prepare(options, panelOptions);

    _styleOptionsGeneration = options.generation();
    _panelOptionsGeneration = panelOptions.generation();

    return _renderer.get();
}


PanelRenderer* PanelStyle::renderer()
{
    return _renderer.get();
}


const PanelRenderer* PanelStyle::renderer() const
{
    return _renderer.get();
}

} // namespace

} // namespace