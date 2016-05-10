/* Copyright (C) 2013 Marc Boris Duerner 
   Copyright (C) 2013 Laurentiu-Gheorghe Crisan

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
   MA  02110-1301  USA
*/

#include <Pt/Hmi/MenuItem.h>
#include <Pt/Hmi/Painter.h>

namespace Pt {

namespace Hmi {

MenuItem::MenuItem()
: _text("empty")
{
    setName("item");
}


MenuItem::~MenuItem()
{
}


void MenuItem::onPaint(PaintSurface& surface, const Gfx::RectF& updateRect)
{
    //Panel::onPaint(surface, updateRect);

    Gfx::Color foreColor = foregroundColor();
    Gfx::Pen   pen(1, foreColor);

    Painter painter(surface);
    painter.setFont(_font);
    painter.setPen(pen);

    Gfx::FontMetrics fm = Painter::fontMetrics( _font, _text);

    double textX = _icon.width() + 8;
    double textY = (size().height() - fm.height()) / 2;
    textY += fm.ascent();

    Gfx::PointF textPos(textX, textY);
    painter.drawText(textPos, _text);
}


void MenuItem::onResizeEvent(const ResizeEvent& ev)
{
    Panel::onResizeEvent(ev);
}


void MenuItem::setText(const Pt::String& t)
{   
    _text = t;
    _contentChanged.send(*this);
}
 

void MenuItem::setIcon(const Gfx::Image& img)
{   
    _icon = img;
    _contentChanged.send(*this);
}


void MenuItem::setFont(const Gfx::Font& font)
{   
    _font = font;
    _contentChanged.send(*this);
}


void MenuItem::setShortcut(const Key& k)
{
    _shortcut = k;
    _contentChanged.send(*this);
}


const Gfx::SizeF MenuItem::defaultSize() const
{
    Gfx::FontMetrics fm = Painter::fontMetrics( _font, _text);
    
    return Gfx::SizeF( fm.width() + padding().leftRight() + _icon.width(),
                       fm.height() + padding().topBottom() + _icon.height() );
}

} // namespace

} // namespace
