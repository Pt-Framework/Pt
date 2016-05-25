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

#include <Pt/Hmi/Menu.h>
#include <Pt/Hmi/MenuItem.h>

namespace Pt {

namespace Hmi {

///////////////////////////////////////////////////////////////////////////////
// Menu
///////////////////////////////////////////////////////////////////////////////

Menu::Menu()
: _currentMenu(0)
, _layout(FlowLayout::Top)
, _iconWidth(0)
, _parentMenu(0)
{
    setBorder(false);
    setMainWidget(&_layout);    
}


Menu::~Menu()
{
    if(_parentMenu)
        _parentMenu->removeMenu(*this);

    while( ! _subMenus.empty() )
    {
        SubMenuItem* item = _subMenus.back();
        item->menu()._parentMenu = 0;
        delete item;
        _subMenus.pop_back();
    }
}


void Menu::setName(const std::string& name)
{ 
    Window::setName(name); 
}


void Menu::addItem(MenuItem& item)
{
    _layout.add(item);
    
    item.triggered() += Pt::slot(*this, &Menu::onItemTriggered);
    item.removed() += Pt::slot(*this, &Menu::onItemRemoved);
    
    onContentChanged();
}


void Menu::removeItem(MenuItem& item)
{
    _layout.remove(item);

    // parent change calls Menu::onItemRemoved
}


void Menu::addMenu(Menu& menu, const Pt::String& text)
{
    if(menu._parentMenu)
        menu._parentMenu->removeMenu(menu);

    SubMenuItem* item = new SubMenuItem(menu, text); 
    item->triggered() += Pt::slot(*this, &Menu::onMenuTriggered);
     
    _subMenus.push_back(item);
    _layout.add(*item);
    
    menu._parentMenu = this;

    onContentChanged();
}


void Menu::removeMenu(Menu& menu)
{
    std::vector<SubMenuItem*>::iterator it;
    for(it = _subMenus.begin(); it != _subMenus.end(); ++it)
    {
        if( &(*it)->menu() == &menu )
        {
            menu._parentMenu = 0;
            delete *it;
            _subMenus.erase(it);
            break;
        }
    }

    onContentChanged();
}


void Menu::show(const Gfx::PointF& pos)
{
    Window::move(pos);
    Window::show();
}


void Menu::onItemTriggered(MenuItem&)
{
    rootMenu().close();    
}


void Menu::onItemRemoved(MenuItem& item)
{
    item.triggered() -= Pt::slot(*this, &Menu::onItemTriggered);
    item.removed() -= Pt::slot(*this, &Menu::onItemRemoved);
    
    onContentChanged();
}


void Menu::onMenuTriggered(MenuItem& m)
{
    _currentMenu = static_cast<SubMenuItem*>(&m);
}


Menu& Menu::rootMenu()
{    
    if( ! _parentMenu )
        return *this;
      
    return _parentMenu->rootMenu();
}


Menu* Menu::findMenu(const Gfx::PointF& pos)
{
    Gfx::RectF rect( Gfx::PointF(0,0), size() );

    if( rect.contains( pos ) )
        return this;

    if( ! _parentMenu )
        return 0;
      
    return _parentMenu->findMenu( this->toScreen(pos) - _parentMenu->position() );
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
void Menu::onContentChanged()
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


void Menu::onPaintEvent(const PaintEvent& ev)
{
    BaseType::onPaintEvent(ev);
}


void Menu::onPaintBackground(const Gfx::RectF& rect)
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


void Menu::onMouseEvent( const MouseEvent& ev )
{
    BaseType::onMouseEvent( ev );

    // TODO: inplement a findMenu that searches through all
    //       currentMenu to simplify this function 
    Menu* menu = findMenu(ev.position() );           

    if( menu )
    {   
        menu->setCaptureMouse(true);    
        return;
    }     

    if( ev.isPress() )
    {
        rootMenu().close();            
    }
    else
    {
        if( _currentMenu )
            _currentMenu->menu().setCaptureMouse(true);
    }    
}


void Menu::onCloseEvent(const CloseEvent& ev)
{
    if( _currentMenu )
        _currentMenu->menu().close();

    BaseType::onCloseEvent(ev);    
    
    if( _parentMenu )
        _parentMenu->_currentMenu = 0;    
}


void Menu::onResizeEvent(const ResizeEvent& ev)
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


void Menu::onShowEvent( const ShowEvent& ev )
{
    BaseType::onShowEvent(ev);

    setCaptureMouse( ev.visible() );
}


void Menu::onEnterEvent( const EnterEvent& ev )
{
    BaseType::onEnterEvent(ev);
    setCaptureMouse( true );
}


void Menu::onLeaveEvent( const LeaveEvent& ev )
{
  if( _parentMenu )
    _parentMenu->setCaptureMouse( true);   
}


///////////////////////////////////////////////////////////////////////////////
// bMenuIt
///////////////////////////////////////////////////////////////////////////////

static const double indicatorWidth = 5.0; 


Menu::SubMenuItem::SubMenuItem(Menu& menu, const Pt::String& text)
: _menu(menu)
{
    setText(text);
}
    

Menu::SubMenuItem::~SubMenuItem()
{
}


void Menu::SubMenuItem::onClicked(const Gfx::PointF& pos)
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


Gfx::SizeF Menu::SubMenuItem::onAutoSize() const
{
    Gfx::SizeF size = BaseType::onAutoSize();
    
    // space for the menu indicator
    size.addWidth(indicatorWidth); 
    
    return size;
}


void Menu::SubMenuItem::onPaintShortcut(PaintSurface& surface, const Gfx::RectF& updateRect)
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
