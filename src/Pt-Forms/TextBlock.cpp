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

#include <Pt/Forms/TextBlock.h>
#include <Pt/Gfx/Painter.h>
#include <Pt/Gfx/FontMetrics.h>
#include <Pt/Forms/PixmapSurface.h>
#include <Pt/Forms/Application.h>
#include <Pt/Forms/Screen.h>
#include <Pt/Forms/Painter.h>
#include <cassert>

namespace Pt {

namespace Forms {

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
    return _textMetrics.advance();
}


double TextLine::height() const
{
    return _fontMetrics.height();
}


double TextLine::maxHeight() const
{
    return _fontMetrics.ascent() + _fontMetrics.descent();
}


double TextLine::ascent() const
{
    return _fontMetrics.ascent();
}


double TextLine::descent() const
{
    return _fontMetrics.descent();
}


const Pt::String& TextLine::text() const
{
    return _text;
}


//void TextLine::setText(const Pt::String& text, const Gfx::Font& font)
//{
//    _text = text;
//    _font = font;
//
//    _textMetrics = PixmapSurface::textMetrics(_font, _text);
//}


void TextLine::setText(const Pt::String& text, const Gfx::TextMetrics& tm,
                      const Gfx::FontMetrics& fm)
{
    _text = text;
    _textMetrics = tm;
    _fontMetrics = fm;
}


double TextLine::cursorToX(const Painter& painter, std::size_t cursorPosition) const
{
    Pt::String left;
    if( cursorPosition <= _text.size() && ! _text.empty() )
        left = _text.substr(0, cursorPosition);

    Gfx::TextMetrics fmLeft = painter.textMetrics(left);

    return fmLeft.advance();
}


std::size_t TextLine::xToCursor(const Painter& painter, double x) const
{
    const Pt::String& str = _text;

    if( str.empty() )
        return 0;

    std::size_t textX = x - _position.x();

    // estimate cursor position
    Gfx::TextMetrics fm = painter.textMetrics(str);
    std::size_t widthPerChar = fm.advance() / str.size();

    if(widthPerChar == 0)
      return 0;

    std::size_t pos = textX / widthPerChar;

    if( pos >= str.size() )
        pos = str.size() - 1;

    Pt::String left = str.substr(0, pos + 1);
    fm = painter.textMetrics(left);

    if( textX < fm.advance() )
    {
        // cursor position was over estimated, so search left
        for( ; pos > 0; --pos)
        {
            left = str.substr(0, pos);
            fm = painter.textMetrics(left);

            if( textX >= fm.advance() )
                break;
        }
    }
    else
    {
        // cursor position was under estimated, so search right
        for(++pos ; pos < str.size(); ++pos)
        {
            left = str.substr(0, pos + 1);
            fm = painter.textMetrics(left);

            if( textX < fm.advance() )
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
, _lineSpacing(0)
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


double TextBlock::maxWidth() const
{
    return _maxWidth;
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


TextBlock::Iterator TextBlock::begin()
{
    return _lines.empty() ? 0 : &_lines[0];
}


TextBlock::Iterator TextBlock::end()
{
    TextLine* line = _lines.empty() ? 0
                                    : &_lines[0] + _lines.size();
    return line;
}


TextBlock::ConstIterator TextBlock::begin() const
{
    return _lines.empty() ? 0 : &_lines[0];
}


TextBlock::ConstIterator TextBlock::end() const
{
    const TextLine* line = _lines.empty() ? 0
                                          : &_lines[0] + _lines.size();
    return line;
}


void TextBlock::layout(const Painter& painter, const Pt::String& text)
{
    _lines.clear();
    _size.set(0, 0);

    typedef std::pair<std::size_t, std::size_t> Word;
    typedef std::vector<Word> Words;

    Words words;
    Word word(0, 0);
    bool onSpace = true;

    for(std::size_t n = 0; n < text.size(); ++n)
    {
        if( Pt::isspace( text[n] ) )
        {
            if(onSpace)
                continue;

            word.second = n;
            words.push_back(word);
            onSpace = true;
        }
        else
        {
            if( ! onSpace )
                continue;

            word.first = n;
            onSpace = false;
        }
    }

    if( ! onSpace )
    {
        word.second = text.size();
        words.push_back(word);
    }

    std::size_t lineBegin = 0;
    std::size_t lineLength = 0;
    std::size_t wordBegin = 0;
    std::size_t wordEnd = 0;
    std::size_t prevWordEnd = 0;
    Pt::String segment;
    Gfx::TextMetrics lineMetrics;
    Gfx::FontMetrics fm = painter.fontMetrics();

    Words::iterator it;
    for(it = words.begin(); it != words.end(); ++it)
    {
        prevWordEnd = wordEnd;
        wordBegin = it->first;
        wordEnd = it->second;

        segment.append(&text[prevWordEnd], wordEnd - prevWordEnd);

        Gfx::TextMetrics tm = painter.textMetrics(segment);
        double segmentWidth = tm.advance();

        if(segmentWidth <= _maxWidth || lineLength == 0)
        {
            lineLength = segment.size();
            lineMetrics = tm;
            continue;
        }

        // remove the line from the segment
        segment.assign(&text[wordBegin], wordEnd - wordBegin );

        // add the whitespace after the last word of the line
        lineLength += wordBegin - prevWordEnd;

        Pt::String line(&text[lineBegin], lineLength);
        //line += ';';
        addLine(line, lineMetrics, fm);

        lineBegin = wordBegin;
        lineLength = wordEnd - wordBegin;
        lineMetrics = painter.textMetrics(segment);
    }

    // add the whitespace after the last word
    lineLength += text.size() - wordEnd;

    Pt::String line(&text[lineBegin], lineLength);
    //line += ';';
    addLine(line, lineMetrics, fm);
}


void TextBlock::addLine(const Pt::String& line,
                        const Gfx::TextMetrics& tm,
                        const Gfx::FontMetrics& fm)
{
    double lineWidth = tm.advance();
    double lineHeight = fm.height();

    double lineX = 0;
    double lineY = _size.height();

    bool firstLine = _lines.empty();

    if( ! firstLine )
        lineY += _lineSpacing;

    _lines.resize(_lines.size() + 1);
    TextLine& textLine = _lines.back();

    textLine.setText(line, tm, fm);

    switch(_adjustment)
    {
        default:
        case Adjustment::Left:
            lineX = 0;
            break;

        case Adjustment::Right:
            lineX = _maxWidth - tm.advance();
            break;

        case Adjustment::Center:
            lineX = (_maxWidth - tm.advance()) / 2;
            break;
    }

    textLine.setPosition(lineX, lineY);

    _size.setWidth( std::max<double>(_size.width(), lineWidth) );
    _size.addHeight(lineHeight);

    if( ! firstLine )
        _size.addHeight(_lineSpacing);
}

} // namespace

} // namespace
