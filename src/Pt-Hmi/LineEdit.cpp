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
            --_cursorPosition;
    }
    else if( ev.key().code() == Pt::Hmi::Key::ArrowRight )
    {
        if( _cursorPosition < _text.size() )
            ++_cursorPosition;
    }
    else if( ev.key().code() == Pt::Hmi::Key::Backspace )
    {
        if(_text.empty() || _cursorPosition == 0)
            return;

        --_cursorPosition;
        _text.erase(_cursorPosition, 1);
    }
    else 
    {
        Pt::Char ch = ev.unicode();
        if( Pt::isprint(ch) )
        {
            _text.insert(_cursorPosition, 1, ch);
            ++_cursorPosition;
        }
    }
    
    update();
}


void LineEdit::onPaint(PaintSurface& surface, const Gfx::RectF& rect)
{
    const LineEditRenderer* renderer = getFacet<LineEditRenderer>();
    if(renderer)
        renderer->render(*this, surface, rect);
}

} // namespace

} // namespace
