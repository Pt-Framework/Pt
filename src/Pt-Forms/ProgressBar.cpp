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
#include <Pt/Forms/ProgressBarStyle.h>
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
    if( const BackgroundOption* background = _progressBarOptions.get<BackgroundOption>() )
        return background->value();

    const StyleOptions& options = Application::instance().styleOptions();
    return options.get<BackgroundOption>()->value();
}


void ProgressBar::setBackground(const Gfx::Brush& b)
{
    BackgroundOption background(b);
    _progressBarOptions.set(background);
    invalidate();
}


const Gfx::Brush& ProgressBar::foreground() const
{
    if( const ForegroundOption* foreground = _progressBarOptions.get<ForegroundOption>() )
        return foreground->value();

    const StyleOptions& options = Application::instance().styleOptions();
    return options.get<ForegroundOption>()->value();
}


void ProgressBar::setForeground(const Gfx::Brush& b)
{
    ForegroundOption foreground(b);
    _progressBarOptions.set(foreground);
    invalidate();
}


const Gfx::Pen& ProgressBar::contour() const
{
    if( const ContourOption* contour = _progressBarOptions.get<ContourOption>() )
        return contour->value();

    const StyleOptions& options = Application::instance().styleOptions();
    return options.get<ContourOption>()->value();
}


void ProgressBar::setContour(const Gfx::Pen& p)
{
    ContourOption contour(p);
    _progressBarOptions.set(contour);
    invalidate();
}


const Gfx::Color& ProgressBar::textColor() const
{
    if( const TextColorOption* textColor = _progressBarOptions.get<TextColorOption>() )
        return textColor->value();

    const StyleOptions& options = Application::instance().styleOptions();
    return options.get<TextColorOption>()->value();
}


void ProgressBar::setTextColor(const Gfx::Color& color)
{
    TextColorOption textColor(color);
    _progressBarOptions.set(textColor);
    invalidate();
}


Gfx::Font ProgressBar::font() const
{
    const StyleOptions& options = Application::instance().styleOptions();
    const Gfx::Font& baseFont = options.get<FontOption>()->value();
    const FontOption* localFont = _progressBarOptions.get<FontOption>();
    return localFont ? localFont->getFont(baseFont) : baseFont;
}


void ProgressBar::setFont(const Gfx::Font& font)
{
    FontOption fontOption;
    fontOption.setFont(font);
    _progressBarOptions.set(fontOption);
    invalidate();
}


void ProgressBar::setFontSize(std::size_t size)
{
    const FontOption* localFont = _progressBarOptions.get<FontOption>();
    FontOption font = localFont ? *localFont : FontOption();
    font.setSize(size);
    _progressBarOptions.set(font);
    invalidate();
}


void ProgressBar::setFontWeight(Gfx::Font::Weight weight)
{
    const FontOption* localFont = _progressBarOptions.get<FontOption>();
    FontOption font = localFont ? *localFont : FontOption();
    font.setWeight(weight);
    _progressBarOptions.set(font);
    invalidate();
}


void ProgressBar::setFontSlant(Gfx::Font::Slant slant)
{
    const FontOption* localFont = _progressBarOptions.get<FontOption>();
    FontOption font = localFont ? *localFont : FontOption();
    font.setSlant(slant);
    _progressBarOptions.set(font);
    invalidate();
}


void ProgressBar::setRenderer(ProgressBarRenderer* renderer)
{
    const StyleOptions& options = Application::instance().styleOptions();

    if(renderer)
        _progressBarStyle.bind(*renderer, options, _progressBarOptions);
    else
        _progressBarStyle.bind(Application::instance().style(), options, _progressBarOptions);

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
    _progressBarStyle.rebind(style, options, _progressBarOptions);

    relayout();
}


Gfx::SizeF ProgressBar::onMeasure(const SizePolicy& policy)
{
    ProgressBarRenderer* renderer = _progressBarStyle.renderer();
    if( ! renderer )
        return Gfx::SizeF(0, 0);

    Gfx::SizeF contentSize(policy.width(), 0);
    Gfx::SizeF sz = renderer->measureFrame(surface(), contentSize);

    return Gfx::SizeF( sz.width() + padding().leftRight(),
                       sz.height() + padding().topBottom() );
}


void ProgressBar::onLayout(const Gfx::RectF& rect)
{
    Base::onLayout(rect);

    ProgressBarRenderer* renderer = _progressBarStyle.renderer();
    if( ! renderer )
        return;

    Gfx::SizeF barSize = renderer->measureBar(surface());
    Gfx::SizeF textSize(0, 0);

    renderer->layoutChrome(surface(), Gfx::RectF(size()), barSize, textSize, _barRect, _textRect);

    float ratio = progress();
    renderer->layoutBar(surface(), _barRect, ratio, _trackRect, _chunkRect);
}


void ProgressBar::onPaint(PaintContext& context, const Gfx::RectF& /*updateRect*/)
{
    ProgressBarRenderer* renderer = _progressBarStyle.renderer();
    if( ! renderer )
        return;

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
    ProgressBarRenderer* renderer = _progressBarStyle.renderer();
    if( ! renderer )
        return;

    renderer->renderChrome(context, rect, trackRect, chunkRect, textRect, text, textPos, state);
}

} // namespace

} // namespace
