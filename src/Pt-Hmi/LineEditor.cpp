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
#include <Pt/Gfx/Painter.h>

namespace Pt {

namespace Hmi {

static const char maskChar = '*';

LineEditor::LineEditor()
: _isMasked(false)
, _cursorPosition(0)
, _scrollOffset(0)
{
}


LineEditor::~LineEditor()
{
}


const Gfx::PointF& LineEditor::position() const
{
    return _position;
}


void LineEditor::setPosition(const Gfx::PointF& p)
{
    _position = p;
}


const Gfx::SizeF& LineEditor::size() const
{
    return _size;
}


void LineEditor::setSize(const Gfx::SizeF& s)
{
    _size = s;
}


Adjustment LineEditor::adjustment() const
{
    return _adjustment;
}


void LineEditor::setAdjustment(Adjustment a)
{
    _adjustment = a;
}


bool LineEditor::isMasked() const
{
    return _isMasked;
}


void LineEditor::setMasked(bool m)
{
    _isMasked = m;

    if(_isMasked)
        _displayText.assign(_text.size(), maskChar);
    else
        _displayText.clear();
}


const Pt::String& LineEditor::text() const
{
    return _text;
}


void LineEditor::setText(const Pt::String& s)
{
    _text = s;
    
    if(_isMasked)
        _displayText.assign(_text.size(), maskChar);
    
    if( _cursorPosition > _text.size() )
        _cursorPosition = _text.size();

    //_cursorPosition = 0;
    //_scrollOffset = 0;
}


const Pt::String& LineEditor::displayText() const
{
    if(_isMasked)
        return _displayText;
    else
        return _text;
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


bool LineEditor::isEmpty() const
{
    return _text.empty();
}


void LineEditor::clear()
{
    _text.clear();
    _displayText.clear();
    _scrollOffset = 0;
    _cursorPosition = 0;
}


void LineEditor::insert(Char ch)
{
    _text.insert(_cursorPosition, 1, ch);

    if(_isMasked)
        _displayText += maskChar;

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

    if( ! _displayText.empty() )
        _displayText.erase( _displayText.begin() );
}


void LineEditor::backspace()
{
    if( _cursorPosition > 0 && ! _text.empty() )
    {
        _text.erase(--_cursorPosition, 1);

        if( ! _displayText.empty() )
            _displayText.erase( _displayText.begin() );
    }
}


void LineEditor::layout(Gfx::Painter& painter, TextLine& line)
{
    layout( painter, displayText(), line );
}


void LineEditor::layout(Gfx::Painter& painter, const Pt::String& text, TextLine& line)
{
    Gfx::FontMetrics fm = painter.fontMetrics(text);
    line.setText(text, fm);

    double lineX = 0;
    double lineY = (_size.height() - line.maxHeight()) / 2;

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
    
    if( line.width() >= _size.width() )
    {
        double cursorX = line.cursorToX(painter, _cursorPosition);
        double maxX = _size.width() + _scrollOffset;

        if( cursorX > maxX )
        {
            double delta = cursorX - maxX;
            _scrollOffset += delta;
        }
        else if( cursorX < _scrollOffset )
        {
            double delta = _scrollOffset - cursorX;
            _scrollOffset -= delta;
        }

        lineX = - _scrollOffset;
    }
    else
      _scrollOffset = 0;

    line.setPosition( lineX + _position.x(), 
                      lineY + _position.y());
}

} // namespace

} // namespace
