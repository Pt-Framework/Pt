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
#include <Pt/Forms/SliderStyle.h>
#include <Pt/Forms/Application.h>
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
    const BackgroundOption* background = _sliderOptions.find<BackgroundOption>();
    if(background)
        return background->value();

    const StyleOptions& options = Application::instance().styleOptions();
    return options.get<BackgroundOption>().value();
}


void Slider::setBackground(const Gfx::Brush& b)
{
    BackgroundOption background(b);
    _sliderOptions.set(background);
    invalidate();
}


const Gfx::Brush& Slider::foreground() const
{
    const ForegroundOption* foreground = _sliderOptions.find<ForegroundOption>();
    if(foreground)
        return foreground->value();

    const StyleOptions& options = Application::instance().styleOptions();
    return options.get<ForegroundOption>().value();
}


void Slider::setForeground(const Gfx::Brush& b)
{
    ForegroundOption foreground(b);
    _sliderOptions.set(foreground);
    invalidate();
}


const Gfx::Pen& Slider::contour() const
{
    const ContourOption* contour = _sliderOptions.find<ContourOption>();
    if(contour)
        return contour->value();

    const StyleOptions& options = Application::instance().styleOptions();
    return options.get<ContourOption>().value();
}


void Slider::setContour(const Gfx::Pen& p)
{
    ContourOption contour(p);
    _sliderOptions.set(contour);
    invalidate();
}


const Gfx::Color& Slider::textColor() const
{
    const TextColorOption* textColor = _sliderOptions.find<TextColorOption>();
    if(textColor)
        return textColor->value();

    const StyleOptions& options = Application::instance().styleOptions();
    return options.get<TextColorOption>().value();
}


void Slider::setTextColor(const Gfx::Color& color)
{
    TextColorOption textColor(color);
    _sliderOptions.set(textColor);
    invalidate();
}


Gfx::Font Slider::font() const
{
    const StyleOptions& options = Application::instance().styleOptions();
    const Gfx::Font& baseFont = options.get<FontOption>().value();
    const FontOption* localFont = _sliderOptions.find<FontOption>();
    return localFont ? localFont->getFont(baseFont) : baseFont;
}


void Slider::setFont(const Gfx::Font& font)
{
    FontOption fontOption;
    fontOption.setFont(font);
    _sliderOptions.set(fontOption);
    invalidate();
}


void Slider::setFontSize(std::size_t size)
{
    const FontOption* localFont = _sliderOptions.find<FontOption>();
    FontOption font = localFont ? *localFont : FontOption();
    font.setSize(size);
    _sliderOptions.set(font);
    invalidate();
}


void Slider::setFontWeight(Gfx::Font::Weight weight)
{
    const FontOption* localFont = _sliderOptions.find<FontOption>();
    FontOption font = localFont ? *localFont : FontOption();
    font.setWeight(weight);
    _sliderOptions.set(font);
    invalidate();
}


void Slider::setFontSlant(Gfx::Font::Slant slant)
{
    const FontOption* localFont = _sliderOptions.find<FontOption>();
    FontOption font = localFont ? *localFont : FontOption();
    font.setSlant(slant);
    _sliderOptions.set(font);
    invalidate();
}


void Slider::setRenderer(SliderRenderer* renderer)
{
    const StyleOptions& options = Application::instance().styleOptions();

    if(renderer)
        _sliderStyle.bind(*renderer, options, _sliderOptions);
    else
        _sliderStyle.bind(Application::instance().style(), options, _sliderOptions);

    invalidate();
}


SliderState Slider::sliderState() const
{
    SliderState state;
    state.setEnabled( isEnabled() );
    state.setHovered( _isHighlighted && isEnabled() );
    state.setFocused( hasFocus() );
    return state;
}


void Slider::onInvalidate()
{
    Base::onInvalidate();

    const Style& style = Application::instance().style();
    const StyleOptions& options = Application::instance().styleOptions();
    _sliderStyle.rebind(style, options, _sliderOptions);

    relayout();
}


Gfx::SizeF Slider::onMeasure(const SizePolicy& policy)
{
    SliderRenderer* renderer = _sliderStyle.renderer();
    if( ! renderer )
        return Gfx::SizeF();

    Gfx::SizeF contentSize(policy.width(), 0);
    Gfx::SizeF sz = renderer->measureFrame(surface(), contentSize);

    return Gfx::SizeF( sz.width() + padding().leftRight(),
                       sz.height() + padding().topBottom() );
}


void Slider::onLayout(const Gfx::RectF& rect)
{
    Base::onLayout(rect);

    SliderRenderer* renderer = _sliderStyle.renderer();
    if( ! renderer )
        return;

    Gfx::SizeF trackSize  = renderer->measureTrack(surface());
    Gfx::SizeF handleSize = renderer->measureHandle(surface());

    renderer->layoutChrome(surface(), Gfx::RectF(size()),
                           trackSize, handleSize, _trackRect, _handleRect);
}


void Slider::onPaint(PaintContext& context, const Gfx::RectF& /*updateRect*/)
{
    SliderRenderer* renderer = _sliderStyle.renderer();
    if( ! renderer )
        return;

    Gfx::RectF handleRect = _handleRect;
    renderer->layoutHandle(surface(), _trackRect, toFraction(), handleRect);

    SliderState state = sliderState();

    onPaintChrome(context, Gfx::RectF(size()), _trackRect, handleRect, state);
}


void Slider::onPaintChrome(PaintContext& context,
                           const Gfx::RectF& rect,
                           const Gfx::RectF& trackRect,
                           const Gfx::RectF& handleRect,
                           const SliderState& state)
{
    SliderRenderer* renderer = _sliderStyle.renderer();
    if( ! renderer )
        return;

    renderer->renderChrome(context, rect, trackRect, handleRect, state);
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
