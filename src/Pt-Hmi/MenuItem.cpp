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

    Pt::uint16_t r16 = r > 65535 ? 65535 : static_cast<Pt::uint16_t>(r);
    Pt::uint16_t g16 = g > 65535 ? 65535 : static_cast<Pt::uint16_t>(g);
    Pt::uint16_t b16 = b > 65535 ? 65535 : static_cast<Pt::uint16_t>(b);

    return Pt::Gfx::Color(c.alpha(), r16, g16, b16);
}


Pt::String shortcutText(const Pt::Hmi::Key& key)
{
    Pt::String text;

    bool hasCtrl = key.modifiers().has(Pt::Hmi::Key::Control);
    if(hasCtrl)
    {
        if( ! text.empty() )
            text += Pt::Char('+');

        text += Pt::Hmi::Key::toString(Pt::Hmi::Key::ControlKey);
    }
        
    bool hasAlt = key.modifiers().has(Pt::Hmi::Key::Alt);
    if(hasAlt)
    {
        if( ! text.empty() )
            text += Pt::Char('+');
            
        text += Pt::Hmi::Key::toString(Pt::Hmi::Key::AltKey);
    }

    bool hasMeta = key.modifiers().has(Pt::Hmi::Key::Meta);
    if(hasMeta)
    {
        if( ! text.empty() )
            text += Pt::Char('+');
            
        text += Pt::Hmi::Key::toString(Pt::Hmi::Key::MetaKey);
    }

    bool hasShift = key.modifiers().has(Pt::Hmi::Key::Shift);
    if(hasShift)
    {
        if( ! text.empty() )
            text += Pt::Char('+');
            
        text += Pt::Hmi::Key::toString(Pt::Hmi::Key::ShiftKey);
    }

    if( ! text.empty() )
        text += Pt::Char('+');

    text += Pt::Hmi::Key::toString( key.code() );

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
    setFocusPolicy(Widget::NormalFocus);
    
    setPadding( Spacing(4, 2, 4, 2) );
    setMargin(0);
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
    invalidate();
}


const Gfx::Image& MenuItem::icon() const
{
    return _icon;
}


void MenuItem::setIcon(const Gfx::Image& img)
{   
    _icon = img;
    invalidate();
}


double MenuItem::iconPadding() const
{
    return _iconWidth;
}


void MenuItem::setIconPadding(double w)
{
    _iconWidth = std::max<double>(w, _icon.width());
}


Signal<MenuItem&>& MenuItem::triggered()
{ 
    return _triggered; 
}


const Gfx::Brush& MenuItem::background() const
{
    return _background.isValid() ? _background.get()
                                 : Application::instance().styleOptions().background();
}


void MenuItem::setBackground(const Gfx::Brush& b)
{
    _background.set(b);
    invalidate();
}


const Gfx::Color& MenuItem::textColor() const
{
    return _textColor.isValid() ? _textColor.get()
                                : Application::instance().styleOptions().textColor();
}


void MenuItem::setTextColor(const Gfx::Color& color)
{
    _textColor.set(color);
    invalidate();
}


const std::string& MenuItem::font() const
{
    return _fontName.isValid() ? _fontName.get()
                               : Application::instance().styleOptions().font().name();
}


void MenuItem::setFont(const std::string& fontName)
{
    _fontName.set(fontName);
    invalidate();
}


std::size_t MenuItem::fontSize() const
{

    return _fontSize.isValid() ? _fontSize.get()
                               : Application::instance().styleOptions().font().size();
}


void MenuItem::setFontSize(const std::size_t s)
{
    _fontSize.set(s);
    invalidate();
}


Gfx::Font::Style MenuItem::fontStyle() const
{
    return _fontStyle.isValid() ? _fontStyle.get()
                                : Application::instance().styleOptions().font().style();
}


void MenuItem::setFontStyle(Gfx::Font::Style style)
{
    _fontStyle.set(style);
    invalidate();
}

void MenuItem::onTriggered()
{   
    _triggered.send(*this);
}


void MenuItem::onParentChanged(Widget* w)
{
    if( ! w && _menu)
        _menu->removeItem(*this);
}


void MenuItem::onShortcut(const KeyEvent& kev)
{
    Base::onShortcut(kev);
    
    onTriggered();
}


Gfx::SizeF MenuItem::onAutoSize() const
{
    Gfx::FontMetrics fm = Painter::fontMetrics(_font, _text);

    double contentHeight = std::max<Pt::ssize_t>( fm.height(), _icon.height() );
    double contentWidth = fm.width() + _picture.width();

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


void MenuItem::onInvalidate()
{
    Base::onInvalidate();

    const StyleOptions& options = Application::instance().styleOptions();

    _brush = background();
    _pen = contour();
    _textPen = textColor();
    _font = Gfx::Font(font(), fontSize(), fontStyle());

    const MenuRenderer* renderer = getFacet<MenuRenderer>();
    if( ! renderer )
        return;

    renderer->prepareItem(*this, options, _icon, 
                          _picture, _brush, _pen, _font, _textPen);
}


void MenuItem::onPaint(PaintSurface& surface, const Gfx::RectF& rect)
{
    const StyleOptions& options = Application::instance().styleOptions();

    const MenuRenderer* renderer = getFacet<MenuRenderer>();
    if( ! renderer )
        return;
    
    Painter painter(surface);
    painter.setClip(rect);

    renderer->renderItem(*this, options, painter, rect, _brush, _pen);
    
    onPaintIcon(surface, rect);
    onPaintText(surface, rect);
    onPaintShortcut(surface, rect);
}


void MenuItem::onPaintIcon(PaintSurface& surface, const Gfx::RectF& updateRect)
{
    Painter painter(surface);
    painter.setClip(updateRect);

    double iconX = (iconPadding() - icon().width()) / 2;
    double iconY = (size().height() - icon().height()) / 2;

    Gfx::PointF iconPos(iconX, iconY);
    painter.drawImage(iconPos, icon());
}


void MenuItem::onPaintText(PaintSurface& surface, const Gfx::RectF& updateRect)
{
    Painter painter(surface);
    painter.setClip(updateRect);
    painter.setFont(_font);
    painter.setPen(_textPen);

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
    painter.setClip(updateRect);
    painter.setFont(_font);
    painter.setPen(_textPen);

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


void MenuItem::onMouseEvent(const MouseEvent& ev)
{    
    Base::onMouseEvent(ev);

    bool inside = Gfx::RectF( size() ).contains( ev.position() );

    if( inside && ev.isRelease() )
    {
        onTriggered();
    }
}


void MenuItem::onTouchEvent(const TouchEvent& ev)
{    
    Base::onTouchEvent(ev);

    bool inside = Gfx::RectF( size() ).contains( ev.position() );
   
    if( inside && ev.isRelease() )
    {
        onTriggered();
    }
}


void MenuItem::onEnterEvent(const EnterEvent& ev)
{
    Base::onEnterEvent(ev);
    update();
}


void MenuItem::onLeaveEvent(const LeaveEvent& ev)
{
    Base::onLeaveEvent(ev);
    update();
}


void MenuItem::onResizeEvent(const ResizeEvent& ev)
{
    Base::onResizeEvent(ev);
}

} // namespace

} // namespace
