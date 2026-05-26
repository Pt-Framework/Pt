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

#include <Pt/Forms/Slider.h>
#include <Pt/Forms/Application.h>
#include <Pt/Forms/Style.h>
#include <Pt/Forms/StyleOptions.h>
#include <Pt/Forms/PaintContext.h>
#include <Pt/Forms/Painter.h>
#include <Pt/Gfx/Painter.h>

namespace Pt {

namespace Forms {

Slider::Slider()
: _position(50)
, _min(0)
, _max(100)
, _isHighlighted(false)
, _customRenderer(false)
, _styleGeneration(0)
, _overrideFlags(0)
{
}


Slider::~Slider()
{
}


int Slider::position() const
{
    return _position;
}


void Slider::setPosition(int pos)
{
    if(pos > _max)
    {
        pos = _max;
    }

    if(pos < _min)
    {
        pos = _min;
    }

    if(pos == _position)
        return;

    _position = pos;
    repaint();

    _positionChanged.send(_position);
}


int Slider::minimum() const
{
    return _min;
}


int Slider::maximum() const
{
    return _max;
}


void Slider::setRange(int min, int max)
{
    _min = min;
    _max = max;

    invalidate();
}


bool Slider::isHighlighted() const
{
    return _isHighlighted;
}


Signal<int>& Slider::positionChanged()
{
    return _positionChanged;
}


const Gfx::Brush& Slider::background() const
{
    if( _renderer )
        return _renderer->background();

    return Application::instance().styleOptions().background();
}


void Slider::setBackground(const Gfx::Brush& b)
{
    _background.reset( new Gfx::Brush(b) );
    _overrideFlags |= OverrideBackground;

    if( SliderRenderer* renderer = getRenderer() )
        renderer->setBackground(*_background);

    invalidate();
}


const Gfx::Color& Slider::foreground() const
{
    if( _renderer )
        return _renderer->foreground().color();

    return Application::instance().styleOptions().accentColor();
}


void Slider::setForeground(const Gfx::Color& b)
{
    _foreground.reset( new Gfx::Color(b) );
    _overrideFlags |= OverrideForeground;

    if( SliderRenderer* renderer = getRenderer() )
        renderer->setForeground( Gfx::Brush(*_foreground) );

    invalidate();
}


const Gfx::Pen& Slider::contour() const
{
    if( _renderer )
        return _renderer->contour();

    return Application::instance().styleOptions().contour();
}


void Slider::setContour(const Gfx::Pen& p)
{
    _contour.reset( new Gfx::Pen(p) );
    _overrideFlags |= OverrideContour;

    if( SliderRenderer* renderer = getRenderer() )
        renderer->setContour(*_contour);

    invalidate();
}


const Gfx::Color& Slider::textColor() const
{
    if( _renderer )
        return _renderer->textColor();

    return Application::instance().styleOptions().textColor();
}


void Slider::setTextColor(const Gfx::Color& color)
{
    _textColor.reset( new Gfx::Color(color) );
    _overrideFlags |= OverrideTextColor;

    if( SliderRenderer* renderer = getRenderer() )
        renderer->setTextColor( Gfx::Pen(*_textColor) );

    invalidate();
}


const Gfx::Font& Slider::font() const
{
    if( _renderer )
        return _renderer->font();

    return Application::instance().styleOptions().font();
}


void Slider::setFont(const Gfx::Font& font)
{
    _customFont = font;
    _overrideFlags |= OverrideFontAll;

    if( SliderRenderer* renderer = getRenderer() )
        renderer->setFont( getFont() );

    invalidate();
}


Gfx::Font Slider::getFont() const
{
    const Gfx::Font& base = Application::instance().styleOptions().font();

    if( ! (_overrideFlags & OverrideFontAny) )
        return base;

    if( _overrideFlags & OverrideFontAll )
        return _customFont;

    std::size_t sz = (_overrideFlags & OverrideFontSize) ? _customFont.size()
                                                        : base.size();
    Gfx::Font::Weight wt = (_overrideFlags & OverrideFontWeight) ? _customFont.weight()
                                                                 : base.weight();
    Gfx::Font::Slant sl = (_overrideFlags & OverrideFontSlant) ? _customFont.slant()
                                                               : base.slant();

    if( base.hasStyleName() )
        return Gfx::Font(base.family(), sz, base.styleName(), wt, sl, base.stretch());

    if( base.category() != Gfx::Font::Category::None )
        return Gfx::Font(base.category(), sz, wt, sl, base.stretch());

    return Gfx::Font(base.family(), sz, wt, sl, base.stretch());
}


void Slider::setFontSize(std::size_t size)
{
    _customFont = _customFont.withSize(size);
    _overrideFlags |= OverrideFontSize;

    if( SliderRenderer* renderer = getRenderer() )
        renderer->setFont( getFont() );

    invalidate();
}


void Slider::setFontWeight(Gfx::Font::Weight weight)
{
    _customFont = _customFont.withWeight(weight);
    _overrideFlags |= OverrideFontWeight;

    if( SliderRenderer* renderer = getRenderer() )
        renderer->setFont( getFont() );

    invalidate();
}


void Slider::setFontSlant(Gfx::Font::Slant slant)
{
    _customFont = _customFont.withSlant(slant);
    _overrideFlags |= OverrideFontSlant;

    if( SliderRenderer* renderer = getRenderer() )
        renderer->setFont( getFont() );

    invalidate();
}


void Slider::setRenderer(SliderRenderer* renderer)
{
    _renderer.reset(renderer);
    _customRenderer = renderer != 0;

    if( renderer )
        applyRenderer(renderer);

    invalidate();
}


SliderRenderer* Slider::getRenderer()
{
    if( ! _renderer )
    {
        const Style& style = Application::instance().style();
        SliderRenderer* proto = style.get<SliderRenderer>();
        if( ! proto )
            return 0;

        _renderer.reset( proto->create() );
    }

    return _renderer.get();
}


void Slider::applyRenderer(SliderRenderer* renderer)
{
    if( _overrideFlags & OverrideBackground )
        renderer->setBackground( *_background );

    if( _overrideFlags & OverrideForeground )
        renderer->setForeground( Gfx::Brush(*_foreground) );

    if( _overrideFlags & OverrideContour )
        renderer->setContour( *_contour );

    if( _overrideFlags & OverrideTextColor )
        renderer->setTextColor( Gfx::Pen(*_textColor) );

    if( _overrideFlags & OverrideFontAny )
        renderer->setFont( getFont() );
}


SliderStyleFlags Slider::sliderStyleFlags() const
{
    StyleFlags common;

    if( isEnabled() )
        common.set(StyleFlags::Enabled);
    else
        common.set(StyleFlags::Disabled);

    if( hasFocus() )
        common.set(StyleFlags::Focused);

    if( _isHighlighted && isEnabled() )
        common.set(StyleFlags::Highlighted);

    return SliderStyleFlags(common);
}


void Slider::onInvalidate()
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
        bool hasOverride = (_overrideFlags != 0);
        if( hasOverride )
        {
            if( SliderRenderer* renderer = getRenderer() )
                applyRenderer(renderer);
        }
        else
        {
            _renderer.reset( Application::instance().style().get<SliderRenderer>() );
        }
    }

