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
#include <Pt/Hmi/Painter.h>
#include <Pt/Hmi/Window.h>
#include <Pt/Hmi/Application.h>
#include <Pt/Hmi/StyleOptions.h>

namespace {

Pt::Gfx::Color brighten(const Pt::Gfx::Color& c, float factor)
{
    Pt::uint16_t r = c.red() * factor;
    Pt::uint16_t g = c.green() * factor;
    Pt::uint16_t b = c.blue() * factor;


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
{
    setAutoSize(true);
    setAcceptsFocus(true);
    setText(text);

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
    invalidate();
}


void MenuBarItem::setHighlighted(bool s)
{
    _highlighted = s;
    invalidate();
}


void MenuBarItem::toggle()
{
    if( ! _menu.isVisible() )
    {
        open();
        grabPointer();
    }
    else
    {
        close();
        releasePointer();
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


// TODO: obsolete
void MenuBarItem::onClicked()
{
    toggle();
}


Gfx::SizeF MenuBarItem::onAutoSize() const
{
    const Gfx::Font& font = Application::instance().font();

    Gfx::FontMetrics fm = Painter::fontMetrics(font, _text);

    return Gfx::SizeF( fm.width() + padding().leftRight(), 
                       fm.height() + padding().topBottom() );
}


void MenuBarItem::onMouseEvent(const MouseEvent& ev)
{ 
    Base::onMouseEvent(ev);

    bool inside = Gfx::RectF( size() ).contains( ev.position() );

    if( inside && ev.isRelease() )
    {
        onClicked();
    }

    // nothing to highlight if outside
    if( inside )
        return;

    // navigate to sibling item if a sub menu is open
    if( _menuBar.selectedMenu() )
    {
        Gfx::PointF pos = toParent( ev.position() );
        MenuBarItem* item = _menuBar.findItem(pos);
        if(item)
        {
            toggle();
            setHighlighted(false);
            
            item->toggle();
            return;
        }
    }

    // navigate to open sub menu
    Gfx::PointF screenPos = toScreen( ev.position() );
    MenuShell* menu = _menu.findMenu(screenPos);   
    if(menu)
    {   
        releasePointer();
        return;
    }

    // cancel when clicked outside any menu item
    if( ev.isPress() )
    {
        releasePointer();
        _menu.cancel();
        return;   
    }
}


void MenuBarItem::onPaintBackground(PaintSurface& surface, const Gfx::RectF& updateRect)
{
    if(_highlighted)
    {
        Gfx::Color bgColor = Application::instance().styleOptions().getProperty<Gfx::Color>("highlightColor");
        Gfx::Brush brush = brighten(bgColor, 0.85f);

        Painter painter(surface);
        painter.setClip(updateRect);
        painter.setBrush(brush);
        painter.fillRect( Gfx::RectF(Gfx::PointF(0,0), size()) );
    }
}


void MenuBarItem::onPaintContent(PaintSurface& surface, const Gfx::RectF& updateRect)
{
    const Gfx::Font& font = Application::instance().font();

    Painter painter(surface);
    painter.setClip(updateRect);
    painter.setFont(font);
    
    painter.setPen(Application::instance().styleOptions().getProperty<Pt::Gfx::Color>("textColor"));

    Gfx::FontMetrics fm = Painter::fontMetrics(font, _text);
    double textX = padding().left();
    double textY = (size().height() - fm.height()) / 2;
    textY += fm.ascent();
    Gfx::PointF textPos(textX, textY);

    painter.drawText(textPos, _text);
}


void MenuBarItem::onEnterEvent(const EnterEvent& ev)
{
    Base::onEnterEvent(ev);
    
    _highlighted = true;
    update();
}


void MenuBarItem::onLeaveEvent(const LeaveEvent& ev)
{
    Base::onLeaveEvent(ev);
    
    _highlighted = false;
    update();
}

///////////////////////////////////////////////////////////////////////////////
// MenuBar
///////////////////////////////////////////////////////////////////////////////

MenuBar::MenuBar()
: _layout(FlowLayout::Left)
, _currentMenu(0)
, _currentMenuItem(0)
{
    //this->setBackground( Gfx::Color(58981, 58981, 58981) );
    //this->setBorderColor( Gfx::Color(32767, 32767, 32767)  );
    //this->setBorderStyle(Panel::NoBorder);

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


Menu* MenuBar::selectedMenu()
{ 
    return _currentMenu; 
}


MenuBarItem* MenuBar::findItem(const Gfx::PointF& pos)
{
    MenuBarItem* item = 0;

    std::vector<MenuBarItem*>::iterator it;
    for(it = _menus.begin(); it != _menus.end(); ++it)
    {
        if( (*it)->geometry().contains(pos) )
        {
            item = *it;
            break;
        }
    }

    return item;
}


void MenuBar::onAddMenu(Menu& menu, const Pt::String& text)
{
    MenuBarItem* item = new MenuBarItem(*this, menu, text);
    item->resize( Gfx::SizeF(50, 0) );

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
    {
        _currentMenu = 0;
        _currentMenuItem = 0;
    }

    update();
}


void MenuBar::onOpenMenu(Menu& menu)
{
    std::vector<MenuBarItem*>::iterator it;
    for(it = _menus.begin(); it != _menus.end(); ++it)
    {
        if( &(*it)->menu() == &menu)
        {
            (*it)->setHighlighted(true);

            _currentMenu = &menu;
            _currentMenuItem = *it;
        }
    }
}


void MenuBar::onCloseMenu(Menu& menu)
{
    if(_currentMenu == &menu)
    {
        if(_currentMenuItem)
            _currentMenuItem->setHighlighted(false);

        _currentMenu = 0;
        _currentMenuItem = 0;
    }
}


void MenuBar::onCancel()
{
    if(_currentMenu)
        _currentMenu->cancel();
}


void MenuBar::onEnter()
{
    if(_currentMenuItem)
        _currentMenuItem->grabPointer();
}


MenuShell* MenuBar::onFindMenu(const Gfx::PointF& screenPos)
{ 
    if( ! isVisible() )
        return 0;

    Gfx::PointF pos = this->window()->fromScreen(screenPos);
    pos = this->fromWindow(pos);

    Gfx::RectF rect( Gfx::PointF(0,0), size() );
    if( rect.contains( pos ) )
        return this;

    if( ! _currentMenu)
        return 0;

    return _currentMenu->findMenu(screenPos);
}


void MenuBar::onPaintBackground(PaintSurface& surface, const Gfx::RectF& updateRect)
{
    Painter painter(surface);
    painter.setCompositionMode(Gfx::CompositionMode::SourceCopy);

    painter.setBrush( Application::instance().styleOptions().getProperty<Gfx::Brush>("background") );
    //painter.setClip(updateRect);
    painter.fillRect(updateRect);
}


void MenuBar::onPaintContent(PaintSurface& surface, const Gfx::RectF& updateRect)
{

}


void MenuBar::onMouseEvent(const MouseEvent& ev)
{ 
    Base::onMouseEvent(ev);
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
    Base::onResizeEvent(ev);
}

} // namespace

} // namespace
