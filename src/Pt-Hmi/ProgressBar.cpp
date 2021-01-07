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

#include <Pt/Hmi/ProgressBar.h>
#include <Pt/Gfx/Painter.h>

namespace Pt {

namespace Hmi {

ProgressBar::ProgressBar()
: _value(50)
, _min(0)
, _max(100)
, _hasRenderer(false)
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
    return _background ? *_background
                       : Application::instance().styleOptions().foreground();
}


void ProgressBar::setBackground(const Gfx::Brush& b)
{
    _background.reset( new Gfx::Brush(b) );
    update();
}

const Gfx::Color& ProgressBar::foreground() const
{
    return _foreground ? *_foreground
                       : Application::instance().styleOptions().accentColor();
}


void ProgressBar::setForeground(const Gfx::Color& b)
{
    _foreground.reset( new Gfx::Color(b) );
    update();
}


const Gfx::Pen& ProgressBar::contour() const
{
    return _contour ? *_contour 
                    : Application::instance().styleOptions().contour();
}


void ProgressBar::setContour(const Gfx::Pen& p)
{
    _contour.reset( new Gfx::Pen(p) );
    update();
}


const Gfx::Color& ProgressBar::textColor() const
{
    return _textColor ? *_textColor
                      : Application::instance().styleOptions().textColor();
}


void ProgressBar::setTextColor(const Gfx::Color& color)
{
    _textColor.reset( new Gfx::Color(color) );
    invalidate();
}


const std::string& ProgressBar::font() const
{
    return _fontName ? *_fontName
                     : Application::instance().styleOptions().font().name();
}


void ProgressBar::setFont(const std::string& fontName)
{
    _fontName.reset( new std::string(fontName) );
    invalidate();
}


std::size_t ProgressBar::fontSize() const
{

    return _fontSize ? *_fontSize
                     : Application::instance().styleOptions().font().size();
}


void ProgressBar::setFontSize(const std::size_t s)
{
    _fontSize.reset( new std::size_t(s) );
    invalidate();
}


const std::string& ProgressBar::fontStyle() const
{
    return _fontStyle ? *_fontStyle
                      : Application::instance().styleOptions().font().style();
}


void ProgressBar::setFontStyle(const std::string& style)
{
    _fontStyle.reset( new std::string(style) );
    invalidate();
}


void ProgressBar::setRenderer(ProgressBarRenderer* renderer)
{
    _renderer.reset(renderer);
    _hasRenderer = renderer != 0;

    invalidate();
}


Gfx::SizeF ProgressBar::onMeasure(const SizePolicy& policy)
{
    double itemsWidth = policy.width();

    // TODO: get requred height from renderer
    double itemsHeight = 15;

    return Gfx::SizeF(policy.width(), 
                      itemsHeight + padding().topBottom() );
}


void ProgressBar::onInvalidate()
{
    Base::onInvalidate();

    const StyleOptions& options = Application::instance().styleOptions();
    const Style& style = Application::instance().style();

    _backgroundBrush = background();
    _foregroundBrush = foreground();
    _contourPen = contour();
    _textPen = textColor();
    _font = Gfx::Font(font(), fontSize(), fontStyle());

    if( ! _hasRenderer )
        _renderer.reset( style.get<ProgressBarRenderer>() );
    
    if( ! _renderer )
        return;

    _renderer->prepare(*this, options, _backgroundBrush, _foregroundBrush,
                       _contourPen, _textPen, _font);
}


void ProgressBar::onPaint(Gfx::PaintSurface& surface, const Gfx::RectF& rect)
{
    const StyleOptions& options = Application::instance().styleOptions();

    if( ! _renderer)
        return;

    Gfx::Painter painter(surface);
    painter.setClip(rect);

    _renderer->render(*this, options, painter, rect, 
                       _backgroundBrush, _foregroundBrush, _contourPen, _textPen, _font);
}

} // namespace

} // namespace
