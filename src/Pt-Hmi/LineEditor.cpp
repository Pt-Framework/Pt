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
// TextBlock
//////////////////////////////////////////////////////////////////////////

TextBlock::TextBlock()
: _position()
, _size()
, _maxWidth(10000)
, _adjustment(Adjustment::Center)
{
}


TextBlock::~TextBlock()
{
}


const Gfx::PointF& TextBlock::position() const
{
    return _position;
}

      
void TextBlock::setPosition(const Gfx::PointF& p)
{
    _position = p;
}


const Gfx::SizeF& TextBlock::size() const
{
    return _size;
}


double TextBlock::width() const
{
    return _size.width();
}


double TextBlock::height() const
{
    return _size.height();
}

        
void TextBlock::setMaxWidth(double w)
{
    _maxWidth = w;
}


Adjustment TextBlock::adjustment() const
{
    return _adjustment;
}


void TextBlock::setAdjustment(Adjustment a)
{
    _adjustment = a;
}


void TextBlock::setText(const Pt::String& text, const Gfx::Font& font)
{
    _text = text;

    double lineHeight = font.size() * 1.5;
    double lineOffset = (lineHeight - font.size()) / 2;

    double lineY = 0;
    Pt::String line;
    Pt::String segment;
    
    Pt::String::iterator consumed = _text.begin();
    Pt::String::iterator it = _text.begin();
    
    while( consumed < _text.end() )
    {
        while( it != _text.end() && ! Pt::isspace(*it) )
        {
            segment += *it++;
        }

        Gfx::FontMetrics fm = Painter::fontMetrics(font, segment);
        if(fm.width() < _maxWidth)
        {
            line = segment;
            segment += *it;
            continue;
        }

        Pt::String::size_type n = line.size();
        if( line.size() < segment.size() )
        {
            Pt::Char ch = segment[ line.size() ];
            if( Pt::isspace(ch) )
            {
                n++;
            }
        }
        
        segment.erase(0, n);
        consumed += n;

        TextLine textLine;
        textLine.setText(line, font);
        
        double lineX = 0;

        switch(_adjustment)
        {
            default:
            case Adjustment::Left:
                lineX = 0;
                break;

            case Adjustment::Right:
                lineX = _maxWidth - textLine.width();
                break;

            case Adjustment::Center:
                lineX = (_maxWidth - textLine.width()) / 2;
                break;
        } 
        
        textLine.setPosition(lineX, 
                             lineY + lineOffset);

        _lines.push_back(textLine);

        lineY += lineHeight;
        line.clear();
    }
}

//////////////////////////////////////////////////////////////////////////
// LineEditor
//////////////////////////////////////////////////////////////////////////

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
    
    _cursorPosition = 0;
    _scrollOffset = 0;
}


const Pt::String& LineEditor::displayText() const
{
    if(_isMasked)
        return _displayText;
    else
        return _text;
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


void LineEditor::layout(TextLine& line)
{
    layout( displayText(), line );
}


void LineEditor::layout(const Pt::String& text, TextLine& line)
{
    line.setText(text, _font);

    double lineX = 0;
    double lineY = (_size.height() - line.height()) / 2;

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
        double cursorX = line.cursorToX(_cursorPosition);
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

    line.setPosition( lineX + _position.x(), 
                      lineY + _position.y());
}

} // namespace

} // namespace
