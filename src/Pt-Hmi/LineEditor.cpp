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


const Gfx::SizeF& TextLine::size() const
{
    return _size;
}


void TextLine::setSize(const Gfx::SizeF& s)
{
    _size = s;
}
   

void TextLine::setText(const Pt::String& text, const Gfx::Font& font)
{
    _text = text;
    _font = font;

    Gfx::FontMetrics fm = Hmi::Painter::fontMetrics(_font, _text);
    _position.setY( fm.ascent() );
}


double TextLine::cursorToX(std::size_t cursorPosition) const
{
    Pt::String left;
    if( cursorPosition <= _text.size() && ! _text.empty() ) 
        left = _text.substr(0, cursorPosition);

    Gfx::FontMetrics fmLeft = Hmi::Painter::fontMetrics( _font, left );

    return fmLeft.width() + _position.x();
}

//////////////////////////////////////////////////////////////////////////
// LineEditor
//////////////////////////////////////////////////////////////////////////

LineEditor::LineEditor()
: _cursorPosition(0)
{
}


LineEditor::~LineEditor()
{
}


const Pt::String& LineEditor::text() const
{
    return _text;
}


void LineEditor::insert(Char ch)
{
    _text.insert(_cursorPosition, 1, ch);
    _cursorPosition++;
}


void LineEditor::setFont(const Gfx::Font& font)
{
    _font = font;
}


void LineEditor::setSize(const Gfx::SizeF& s)
{
    _size = s;
}


void LineEditor::layout(TextLine& line)
{
    line.setPosition(0, 0);
    line.setSize(_size);
    line.setText(_text, _font);
    
    double cursorX = line.cursorToX(_cursorPosition);

    double x = 0;
    double y = line.position().y();

    if( cursorX > line.size().width() )
    {
        x = line.size().width() - cursorX;
    }

    line.setPosition(x, y);
}

} // namespace

} // namespace
