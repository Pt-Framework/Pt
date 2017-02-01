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

#include <Pt/Hmi/ComboBox.h>
#include <Pt/Hmi/Painter.h>

namespace Pt {

namespace Hmi {

ComboBox::ComboBox()
: _menu(0, Window::Popup)
{
    setTextInput(true);
    setFocusPolicy(Widget::NormalFocus);
    _text = "Hallo";

    _item1.resize( Gfx::SizeF(20, 20) );
    _item2.resize( Gfx::SizeF(20, 20) );

    _items.addItem(_item1);
    _items.addItem(_item2);
    
    _menu.setMainWidget(&_items);
    _menu.eventReady() += Pt::slot(*this, &ComboBox::onMenuKeyEvent);
}


ComboBox::~ComboBox()
{
}


void ComboBox::onInvalidate()
{
    Base::onInvalidate();
}


void ComboBox::onPaint(PaintSurface& surface, const Gfx::RectF& rect)
{
    const StyleOptions& options = Application::instance().styleOptions();

    Painter painter(surface);
    painter.setClip(rect);
    
    Gfx::Pen pen     = Gfx::Color::fromRgb8(0, 0, 0);
    Gfx::Brush brush = Gfx::Color::fromRgb8(255, 255, 255);
    Gfx::Font font   = options.font();
    
    painter.setPen(pen);
    painter.setBrush(brush);
    painter.setFont(font);

    painter.fillRect(rect);
    painter.drawRect(rect);

    Gfx::FontMetrics fm = painter.fontMetrics(font, _text);

    Gfx::PointF textPos( 5, fm.ascent() + 5);
    painter.drawText(textPos, _text);
}


void ComboBox::onMenuKeyEvent(const KeyEvent& ev)
{
    if( ! ev.isPress() )
        return;

    _text += ev.unicode();
    invalidate();
}


void ComboBox::onMouseEvent(const MouseEvent& ev)
{    
    Base::onMouseEvent(ev);

    if( ev.isPress() )
    {
        Gfx::SizeF s = size();
        _menu.resize( Gfx::SizeF(size().width(), 200) );
        _menu.setTopMost(true);

        Gfx::PointF pos(0, size().height());
        pos = this->toScreen(pos);
        _menu.move(pos);

        _menu.show();
    }
}

} // namespace

} // namespace


