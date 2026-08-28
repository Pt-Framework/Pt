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
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
  MA 02110-1301 USA
*/

#include <Pt/Forms/ProgressBar.h>
#include <Pt/Forms/ProgressBarStyler.h>
#include <Pt/Forms/Application.h>
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
    return _styler.background();
}


void ProgressBar::setBackground(const Gfx::Brush& b)
{
    _styler.setBackground(b);
    invalidate();
}


const Gfx::Brush& ProgressBar::foreground() const
{
    return _styler.foreground();
}


void ProgressBar::setForeground(const Gfx::Brush& b)
{
    _styler.setForeground(b);
    invalidate();
}


const Gfx::Pen& ProgressBar::contour() const
{
    return _styler.contour();
}


void ProgressBar::setContour(const Gfx::Pen& p)
{
    _styler.setContour(p);
    invalidate();
}


const Gfx::Color& ProgressBar::textColor() const
{
    return _styler.textColor();
}


void ProgressBar::setTextColor(const Gfx::Color& color)
{
    _styler.setTextColor(color);
    invalidate();
}


Gfx::Font ProgressBar::font() const
{
    return _styler.font();
}


void ProgressBar::setFont(const Gfx::Font& font)
{
    _styler.setFont(font);
    invalidate();
}


void ProgressBar::setFontSize(std::size_t size)
{
    _styler.setFontSize(size);
    invalidate();
}


void ProgressBar::setFontWeight(Gfx::Font::Weight weight)
{
    _styler.setFontWeight(weight);
    invalidate();
}


void ProgressBar::setFontSlant(Gfx::Font::Slant slant)
{
    _styler.setFontSlant(slant);
    invalidate();
}


void ProgressBar::setRenderer(ProgressBarRenderer* renderer)
{
    _styler.setRenderer(renderer);
    _styler.bind(Application::instance().style(), Application::instance().styleOptions());

    _barRect = Gfx::RectF();
    _textRect = Gfx::RectF();
    _trackRect = Gfx::RectF();
    _chunkRect = Gfx::RectF();

    invalidate();
}


ProgressBarState ProgressBar::progressBarState() const
{
    ProgressBarState state;
    state.setEnabled( isEnabled() );
    state.setFocused( hasFocus() );
    return state;
}


void ProgressBar::onInvalidate()
{
    Base::onInvalidate();

    const Style& style = Application::instance().style();
    const StyleOptions& options = Application::instance().styleOptions();

    if( _styler.bind(style, options) )
    {
        _barRect = Gfx::RectF();
        _textRect = Gfx::RectF();
        _trackRect = Gfx::RectF();
        _chunkRect = Gfx::RectF();
    }

    relayout();
}


Gfx::SizeF ProgressBar::onMeasure(const SizePolicy& policy)
{
    Gfx::SizeF contentSize(policy.width(), 0);
    Gfx::SizeF sz = _styler.measureFrame(surface(), contentSize);

    return Gfx::SizeF( sz.width() + padding().leftRight(),
                       sz.height() + padding().topBottom() );
}


void ProgressBar::onLayout(const Gfx::RectF& rect)
{
    Base::onLayout(rect);

    Gfx::SizeF barSize = _styler.measureBar(surface());
    Gfx::SizeF textSize(0, 0);

    _styler.layoutChrome(surface(), Gfx::RectF(size()), barSize, textSize, _barRect, _textRect);

    float ratio = progress();
    _styler.layoutBar(surface(), _barRect, ratio, _trackRect, _chunkRect);
}


void ProgressBar::onPaint(PaintContext& context, const Gfx::RectF& /*updateRect*/)
{
    ProgressBarState state = progressBarState();
    String txt;

    onPaintChrome(context, Gfx::RectF(size()), _trackRect, _chunkRect, _textRect, txt, Gfx::PointF(), state);
}


void ProgressBar::onPaintChrome(PaintContext& context,
                                const Gfx::RectF& rect,
                                const Gfx::RectF& trackRect,
                                const Gfx::RectF& chunkRect,
                                const Gfx::RectF& textRect,
                                const String& text,
                                const Gfx::PointF& textPos,
                                const ProgressBarState& state)
{
    _styler.renderChrome(context, rect, trackRect, chunkRect, textRect, text, textPos, state);
}

} // namespace

} // namespace
