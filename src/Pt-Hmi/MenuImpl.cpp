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

#include "MenuImpl.h"
#include <Pt/Hmi/Menu.h>

namespace Pt {

namespace Hmi {

///////////////////////////////////////////////////////////////////////////////
// MenuImpl
///////////////////////////////////////////////////////////////////////////////

MenuImpl::MenuImpl()
: _currentMenu(0)
, _layout(FlowLayout::Top)
, _iconWidth(0)
, _parentMenu(0)
{
    setBorder(false);
    setMainWidget(&_layout);    
}


MenuImpl::~MenuImpl()
{
    // remove this menu from its parent
    if(_parentMenu)
        _parentMenu->impl()->removeMenu(*this);

    while( ! _subMenus.empty() )
        eraseMenu( _subMenus.begin() );
}


void MenuImpl::addItem(MenuItem& item)
{
    _layout.add(item);
    
    item.triggered() += Pt::slot(*this, &MenuImpl::onItemTriggered);

    // TODO: Menu pointer in MenuItem instead of removed signal
    item.removed() += Pt::slot(*this, &MenuImpl::onItemRemoved);
    
    onContentChanged();
}


void MenuImpl::removeItem(MenuItem& item)
{
    // causes item to send removed() signal
    _layout.remove(item);
}


void MenuImpl::onItemTriggered(MenuItem&)
{
    rootMenu().close();    
}


void MenuImpl::onItemRemoved(MenuItem& item)
{
    item.triggered() -= Pt::slot(*this, &MenuImpl::onItemTriggered);
    item.removed() -= Pt::slot(*this, &MenuImpl::onItemRemoved);
    
    onContentChanged();
}


void MenuImpl::addMenu(Menu& parent, Menu& menu, const Pt::String& text)
{
    // remove this menu from its parent
    Menu* oldParent = menu.impl()->_parentMenu;
    if(oldParent)
        oldParent->impl()->removeMenu(*this);

    SubMenuItem* item = new SubMenuItem(menu, text); 
    item->triggered() += Pt::slot(*this, &MenuImpl::onMenuTriggered);
     
    _subMenus.push_back(item);
    _layout.add(*item);
    
    menu.impl()->_parentMenu = &parent;

    onContentChanged();
}


void MenuImpl::removeMenu(MenuImpl& impl)
{
    std::vector<SubMenuItem*>::iterator it;
    for(it = _subMenus.begin(); it != _subMenus.end(); ++it)
    {
        if( (*it)->menu().impl() == &impl )
        {
            eraseMenu(it);
            break;
        }
    }

    onContentChanged();
}


void MenuImpl::eraseMenu(std::vector<SubMenuItem*>::iterator it)
{
    SubMenuItem* item = *it;
    MenuImpl* subMenu = item->menu().impl();

    // this menu is no longer the parent of its submenu
    subMenu->_parentMenu = 0;

    if(_currentMenu == item)
        _currentMenu = 0;
            
    delete *it;
    _subMenus.erase(it);
}


void MenuImpl::onMenuTriggered(MenuItem& m)
{
    _currentMenu = static_cast<SubMenuItem*>(&m);
}


MenuImpl* MenuImpl::findMenu(const Gfx::PointF& pos)
{
    Gfx::RectF rect( Gfx::PointF(0,0), size() );
    if( rect.contains(pos) )
        return this;

    Gfx::PointF screenPos = this->toScreen(pos);

    MenuImpl* menu = findParentMenu(screenPos);
    if( ! menu )
        menu = findSubMenu(screenPos);
   
    return menu;
}


MenuImpl* MenuImpl::findParentMenu(const Gfx::PointF& screenPos)
{
    if( ! _parentMenu )
        return 0;

    MenuImpl* menu = _parentMenu->impl();

    Gfx::RectF rect( menu->position(), menu->size() );
    if( rect.contains(screenPos) )
        return menu;

    return menu->findParentMenu(screenPos);
}


MenuImpl* MenuImpl::findSubMenu(const Gfx::PointF& screenPos)
{
    if( ! _currentMenu )
        return 0;

    MenuImpl* menu = _currentMenu->menu().impl();

    Gfx::RectF rect( menu->position(), menu->size() );
    if( rect.contains(screenPos) )
        return menu;

    return menu->findSubMenu(screenPos);
}


MenuImpl& MenuImpl::rootMenu()
{    
    if( ! _parentMenu )
        return *this;
      
    return _parentMenu->impl()->rootMenu();
}

/* TODO: 
this happens when item->resize() is called in onContentChanged
One soluton is to assign the _size member in Window::resize immediately
and not only when the ResizeEvent is received

1 Menu Resize      10x10    (setMainWidget)
2 Item Resize      43x16    (Menu::add)
3 Menu Resize      43x60

4 Menu ResizeEvent 10x10
5 Item ResizeEvent 10x0
6 Menu ResizeEvent 43x60

7 Item ResizeEvent 10x0    (von 4) !!!!
*/
void MenuImpl::onContentChanged()
{
    _iconWidth = 0;

    double menuWidth = 0;
    double menuHeight = 0;

    // determine menu size
    for(std::size_t i = 0; i < _layout.widgets().size(); ++i)
    {
        MenuItem* item = static_cast<MenuItem*>(_layout.widgets().at(i));

        // item size with margin
        Gfx::SizeF itemSize = item->preferredSize();
        itemSize.addWidth( item->margin().leftRight() );
        itemSize.addHeight( item->margin().topBottom() );

        // the width of the menu is the width of the widest item
        menuWidth = std::max( menuWidth, itemSize.width() );

        // the height of the menu is the sum of the item heights
        menuHeight += itemSize.height();

        _iconWidth = std::max(item->icon().width(), _iconWidth);
    }
    
    int iconPadding = 4;
    int menuPadding = 4;
   
    if(_iconWidth > 0)
    {
        _iconWidth += 2 * iconPadding;
        menuWidth += 2 * iconPadding;
    }

    _layout.setPadding(menuPadding);
    
    Gfx::SizeF size(menuWidth, menuHeight);
    size.addWidth( _layout.padding().leftRight() );
    size.addHeight( _layout.padding().topBottom() );

    resize(size);
}


void MenuImpl::onPaintEvent(const PaintEvent& ev)
{
    BaseType::onPaintEvent(ev);
}


void MenuImpl::onPaintBackground(const Gfx::RectF& rect)
{
    BaseType::onPaintBackground(rect);

    Painter painter( surface() );

    //
    // icon strip on the left side
    //
    if(_iconWidth > 0)
    {
        Gfx::RectF iconStrip( Gfx::PointF(0, 0),
                              Gfx::SizeF(_layout.padding().left() + _iconWidth,
                                          size().height()) );
        
        // only the damaged region
        iconStrip = iconStrip.intersect(rect);

        Gfx::Brush brush = Pt::Gfx::Color(0.95f, 0.95f, 0.95f);
        painter.setBrush(brush);
        painter.fillRect(iconStrip);
    }

    //
    // menu border
    //
    Gfx::RectF borderRect(Gfx::PointF(0, 0), size());

    Gfx::Pen pen(1, Gfx::Color(0.5f, 0.5f, 0.51f) );
    painter.setPen(pen);
    painter.drawRect(borderRect);
}


void MenuImpl::onMouseEvent(const MouseEvent& ev)
{
    BaseType::onMouseEvent( ev );

    MenuImpl* menu = findMenu( ev.position() );           
    if(menu)
    {   
        menu->grabMouse();
        return;
    }

    if( ev.isPress() )
    {
        rootMenu().close();            
    }
}


void MenuImpl::onCloseEvent(const CloseEvent& ev)
{
    if( _currentMenu )
        _currentMenu->menu().close();

    BaseType::onCloseEvent(ev);    
    
    if( _parentMenu )
        _parentMenu->impl()->_currentMenu = 0;    
}


void MenuImpl::onResizeEvent(const ResizeEvent& ev)
{
    for(std::size_t i = 0; i < _layout.widgets().size(); ++i)
    {
        MenuItem* item = static_cast<MenuItem*>(_layout.widgets().at(i));

        item->setIconPadding(_iconWidth);

        Gfx::SizeF itemSize = item->preferredSize();
        item->resize(itemSize);
    }

    // _layout positions the items now in OnResizeEvent
    // TODO: our overall design should make this clearer
    BaseType::onResizeEvent(ev);
}


void MenuImpl::onShowEvent(const ShowEvent& ev)
{
    BaseType::onShowEvent(ev);

    if( ! ev.visible() )
        releaseMouse();
}


void MenuImpl::onEnterEvent( const EnterEvent& ev )
{
    BaseType::onEnterEvent(ev);
    
    grabMouse();
}


void MenuImpl::onLeaveEvent( const LeaveEvent& ev )
{
  BaseType::onLeaveEvent(ev);

  if( _parentMenu )
      _parentMenu->impl()->grabMouse();
}


///////////////////////////////////////////////////////////////////////////////
// SubMenuItem
///////////////////////////////////////////////////////////////////////////////

static const double indicatorWidth = 5.0; 


SubMenuItem::SubMenuItem(Menu& menu, const Pt::String& text)
: _menu(menu)
{
    setText(text);
}
    

SubMenuItem::~SubMenuItem()
{
}


void SubMenuItem::onClicked(const Gfx::PointF& pos)
{
    BaseType::onClicked(pos);

    // TODO: open menu on mouse enter and close menu on mouse leave
    //       possibly delayed by a 500ms timer
    
    if( ! _menu.isVisible() )
    {
        Gfx::PointF topRight(size().width(), 0);
        Gfx::PointF wpos = this->toWindow(topRight);
        Gfx::PointF menuPos = window()->toScreen(wpos);

        _menu.show(menuPos);
    }
    else
    {
       _menu.close();       
    }
}


Gfx::SizeF SubMenuItem::onAutoSize() const
{
    Gfx::SizeF size = BaseType::onAutoSize();
    
    // space for the menu indicator
    size.addWidth(indicatorWidth); 
    
    return size;
}


void SubMenuItem::onPaintShortcut(PaintSurface& surface, const Gfx::RectF& updateRect)
{
    //BaseType::onPaintShortcut(surface, updateRect);

    Painter painter(surface);

    //
    // draw menu indicator
    //
    double x = size().width() - indicatorWidth - padding().right();
    double y = size().height() / 2;

    Gfx::PointF indicator[3] = { Gfx::PointF(x - 3, y - 4),
                                 Gfx::PointF(x + 1, y),
                                 Gfx::PointF(x - 3, y + 4) };
  
    Gfx::Color fgColor = this->foregroundColor();
    Gfx::Brush brush(fgColor);
    painter.setBrush(brush);
    painter.fillPolygon(indicator, 3);
}

} // namespace

} // namespace
