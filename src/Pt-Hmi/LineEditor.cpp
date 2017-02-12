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
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.
 
 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, 
 MA 02110-1301 USA
*/

#include <Pt/Hmi/LineEditor.h>
#include <Pt/Hmi/Painter.h>

namespace Pt {

namespace Hmi {

//////////////////////////////////////////////////////////////////////////
// TextLine
//////////////////////////////////////////////////////////////////////////

TextLine::TextLine()
{
}


TextLine::~TextLine()
{
}


const Gfx::PointF& TextLine::position() const
{
    return _position;
}

        
void TextLine::setPosition(const Gfx::PointF& p)
{
    _position = p;
}


void TextLine::setPosition(double x, double y)
{
    _position.set(x, y);
}
   

double TextLine::width() const
{
    return _textMetrics.width();
}


double TextLine::height() const
{
    return _textMetrics.ascent() + _textMetrics.descent();
}


double TextLine::ascent() const
{
    return _textMetrics.ascent();
}


double TextLine::descent() const
{
    return _textMetrics.descent();
}


void TextLine::setText(const Pt::String& text, const Gfx::Font& font)
{
    _text = text;
    _font = font;

    _textMetrics = Hmi::Painter::fontMetrics(_font, _text);
}


double TextLine::cursorToX(std::size_t cursorPosition) const
{
    Pt::String left;
    if( cursorPosition <= _text.size() && ! _text.empty() ) 
        left = _text.substr(0, cursorPosition);

    Gfx::FontMetrics fmLeft = Hmi::Painter::fontMetrics(_font, left);

    return fmLeft.width();
}


std::size_t TextLine::xToCursor(double x) const
{
    const Pt::String& str = _text;

    if( str.empty() )
        return 0;

    std::size_t textX = x - _position.x();

    // estimate cursor position
    Gfx::FontMetrics fm = Hmi::Painter::fontMetrics( _font, str );
    std::size_t widthPerChar = fm.width() / str.size();
    std::size_t pos = textX / widthPerChar;

    if( pos >= str.size() )
        pos = str.size() - 1;

    Pt::String left = str.substr(0, pos + 1);
    fm = Hmi::Painter::fontMetrics( _font, left );

    if( textX < fm.width() )
    {
        // cursor position was over estimated, so search left
        for( ; pos > 0; --pos)
        {
            left = str.substr(0, pos);
            fm = Hmi::Painter::fontMetrics( _font, left );
      
            if( textX >= fm.width() )
                break;
        }
    }
    else 
    {
        // cursor position was under estimated, so search right
        for(++pos ; pos < str.size(); ++pos)
        {
            left = str.substr(0, pos + 1);
            fm = Hmi::Painter::fontMetrics( _font, left );
      
            if( textX < fm.width() )
                break;
        }
    }

    return pos;
}

//////////////////////////////////////////////////////////////////////////
// LineEditor
//////////////////////////////////////////////////////////////////////////

LineEditor::LineEditor()
: _cursorPosition(0)
, _scrollOffset(0)
{
}


LineEditor::~LineEditor()
{
}


const Gfx::SizeF& LineEditor::size() const
{
    return _size;
}


const Gfx::PointF& LineEditor::position() const
{
    return _position;
}


void LineEditor::setPosition(const Gfx::PointF& p)
{
    _position = p;
}


void LineEditor::setSize(const Gfx::SizeF& s)
{
    _size = s;
}


void LineEditor::setAdjustment(Adjustment a)
{
    _adjustment = a;
}


Adjustment LineEditor::adjustment() const
{
    return _adjustment;
}


const Pt::String& LineEditor::text() const
{
    return _text;
}


void LineEditor::setText(const Pt::String& s)
{
    _text = s;
    _cursorPosition = 0;
    _scrollOffset = 0;
}


const Gfx::Font& LineEditor::font() const
{
    return _font;
}


void LineEditor::setFont(const Gfx::Font& font)
{
    _font = font;
}


std::size_t LineEditor::cursorPosition() const
{
    return _cursorPosition;
}


void LineEditor::setCursorPosition(std::size_t n)
{
    if( n > _text.size() )
        n = _text.size();

    _cursorPosition = n;
}


void LineEditor::insert(Char ch)
{
    _text.insert(_cursorPosition, 1, ch);
    _cursorPosition++;
}


void LineEditor::left()
{
    if(_cursorPosition > 0)
        _cursorPosition--;
}


void LineEditor::right()
{
    if( _cursorPosition < _text.size() )
        _cursorPosition++;
}


void LineEditor::del()
{
    if( ! _text.empty() )
        _text.erase(_cursorPosition, 1);
}


void LineEditor::backspace()
{
    if( _cursorPosition > 0 && ! _text.empty() )
        _text.erase(--_cursorPosition, 1);
}


void LineEditor::layout(TextLine& line)
{
    line.setText(_text, _font);
    
    double cursorX = line.cursorToX(_cursorPosition);
    double maxX = _size.width() + _scrollOffset;

    double lineX = 0;
    double lineY = (_size.height() - line.height()) / 2;

    if(line.width() < _size.width() )
    {
        switch(_adjustment)
        {
            default:
            case Adjustment::Left:
                lineX = 0;
                break;

            case Adjustment::Right:
                lineX = _size.width() - line.width();
                break;

            case Adjustment::Center:
                lineX = (_size.width() - line.width()) / 2;
                break;
        }
    }
    else
    {
        if( cursorX > maxX )
        {
            double delta = cursorX - maxX;
            _scrollOffset += delta;
        }

        if( cursorX < _scrollOffset )
        {
            double delta = _scrollOffset - cursorX;
            _scrollOffset -= delta;
        }

        lineX = - _scrollOffset;
    }

    line.setPosition( lineX + _position.x(), 
                      lineY + _position.y());
}

} // namespace

} // namespace
