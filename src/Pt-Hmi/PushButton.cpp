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

#include <Pt/Hmi/PushButton.h>
#include <Pt/Hmi/Style.h>
#include <Pt/Hmi/StyleOptions.h>
#include <Pt/Hmi/Painter.h>

namespace Pt {

namespace Hmi {

PushButton::PushButton()
: _isToggle(false)
, _isFlat(false)
{
}


PushButton::~PushButton()
{
}


bool PushButton::isToggle() const
{
    return _isToggle;
}


void PushButton::setToggle(bool toggle)
{
    _isToggle = toggle;
}


void PushButton::setImage(const Gfx::Image& image)
{
    _picture.set(image);
    update();
}


const Picture& PushButton::picture() const
{
    return _picture;
}


bool PushButton::isFlat() const
{
    return _isFlat;
}


void PushButton::setFlat(bool f)
{
    _isFlat = f;
    invalidate();
}


void PushButton::onPressed()
{
    Base::onPressed();

    if( isToggle() )
        setPressed( ! isPressed() );
    else
        setPressed(true);
}


void PushButton::onReleased()
{
    Base::onReleased();

    if( ! isToggle() )
        setPressed(false);
    
    clicked().send(*this);
}


void PushButton::onPaint(PaintSurface& surface, const Gfx::RectF& rect)
{
    const StyleOptions* options = getFacet<StyleOptions>();
    if( ! options )
      return;

    const ButtonRenderer* renderer = getFacet<ButtonRenderer>();
    if( ! renderer)
        return;

    Painter painter(surface);
    painter.setClip(rect);

    //
    // button shape
    //

    if( ! _isFlat )
    {
        renderer->renderBackground(*this, *options, painter, rect, 
                                  options->foreground(), options->contourColor());
    }

    Gfx::Color textColor = options->textColor();
    const Gfx::Font& font = options->font();

    painter.setFont(font);
    Gfx::FontMetrics fm = painter.fontMetrics( text() );

    //
    // button icon
    //

    double spacing = _picture.empty() || text().empty() ? 0 : fm.height() * 0.5;
    double itemsWidth = fm.width() + spacing + _picture.width();
    double itemX = (size().width() - itemsWidth) / 2;

    if( ! _picture.empty() )
    {
        double pictureY = ((size().height() - _picture.height()) / 2);
        Gfx::PointF picturePos(itemX, pictureY);
        painter.drawPicture(picturePos, _picture);
    }

    //
    // button text including menomnic
    //

    itemX += _picture.width() + spacing;
    double textY = ((size().height() - fm.height()) / 2) + fm.ascent();
    Gfx::PointF textPos(itemX, textY);

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
                         text(), textPos, font, textColor,
                         mnemonicRect);
}

} // namespace

} // namespace
