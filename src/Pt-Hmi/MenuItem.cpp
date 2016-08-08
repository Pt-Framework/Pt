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
   MA  02110-1301  USA
*/

#include <Pt/Hmi/MenuItem.h>
#include <Pt/Hmi/Menu.h>
#include <Pt/Hmi/Application.h>
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
: _menu(0)
, _iconWidth(0)
, _text("(empty)")
{
    setAutoSize(true);
    setAcceptsFocus(true);
    
    setPadding( Spacing(4, 2, 4, 2) );
    setMargin(0);

    setBorderStyle(Panel::NoBorder);
}


MenuItem::~MenuItem()
{
    if(_menu)
        _menu->removeItem(*this);
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


double MenuItem::iconPadding() const
{
    return _iconWidth;
}


void MenuItem::setIconPadding(double w)
{
    _iconWidth = std::max<double>(w, _icon.width());
}


const Gfx::Font& MenuItem::font() const
{
    return _font;
}


void MenuItem::setFont(const Gfx::Font& font)
{   
    _font = font;
}


Signal<MenuItem&>& MenuItem::triggered()
{ 
    return _triggered; 
}


void MenuItem::onParentChanged(Widget* w)
{
    if( ! w && _menu)
        _menu->removeItem(*this);
}


void MenuItem::onClicked(const Gfx::PointF& pos)
{
    WidgetBaseType::onClicked(pos);
    
    _triggered.send(*this);
}


void MenuItem::onShortcut(const KeyEvent& kev)
{
    WidgetBaseType::onShortcut(kev);
    
    _triggered.send(*this);
}


Gfx::SizeF MenuItem::onAutoSize() const
{
    Gfx::FontMetrics fm = Painter::fontMetrics(_font, _text);

    double contentHeight = std::max( fm.height(), _icon.height() );
    double contentWidth = fm.width() + _icon.width();

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
    //WidgetBaseType::onPaint(surface, updateRect);

    onPaintBackground(surface, updateRect);
    onPaintIcon(surface, updateRect);
    onPaintItem(surface, updateRect);
    onPaintShortcut(surface, updateRect);
}


void MenuItem::onPaintBackground(PaintSurface& surface, const Gfx::RectF& updateRect)
{
    bool mouseOver = Application::instance().pointerWidget() == this;

    if(mouseOver)
    {
        Gfx::Color bgColor = backgroundColor();
        Gfx::Brush brush = brighten(bgColor, 0.85f);

        Painter painter(surface);
        painter.setBrush(brush);
        painter.fillRect( Gfx::RectF(Gfx::PointF(0,0), size()) );
    }
}


void MenuItem::onPaintIcon(PaintSurface& surface, const Gfx::RectF& updateRect)
{
    Painter painter(surface);

    double iconX = (iconPadding() - icon().width()) / 2;
    double iconY = (size().height() - icon().height()) / 2;

    Gfx::PointF iconPos(iconX, iconY);
    painter.drawImage(iconPos, icon(), Gfx::RenderFlags::AlphaBlend);
}


void MenuItem::onPaintItem(PaintSurface& surface, const Gfx::RectF& updateRect)
{
    Painter painter(surface);
    painter.setFont(_font);

    Gfx::Color fgColor = foregroundColor();
    Gfx::Pen pen(1, fgColor);
    painter.setPen(pen);

    Gfx::FontMetrics fm = Painter::fontMetrics(_font, _text);
    double textX = padding().left() + _iconWidth;
    double textY = (size().height() - fm.height()) / 2;
    textY += fm.ascent();
    Gfx::PointF textPos(textX, textY);

    painter.drawText(textPos, _text);
}


void MenuItem::onPaintShortcut(PaintSurface& surface, const Gfx::RectF& updateRect)
{
    Painter painter(surface);
    painter.setFont(_font);

    Gfx::Color fgColor = foregroundColor();
    Gfx::Pen pen(1, fgColor);
    painter.setPen(pen);

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
    WidgetBaseType::onEnterEvent(ev);
    update();
}


void MenuItem::onLeaveEvent(const LeaveEvent& ev)
{
    WidgetBaseType::onLeaveEvent(ev);
    update();
}


void MenuItem::onResizeEvent(const ResizeEvent& ev)
{
    WidgetBaseType::onResizeEvent(ev);
}

} // namespace

} // namespace
