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

#include <Pt/Hmi/Slider.h>
#include <Pt/Hmi/Painter.h>

namespace Pt {

namespace Hmi {

Slider::Slider()
: _position(50)
, _min(0)
, _max(100)
, _interval(0)
, _hasRenderer(false)
, _dragingMode(false)
{
	_knobeRect.set(0, 6, 11, 17);
}


Slider::~Slider()
{
}


void Slider::setPosition(int pos)
{
	if( pos > _max)
	{
		_position = _max;
	}
	if(pos < 0 )
	{
		_position = 0;
	}

	_position = pos;

	invalidate();
}



float Slider::position() const
{
	if( _position <= _min )
		return 0.f;

	if(_position > _max)
		return 1.f;

	return static_cast<float>(_position );
	//return static_cast<float>(_position - _min)/(_max - _min);
}


void Slider::setRange(int min, int max)
{
	_min = min;
	_max = max;

	invalidate();
}


int Slider::interval() const
{
	return _max - _min;
}


const Gfx::RectF& Slider::knobeRect() const
{ 
	return _knobeRect;
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
    update();
}

const Gfx::Color& Slider::foreground() const
{
    return _foreground ? *_foreground
											 : Application::instance().styleOptions().accentColor();
}


void Slider::setForeground(const Gfx::Color& b)
{
    _foreground.reset( new Gfx::Color(b) );
    update();
}


const Gfx::Pen& Slider::contour() const
{
    return _contour ? *_contour 
										: Application::instance().styleOptions().contour();
}


void Slider::setContour(const Gfx::Pen& p)
{
    _contour.reset( new Gfx::Pen(p) );
    update();
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


const std::string& Slider::font() const
{
    return _fontName ? *_fontName
                     : Application::instance().styleOptions().font().name();
}


void Slider::setFont(const std::string& fontName)
{
    _fontName.reset( new std::string(fontName) );
    invalidate();
}


std::size_t Slider::fontSize() const
{

    return _fontSize ? *_fontSize
                     : Application::instance().styleOptions().font().size();
}


void Slider::setFontSize(const std::size_t s)
{
    _fontSize.reset( new std::size_t(s) );
    invalidate();
}


Gfx::Font::Style Slider::fontStyle() const
{
    return _fontStyle ? *_fontStyle
                      : Application::instance().styleOptions().font().style();
}


void Slider::setFontStyle(Gfx::Font::Style style)
{
    _fontStyle.reset( new Gfx::Font::Style(style) );
    invalidate();
}


void Slider::setRenderer(SliderRenderer* renderer)
{
    _renderer.reset(renderer);
    _hasRenderer = renderer != 0;

    invalidate();
}


Gfx::SizeF Slider::onAutoSize(const SizePolicy& policy) const
{
    return Base::onAutoSize(policy);
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
    _font = Gfx::Font(font(), fontSize(), fontStyle());

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

	if( ev.isPress() && _knobeRect.contains( ev.position() ))
	{
		_dragingMode = true;
		_beginDrag = ev.position();
	}

	if( ev.isReleased() )
		_dragingMode = false;

	if( _dragingMode )
	{
		_position  +=  ev.position().x() - _beginDrag.x();

		_knobeRect.set(_position, _position+6 , _knobeRect.top(), _knobeRect.bottom() );

		invalidate();
	}

  return true;
}


void Slider::onTouchEvent(const TouchEvent& ev)
{
	Base::onTouchEvent(ev);
}

} // namespace

} // namespace
