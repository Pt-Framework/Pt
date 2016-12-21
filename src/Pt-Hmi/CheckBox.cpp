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

#include <Pt/Hmi/CheckBox.h>
#include <Pt/Hmi/Style.h>
#include <Pt/Hmi/StyleOptions.h>
#include <Pt/Hmi/Painter.h>

namespace Pt {

namespace Hmi {

CheckBox::CheckBox()
: _state(Unchecked)
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
    setPressed(s == Checked);
}


bool CheckBox::isChecked() const
{
    return _state == Checked;
}


void CheckBox::onPressed()
{
    Base::onPressed();
}


void CheckBox::onReleased()
{
    Base::onReleased();

    if(_state == Checked)
        _state = Unchecked;
    else
        _state = Checked;

    setPressed(_state == Checked);

    clicked().send(*this);
}


void CheckBox::onInvalidate()
{
    Base::onInvalidate();

    const StyleOptions* options = getFacet<StyleOptions>();
    if( ! options )
      return;

    const CheckBoxRenderer* renderer = getFacet<CheckBoxRenderer>();
    if( ! renderer )
        return;

    renderer->prepare(*this, *options, _brush, _pen, _font, _textPen, _boxSize);
}


void CheckBox::onPaint(PaintSurface& surface, const Gfx::RectF& rect)
{
    const StyleOptions* options = getFacet<StyleOptions>();
    if( ! options )
      return;

    const CheckBoxRenderer* renderer = getFacet<CheckBoxRenderer>();
    if( ! renderer )
        return;

    Painter painter(surface);
    painter.setClip(rect);

    double boxX = padding().left();
    double boxY = (size().height() - _boxSize.height()) / 2;
    
    Gfx::RectF boxRect(Gfx::PointF(boxX, boxY), _boxSize);

    renderer->renderBox(*this, *options, painter, rect, 
                         boxRect, _brush, _pen);

    painter.setFont(_font);
    Gfx::FontMetrics tm = painter.fontMetrics( text() );

    double boxSize = _boxSize.width();

    double textX = padding().leftRight() + _boxSize.width() + padding().left();
    double textY = (size().height() / 2) - (tm.height() / 2) + tm.ascent();
    Gfx::PointF textPos(textX, textY);

    Gfx::RectF mnemonicRect;

    const Char* m = mnemonic();
    if(m)
    {
        String::size_type n = text().find(*m);
        if(n != String::npos)
        {
            Pt::String mnemonicText(text(), 0, n);
            Gfx::FontMetrics fmLeft = painter.fontMetrics(mnemonicText);

            mnemonicText = *m;
            Gfx::FontMetrics fmChar = painter.fontMetrics(mnemonicText);

            mnemonicRect.set( Gfx::PointF(textPos.x() + fmLeft.width(), 
                                          textPos.y() - fmChar.ascent()),
                              Gfx::SizeF(fmChar.width(), 
                                         fmChar.height()) );
        }
    }

    renderer->renderText(*this, *options, painter, rect,
                         text(), textPos, tm, _font, _textPen,
                         mnemonicRect);
}

} // namespace

} // namespace
