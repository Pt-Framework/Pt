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
#include <Pt/Hmi/Menu.h>
#include <Pt/Hmi/Window.h>
#include <Pt/Hmi/Painter.h>

namespace {

Pt::Gfx::Color brighten(const Pt::Gfx::Color& c, float factor)
{
    float r = c.red() * factor;
    float g = c.green() * factor;
    float b = c.blue() * factor;

    r = r > 1.0f ? 1.0f : r;
    g = g > 1.0f ? 1.0f : g;
    b = b > 1.0f ? 1.0f : b;

    return Pt::Gfx::Color(c.alpha(), r, g, b);
}


Pt::String shortcutText(const Pt::Hmi::Key& key)
{
    Pt::String text;

    bool hasCtrl = key.hasModifiers(Pt::Hmi::Key::Control);
    if(hasCtrl)
    {
        if( ! text.empty() )
            text += Pt::Char('+');

        text += Pt::Hmi::Key::toString(Pt::Hmi::Key::Control);
    }
        
    bool hasAlt = key.hasModifiers(Pt::Hmi::Key::Alt);
    if(hasAlt)
    {
        if( ! text.empty() )
            text += Pt::Char('+');
            
        text += Pt::Hmi::Key::toString(Pt::Hmi::Key::Alt);
    }

    bool hasMeta = key.hasModifiers(Pt::Hmi::Key::Meta);
    if(hasMeta)
    {
        if( ! text.empty() )
            text += Pt::Char('+');
            
        text += Pt::Hmi::Key::toString(Pt::Hmi::Key::Meta);
    }

    bool hasShift = key.hasModifiers(Pt::Hmi::Key::Shift);
    if(hasShift)
    {
        if( ! text.empty() )
            text += Pt::Char('+');
            
        text += Pt::Hmi::Key::toString(Pt::Hmi::Key::Shift);
    }

    if( ! text.empty() )
        text += Pt::Char('+');

    text += Pt::Hmi::Key::toString(key.keyCode());

    return text;
}

} // namespace

namespace Pt {

namespace Hmi {

MenuItem::MenuItem()
: _text("(empty)")
{
    setAutoSize(true);
    setAcceptsFocus(true);
    
    setPadding( Spacing(4, 2, 4, 2) );
    setMargin(0);

    setBorderStyle(Panel::NoBorder);
}


MenuItem::~MenuItem()
{
}


const String& MenuItem::text() const 
{
    return _text;
}


void MenuItem::setText(const Pt::String& t)
{   
    _text = t;
}


const Gfx::Image& MenuItem::icon() const
{
    return _icon;
}


void MenuItem::setIcon(const Gfx::Image& img)
{   
    _icon = img;
}


const Gfx::Font& MenuItem::font() const
{
    return _font;
}


void MenuItem::setFont(const Gfx::Font& font)
{   
    _font = font;
}


Signal<>& MenuItem::triggered()
{ 
    return _triggered; 
}


void MenuItem::onClicked(const Gfx::PointF& pos)
{
    BaseType::onClicked(pos);
    _triggered.send();
}


void MenuItem::onShortcut(const KeyEvent& kev)
{
    BaseType::onShortcut(kev);
    _triggered.send();
}


Gfx::SizeF MenuItem::onAutoSize() const
{
    Gfx::FontMetrics fm = Painter::fontMetrics(_font, _text);
    
    double contentHeight = std::max( fm.height(), _icon.height() );
    double contentWidth = fm.width();
    
    const Key* sk = shortcut();
    if(sk)
    {
        Pt::String text = shortcutText(*sk);
        contentWidth += fm.height() * 2.5; // spacing towards shortcut text
        contentWidth += Painter::fontMetrics(_font, text).width();
    }

    return Gfx::SizeF( contentWidth + padding().leftRight(),
                       contentHeight + padding().topBottom() );
}


void MenuItem::onPaint(PaintSurface& surface, const Gfx::RectF& updateRect)
{
    //BaseType::onPaint(surface, updateRect);

    bool mouseOver = this->window()->pointerWidget() == this;

    Gfx::Color fgColor = foregroundColor();
    Gfx::Color bgColor = backgroundColor();
    
    if(mouseOver)
        bgColor = brighten(bgColor, 0.85f);

    Painter painter(surface);
    painter.setFont(_font);

    Gfx::Pen pen(1, fgColor);
    painter.setPen(pen);

    Gfx::Brush brush(bgColor);
    painter.setBrush(brush);

    if(mouseOver)
    {
        painter.fillRect( Gfx::RectF(Gfx::PointF(0,0), size()) );
    }

    //
    // draw item text
    //
    Gfx::FontMetrics fm = Painter::fontMetrics(_font, _text);
    double textX = padding().left();
    double textY = (size().height() - fm.height()) / 2;
    textY += fm.ascent();
    Gfx::PointF textPos(textX, textY);

    painter.drawText(textPos, _text);
    
    //
    // draw shortcut text
    //
    const Key* sk = shortcut();
    if(sk)
    {
        Pt::String skText = shortcutText(*sk);
        Gfx::FontMetrics skm = Painter::fontMetrics(_font, skText);

        double skX = size().width() - skm.width() - padding().right();
        double skY = (size().height() - skm.height()) / 2;
        skY += skm.ascent();
        Gfx::PointF skPos(skX, skY);

        painter.drawText(skPos, skText);
    }
}


void MenuItem::onEnterEvent(const EnterEvent& ev)
{
    BaseType::onEnterEvent(ev);
    update();
}


void MenuItem::onLeaveEvent(const LeaveEvent& ev)
{
    BaseType::onLeaveEvent(ev);
    update();
}


void MenuItem::onResizeEvent(const ResizeEvent& ev)
{
    BaseType::onResizeEvent(ev);
}


SubMenu::SubMenu(Menu* menu)
: _menu(menu)
{
    if(_menu)
        _menu->destroyed() += Pt::slot(*this, &SubMenu::onDestroyedMenu);
}
    

SubMenu::~SubMenu()
{
}


void SubMenu::setMenu(Menu* menu)
{
    if(_menu)
        _menu->destroyed() -= Pt::slot(*this, &SubMenu::onDestroyedMenu);

    _menu = menu;
}


void SubMenu::onDestroyedMenu(Window&)
{
    _menu = 0;
}


void SubMenu::onClicked(const Gfx::PointF& pos)
{
    if(_menu && window() )
    {
        Gfx::PointF topRight(size().width(), 0);
        Gfx::PointF wpos = this->toWindow(topRight);
        Gfx::PointF menuPos = window()->toScreen(wpos);

        _menu->move(menuPos);
        _menu->show();
    }
}


static const double indicatorWidth = 10.0; 


Gfx::SizeF SubMenu::onAutoSize() const
{
    Gfx::SizeF size = BaseType::onAutoSize();
    
    // space for the menu indicator
    size.addWidth(indicatorWidth); 
    
    return size;
}


void SubMenu::onPaint(PaintSurface& surface, const Gfx::RectF& updateRect)
{
    BaseType::onPaint(surface, updateRect);

    Gfx::Color fgColor = this->foregroundColor();

    Painter painter(surface);

    //
    // draw menu indicator
    //

    double x = size().width() - indicatorWidth - padding().right();
    double y = size().height() / 2;

    Gfx::PointF indicator[3] = { Gfx::PointF(x - 3, y - 4),
                                 Gfx::PointF(x + 1, y),
                                 Gfx::PointF(x - 3, y + 4) };

    Gfx::Brush brush(fgColor);
    painter.setBrush(brush);
    painter.fillPolygon(indicator, 3);
}

} // namespace

} // namespace
