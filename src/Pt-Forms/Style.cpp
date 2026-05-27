/* Copyright (C) 2016 Marc Boris Duerner 
   Copyright (C) 2016 Laurentiu-Gheorghe Crisan
   Copyright (C) 2017 Ilja Maier

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
   MA  02110-1301  USA
*/

#include <Pt/Forms/Style.h>
#include <Pt/Forms/StyleOptions.h>
#include <Pt/Forms/Application.h>
#include <Pt/Forms/PaintContext.h>
#include <Pt/Forms/Painter.h>
#include <Pt/Forms/Pixmap.h>
#include <Pt/Forms/TextBlock.h>

#include <algorithm>

namespace Pt {

namespace Forms {

namespace {

std::size_t nextStyleGeneration()
{
    static std::size_t generation = 0;
    return ++generation;
}

}

///////////////////////////////////////////////////////////////////////////////
// Style
///////////////////////////////////////////////////////////////////////////////

Style::Style()
: _generation( nextStyleGeneration() )
{
}


Style::Style(const Style& style)
{
    assign(style);
}


Style::~Style()
{
    FacetMap::iterator it;
    for(it = _facets.begin(); it != _facets.end(); ++it)
    {
        Facet* facet = it->second;
        if( 0 == facet->unref() )
            delete facet;
    }
}


Style& Style::operator=(const Style& style)
{
    assign(style);
    return *this;
}


void Style::assign(const Style& style)
{
    if(this == &style)
        return;

    for(FacetMap::iterator it = _facets.begin(); it != _facets.end(); ++it)
    {
        Facet* facet = it->second;
        
        if( 0 == facet->unref() )
            delete facet;
    }

    _facets.clear();
    _generation = nextStyleGeneration();

    FacetMap::const_iterator cit;
    for(cit = style._facets.begin(); cit != style._facets.end(); ++cit)
    {
        set(cit->second);
    }
}


void Style::combine(const Style& style)
{
    if(this == &style)
        return;

    _facets.clear();
    _generation = nextStyleGeneration();

    FacetMap::const_iterator it;
    for(it = style._facets.begin(); it != style._facets.end(); ++it)
    {
        set(it->second);
    }
}


void Style::set(Facet* facet)
{
    if( ! facet )
        return;

    FacetMap::value_type val(facet->typeId(), facet);
    
    std::pair<FacetMap::iterator, bool> r = _facets.insert(val);
    if( ! r.second )
    {
        Facet* prev = r.first->second;
        if( 0 == prev->unref() )
            delete prev;

        r.first->second = facet;
    }

    facet->ref();
    _generation = nextStyleGeneration();
}


Style::Facet* Style::find(const std::type_info& ti) const
{
    FacetMap::const_iterator it = _facets.find(ti);
    if( it == _facets.end() )
        return 0;

    return it->second;
}

///////////////////////////////////////////////////////////////////////////////
// PanelRenderer
///////////////////////////////////////////////////////////////////////////////

PanelRenderer::PanelRenderer(std::size_t refs)
: Style::Facet( typeid(PanelRenderer), refs )
, _styleGeneration( std::size_t(-1) )
{
}


PanelRenderer::~PanelRenderer()
{
}


PanelRenderer* PanelRenderer::create() const
{
    return onCreate();
}


const Gfx::Brush& PanelRenderer::background() const
{
    if( _background )
        return *_background;

    return Application::instance().styleOptions().background();
}


void PanelRenderer::setBackground(const Gfx::Brush& b)
{
    _background.reset( new Gfx::Brush(b) );
    _styleGeneration = std::size_t(-1);
}


const Gfx::Pen& PanelRenderer::contour() const
{
    if( _contour )
        return *_contour;

    return Application::instance().styleOptions().contour();
}


void PanelRenderer::setContour(const Gfx::Pen& p)
{
    _contour.reset( new Gfx::Pen(p) );
    _styleGeneration = std::size_t(-1);
}


const Gfx::Font& PanelRenderer::font() const
{
    if( _font )
        return *_font;

    return Application::instance().styleOptions().font();
}


void PanelRenderer::setFont(const Gfx::Font& f)
{
    _font.reset( new Gfx::Font(f) );
    _styleGeneration = std::size_t(-1);
}


const Gfx::Color& PanelRenderer::textColor() const
{
    if( _textColor )
        return _textColor->color();

    return Application::instance().styleOptions().textColor();
}


void PanelRenderer::setTextColor(const Gfx::Pen& p)
{
    _textColor.reset( new Gfx::Pen(p) );
    _styleGeneration = std::size_t(-1);
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
    prepare();
    return onGetTextPainter(surface);
}


void PanelRenderer::renderBackground(PaintContext& context,
                                     const Gfx::RectF& rect,
                                     StyleFlags state)
{
    const StyleOptions& opts = prepare();
    onRenderBackground(context, rect, opts, state);
}


void PanelRenderer::renderFrame(PaintContext& context,
                                const Gfx::RectF& rect,
                                StyleFlags state)
{
    const StyleOptions& opts = prepare();
    onRenderFrame(context, rect, opts, state);
}


void PanelRenderer::renderText(PaintContext& context,
                               const Gfx::RectF& rect,
                               const String& text,
                               const Gfx::PointF& pos,
                               StyleFlags state)
{
    const StyleOptions& opts = prepare();
    onRenderText(context, rect, opts, text, pos, state);
}


void PanelRenderer::renderIcon(PaintContext& context,
                               const Gfx::RectF& rect,
                               const Pixmap& picture,
                               const Gfx::PointF& pos,
                               StyleFlags state)
{
    const StyleOptions& opts = prepare();
    onRenderIcon(context, rect, opts, picture, pos, state);
}


const StyleOptions& PanelRenderer::prepare()
{
    const StyleOptions& opts = Application::instance().styleOptions();

    if( _styleGeneration != opts.generation() )
    {
        _styleGeneration = opts.generation();
        onPrepare(opts);
    }

    return opts;
}

///////////////////////////////////////////////////////////////////////////////
// ButtonRenderer
///////////////////////////////////////////////////////////////////////////////

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
    return _font.get();
}


void ButtonStyleOptions::setFont(const Gfx::Font& font)
{
    _font.reset( new Gfx::Font(font) );
    setOverride(ButtonStyleOptions::FontAll);
}


void ButtonStyleOptions::setFontSize(std::size_t size)
{
    if( ! _font )
        _font.reset( new Gfx::Font );

    *_font = _font->withSize(size);
    setOverride(ButtonStyleOptions::FontSize);
}


void ButtonStyleOptions::setFontWeight(Gfx::Font::Weight weight)
{
    if( ! _font )
        _font.reset( new Gfx::Font );

    *_font = _font->withWeight(weight);
    setOverride(ButtonStyleOptions::FontWeight);
}


void ButtonStyleOptions::setFontSlant(Gfx::Font::Slant slant)
{
    if( ! _font )
        _font.reset( new Gfx::Font );

    *_font = _font->withSlant(slant);
    setOverride(ButtonStyleOptions::FontSlant);
}


Gfx::Font ButtonStyleOptions::getFont(const Gfx::Font& base) const
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
    onLayoutContent(surface, rect, direction, iconSize, textSize, iconRect, textRect);
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


ButtonStyle::ButtonStyle()
: _binding(Style)
, _boundStyleGeneration( std::size_t(-1) )
, _styleOptionsGeneration( std::size_t(-1) )
, _buttonOptionsGeneration( std::size_t(-1) )
{
}


bool ButtonStyle::isBound() const
{
    return _renderer != 0;
}


bool ButtonStyle::isCustom() const
{
    return _binding == Custom;
}


ButtonRenderer* ButtonStyle::bind(const Pt::Forms::Style& style,
                                  const StyleOptions& options,
                                  const ButtonStyleOptions& buttonOptions)
{
    const bool hasOverrides = buttonOptions.hasOverrides();
    const Binding targetBinding = hasOverrides ? Override : Style;

    const bool needsRebind = ! _renderer ||
                             _binding != targetBinding ||
                             _boundStyleGeneration != style.generation();

    const bool needsPrepare = needsRebind ||
                              _styleOptionsGeneration != options.generation() ||
                              _buttonOptionsGeneration != buttonOptions.generation();

    if(needsRebind)
    {
        ButtonRenderer* renderer = style.get<ButtonRenderer>();

        if( hasOverrides && renderer )
            renderer = renderer->create();

        _renderer.reset(renderer);
    }

    _boundStyleGeneration = style.generation();

    if( ! _renderer )
    {
        _binding = Style;
        return 0;
    }

    _binding = targetBinding;

    if( needsPrepare )
        _renderer->prepare(options, buttonOptions);

    _styleOptionsGeneration = options.generation();
    _buttonOptionsGeneration = buttonOptions.generation();

    return _renderer.get();
}


ButtonRenderer* ButtonStyle::bind(ButtonRenderer* renderer,
                                  const StyleOptions& options,
                                  const ButtonStyleOptions& buttonOptions)
{
    _renderer.reset(renderer);
    _boundStyleGeneration = std::size_t(-1);

    if( ! _renderer )
    {
        _binding = Style;
        _styleOptionsGeneration = std::size_t(-1);
        _buttonOptionsGeneration = std::size_t(-1);
        return 0;
    }

    _binding = Custom;

    _renderer->prepare(options, buttonOptions);

    _styleOptionsGeneration = options.generation();
    _buttonOptionsGeneration = buttonOptions.generation();

    return _renderer.get();
}


ButtonRenderer* ButtonStyle::rebind(const StyleOptions& options,
                                    const ButtonStyleOptions& buttonOptions)
{
    if( ! _renderer || _binding != Custom )
        return 0;

    const bool needsPrepare = _styleOptionsGeneration != options.generation() ||
                              _buttonOptionsGeneration != buttonOptions.generation();

    if( needsPrepare )
        _renderer->prepare(options, buttonOptions);

    _styleOptionsGeneration = options.generation();
    _buttonOptionsGeneration = buttonOptions.generation();

    return _renderer.get();
}


ButtonRenderer* ButtonStyle::renderer()
{
    return _renderer.get();
}


const ButtonRenderer* ButtonStyle::renderer() const
{
    return _renderer.get();
}

///////////////////////////////////////////////////////////////////////////////
// CheckBoxRenderer
///////////////////////////////////////////////////////////////////////////////

CheckBoxRenderer::CheckBoxRenderer(std::size_t refs)
: Style::Facet( typeid(CheckBoxRenderer), refs )
, _styleGeneration( std::size_t(-1) )
{
}

    
CheckBoxRenderer::~CheckBoxRenderer()
{
}


CheckBoxRenderer* CheckBoxRenderer::create() const
{
    return onCreate();
}


const Gfx::Brush* CheckBoxRenderer::background() const
{
    return _background.get();
}


void CheckBoxRenderer::setBackground(const Gfx::Brush& b)
{
    _background.reset( new Gfx::Brush(b) );
    _styleGeneration = std::size_t(-1);
}


const Gfx::Pen* CheckBoxRenderer::contour() const
{
    return _contour.get();
}


void CheckBoxRenderer::setContour(const Gfx::Pen& p)
{
    _contour.reset( new Gfx::Pen(p) );
    _styleGeneration = std::size_t(-1);
}


const Gfx::Font& CheckBoxRenderer::font() const
{
    if( _font )
        return *_font;

    return Application::instance().styleOptions().font();
}


void CheckBoxRenderer::setFont(const Gfx::Font& f)
{
    _font.reset( new Gfx::Font(f) );
    _styleGeneration = std::size_t(-1);
}


const Gfx::Color& CheckBoxRenderer::textColor() const
{
    if( _textColor )
        return _textColor->color();

    return Application::instance().styleOptions().textColor();
}


void CheckBoxRenderer::setTextColor(const Gfx::Pen& p)
{
    _textColor.reset( new Gfx::Pen(p) );
    _styleGeneration = std::size_t(-1);
}


Gfx::SizeF CheckBoxRenderer::measureIndicator(PaintSurface& surface)
{
    prepare();
    return onMeasureIndicator(surface);
}


Gfx::SizeF CheckBoxRenderer::measureContent(PaintSurface& surface,
                                            const Gfx::SizeF& indicatorSize,
                                            const Gfx::SizeF& textSize)
{
    prepare();
    return onMeasureContent(surface, indicatorSize, textSize);
}


Gfx::SizeF CheckBoxRenderer::measureFrame(PaintSurface& surface,
                                          const Gfx::SizeF& contentSize)
{
    prepare();
    return onMeasureFrame(surface, contentSize);
}


Gfx::RectF CheckBoxRenderer::layoutFrame(PaintSurface& surface,
                                         const Gfx::RectF& frameRect)
{
    prepare();
    return onLayoutFrame(surface, frameRect);
}


void CheckBoxRenderer::layoutContent(PaintSurface& surface,
                                     const Gfx::RectF& contentRect,
                                     const Gfx::SizeF& indicatorSize,
                                     const Gfx::SizeF& textSize,
                                     Gfx::RectF& indicatorRect,
                                     Gfx::RectF& textRect)
{
    prepare();
    onLayoutContent(surface, contentRect, indicatorSize, textSize,
                    indicatorRect, textRect);
}


Gfx::RectF CheckBoxRenderer::layoutMnemonic(PaintSurface& surface,
                                            const String& text,
                                            const Gfx::PointF& textPos,
                                            const Gfx::FontMetrics& fontMetrics,
                                            String::size_type mnemonicIndex)
{
    prepare();
    return onLayoutMnemonic(surface, text, textPos, fontMetrics, mnemonicIndex);
}


const Painter& CheckBoxRenderer::textPainter(PaintSurface& surface)
{
    prepare();
    return onGetTextPainter(surface);
}


void CheckBoxRenderer::renderChrome(PaintContext& context,
                                       const Gfx::RectF& rect,
                                       const Gfx::RectF& boxRect,
                                       CheckBoxStyleFlags state)
{
    const StyleOptions& opts = prepare();
    onRenderChrome(context, rect, opts, boxRect, state);
}


void CheckBoxRenderer::renderText(PaintContext& context,
                                  const Gfx::RectF& textRect,
                                  const String& text,
                                  const Gfx::PointF& pos,
                                  CheckBoxStyleFlags state)
{
    const StyleOptions& opts = prepare();
    onRenderText(context, textRect, opts, text, pos, state);
}


void CheckBoxRenderer::renderMnemonic(PaintContext& context,
                                      const Gfx::RectF& rect,
                                      const Gfx::RectF& mnemonic,
                                      CheckBoxStyleFlags state)
{
    const StyleOptions& opts = prepare();
    onRenderMnemonic(context, rect, opts, mnemonic, state);
}


const StyleOptions& CheckBoxRenderer::prepare()
{
    const StyleOptions& opts = Application::instance().styleOptions();

    if( _styleGeneration != opts.generation() )
    {
        _styleGeneration = opts.generation();
        onPrepare(opts);
    }

    return opts;
}

///////////////////////////////////////////////////////////////////////////////
// SpinBoxRenderer
///////////////////////////////////////////////////////////////////////////////

SpinBoxRenderer::SpinBoxRenderer(std::size_t refs)
: Style::Facet( typeid(SpinBoxRenderer), refs )
, _styleGeneration( std::size_t(-1) )
{
}


SpinBoxRenderer::~SpinBoxRenderer()
{
}


SpinBoxRenderer* SpinBoxRenderer::create() const
{
    return onCreate();
}


const Gfx::Brush* SpinBoxRenderer::background() const
{
    return _background.get();
}


void SpinBoxRenderer::setBackground(const Gfx::Brush& b)
{
    _background.reset( new Gfx::Brush(b) );
    _styleGeneration = std::size_t(-1);
}


const Gfx::Pen* SpinBoxRenderer::contour() const
{
    return _contour.get();
}


void SpinBoxRenderer::setContour(const Gfx::Pen& p)
{
    _contour.reset( new Gfx::Pen(p) );
    _styleGeneration = std::size_t(-1);
}


const Gfx::Brush* SpinBoxRenderer::foreground() const
{
    return _foreground.get();
}


void SpinBoxRenderer::setForeground(const Gfx::Brush& b)
{
    _foreground.reset( new Gfx::Brush(b) );
    _styleGeneration = std::size_t(-1);
}


const Gfx::Font& SpinBoxRenderer::font() const
{
    if( _font )
        return *_font;

    return Application::instance().styleOptions().font();
}


void SpinBoxRenderer::setFont(const Gfx::Font& f)
{
    _font.reset( new Gfx::Font(f) );
    _styleGeneration = std::size_t(-1);
}


const Gfx::Color& SpinBoxRenderer::textColor() const
{
    if( _textColor )
        return _textColor->color();

    return Application::instance().styleOptions().textColor();
}


void SpinBoxRenderer::setTextColor(const Gfx::Pen& p)
{
    _textColor.reset( new Gfx::Pen(p) );
    _styleGeneration = std::size_t(-1);
}


Gfx::SizeF SpinBoxRenderer::measureFrame(PaintSurface& surface,
                                         const Gfx::SizeF& contentSize)
{
    prepare();
    return onMeasureFrame(surface, contentSize);
}


Gfx::SizeF SpinBoxRenderer::measureEntry(PaintSurface& surface,
                                          const Gfx::SizeF& contentSize)
{
    prepare();
    return onMeasureEntry(surface, contentSize);
}


Gfx::SizeF SpinBoxRenderer::measureIndicator(PaintSurface& surface)
{
    prepare();
    return onMeasureIndicator(surface);
}


void SpinBoxRenderer::layoutChrome(PaintSurface& surface,
                                   const Gfx::RectF& rect,
                                   Gfx::RectF& entryRect,
                                   Gfx::RectF& upButtonRect,
                                   Gfx::RectF& downButtonRect,
                                   Gfx::RectF& textRect)
{
    prepare();
    onLayoutChrome(surface, rect, entryRect, upButtonRect, downButtonRect, textRect);
}


Gfx::RectF SpinBoxRenderer::layoutEntry(PaintSurface& surface,
                                         const Gfx::RectF& entryRect)
{
    prepare();
    return onLayoutEntry(surface, entryRect);
}


const Painter& SpinBoxRenderer::textPainter(PaintSurface& surface)
{
    prepare();
    return onGetTextPainter(surface);
}


void SpinBoxRenderer::renderChrome(PaintContext& context,
                                    const Gfx::RectF& rect,
                                    const Gfx::RectF& entryRect,
                                    const Gfx::RectF& upButtonRect,
                                    const Gfx::RectF& downButtonRect,
                                    SpinBoxStyleFlags state,
                                    ButtonStyleFlags upButtonState,
                                    ButtonStyleFlags downButtonState)
{
    const StyleOptions& opts = prepare();
    onRenderChrome(context, rect, opts, entryRect, upButtonRect, downButtonRect,
                    state, upButtonState, downButtonState);
}


void SpinBoxRenderer::renderText(PaintContext& context,
                                 const Gfx::RectF& textRect,
                                 const String& text,
                                 const Gfx::PointF& textPos,
                                 const Gfx::RectF& cursor,
                                 SpinBoxStyleFlags state)
{
    const StyleOptions& opts = prepare();
    onRenderText(context, opts, textRect, text, textPos, cursor, state);
}


void SpinBoxRenderer::renderEntry(PaintContext& context,
                                  const Gfx::RectF& entryRect,
                                  SpinBoxStyleFlags state)
{
    const StyleOptions& opts = prepare();
    this->onRenderEntry(context, entryRect, opts, state);
}


void SpinBoxRenderer::onRenderChrome(PaintContext& context,
                                      const Gfx::RectF& /*rect*/,
                                      const StyleOptions& options,
                                      const Gfx::RectF& entryRect,
                                      const Gfx::RectF& upButtonRect,
                                      const Gfx::RectF& downButtonRect,
                                      SpinBoxStyleFlags state,
                                      ButtonStyleFlags upButtonState,
                                      ButtonStyleFlags downButtonState)
{
    onRenderEntry(context, entryRect, options, state);
    onRenderUpButton(context, upButtonRect, options, state, upButtonState);
    onRenderDownButton(context, downButtonRect, options, state, downButtonState);
}


const StyleOptions& SpinBoxRenderer::prepare()
{
    const StyleOptions& opts = Application::instance().styleOptions();

    if( _styleGeneration != opts.generation() )
    {
        _styleGeneration = opts.generation();
        onPrepare(opts);
    }

    return opts;
}

///////////////////////////////////////////////////////////////////////////////
// LineEditRenderer
///////////////////////////////////////////////////////////////////////////////

LineEditRenderer::LineEditRenderer(std::size_t refs)
: Style::Facet( typeid(LineEditRenderer), refs )
, _styleGeneration( std::size_t(-1) )
{
}

    
LineEditRenderer::~LineEditRenderer()
{
}


LineEditRenderer* LineEditRenderer::create() const
{
    return onCreate();
}


const Gfx::Brush& LineEditRenderer::background() const
{
    if( _background )
        return *_background;

    return Application::instance().styleOptions().textBackground();
}


void LineEditRenderer::setBackground(const Gfx::Brush& b)
{
    _background.reset( new Gfx::Brush(b) );
    _styleGeneration = std::size_t(-1);
}


const Gfx::Pen& LineEditRenderer::contour() const
{
    if( _contour )
        return *_contour;

    return Application::instance().styleOptions().contour();
}


void LineEditRenderer::setContour(const Gfx::Pen& p)
{
    _contour.reset( new Gfx::Pen(p) );
    _styleGeneration = std::size_t(-1);
}


const Gfx::Font& LineEditRenderer::font() const
{
    if( _font )
        return *_font;

    return Application::instance().styleOptions().font();
}


void LineEditRenderer::setFont(const Gfx::Font& f)
{
    _font.reset( new Gfx::Font(f) );
    _styleGeneration = std::size_t(-1);
}


const Gfx::Color& LineEditRenderer::textColor() const
{
    if( _textColor )
        return _textColor->color();

    return Application::instance().styleOptions().textColor();
}


void LineEditRenderer::setTextColor(const Gfx::Pen& p)
{
    _textColor.reset( new Gfx::Pen(p) );
    _styleGeneration = std::size_t(-1);
}


const Gfx::Brush* LineEditRenderer::selectionBackground() const
{
    return _selectionBackground.get();
}


void LineEditRenderer::setSelectionBackground(const Gfx::Brush& b)
{
    _selectionBackground.reset( new Gfx::Brush(b) );
    _styleGeneration = std::size_t(-1);
}


const Gfx::Pen* LineEditRenderer::selectionTextColor() const
{
    return _selectionTextColor.get();
}


void LineEditRenderer::setSelectionTextColor(const Gfx::Pen& p)
{
    _selectionTextColor.reset( new Gfx::Pen(p) );
    _styleGeneration = std::size_t(-1);
}


Gfx::SizeF LineEditRenderer::measureFrame(PaintSurface& surface,
                                          const Gfx::SizeF& contentSize)
{
    prepare();
    return onMeasureFrame(surface, contentSize);
}


Gfx::RectF LineEditRenderer::layoutFrame(PaintSurface& surface,
                                         const Gfx::RectF& rect)
{
    prepare();
    return onLayoutFrame(surface, rect);
}


const Painter& LineEditRenderer::textPainter(PaintSurface& surface)
{
    prepare();
    return onGetTextPainter(surface);
}


void LineEditRenderer::renderChrome(PaintContext& context,
                              const Gfx::RectF& rect,
                              const Gfx::RectF& textRect,
                              const String& text,
                              const Gfx::PointF& textPos,
                              const Gfx::RectF& cursor,
                              const Gfx::RectF& selection,
                              LineEditStyleFlags state)
{
    const StyleOptions& opts = prepare();
    onRenderChrome(context, rect, textRect, opts, text, textPos,
             cursor, selection, state);
}


void LineEditRenderer::renderEntry(PaintContext& context,
                                        const Gfx::RectF& rect,
                                        LineEditStyleFlags state)
{
    const StyleOptions& opts = prepare();
    onRenderEntry(context, rect, opts, state);
}


void LineEditRenderer::renderSelection(PaintContext& context,
                                       const Gfx::RectF& textRect,
                                       const Gfx::RectF& selection,
                                       LineEditStyleFlags state)
{
    const StyleOptions& opts = prepare();
    onRenderSelection(context, textRect, opts, selection, state);
}


void LineEditRenderer::renderText(PaintContext& context,
                                  const Gfx::RectF& textRect,
                                  const String& text,
                                  const Gfx::PointF& textPos,
                                  LineEditStyleFlags state)
{
    const StyleOptions& opts = prepare();
    onRenderText(context, textRect, opts, text, textPos, state);
}


void LineEditRenderer::renderCursor(PaintContext& context,
                                    const Gfx::RectF& textRect,
                                    const Gfx::RectF& cursor,
                                    LineEditStyleFlags state)
{
    const StyleOptions& opts = prepare();
    onRenderCursor(context, textRect, opts, cursor, state);
}


void LineEditRenderer::onRenderChrome(PaintContext& context,
                                const Gfx::RectF& rect,
                                const Gfx::RectF& textRect,
                                const StyleOptions& options,
                                const String& text,
                                const Gfx::PointF& textPos,
                                const Gfx::RectF& cursor,
                                const Gfx::RectF& selection,
                                LineEditStyleFlags state)
{
    onRenderEntry(context, rect, options, state);
    onRenderSelection(context, textRect, options, selection, state);
    onRenderText(context, textRect, options, text, textPos, state);
    onRenderCursor(context, textRect, options, cursor, state);
}


const StyleOptions& LineEditRenderer::prepare()
{
    const StyleOptions& opts = Application::instance().styleOptions();

    if( _styleGeneration != opts.generation() )
    {
        _styleGeneration = opts.generation();
        onPrepare(opts);
    }

    return opts;
}

///////////////////////////////////////////////////////////////////////////////
// ProgressBarRenderer
///////////////////////////////////////////////////////////////////////////////

ProgressBarRenderer::ProgressBarRenderer(std::size_t refs)
: Style::Facet( typeid(ProgressBarRenderer), refs )
, _styleGeneration(std::size_t(-1))
{
}


ProgressBarRenderer::~ProgressBarRenderer()
{
}


ProgressBarRenderer* ProgressBarRenderer::create() const
{
    return onCreate();
}


const Gfx::Brush& ProgressBarRenderer::background() const
{
    if( _background )
        return *_background;

    return Application::instance().styleOptions().background();
}

void ProgressBarRenderer::setBackground(const Gfx::Brush& b)
{
    _background.reset( new Gfx::Brush(b) );
    _styleGeneration = std::size_t(-1);
}


const Gfx::Pen& ProgressBarRenderer::contour() const
{
    if( _contour )
        return *_contour;

    return Application::instance().styleOptions().contour();
}

void ProgressBarRenderer::setContour(const Gfx::Pen& p)
{
    _contour.reset( new Gfx::Pen(p) );
    _styleGeneration = std::size_t(-1);
}


const Gfx::Brush& ProgressBarRenderer::foreground() const
{
    if( _foreground )
        return *_foreground;

    return _defaultForeground;
}

void ProgressBarRenderer::setForeground(const Gfx::Brush& b)
{
    _foreground.reset( new Gfx::Brush(b) );
    _styleGeneration = std::size_t(-1);
}


const Gfx::Font& ProgressBarRenderer::font() const
{
    static Gfx::Font defaultFont;
    return _font ? *_font : defaultFont; // Or query application setup
}

void ProgressBarRenderer::setFont(const Gfx::Font& f)
{
    _font.reset( new Gfx::Font(f) );
    _styleGeneration = std::size_t(-1);
}

const Gfx::Color& ProgressBarRenderer::textColor() const
{
    static Gfx::Color defaultColor;
    return _textColor ? _textColor->color() : defaultColor; 
}

void ProgressBarRenderer::setTextColor(const Gfx::Pen& p)
{
    _textColor.reset( new Gfx::Pen(p) );
    _styleGeneration = std::size_t(-1);
}

Gfx::SizeF ProgressBarRenderer::measureFrame(PaintSurface& surface,
                                             const Gfx::SizeF& contentSize)
{
    prepare();
    return onMeasureFrame(surface, contentSize);
}

Gfx::SizeF ProgressBarRenderer::measureBar(PaintSurface& surface)
{
    prepare();
    return onMeasureBar(surface);
}

void ProgressBarRenderer::layoutChrome(PaintSurface& surface,
                                      const Gfx::RectF& rect,
                                      const Gfx::SizeF& barSize,
                                      const Gfx::SizeF& textSize,
                                      Gfx::RectF& barRect,
                                      Gfx::RectF& textRect)
{
    prepare();
    onLayoutChrome(surface, rect, barSize, textSize, barRect, textRect);
}

void ProgressBarRenderer::layoutBar(PaintSurface& surface,
                                    const Gfx::RectF& barRect,
                                    float progressRatio,
                                    Gfx::RectF& trackRect,
                                    Gfx::RectF& chunkRect)
{
    prepare();
    onLayoutBar(surface, barRect, progressRatio, trackRect, chunkRect);
}

const Painter& ProgressBarRenderer::textPainter(PaintSurface& surface)
{
    prepare();
    return onGetTextPainter(surface);
}

void ProgressBarRenderer::renderChrome(PaintContext& context,
                                      const Gfx::RectF& rect,
                                      const Gfx::RectF& trackRect,
                                      const Gfx::RectF& chunkRect,
                                      const Gfx::RectF& textRect,
                                      const String& text,
                                      const Gfx::PointF& textPos,
                                      ProgressBarStyleFlags state)
{
    const StyleOptions& opts = prepare();
    onRenderChrome(context, rect, opts, trackRect, chunkRect, textRect, text, textPos, state);
}

void ProgressBarRenderer::renderBar(PaintContext& context,
                                    const Gfx::RectF& trackRect,
                                    const Gfx::RectF& chunkRect,
                                    ProgressBarStyleFlags state)
{
    const StyleOptions& opts = prepare();
    onRenderBar(context, trackRect, chunkRect, opts, state);
}

void ProgressBarRenderer::renderTrack(PaintContext& context,
                                      const Gfx::RectF& trackRect,
                                      ProgressBarStyleFlags state)
{
    const StyleOptions& opts = prepare();
    onRenderTrack(context, trackRect, opts, state);
}

void ProgressBarRenderer::renderChunk(PaintContext& context,
                                      const Gfx::RectF& chunkRect,
                                      ProgressBarStyleFlags state)
{
    const StyleOptions& opts = prepare();
    onRenderChunk(context, chunkRect, opts, state);
}

void ProgressBarRenderer::renderText(PaintContext& context,
                                     const Gfx::RectF& textRect,
                                     const Gfx::RectF& chunkRect,
                                     const String& text,
                                     const Gfx::PointF& textPos,
                                     ProgressBarStyleFlags state)
{
    const StyleOptions& opts = prepare();
    onRenderText(context, textRect, chunkRect, opts, text, textPos, state);
}

const StyleOptions& ProgressBarRenderer::prepare()
{
    const StyleOptions& opts = Application::instance().styleOptions();

    if( _styleGeneration != opts.generation() )
    {
        _styleGeneration = opts.generation();
        _defaultForeground = Gfx::Brush(opts.accentColor());
        onPrepare(opts);
    }

    return opts;
}

void ProgressBarRenderer::onRenderChrome(PaintContext& context,
                                        const Gfx::RectF& rect,
                                        const StyleOptions& options,
                                        const Gfx::RectF& trackRect,
                                        const Gfx::RectF& chunkRect,
                                        const Gfx::RectF& textRect,
                                        const String& text,
                                        const Gfx::PointF& textPos,
                                        ProgressBarStyleFlags state)
{
    onRenderBar(context, trackRect, chunkRect, options, state);
    onRenderText(context, textRect, chunkRect, options, text, textPos, state);
}

void ProgressBarRenderer::onRenderBar(PaintContext& context,
                                      const Gfx::RectF& trackRect,
                                      const Gfx::RectF& chunkRect,
                                      const StyleOptions& options,
                                      ProgressBarStyleFlags state)
{
    onRenderTrack(context, trackRect, options, state);
    onRenderChunk(context, chunkRect, options, state);
}

///////////////////////////////////////////////////////////////////////////////
// SliderRenderer
///////////////////////////////////////////////////////////////////////////////

SliderRenderer::SliderRenderer(std::size_t refs)
: Style::Facet( typeid(SliderRenderer), refs )
, _styleGeneration(std::size_t(-1))
{
}

    
SliderRenderer::~SliderRenderer()
{
}


SliderRenderer* SliderRenderer::create() const
{
    return onCreate();
}


const Gfx::Brush& SliderRenderer::background() const
{
    if( _background )
        return *_background;

    return Application::instance().styleOptions().background();
}


void SliderRenderer::setBackground(const Gfx::Brush& b)
{
    _background.reset( new Gfx::Brush(b) );
    _styleGeneration = std::size_t(-1);
}


const Gfx::Pen& SliderRenderer::contour() const
{
    if( _contour )
        return *_contour;

    return Application::instance().styleOptions().contour();
}


void SliderRenderer::setContour(const Gfx::Pen& p)
{
    _contour.reset( new Gfx::Pen(p) );
    _styleGeneration = std::size_t(-1);
}


const Gfx::Brush& SliderRenderer::foreground() const
{
    if( _foreground )
        return *_foreground;

    return _defaultForeground;
}


void SliderRenderer::setForeground(const Gfx::Brush& b)
{
    _foreground.reset( new Gfx::Brush(b) );
    _styleGeneration = std::size_t(-1);
}


const Gfx::Font& SliderRenderer::font() const
{
    static Gfx::Font defaultFont;
    return _font ? *_font : defaultFont;
}


void SliderRenderer::setFont(const Gfx::Font& f)
{
    _font.reset( new Gfx::Font(f) );
    _styleGeneration = std::size_t(-1);
}


const Gfx::Color& SliderRenderer::textColor() const
{
    static Gfx::Color defaultColor;
    return _textColor ? _textColor->color() : defaultColor;
}


void SliderRenderer::setTextColor(const Gfx::Pen& p)
{
    _textColor.reset( new Gfx::Pen(p) );
    _styleGeneration = std::size_t(-1);
}


Gfx::SizeF SliderRenderer::measureFrame(PaintSurface& surface,
                                         const Gfx::SizeF& contentSize)
{
    prepare();
    return onMeasureFrame(surface, contentSize);
}


Gfx::SizeF SliderRenderer::measureTrack(PaintSurface& surface)
{
    prepare();
    return onMeasureTrack(surface);
}


Gfx::SizeF SliderRenderer::measureHandle(PaintSurface& surface)
{
    prepare();
    return onMeasureHandle(surface);
}


void SliderRenderer::layoutChrome(PaintSurface& surface,
                                 const Gfx::RectF& rect,
                                 const Gfx::SizeF& trackSize,
                                 const Gfx::SizeF& handleSize,
                                 Gfx::RectF& trackRect,
                                 Gfx::RectF& handleRect)
{
    prepare();
    onLayoutChrome(surface, rect, trackSize, handleSize, trackRect, handleRect);
}


void SliderRenderer::layoutHandle(PaintSurface& surface,
                                  const Gfx::RectF& trackRect,
                                  float fraction,
                                  Gfx::RectF& handleRect)
{
    prepare();
    onLayoutHandle(surface, trackRect, fraction, handleRect);
}


void SliderRenderer::renderChrome(PaintContext& context,
                                 const Gfx::RectF& rect,
                                 const Gfx::RectF& trackRect,
                                 const Gfx::RectF& handleRect,
                                 SliderStyleFlags state)
{
    const StyleOptions& opts = prepare();
    onRenderChrome(context, rect, opts, trackRect, handleRect, state);
}


void SliderRenderer::renderTrack(PaintContext& context,
                                 const Gfx::RectF& trackRect,
                                 SliderStyleFlags state)
{
    const StyleOptions& opts = prepare();
    onRenderTrack(context, trackRect, opts, state);
}


void SliderRenderer::renderHandle(PaintContext& context,
                                  const Gfx::RectF& handleRect,
                                  SliderStyleFlags state)
{
    const StyleOptions& opts = prepare();
    onRenderHandle(context, handleRect, opts, state);
}


const StyleOptions& SliderRenderer::prepare()
{
    const StyleOptions& opts = Application::instance().styleOptions();

    if( _styleGeneration != opts.generation() )
    {
        _styleGeneration = opts.generation();
        _defaultForeground = Gfx::Brush(opts.accentColor());
        onPrepare(opts);
    }

    return opts;
}


void SliderRenderer::onRenderChrome(PaintContext& context,
                                   const Gfx::RectF& rect,
                                   const StyleOptions& options,
                                   const Gfx::RectF& trackRect,
                                   const Gfx::RectF& handleRect,
                                   SliderStyleFlags state)
{
    onRenderTrack(context, trackRect, options, state);
    onRenderHandle(context, handleRect, options, state);
}

////////////////////////////////////////////////////////////////////////////
// ScrollBarRenderer
//////////////////////////////////////////////////////////////////////////////

ScrollBarRenderer::ScrollBarRenderer(std::size_t refs)
: Style::Facet( typeid(ScrollBarRenderer), refs )
, _styleGeneration(std::size_t(-1))
{
}

    
ScrollBarRenderer::~ScrollBarRenderer()
{
}


ScrollBarRenderer* ScrollBarRenderer::create() const
{
    return onCreate();
}


const Gfx::Brush& ScrollBarRenderer::background() const
{
    if( _background )
        return *_background;

    return Application::instance().styleOptions().background();
}


void ScrollBarRenderer::setBackground(const Gfx::Brush& b)
{
    _background.reset( new Gfx::Brush(b) );
    _styleGeneration = std::size_t(-1);
}


const Gfx::Pen& ScrollBarRenderer::contour() const
{
    if( _contour )
        return *_contour;

    return Application::instance().styleOptions().contour();
}


void ScrollBarRenderer::setContour(const Gfx::Pen& p)
{
    _contour.reset( new Gfx::Pen(p) );
    _styleGeneration = std::size_t(-1);
}


const Gfx::Brush& ScrollBarRenderer::foreground() const
{
    if( _foreground )
        return *_foreground;

    return _defaultForeground;
}


void ScrollBarRenderer::setForeground(const Gfx::Brush& b)
{
    _foreground.reset( new Gfx::Brush(b) );
    _styleGeneration = std::size_t(-1);
}


Gfx::SizeF ScrollBarRenderer::measureFrame(PaintSurface& surface,
                                            const Gfx::SizeF& contentSize,
                                            Direction direction)
{
    prepare();
    return onMeasureFrame(surface, contentSize, direction);
}


Gfx::SizeF ScrollBarRenderer::measureTrack(PaintSurface& surface,
                                            Direction direction)
{
    prepare();
    return onMeasureTrack(surface, direction);
}


Gfx::SizeF ScrollBarRenderer::measureHandle(PaintSurface& surface,
                                             Direction direction)
{
    prepare();
    return onMeasureHandle(surface, direction);
}


Gfx::SizeF ScrollBarRenderer::measureButton(PaintSurface& surface,
                                             Direction direction)
{
    prepare();
    return onMeasureButton(surface, direction);
}


void ScrollBarRenderer::layoutChrome(PaintSurface& surface,
                                    const Gfx::RectF& rect,
                                    Direction direction,
                                    const Gfx::SizeF& buttonSize,
                                    Gfx::RectF& trackRect,
                                    Gfx::RectF& decreaseRect,
                                    Gfx::RectF& increaseRect)
{
    prepare();
    onLayoutChrome(surface, rect, direction, buttonSize, trackRect, decreaseRect, increaseRect);
}


void ScrollBarRenderer::layoutHandle(PaintSurface& surface,
                                     const Gfx::RectF& trackRect,
                                     Direction direction,
                                     float fraction,
                                     float viewProportion,
                                     Gfx::RectF& handleRect)
{
    prepare();
    onLayoutHandle(surface, trackRect, direction, fraction, viewProportion, handleRect);
}


void ScrollBarRenderer::renderChrome(PaintContext& context,
                                    const Gfx::RectF& rect,
                                    Direction direction,
                                    const Gfx::RectF& trackRect,
                                    const Gfx::RectF& handleRect,
                                    const Gfx::RectF& decreaseRect,
                                    const Gfx::RectF& increaseRect,
                                    ScrollBarStyleFlags state,
                                    ButtonStyleFlags decreaseState,
                                    ButtonStyleFlags increaseState)
{
    const StyleOptions& opts = prepare();
    onRenderChrome(context, rect, opts, direction, trackRect, handleRect,
                  decreaseRect, increaseRect, state, decreaseState, increaseState);
}


void ScrollBarRenderer::renderTrack(PaintContext& context,
                                    const Gfx::RectF& trackRect,
                                    Direction direction,
                                    ScrollBarStyleFlags state)
{
    const StyleOptions& opts = prepare();
    onRenderTrack(context, trackRect, opts, direction, state);
}


void ScrollBarRenderer::renderHandle(PaintContext& context,
                                     const Gfx::RectF& handleRect,
                                     Direction direction,
                                     ScrollBarStyleFlags state)
{
    const StyleOptions& opts = prepare();
    onRenderHandle(context, handleRect, opts, direction, state);
}


void ScrollBarRenderer::renderDecreaseButton(PaintContext& context,
                                             const Gfx::RectF& buttonRect,
                                             Direction direction,
                                             ScrollBarStyleFlags state,
                                             ButtonStyleFlags buttonState)
{
    const StyleOptions& opts = prepare();
    onRenderDecreaseButton(context, buttonRect, opts, direction, state, buttonState);
}


void ScrollBarRenderer::renderIncreaseButton(PaintContext& context,
                                             const Gfx::RectF& buttonRect,
                                             Direction direction,
                                             ScrollBarStyleFlags state,
                                             ButtonStyleFlags buttonState)
{
    const StyleOptions& opts = prepare();
    onRenderIncreaseButton(context, buttonRect, opts, direction, state, buttonState);
}


const StyleOptions& ScrollBarRenderer::prepare()
{
    const StyleOptions& opts = Application::instance().styleOptions();

    if( _styleGeneration != opts.generation() )
    {
        _styleGeneration = opts.generation();
        _defaultForeground = Gfx::Brush(opts.accentColor());
        onPrepare(opts);
    }

    return opts;
}


void ScrollBarRenderer::onRenderChrome(PaintContext& context,
                                      const Gfx::RectF& rect,
                                      const StyleOptions& options,
                                      Direction direction,
                                      const Gfx::RectF& trackRect,
                                      const Gfx::RectF& handleRect,
                                      const Gfx::RectF& decreaseRect,
                                      const Gfx::RectF& increaseRect,
                                      ScrollBarStyleFlags state,
                                      ButtonStyleFlags decreaseState,
                                      ButtonStyleFlags increaseState)
{
    onRenderTrack(context, trackRect, options, direction, state);
    onRenderHandle(context, handleRect, options, direction, state);
    onRenderDecreaseButton(context, decreaseRect, options, direction, state, decreaseState);
    onRenderIncreaseButton(context, increaseRect, options, direction, state, increaseState);
}

///////////////////////////////////////////////////////////////////////////////
// ListBoxRenderer
///////////////////////////////////////////////////////////////////////////////

ListBoxRenderer::ListBoxRenderer(std::size_t refs)
: Style::Facet( typeid(ListBoxRenderer), refs )
, _styleGeneration( std::size_t(-1) )
{
}

    
ListBoxRenderer::~ListBoxRenderer()
{
}


ListBoxRenderer* ListBoxRenderer::create() const
{
    return onCreate();
}


const Gfx::Brush* ListBoxRenderer::background() const
{
    return _background.get();
}


void ListBoxRenderer::setBackground(const Gfx::Brush& b)
{
    _background.reset( new Gfx::Brush(b) );
    _styleGeneration = std::size_t(-1);
}


const Gfx::Pen* ListBoxRenderer::contour() const
{
    return _contour.get();
}


void ListBoxRenderer::setContour(const Gfx::Pen& p)
{
    _contour.reset( new Gfx::Pen(p) );
    _styleGeneration = std::size_t(-1);
}


Gfx::SizeF ListBoxRenderer::measureFrame(PaintSurface& surface,
                                          const Gfx::SizeF& contentSize)
{
    prepare();
    return onMeasureFrame(surface, contentSize);
}


Gfx::RectF ListBoxRenderer::layoutFrame(PaintSurface& surface,
                                         const Gfx::RectF& rect)
{
    prepare();
    return onLayoutFrame(surface, rect);
}


void ListBoxRenderer::renderBackground(PaintContext& context,
                                       const Gfx::RectF& rect,
                                       ListBoxStyleFlags state)
{
    const StyleOptions& opts = prepare();
    onRenderBackground(context, rect, opts, state);
}


void ListBoxRenderer::renderChrome(PaintContext& context,
                                   const Gfx::RectF& rect,
                                   ListBoxStyleFlags state)
{
    const StyleOptions& opts = prepare();
    onRenderChrome(context, rect, opts, state);
}


const StyleOptions& ListBoxRenderer::prepare()
{
    const StyleOptions& opts = Application::instance().styleOptions();

    if( _styleGeneration != opts.generation() )
    {
        _styleGeneration = opts.generation();
        onPrepare(opts);
    }

    return opts;
}

///////////////////////////////////////////////////////////////////////////////
// ListItemRenderer
///////////////////////////////////////////////////////////////////////////////

ListItemRenderer::ListItemRenderer(std::size_t refs)
: Style::Facet( typeid(ListItemRenderer), refs )
, _styleGeneration( std::size_t(-1) )
{
}


ListItemRenderer::~ListItemRenderer()
{
}


ListItemRenderer* ListItemRenderer::create() const
{
    return onCreate();
}


const Gfx::Brush* ListItemRenderer::background() const
{
    return _background.get();
}


void ListItemRenderer::setBackground(const Gfx::Brush& b)
{
    _background.reset( new Gfx::Brush(b) );
    _styleGeneration = std::size_t(-1);
}


const Gfx::Brush* ListItemRenderer::foreground() const
{
    return _foreground.get();
}


void ListItemRenderer::setForeground(const Gfx::Brush& b)
{
    _foreground.reset( new Gfx::Brush(b) );
    _styleGeneration = std::size_t(-1);
}


const Gfx::Font& ListItemRenderer::font() const
{
    if( _font )
        return *_font;

    return Application::instance().styleOptions().font();
}


void ListItemRenderer::setFont(const Gfx::Font& f)
{
    _font.reset( new Gfx::Font(f) );
    _styleGeneration = std::size_t(-1);
}


const Gfx::Color& ListItemRenderer::textColor() const
{
    if( _textColor )
        return _textColor->color();

    return Application::instance().styleOptions().textColor();
}


void ListItemRenderer::setTextColor(const Gfx::Pen& p)
{
    _textColor.reset( new Gfx::Pen(p) );
    _styleGeneration = std::size_t(-1);
}


Gfx::SizeF ListItemRenderer::measureContent(PaintSurface& surface,
                                            const Gfx::SizeF& iconSize,
                                            const Gfx::SizeF& textSize)
{
    prepare();
    return onMeasureContent(surface, iconSize, textSize);
}


Gfx::SizeF ListItemRenderer::measureFrame(PaintSurface& surface,
                                           const Gfx::SizeF& contentSize)
{
    prepare();
    return onMeasureFrame(surface, contentSize);
}


const Painter& ListItemRenderer::textPainter(PaintSurface& surface)
{
    prepare();
    return onGetTextPainter(surface);
}


Gfx::RectF ListItemRenderer::layoutFrame(PaintSurface& surface,
                                          const Gfx::RectF& rect)
{
    prepare();
    return onLayoutFrame(surface, rect);
}


void ListItemRenderer::layoutContent(PaintSurface& surface,
                                     const Gfx::RectF& contentRect,
                                     const Gfx::SizeF& iconSize,
                                     const Gfx::SizeF& textSize,
                                     Gfx::RectF& iconRect,
                                     Gfx::RectF& textRect)
{
    prepare();
    onLayoutContent(surface, contentRect, iconSize, textSize, iconRect, textRect);
}


void ListItemRenderer::renderBackground(PaintContext& context,
                                        const Gfx::RectF& rect,
                                        ListItemStyleFlags state)
{
    const StyleOptions& opts = prepare();
    onRenderBackground(context, rect, opts, state);
}


void ListItemRenderer::renderText(PaintContext& context,
                                  const Gfx::RectF& textRect,
                                  const String& text,
                                  const Gfx::PointF& pos,
                                  ListItemStyleFlags state)
{
    const StyleOptions& opts = prepare();
    onRenderText(context, textRect, opts, text, pos, state);
}


void ListItemRenderer::renderIcon(PaintContext& context,
                                  const Gfx::RectF& iconRect,
                                  const Pixmap& picture,
                                  const Gfx::PointF& pos,
                                  ListItemStyleFlags state)
{
    const StyleOptions& opts = prepare();
    onRenderIcon(context, iconRect, opts, picture, pos, state);
}


const StyleOptions& ListItemRenderer::prepare()
{
    const StyleOptions& opts = Application::instance().styleOptions();

    if( _styleGeneration != opts.generation() )
    {
        _styleGeneration = opts.generation();
        onPrepare(opts);
    }

    return opts;
}

///////////////////////////////////////////////////////////////////////////////
// ComboBoxRenderer
///////////////////////////////////////////////////////////////////////////////

ComboBoxRenderer::ComboBoxRenderer(std::size_t refs)
: Style::Facet( typeid(ComboBoxRenderer), refs )
, _styleGeneration( std::size_t(-1) )
{
}


ComboBoxRenderer::~ComboBoxRenderer()
{
}


ComboBoxRenderer* ComboBoxRenderer::create() const
{
    return onCreate();
}


const Gfx::Brush& ComboBoxRenderer::background() const
{
    if( _background )
        return *_background;

    return Application::instance().styleOptions().textBackground();
}


void ComboBoxRenderer::setBackground(const Gfx::Brush& b)
{
    _background.reset( new Gfx::Brush(b) );
    _styleGeneration = std::size_t(-1);
}


const Gfx::Pen& ComboBoxRenderer::contour() const
{
    if( _contour )
        return *_contour;

    return Application::instance().styleOptions().contour();
}


void ComboBoxRenderer::setContour(const Gfx::Pen& p)
{
    _contour.reset( new Gfx::Pen(p) );
    _styleGeneration = std::size_t(-1);
}


const Gfx::Brush& ComboBoxRenderer::foreground() const
{
    if( _foreground )
        return *_foreground;

    return Application::instance().styleOptions().foreground();
}


void ComboBoxRenderer::setForeground(const Gfx::Brush& b)
{
    _foreground.reset( new Gfx::Brush(b) );
    _styleGeneration = std::size_t(-1);
}


const Gfx::Font& ComboBoxRenderer::font() const
{
    if( _font )
        return *_font;

    return Application::instance().styleOptions().font();
}


void ComboBoxRenderer::setFont(const Gfx::Font& f)
{
    _font.reset( new Gfx::Font(f) );
    _styleGeneration = std::size_t(-1);
}


const Gfx::Color& ComboBoxRenderer::textColor() const
{
    if( _textColor )
        return _textColor->color();

    return Application::instance().styleOptions().textColor();
}


void ComboBoxRenderer::setTextColor(const Gfx::Pen& p)
{
    _textColor.reset( new Gfx::Pen(p) );
    _styleGeneration = std::size_t(-1);
}


const StyleOptions& ComboBoxRenderer::prepare()
{
    const StyleOptions& options = Application::instance().styleOptions();

    if( _styleGeneration != options.generation() )
    {
        _styleGeneration = options.generation();
        onPrepare(options);
    }

    return options;
}


Gfx::SizeF ComboBoxRenderer::measureFrame(PaintSurface& surface,
                                           const Gfx::SizeF& contentSize)
{
    prepare();
    return onMeasureFrame(surface, contentSize);
}


Gfx::SizeF ComboBoxRenderer::measureButton(PaintSurface& surface)
{
    prepare();
    return onMeasureButton(surface);
}


void ComboBoxRenderer::layoutChrome(PaintSurface& surface,
                                   const Gfx::RectF& rect,
                                   Gfx::RectF& entryRect,
                                   Gfx::RectF& buttonRect,
                                   Gfx::RectF& textRect)
{
    prepare();
    onLayoutChrome(surface, rect, entryRect, buttonRect, textRect);
}


const Painter& ComboBoxRenderer::textPainter(PaintSurface& surface)
{
    prepare();
    return onGetTextPainter(surface);
}


void ComboBoxRenderer::renderChrome(PaintContext& context,
                                    const Gfx::RectF& rect,
                                    const Gfx::RectF& entryRect,
                                    const Gfx::RectF& buttonRect,
                                    ComboBoxStyleFlags state,
                                    ButtonStyleFlags buttonState)
{
    const StyleOptions& options = prepare();
    onRenderChrome(context, rect, entryRect, buttonRect, options, state, buttonState);
}


void ComboBoxRenderer::renderButton(PaintContext& context,
                                    const Gfx::RectF& buttonRect,
                                    ComboBoxStyleFlags state,
                                    ButtonStyleFlags buttonState)
{
    const StyleOptions& options = prepare();
    onRenderButton(context, buttonRect, options, state, buttonState);
}


void ComboBoxRenderer::renderText(PaintContext& context,
                                  const Gfx::RectF& textRect,
                                  const String& text,
                                  const Gfx::PointF& textPos,
                                  const Gfx::RectF& cursor,
                                  ComboBoxStyleFlags state)
{
    const StyleOptions& options = prepare();
    onRenderText(context, textRect, options, text, textPos, cursor, state);
}


void ComboBoxRenderer::onRenderChrome(PaintContext& context,
                                      const Gfx::RectF& rect,
                                      const Gfx::RectF& entryRect,
                                      const Gfx::RectF& buttonRect,
                                      const StyleOptions& options,
                                      ComboBoxStyleFlags state,
                                      ButtonStyleFlags buttonState)
{
    onRenderEntry(context, entryRect, options, state);
    onRenderButton(context, buttonRect, options, state, buttonState);
}

///////////////////////////////////////////////////////////////////////////////
// TabViewRenderer
///////////////////////////////////////////////////////////////////////////////

TabViewRenderer::TabViewRenderer(std::size_t refs)
: Style::Facet( typeid(TabViewRenderer), refs )
, _styleGeneration( std::size_t(-1) )
{
}

    
TabViewRenderer::~TabViewRenderer()
{
}


TabViewRenderer* TabViewRenderer::create() const
{
    return onCreate();
}


const Gfx::Brush* TabViewRenderer::background() const
{
    return _background.get();
}


void TabViewRenderer::setBackground(const Gfx::Brush& b)
{
    _background.reset( new Gfx::Brush(b) );
    _styleGeneration = std::size_t(-1);
}


const Gfx::Pen* TabViewRenderer::contour() const
{
    return _contour.get();
}


void TabViewRenderer::setContour(const Gfx::Pen& p)
{
    _contour.reset( new Gfx::Pen(p) );
    _styleGeneration = std::size_t(-1);
}


const Gfx::Font& TabViewRenderer::font() const
{
    if( _font )
        return *_font;

    return Application::instance().styleOptions().font();
}


void TabViewRenderer::setFont(const Gfx::Font& f)
{
    _font.reset( new Gfx::Font(f) );
    _styleGeneration = std::size_t(-1);
}


const Gfx::Color& TabViewRenderer::textColor() const
{
    if( _textColor )
        return _textColor->color();

    return Application::instance().styleOptions().textColor();
}


void TabViewRenderer::setTextColor(const Gfx::Pen& p)
{
    _textColor.reset( new Gfx::Pen(p) );
    _styleGeneration = std::size_t(-1);
}


void TabViewRenderer::prepare()
{
    const StyleOptions& options = Application::instance().styleOptions();

    if( _styleGeneration == options.generation() )
        return;

    _styleGeneration = options.generation();
    onPrepare(options);
}


Gfx::SizeF TabViewRenderer::measureTab(PaintSurface& surface,
                                        const Pt::String& text)
{
    prepare();
    return onMeasureTab(surface, text);
}


Gfx::RectF TabViewRenderer::layoutTab(PaintSurface& surface,
                                       const Gfx::RectF& tabRect)
{
    prepare();
    return onLayoutTab(surface, tabRect);
}


const Painter& TabViewRenderer::textPainter(PaintSurface& surface)
{
    prepare();
    return onGetTextPainter(surface);
}


void TabViewRenderer::renderBackground(PaintContext& context,
                                       const Gfx::RectF& contentRect,
                                       TabViewStyleFlags state)
{
    prepare();
    const StyleOptions& options = Application::instance().styleOptions();
    onRenderBackground(context, contentRect, options, state);
}


void TabViewRenderer::renderChrome(PaintContext& context,
                                   const Gfx::RectF& contentRect,
                                   const Gfx::RectF& activeTabRect,
                                   TabViewStyleFlags state)
{
    prepare();
    const StyleOptions& options = Application::instance().styleOptions();
    onRenderChrome(context, contentRect, activeTabRect, options, state);
}


void TabViewRenderer::renderTab(PaintContext& context,
                                const Gfx::RectF& tabRect,
                                const Pt::String& text,
                                const Gfx::PointF& textPos,
                                TabItemStyleFlags state)
{
    prepare();
    const StyleOptions& options = Application::instance().styleOptions();
    onRenderTab(context, tabRect, text, textPos, options, state);
}

} // namespace

} // namespace
