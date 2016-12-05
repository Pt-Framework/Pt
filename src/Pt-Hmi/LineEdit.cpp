/* Copyright (C) 2015 Marc Boris Duerner 
   Copyright (C) 2015 Laurentiu-Gheorghe Crisan

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

#include <Pt/Hmi/LineEdit.h>
#include <Pt/Hmi/Painter.h>
#include <Pt/Gfx/ImagePainter.h>

namespace Pt {

namespace Hmi {

LineEdit::LineEdit()
: _cursorPosition(0)
, _hoffset(0)
{
    setAcceptsFocus(true);

    setPadding(5);
}


LineEdit::~LineEdit()
{
}


void LineEdit::setText(const Pt::String& t)
{
    _text = t;
    update();
}


const Pt::String& LineEdit::text() const
{
    return _text;
}


std::size_t LineEdit::cursorPosition() const
{
    return _cursorPosition;
}


void LineEdit::setCursorPosition(std::size_t n)
{
    if( n > _text.size() )
        n = _text.size();

    const StyleOptions* options = getFacet<StyleOptions>();
    if( ! options )
        return;

    if( ! _text.empty() )
    { 
        Pt::String left = _text.substr(0, n);
        Gfx::FontMetrics fm = Hmi::Painter::fontMetrics( options->font(), left );

        if(n > _cursorPosition)
        {
            double pos = fm.width() + padding().leftRight();
            if(pos > size().width() + _hoffset)
                _hoffset = pos - size().width();
        }
        else
        {   
            double pos = fm.width();
            if( pos < _hoffset + padding().left() )
            {
                double delta = (_hoffset + padding().left()) - pos;
                if(delta > _hoffset)
                    _hoffset = 0;
                else
                    _hoffset -= delta;
            }
        }
    }

    _cursorPosition = n;
    update();
}


void LineEdit::onKeyEvent(const KeyEvent& ev)
{  
    Base::onKeyEvent(ev);

    if( ! ev.isPress() )
        return;

    if( ev.key().code() == Pt::Hmi::Key::ArrowLeft )
    {
        if(_cursorPosition > 0)
            setCursorPosition(_cursorPosition - 1);
    }
    else if( ev.key().code() == Pt::Hmi::Key::ArrowRight )
    {
        setCursorPosition(_cursorPosition + 1);
    }
    else if( ev.key().code() == Pt::Hmi::Key::Backspace )
    {
        if( _cursorPosition != 0 && ! _text.empty() )
            _text.erase(_cursorPosition - 1, 1);
        
        if(_cursorPosition > 0)
            setCursorPosition(_cursorPosition - 1);
    }
    else
    {
        Pt::Char ch = ev.unicode();
        if( Pt::isprint(ch) )
        {
            _text.insert(_cursorPosition, 1, ch);

            setCursorPosition(_cursorPosition + 1);
        }
    }
    
    update();
}


std::size_t LineEdit::xToCursor(double x)
{
    if( _text.empty() )
        return 0;

    const StyleOptions* options = getFacet<StyleOptions>();
    if( ! options )
        return 0;

    std::size_t textX = padding().left() < x ?  x - padding().left()
                                             : 0;
    
    std::size_t n = _text.length();
    Gfx::FontMetrics fm = Hmi::Painter::fontMetrics( options->font(), _text );

    std::size_t widthPerChar = fm.width() / _text.size();
    std::size_t pos = textX / widthPerChar;

    if( pos >= _text.size() )
        pos = _text.size() - 1;

    Pt::String left = _text.substr(0, pos + 1);
    fm = Hmi::Painter::fontMetrics( options->font(), left );

    if( textX < fm.width() ) // search left
    {
        for( ; pos > 0; --pos)
        {
            left = _text.substr(0, pos);
            fm = Hmi::Painter::fontMetrics( options->font(), left );
      
            if( textX >= fm.width() )
                break;
        }
    }
    else // if( textX >= fm.width() ) search right 
    {
        for(++pos ; pos < _text.size(); ++pos)
        {
            left = _text.substr(0, pos + 1);
            fm = Hmi::Painter::fontMetrics( options->font(), left );
      
            if( textX < fm.width() )
            {
                --pos;
                break;
            }
        }
    }

    return pos;
}


void LineEdit::onMouseEvent(const MouseEvent& mev)
{
    Base::onMouseEvent(mev);

    if( ! mev.isPress() )
        return;

    std::size_t pos = xToCursor( mev.x() );
    setCursorPosition(pos);
}


void LineEdit::onTouchEvent(const TouchEvent& tev)
{
    Base::onTouchEvent(tev);

    if( ! tev.isPress() )
        return;

    std::size_t pos = xToCursor( tev.x() );
    setCursorPosition(pos);
}


void LineEdit::onPaint(PaintSurface& surface, const Gfx::RectF& rect)
{
    const LineEditRenderer* renderer = getFacet<LineEditRenderer>();
    if(renderer)
        renderer->render(*this, surface, rect);
}

} // namespace

} // namespace