    if( ! _renderer )
        return;

    Base::onInvalidate();

    relayout();
}


Gfx::SizeF Slider::onMeasure(const SizePolicy& policy)
{
    if( ! _renderer )
        return Gfx::SizeF(0, 0);

    Gfx::SizeF contentSize(policy.width(), 0);
    Gfx::SizeF sz = _renderer->measureFrame(surface(), contentSize);

    return Gfx::SizeF( sz.width() + padding().leftRight(),
                       sz.height() + padding().topBottom() );
}


void Slider::onLayout(const Gfx::RectF& rect)
{
    Base::onLayout(rect);

    if( ! _renderer )
        return;

    Gfx::SizeF trackSize  = _renderer->measureTrack(surface());
    Gfx::SizeF handleSize = _renderer->measureHandle(surface());

    _renderer->layoutFrame(surface(), Gfx::RectF(size()),
                           trackSize, handleSize, _trackRect, _handleRect);

    repaint();
}


void Slider::onPaint(PaintContext& context, const Gfx::RectF& /*updateRect*/)
{
    if( ! _renderer )
        return;

    Gfx::RectF handleRect = _handleRect;
    _renderer->layoutHandle(surface(), _trackRect, toFraction(), handleRect);

    SliderStyleFlags state = sliderStyleFlags();

    _renderer->renderFrame(context, Gfx::RectF(size()),
                           _trackRect, handleRect, state);
}


float Slider::toFraction() const
{
    if( _max == _min )
        return 0.0f;

    float fraction = static_cast<float>(_position - _min) /
                     static_cast<float>(_max - _min);

    if( fraction < 0.0f ) fraction = 0.0f;
    if( fraction > 1.0f ) fraction = 1.0f;

    return fraction;
}


int Slider::toPosition(double x) const
{
    if( _max == _min )
        return _min;

    if( _trackRect.width() <= 0.0 )
        return _min;

    float ratio = static_cast<float>( (x - _trackRect.x()) / _trackRect.width() );

    if( ratio < 0.0f ) ratio = 0.0f;
    if( ratio > 1.0f ) ratio = 1.0f;

    int pos = _min + static_cast<int>( ratio * (_max - _min) );
    return pos;
}


bool Slider::onMouseEvent(const MouseEvent& ev)
{
    Base::onMouseEvent(ev);

    if( ev.isPressed() )
    {
        int pos = toPosition( ev.position().x() );
        setPosition(pos);
    }

    return true;
}


bool Slider::onTouchEvent(const TouchEvent& ev)
{
    Base::onTouchEvent(ev);

    if( ev.isPressed() )
    {
        int pos = toPosition( ev.position().x() );
        setPosition(pos);
    }

    return true;
}


bool Slider::onEnterEvent(const EnterEvent& ev)
{
    Base::onEnterEvent(ev);

    _isHighlighted = true;

    repaint();
    return true;
}


bool Slider::onLeaveEvent(const LeaveEvent& ev)
{
    Base::onLeaveEvent(ev);

    _isHighlighted = false;

    repaint();
    return true;
}

} // namespace

} // namespace
