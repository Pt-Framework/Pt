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

namespace Pt {

namespace Hmi {

///////////////////////////////////////////////////////////////////////////////
// MenuBarItem
///////////////////////////////////////////////////////////////////////////////

MenuBarItem::MenuBarItem(Menu& menu, const Pt::String& text)
: _menu(menu)
{
    setAutoSize(true);
    setBorderStyle(Panel::NoBorder);
    setAcceptsFocus(true);
    setText(text);

    setPadding( Spacing(8, 0, 8, 0) );
    setMargin(0);
}


MenuBarItem::~MenuBarItem()
{
}


void MenuBarItem::onClicked(const Gfx::PointF&)
{
    if( ! _menu.isVisible() )
    {
        Gfx::PointF menuPos(0, size().height());
        menuPos = toWindow(menuPos);

        if( window() )
            menuPos = window()->toScreen(menuPos);

        _menu.show(menuPos);
    }
    else
    {
       _menu.close();       
    }
}


void MenuBarItem::onPaint(PaintSurface& surface, const Gfx::RectF& updateRect)
{
    Button::onPaint(surface, updateRect);
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
    this->setBorderStyle(Panel::Single);

    _layout.move( Gfx::PointF(0,0) );
    _layout.setPadding(1);

    add(_layout);
}


MenuBar::~MenuBar()
{
}


void MenuBar::onAddMenu(Menu& menu, const Pt::String& text)
{
    MenuBarItem* item = new MenuBarItem(menu, text);
    item->resize( Gfx::SizeF(50, 0) );

    _menus.push_back(item);
    _layout.add(*item);

    //
    //menu.impl()->_parentMenu = &parent;

    //onContentChanged();
}


void MenuBar::onRemoveMenu(Menu& menu)
{
    // TODO: delete menu bar item

    if(_currentMenu == &menu)
        _currentMenu = 0;
}


void MenuBar::onOpenMenu(Menu& menu)
{
    _currentMenu = &menu;
}


void MenuBar::onCloseMenu(Menu& menu)
{
    if(_currentMenu == &menu)
        _currentMenu = 0;
}


void MenuBar::onCancel()
{
    if(_currentMenu)
        _currentMenu->cancel();
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


void MenuBar::onResizeEvent(const ResizeEvent& ev)
{
    for(std::size_t i = 0; i < _layout.widgets().size(); ++i)
    {
        MenuBarItem* item = static_cast<MenuBarItem*>(_layout.widgets().at(i));
        Gfx::SizeF itemSize = item->preferredSize();
        item->resize(itemSize);
    }

    _layout.resize( ev.size() );

    // _layout positions the items now in OnResizeEvent
    // TODO: our overall design should make this clearer
    WidgetBaseType::onResizeEvent(ev);
}


void MenuBar::onEnterEvent(const EnterEvent& ev)
{
    WidgetBaseType::onEnterEvent(ev);
    
    //grabMouse();
}


void MenuBar::onLeaveEvent(const LeaveEvent& ev)
{
    WidgetBaseType::onLeaveEvent(ev);
}

} // namespace

} // namespace
