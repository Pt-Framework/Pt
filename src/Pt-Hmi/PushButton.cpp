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
    invalidate();
}


void PushButton::setImage(const Gfx::Image& image)
{
    _image = image;
    _picture.set(image);

    const StyleOptions& options = styleOptions();

    //const ButtonRenderer* renderer = style().get<ButtonRenderer>();
    const ButtonRenderer* renderer = _renderer.get();
    if( ! renderer )
        return;

    renderer->prepareIcon(*this, options, image, _picture);
    
    invalidate();
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


const Gfx::Brush& PushButton::foreground() const
{
    return _foreground ? *_foreground
                       : styleOptions().foreground();
}


void PushButton::setForeground(const Gfx::Brush& b)
{
    _foreground.reset( new Gfx::Brush(b) );
    invalidate();
}


const Gfx::Pen& PushButton::contour() const
{
    return _contour.isValid() ? _contour.get()
                              : styleOptions().contour();
}


void PushButton::setContour(const Gfx::Pen& p)
{
    _contour.set(p);
    invalidate();
}


const Gfx::Color& PushButton::textColor() const
{
    return _textColor.isValid() ? _textColor.get()
                                : styleOptions().textColor();
}


void PushButton::setTextColor(const Gfx::Color& color)
{
    _textColor.set(color);
    invalidate();
}


const std::string& PushButton::font() const
{
    return _fontName.isValid() ? _fontName.get()
                               : styleOptions().font().name();
}


void PushButton::setFont(const std::string& fontName)
{
    _fontName.set(fontName);
    invalidate();
}


std::size_t PushButton::fontSize() const
{

    return _fontSize.isValid() ? _fontSize.get()
                               : styleOptions().font().size();
}


void PushButton::setFontSize(const std::size_t s)
{
    _fontSize.set(s);
    invalidate();
}


Gfx::Font::Style PushButton::fontStyle() const
{
    return _fontStyle.isValid() ? _fontStyle.get()
                                : styleOptions().font().style();
}


void PushButton::setFontStyle(Gfx::Font::Style style)
{
    _fontStyle.set(style);
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


void PushButton::onInvalidate()
{
    Base::onInvalidate();

    const StyleOptions& options = styleOptions();

    _brush = foreground();
    _pen = contour();
    _textPen = textColor();
    _font = Gfx::Font(font(), fontSize(), fontStyle());

    _renderer.reset( style().get<ButtonRenderer>() );

    //const ButtonRenderer* renderer = style().get<ButtonRenderer>();
    //if( ! renderer )
    //    return;

    _renderer->prepare(*this, options, _brush, _pen, _font, _textPen);
}


void PushButton::onPaint(PaintSurface& surface, const Gfx::RectF& rect)
{
    const StyleOptions& options = styleOptions();

    //const ButtonRenderer* renderer = style().get<ButtonRenderer>();
    const ButtonRenderer* renderer = _renderer.get();
    if( ! renderer )
        return;

    Painter painter(surface);
    painter.setClip(rect);

    //
    // button shape
    //

    if( ! _isFlat )
    {
        renderer->renderBackground(*this, options, painter, rect, 
                                   _brush, _pen);
    }

    painter.setFont(_font);
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

    renderer->renderText(*this, options, painter, rect,
                         text(), textPos, _font, _textPen,
                         mnemonicRect);
}


void PushButton::onEnableEvent(const EnableEvent& ev)
{
    Base::onEnableEvent(ev);

    const StyleOptions& options = styleOptions();

    //const ButtonRenderer* renderer = style().get<ButtonRenderer>();
    const ButtonRenderer* renderer = _renderer.get();
    if( ! renderer )
        return;

    renderer->prepareIcon(*this, options, _image, _picture);
}

} // namespace

} // namespace
