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
#include <Pt/Forms/Painter.h>
#include <Pt/Gfx/Painter.h>

namespace Pt {

namespace Forms {

Slider::Slider()
: _position(50)
, _min(0)
, _max(100)
, _isHighlighted(false)
, _hasRenderer(false)
, _fontOverride(0)
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

    _position = pos;
    invalidate();

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
    return _background ? *_background
                       : Application::instance().styleOptions().foreground();
}


void Slider::setBackground(const Gfx::Brush& b)
{
    _background.reset( new Gfx::Brush(b) );
    repaint();
}


const Gfx::Color& Slider::foreground() const
{
    return _foreground ? *_foreground
                       : Application::instance().styleOptions().accentColor();
}


void Slider::setForeground(const Gfx::Color& b)
{
    _foreground.reset( new Gfx::Color(b) );
    repaint();
}


const Gfx::Pen& Slider::contour() const
{
    return _contour ? *_contour 
                    : Application::instance().styleOptions().contour();
}


void Slider::setContour(const Gfx::Pen& p)
{
    _contour.reset( new Gfx::Pen(p) );
    repaint();
}


const Gfx::Color& Slider::textColor() const
{
    return _textColor ? *_textColor
                      : Application::instance().styleOptions().textColor();
}


void Slider::setTextColor(const Gfx::Color& color)
{
    _textColor.reset( new Gfx::Color(color) );
    invalidate();
}


const Gfx::Font& Slider::font() const
{
    return _font;
}


void Slider::setFont(const Gfx::Font& font)
{
    _customFont = font;
    _fontOverride = OverrideAll;

    invalidate();
}


Gfx::Font Slider::getFont() const
{
    const Gfx::Font& base = Application::instance().styleOptions().font();

    if( _fontOverride == 0 )
        return base;

    if( _fontOverride == OverrideAll )
        return _customFont;

    std::size_t sz = (_fontOverride & OverrideSize) ? _customFont.size()
                                                    : base.size();
    Gfx::Font::Weight wt = (_fontOverride & OverrideWeight) ? _customFont.weight()
                                                            : base.weight();
    Gfx::Font::Slant sl = (_fontOverride & OverrideSlant) ? _customFont.slant()
                                                          : base.slant();

    if( base.hasStyleName() )
        return Gfx::Font(base.family(), sz, base.styleName(), wt, sl, base.stretch());

    if( base.category() != Gfx::Font::Category::None )
        return Gfx::Font(base.category(), sz, wt, sl, base.stretch());

    return Gfx::Font(base.family(), sz, wt, sl, base.stretch());
}


void Slider::setFontSize(std::size_t size)
{
    _customFont = Gfx::Font(_customFont.family(), size,
                            _customFont.weight(), _customFont.slant(),
                            _customFont.stretch());
    _fontOverride |= OverrideSize;

    invalidate();
}


void Slider::setFontWeight(Gfx::Font::Weight weight)
{
    _customFont = Gfx::Font(_customFont.family(), _customFont.size(),
                            weight, _customFont.slant(),
                            _customFont.stretch());
    _fontOverride |= OverrideWeight;

    invalidate();
}


void Slider::setFontSlant(Gfx::Font::Slant slant)
{
    _customFont = Gfx::Font(_customFont.family(), _customFont.size(),
                            _customFont.weight(), slant,
                            _customFont.stretch());
    _fontOverride |= OverrideSlant;

    invalidate();
}


void Slider::setRenderer(SliderRenderer* renderer)
{
    _renderer.reset(renderer);
    _hasRenderer = renderer != 0;

    invalidate();
}


Gfx::SizeF Slider::onMeasure(const SizePolicy& policy)
{
    double itemsWidth = policy.width();

    // TODO: get requred height from renderer
    double itemsHeight = 15;

    return Gfx::SizeF(policy.width(), 
                      itemsHeight + padding().topBottom() );
}


void Slider::onInvalidate()
{
    Base::onInvalidate();

    const StyleOptions& options = Application::instance().styleOptions();
    const Style& style = Application::instance().style();

    _backgroundBrush = background();
    _foregroundBrush = foreground();
    _contourPen = contour();
    _textPen = textColor();
    _font = getFont();

    if( ! _hasRenderer )
        _renderer.reset( style.get<SliderRenderer>() );
    
    if( ! _renderer )
        return;

    _renderer->prepare(*this, options, _backgroundBrush, _foregroundBrush,
                       _contourPen, _textPen, _font);
}


void Slider::onPaint(PaintSurface& surface, const Gfx::RectF& rect)
{
    const StyleOptions& options = Application::instance().styleOptions();

    if( ! _renderer )
        return;

    Painter painter(surface);
    painter.setClip(rect);

    _renderer->render(*this, options, painter, rect, 
                      _backgroundBrush, _foregroundBrush, _contourPen, _textPen, _font);
}


bool Slider::onMouseEvent(const MouseEvent& ev)
{
    Base::onMouseEvent(ev);

    //if( ev.isPress() )
    //    grabPointer();
    
    //if( ev.isRelease() )
    //    releasePointer();

    if( ev.isPressed() )
    {
        double x = ev.position().x();
        double width = size().width();

        double offset = x * (_max - _min) / width;
        setPosition(_min + offset);
    }

    return true;
}


bool Slider::onTouchEvent(const TouchEvent& ev)
{
    Base::onTouchEvent(ev);

    //if( ev.isPress() )
    //    grabPointer();
    //
    //if( ev.isRelease() )
    //    releasePointer();

    if( ev.isPressed() )
    {
        double x = ev.position().x();
        double width = size().width();

        double offset = x * (_max - _min) / width;
        setPosition(_min + offset);
    }

    return true;
}


bool Slider::onEnterEvent(const EnterEvent& ev)
{
    Base::onEnterEvent(ev);

    _isHighlighted = true;
    
    invalidate();
    return true;
}


bool Slider::onLeaveEvent(const LeaveEvent& ev)
{
    Base::onLeaveEvent(ev);

    _isHighlighted = false;

    invalidate();
    return true;
}

} // namespace

} // namespace
