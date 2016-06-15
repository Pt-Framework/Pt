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

#include <Pt/Hmi/MenuBar.h>
#include <Pt/Hmi/Menu.h>
#include <Pt/Hmi/Window.h>

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

} // namepace

namespace Pt {

namespace Hmi {

///////////////////////////////////////////////////////////////////////////////
// MenuBarItem
///////////////////////////////////////////////////////////////////////////////

MenuBarItem::MenuBarItem(MenuBar& mb, Menu& menu, const Pt::String& text)
: _menuBar(mb)
, _menu(menu)
, _highlighted(false)
, _selected(false)
{
    setAutoSize(true);
    setBorderStyle(Panel::NoBorder);
    setAcceptsFocus(true);
    
    setBackgroundColor( Gfx::Color(0.9f, 0.9f, 0.91f) );

    setText(text);
    setBorderRound(false);
    
    setPadding( Spacing(8, 0, 8, 0) );
    setMargin(0);
}


MenuBarItem::~MenuBarItem()
{
}


const String& MenuBarItem::text() const 
{
    return _text;
}


void MenuBarItem::setText(const Pt::String& t)
{   
    _text = t;
}


const Gfx::Font& MenuBarItem::font() const
{
    return _font;
}


void MenuBarItem::setFont(const Gfx::Font& font)
{   
    _font = font;
}


Signal<MenuBarItem&>& MenuBarItem::triggered()
{
    return _triggered;
}


void MenuBarItem::setSelected(bool s)
{
    _selected = s;
    update();      
}


void MenuBarItem::toggle()
{
    if( ! _menu.isVisible() )
    {
        open();
        grabMouse();
    }
    else
    {
        close();
        releaseMouse();
     }      
}


void MenuBarItem::open()
{
    Gfx::PointF menuPos(0, size().height());
    menuPos = toWindow(menuPos);

    if( window() )
        menuPos = window()->toScreen(menuPos);

    _menu.show(menuPos);
}


void MenuBarItem::close()
{
    _menu.close();       
}


Gfx::SizeF MenuBarItem::onAutoSize() const
{
    Gfx::FontMetrics fm = Painter::fontMetrics(_font, _text);

    return Gfx::SizeF( fm.width() + padding().leftRight(), 
                       fm.height() + padding().topBottom() );
}


void MenuBarItem::onClicked(const Gfx::PointF& pos)
{
    WidgetBaseType::onClicked(pos);
    
    _triggered.send(*this);
}


void MenuBarItem::onPointerEvent(const MouseEvent& ev)
{ 
    WidgetBaseType::onPointerEvent(ev);

    // nothing to do if on this item
    Gfx::RectF rect( Gfx::PointF(0,0), size() );
    if( rect.contains( ev.position() ) )
        return;

    // navigate to sibling item
    Gfx::PointF pos = toParent( ev.position() );

    if(_menuBar._currentMenu == &_menu)
    {
        std::vector<MenuBarItem*>::iterator it;
        for(it = _menuBar._menus.begin(); it != _menuBar._menus.end(); ++it)
        {
            MenuBarItem* item = *it;
            if( item->geometry().contains(pos) )
            {
                toggle();
                _highlighted = false;
                setSelected(false);
                item->toggle();
            }
        }
    }

    // navigate to sub menu
    Gfx::PointF screenPos = toScreen( ev.position() );
    
    MenuShell* menu = _menu.findMenu(screenPos);   
    if(menu)
    {   
        releaseMouse();
    }
    
    // TODO:
    //else if( ev.isPress() )
    //{
    //    // cancel when clicked outside menu chain
    //    _menu.cancel();          
    //}
}


void MenuBarItem::onPaint(PaintSurface& surface, const Gfx::RectF& updateRect)
{
    //WidgetBaseType::onPaint(surface, updateRect);

    onPaintBackground(surface, updateRect);
    onPaintContent(surface, updateRect);
}


void MenuBarItem::onPaintBackground(PaintSurface& surface, const Gfx::RectF& updateRect)
{
    //bool mouseOver = this->window()->pointerWidget() == this;

    if(/*mouseOver ||*/ _highlighted || _selected)
    {
        Gfx::Color bgColor = backgroundColor();
        Gfx::Brush brush = brighten(bgColor, 0.85f);

        Painter painter(surface);
        painter.setBrush(brush);
        painter.fillRect( Gfx::RectF(Gfx::PointF(0,0), size()) );
    }
}


void MenuBarItem::onPaintContent(PaintSurface& surface, const Gfx::RectF& updateRect)
{
    Painter painter(surface);
    painter.setFont(_font);

    Gfx::Color fgColor = foregroundColor();
    Gfx::Pen pen(1, fgColor);
    painter.setPen(pen);

    Gfx::FontMetrics fm = Painter::fontMetrics(_font, _text);
    double textX = padding().left();
    double textY = (size().height() - fm.height()) / 2;
    textY += fm.ascent();
    Gfx::PointF textPos(textX, textY);

    painter.drawText(textPos, _text);
}


void MenuBarItem::onEnterEvent(const EnterEvent& ev)
{
    //std::clog << "enter " << text().narrow() << std::endl;

    WidgetBaseType::onEnterEvent(ev);
    _highlighted = true;
    update();
}


void MenuBarItem::onLeaveEvent(const LeaveEvent& ev)
{
    //std::clog << "leave " << text().narrow() << std::endl;

    WidgetBaseType::onLeaveEvent(ev);
    _highlighted = false;
    update();
}

///////////////////////////////////////////////////////////////////////////////
// MenuBar
///////////////////////////////////////////////////////////////////////////////

MenuBar::MenuBar()
: _layout(FlowLayout::Left)
, _currentMenu(0)
{
    this->setBackgroundColor( Gfx::Color(0.9f, 0.9f, 0.91f) );
    this->setBorderColor( Gfx::Color(0.5f, 0.5f, 0.51f)  );
    this->setBorderStyle(Panel::NoBorder);

    _layout.move( Gfx::PointF(0,0) );
    _layout.setPadding(1);

    add(_layout);
}


MenuBar::~MenuBar()
{
    std::vector<MenuBarItem*>::iterator it;
    for(it = _menus.begin(); it != _menus.end(); ++it)
    {
        delete *it;
    }
}


void MenuBar::onAddMenu(Menu& menu, const Pt::String& text)
{
    MenuBarItem* item = new MenuBarItem(*this, menu, text);
    item->resize( Gfx::SizeF(50, 0) );
    item->triggered() += Pt::slot(*this, &MenuBar::onMenuTriggered);

    _menus.push_back(item);
    _layout.add(*item);

    update();
}


void MenuBar::onRemoveMenu(Menu& menu)
{
    std::vector<MenuBarItem*>::iterator it;
    for(it = _menus.begin(); it != _menus.end(); ++it)
    {
        if( &(*it)->menu() == &menu )
        {
            delete *it;
            _menus.erase(it);
            break;
        }
    }

    if(_currentMenu == &menu)
        _currentMenu = 0;

    update();
}


void MenuBar::onMenuTriggered(MenuBarItem& m)
{
    m.toggle();
}


void MenuBar::onOpenMenu(Menu& menu)
{
    _currentMenu = &menu;
    ////grabMouse();

    std::vector<MenuBarItem*>::iterator it;
    for(it = _menus.begin(); it != _menus.end(); ++it)
    {
        if( &(*it)->menu() == &menu)
        {
            (*it)->setSelected(true);
        }
    }
}


void MenuBar::onCloseMenu(Menu& menu)
{
    if(_currentMenu == &menu)
    {
        _currentMenu = 0;
        ////releaseMouse();
    }

    std::vector<MenuBarItem*>::iterator it;
    for(it = _menus.begin(); it != _menus.end(); ++it)
    {
        if( &(*it)->menu() == &menu)
        {
            (*it)->setSelected(false);
        }
    }
}


void MenuBar::onCancel()
{
    if(_currentMenu)
        _currentMenu->cancel();
}


void MenuBar::onEnter()
{
    ////grabMouse();

    std::vector<MenuBarItem*>::iterator it;
    for(it = _menus.begin(); it != _menus.end(); ++it)
    {
        if( &(*it)->menu() == _currentMenu)
        {
            (*it)->grabMouse();
        }
    }
}


MenuShell* MenuBar::onFindMenu(const Gfx::PointF& screenPos)
{ 
    if( ! isVisible() )
        return false;

    Gfx::PointF pos = this->window()->fromScreen(screenPos);
    pos = this->fromWindow(pos);

    Gfx::RectF rect( Gfx::PointF(0,0), size() );
    if( rect.contains( pos ) )
        return this;

    if( ! _currentMenu)
        return 0;

    return _currentMenu->findMenu(screenPos);
}


void MenuBar::onPointerEvent(const MouseEvent& ev)
{ 
    WidgetBaseType::onPointerEvent(ev);

    ////Gfx::PointF screenPos = window()->toScreen( ev.position() );
    ////
    ////// check if a sub menu was entered
    ////MenuShell* menu = findMenu(screenPos);   
    ////if(menu && menu != this)
    ////{
    ////    releaseMouse();
    ////    // menu->onEnter();
    ////    return; 
    ////}

    ////MenuBarItem* item = 0;
    ////std::vector<MenuBarItem*>::iterator it;
    ////for(it = _menus.begin(); it != _menus.end(); ++it)
    ////{
    ////    Gfx::RectF itemRect( (*it)->position(), (*it)->size() );
    ////    if(itemRect.contains( ev.position() ) )
    ////    {
    ////        item = *it;
    ////        break;
    ////    }
    ////}

    ////// clicking outside the menu cancels the menu chain
    ////Gfx::RectF rect( Gfx::PointF(0,0), size() );
    ////bool outside = ! rect.contains( ev.position() );
    ////
    ////if( ! item || outside )
    ////{
    ////    if( ev.isPress() )
    ////    {
    ////        cancel();
    ////    }
    ////    
    ////    return;
    ////}

    ////// clicking an item closes the sub menu
    ////if( _currentMenu == &item->menu() && ev.isRelease() )
    ////{
    ////    _currentMenu->close();
    ////    return;
    ////}

    ////// if a sub menu is open show the next one
    ////if( _currentMenu != &item->menu() && _currentMenu )
    ////{
    ////    _currentMenu->close();
    ////    
    ////    item->open();
    ////    return;
    ////}
}


void MenuBar::onResizeEvent(const ResizeEvent& ev)
{
    for(std::size_t i = 0; i < _layout.widgets().size(); ++i)
    {
        Widget* item = _layout.widgets().at(i);
        Gfx::SizeF itemSize = item->preferredSize();
        item->resize(itemSize);
    }

    _layout.resize( ev.size() );

    // _layout positions the items now in OnResizeEvent
    // TODO: our overall design should make this clearer
    WidgetBaseType::onResizeEvent(ev);
}

} // namespace

} // namespace
