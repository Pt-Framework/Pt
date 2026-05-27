/* Copyright (C) 2017 Marc Boris Duerner 
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
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, 
  MA 02110-1301 USA
*/

#include <Pt/Forms/ProgressBar.h>
#include <Pt/Forms/Application.h>
#include <Pt/Forms/Style.h>
#include <Pt/Forms/StyleOptions.h>
#include <Pt/Forms/PaintContext.h>
#include <Pt/Forms/Painter.h>
#include <Pt/Gfx/Painter.h>

namespace Pt {

namespace Forms {

ProgressBar::ProgressBar()
: _value(50)
, _min(0)
, _max(100)
, _customRenderer(false)
, _styleGeneration(0)
, _overrides(0)
{
}


ProgressBar::~ProgressBar()
{
}


int ProgressBar::maximum() const
{
    return _max;
}


int ProgressBar::minimum() const
{
    return _min;
}


void ProgressBar::setRange(int minpos, int maxpos)
{
    _min = minpos;
    _max = maxpos;

    invalidate();
}


int ProgressBar::value() const
{
    return _value;
}


void ProgressBar::setValue(int n)
{
    if( n < _min )
        n = _min;

    if( n > _max )
        n = _max;
        
    _value = n;

    invalidate();

    _valueChanged.send(_value);
}


float ProgressBar::progress() const
{
    if( _value <= _min )
        return 0.f;

    if(_value > _max)
        return 1.f;

    return static_cast<float>(_value - _min)/(_max - _min);
}


void ProgressBar::reset()
{
    setValue( minimum() );
}


Signal<int>& ProgressBar::valueChanged()
{
    return _valueChanged;
}


const Gfx::Brush& ProgressBar::background() const
{
    if( _renderer )
        return _renderer->background();

    return Application::instance().styleOptions().background();
}


void ProgressBar::setBackground(const Gfx::Brush& b)
{
    _background.reset( new Gfx::Brush(b) );
    _overrides |= OverrideBackground;

    if( ProgressBarRenderer* renderer = getRenderer() )
        renderer->setBackground(*_background);

    invalidate();
}

const Gfx::Color& ProgressBar::foreground() const
{
    if( _renderer )
        return _renderer->foreground().color();

    return Application::instance().styleOptions().accentColor();
}


void ProgressBar::setForeground(const Gfx::Color& b)
{
    _foreground.reset( new Gfx::Color(b) );
    _overrides |= OverrideForeground;

    if( ProgressBarRenderer* renderer = getRenderer() )
        renderer->setForeground( Gfx::Brush(*_foreground) );

    invalidate();
}


const Gfx::Pen& ProgressBar::contour() const
{
    if( _renderer )
        return _renderer->contour();

    return Application::instance().styleOptions().contour();
}


void ProgressBar::setContour(const Gfx::Pen& p)
{
    _contour.reset( new Gfx::Pen(p) );
    _overrides |= OverrideContour;

    if( ProgressBarRenderer* renderer = getRenderer() )
        renderer->setContour(*_contour);

    invalidate();
}


const Gfx::Color& ProgressBar::textColor() const
{
    if( _renderer )
        return _renderer->textColor();

    return Application::instance().styleOptions().textColor();
}


void ProgressBar::setTextColor(const Gfx::Color& color)
{
    _textColor.reset( new Gfx::Color(color) );
    _overrides |= OverrideTextColor;

    if( ProgressBarRenderer* renderer = getRenderer() )
        renderer->setTextColor( Gfx::Pen(*_textColor) );

    invalidate();
}


const Gfx::Font& ProgressBar::font() const
{
    if( _renderer )
        return _renderer->font();

    return Application::instance().styleOptions().font();
}


void ProgressBar::setFont(const Gfx::Font& font)
{
    _customFont = font;
    _overrides |= OverrideFontAll;

    if( ProgressBarRenderer* renderer = getRenderer() )
        renderer->setFont( getFont() );

    invalidate();
}


Gfx::Font ProgressBar::getFont() const
{
    const Gfx::Font& base = Application::instance().styleOptions().font();

    if( ! (_overrides & OverrideFontAny) )
        return base;

    if( _overrides & OverrideFontAll )
        return _customFont;

    std::size_t sz = (_overrides & OverrideFontSize) ? _customFont.size()
                                                        : base.size();
    Gfx::Font::Weight wt = (_overrides & OverrideFontWeight) ? _customFont.weight()
                                                                 : base.weight();
    Gfx::Font::Slant sl = (_overrides & OverrideFontSlant) ? _customFont.slant()
                                                               : base.slant();

    if( base.hasStyleName() )
        return Gfx::Font(base.family(), sz, base.styleName(), wt, sl, base.stretch());

    if( base.category() != Gfx::Font::Category::None )
        return Gfx::Font(base.category(), sz, wt, sl, base.stretch());

    return Gfx::Font(base.family(), sz, wt, sl, base.stretch());
}


void ProgressBar::setFontSize(std::size_t size)
{
    _customFont = _customFont.withSize(size);
    _overrides |= OverrideFontSize;

    if( ProgressBarRenderer* renderer = getRenderer() )
        renderer->setFont( getFont() );

    invalidate();
}


void ProgressBar::setFontWeight(Gfx::Font::Weight weight)
{
    _customFont = _customFont.withWeight(weight);
    _overrides |= OverrideFontWeight;

    if( ProgressBarRenderer* renderer = getRenderer() )
        renderer->setFont( getFont() );

    invalidate();
}


void ProgressBar::setFontSlant(Gfx::Font::Slant slant)
{
    _customFont = _customFont.withSlant(slant);
    _overrides |= OverrideFontSlant;

    if( ProgressBarRenderer* renderer = getRenderer() )
        renderer->setFont( getFont() );

    invalidate();
}


void ProgressBar::setRenderer(ProgressBarRenderer* renderer)
{
    _renderer.reset(renderer);
    _customRenderer = renderer != 0;

    if( renderer )
        applyRenderer(renderer);

    invalidate();
}


ProgressBarRenderer* ProgressBar::getRenderer()
{
    if( ! _renderer )
    {
        const Style& style = Application::instance().style();
        ProgressBarRenderer* proto = style.get<ProgressBarRenderer>();
        if( ! proto )
            return 0;

        _renderer.reset( proto->create() );
    }

    return _renderer.get();
}


void ProgressBar::applyRenderer(ProgressBarRenderer* renderer)
{
    if( _overrides & OverrideBackground )
        renderer->setBackground( *_background );

    if( _overrides & OverrideForeground )
        renderer->setForeground( Gfx::Brush(*_foreground) );

    if( _overrides & OverrideContour )
        renderer->setContour( *_contour );

    if( _overrides & OverrideTextColor )
        renderer->setTextColor( Gfx::Pen(*_textColor) );

    if( _overrides & OverrideFontAny )
        renderer->setFont( getFont() );
}


ProgressBarStyleFlags ProgressBar::progressBarStyleFlags() const
{
    StyleFlags common;

    if( isEnabled() )
        common.set(StyleFlags::Enabled);
    else
        common.set(StyleFlags::Disabled);

    if( hasFocus() )
        common.set(StyleFlags::Focused);

    return ProgressBarStyleFlags(common);
}


Gfx::SizeF ProgressBar::onMeasure(const SizePolicy& policy)
{
    if( ! _renderer )
        return Gfx::SizeF(0, 0);

    Gfx::SizeF contentSize(policy.width(), 0);
    Gfx::SizeF sz = _renderer->measureFrame(surface(), contentSize);

    return Gfx::SizeF( sz.width() + padding().leftRight(), 
                       sz.height() + padding().topBottom() );
}


void ProgressBar::onInvalidate()
{
    std::size_t gen = Application::instance().styleOptions().generation();
    if( _styleGeneration != gen )
    {
        _styleGeneration = gen;
        if( ! _customRenderer )
            _renderer.reset();
    }

    if( ! _renderer )
    {
        bool hasOverride = (_overrides != 0);
        if(hasOverride)
        {
            if( ProgressBarRenderer* renderer = getRenderer() )
                applyRenderer(renderer);
        }
        else
        {
            _renderer.reset( Application::instance().style().get<ProgressBarRenderer>() );
        }
    }

    if( ! _renderer )
        return;

    Base::onInvalidate();

    relayout();
}


void ProgressBar::onPaint(PaintContext& context, const Gfx::RectF& /*updateRect*/)
{
    if( ! _renderer)
        return;

    Gfx::RectF trackRect;
    Gfx::RectF chunkRect;
    Gfx::RectF barRect;
    Gfx::RectF textRect;
    
    Gfx::SizeF barSize = _renderer->measureBar(surface());
    
    String txt;
    Gfx::SizeF textSize(0, 0);
    
    _renderer->layoutChrome(surface(), Gfx::RectF(size()), barSize, textSize, barRect, textRect);
    
    float ratio = progress();
    
    _renderer->layoutBar(surface(), barRect, ratio, trackRect, chunkRect);

    ProgressBarStyleFlags state = progressBarStyleFlags();

    _renderer->renderChrome(context, Gfx::RectF(size()), trackRect, chunkRect, textRect, txt, Gfx::PointF(), state);
}

} // namespace

} // namespace
