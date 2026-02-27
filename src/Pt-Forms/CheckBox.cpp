/* Copyright (C) 2016 Marc Boris Duerner 
   Copyright (C) 2016 Laurentiu-Gheorghe Crisan
  
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

#include <Pt/Forms/CheckBox.h>
#include <Pt/Forms/Application.h>
#include <Pt/Forms/Style.h>
#include <Pt/Forms/StyleOptions.h>
#include <Pt/Forms/PixmapSurface.h>
#include <Pt/Gfx/Painter.h>

namespace Pt {

namespace Forms {

CheckBox::CheckBox()
: _state(Unchecked)
, _hasRenderer(false)
{
}


CheckBox::~CheckBox()
{
}


CheckBox::State CheckBox::state() const
{
    return _state;
}


void CheckBox::setState(State s)
{
    _state = s;
    invalidate();
}


bool CheckBox::isChecked() const
{
    return _state == Checked;
}


const Gfx::Brush& CheckBox::background() const
{
    return _background ? *_background
                       : Application::instance().styleOptions().textBackground();
}


void CheckBox::setBackground(const Gfx::Brush& b)
{
    _background.reset( new Gfx::Brush(b) );
    invalidate();
}


const Gfx::Pen& CheckBox::contour() const
{
    return _contour ? *_contour
                    : Application::instance().styleOptions().contour();
}


void CheckBox::setContour(const Gfx::Pen& p)
{
    _contour.reset( new Gfx::Pen(p) );
    invalidate();
}


const Gfx::ColorF& CheckBox::textColor() const
{
    return _textColor ? *_textColor
                      : Application::instance().styleOptions().textColor();
}


void CheckBox::setTextColor(const Gfx::ColorF& color)
{
    _textColor.reset( new Gfx::ColorF(color) );
    invalidate();
}


const std::string& CheckBox::font() const
{
    return _fontName ? *_fontName
                     : Application::instance().styleOptions().font().name();
}


void CheckBox::setFont(const std::string& fontName)
{
    _fontName.reset( new std::string(fontName) );
    invalidate();
}


std::size_t CheckBox::fontSize() const
{

    return _fontSize ? *_fontSize
                     : Application::instance().styleOptions().font().size();
}


void CheckBox::setFontSize(const std::size_t s)
{
    _fontSize.reset( new std::size_t(s) );
    invalidate();
}


const std::string& CheckBox::fontStyle() const
{
    return _fontStyle ? *_fontStyle
                      : Application::instance().styleOptions().font().style();
}


void CheckBox::setFontStyle(const std::string& style)
{
    _fontStyle.reset( new std::string(style) );
    invalidate();
}


void CheckBox::setRenderer(CheckBoxRenderer* renderer)
{
    _renderer.reset(renderer);
    _hasRenderer = renderer != 0;

    invalidate();
}


void CheckBox::onPressed()
{
    Base::onPressed();
}


void CheckBox::onReleased()
{
    Base::onReleased();

    State changedState = _state == Checked ? Unchecked
                                           : Checked;
    setState(changedState);

    clicked().send();
}


void CheckBox::onCanceled()
{
    Base::onCanceled();
}


Gfx::SizeF CheckBox::onMeasure(const SizePolicy& policy)
{
    //Gfx::TextMetrics tm = PixmapSurface::textMetrics( _font, text() );

    Gfx::Painter painter( surface() );
    painter.setFont(_font);
    Gfx::TextMetrics tm = painter.textMetrics( text() );

    double space = std::min<double>(_boxSize.width() / 2, _font.size() / 2);
    double boxWidth = _boxSize.width();
    double boxHeight = _boxSize.height();

    double itemsWidth = space + boxWidth + space + tm.width();
    double itemsHeight = std::max<double>(tm.lineHeight(), boxHeight);

    return Gfx::SizeF( itemsWidth + padding().leftRight(), 
                       itemsHeight + padding().topBottom() );
}


void CheckBox::onInvalidate()
{
    Base::onInvalidate();

    const StyleOptions& options = Application::instance().styleOptions();
    const Style& style = Application::instance().style();

    _brush = background();
    _pen = contour();
    _textPen = textColor();
    _font = Gfx::Font(font(), fontSize(), fontStyle());

    if( ! _hasRenderer )
        _renderer.reset( style.get<CheckBoxRenderer>() );
    
    if( ! _renderer )
        return;

    _renderer->prepare(*this, options, _brush, _pen, _font, _textPen, _boxSize);
}


void CheckBox::onPaint(PaintSurface& surface, const Gfx::RectF& rect)
{
    const StyleOptions& options = Application::instance().styleOptions();

    if( ! _renderer )
        return;

    Gfx::Painter painter(surface);
    painter.setClip(rect);

    const Gfx::Scaling& scaling = this->scaling();

    double space = std::min<double>(_boxSize.width() / 2, _font.size() / 1.5);

    double boxX = space;
    double boxY = size().height() / 2.0 - _boxSize.height() / 2.0;
    Gfx::RectF boxRect(Gfx::PointF(boxX, boxY), _boxSize);

    _renderer->renderBox(*this, options, painter, rect, 
                          boxRect, _brush, _pen);

    painter.setFont(_font);
    Gfx::TextMetrics tm = painter.textMetrics( text() );

    double textX = space + _boxSize.width() + space;
    //double textY = size().height() / 2.0 - tm.height() / 2.0 + tm.ascent();
    double textY = scaling.align(size().height() / 2.0) + tm.capHeight() / 2.0;

    // NOTE: textY needs to be aligned to match the alignment of the box rect

    Gfx::PointF textPos(textX, textY);
    textPos = scaling.align(textPos);

    Gfx::RectF mnemonicRect;

    const Char* m = mnemonic();
    if(m)
    {
        String::size_type n = text().find(*m);
        if(n != String::npos)
        {
            Pt::String mnemonicText(text(), 0, n);
            Gfx::TextMetrics fmLeft = painter.textMetrics(mnemonicText);

            mnemonicText = *m;
            Gfx::TextMetrics fmChar = painter.textMetrics(mnemonicText);

            mnemonicRect.set( Gfx::PointF(textPos.x() + fmLeft.width(), 
                                          textPos.y() - fmChar.ascent()),
                              Gfx::SizeF(fmChar.width(), 
                                         fmChar.height()) );
        }
    }

    _renderer->renderText(*this, options, painter, rect,
                          text(), textPos, tm, _font, _textPen,
                          mnemonicRect);
}

} // namespace

} // namespace
