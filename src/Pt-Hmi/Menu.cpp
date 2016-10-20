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

namespace Pt {

namespace Hmi {

///////////////////////////////////////////////////////////////////////////////
// SubMenuItem
///////////////////////////////////////////////////////////////////////////////

static const double indicatorWidth = 5.0; 

class SubMenuItem : public MenuItem
{
    typedef MenuItem BaseType;

    public:
        SubMenuItem(Menu& menu, const Pt::String& text)
        : _menu(menu)
        {
            setText(text);
        }
    
        virtual ~SubMenuItem()
        { }

        Menu& menu()
        { return _menu; }

    protected:
        virtual Gfx::SizeF onAutoSize() const
        {
            Gfx::SizeF size = BaseType::onAutoSize();
    
            // space for the menu indicator
            size.addWidth(indicatorWidth); 
    
            return size;
        }

        virtual void onPaintShortcut(PaintSurface& surface, 
                                     const Gfx::RectF& updateRect)
        {
            //BaseType::onPaintShortcut(surface, updateRect);

            Painter painter(surface);
            painter.setClip(updateRect);

            //
            // draw menu indicator
            //
            double x = size().width() - indicatorWidth - padding().right();
            double y = size().height() / 2;

            Gfx::PointF indicator[3] = { Gfx::PointF(x - 3, y - 4),
                                          Gfx::PointF(x + 1, y),
                                          Gfx::PointF(x - 3, y + 4) };
  
            Gfx::Color fgColor = this->foregroundPen().color();
            Gfx::Brush brush(fgColor);
            painter.setBrush(brush);
            painter.fillPolygon(indicator, 3);
        }

    private:
        Menu& _menu;
};

///////////////////////////////////////////////////////////////////////////////
// Menu
///////////////////////////////////////////////////////////////////////////////

Menu::Menu()
: Window(0, Window::Popup)
, _parentShell(0)
, _parentMenu(0)
, _currentMenu(0)
, _layout(FlowLayout::Top)
, _iconWidth(0)
{
    setMainWidget(&_layout);    
}


Menu::~Menu()
{
    if( parentShell() )
        parentShell()->removeMenu(*this);

    std::vector<SubMenuItem*>::iterator it;
    for(it = _subMenus.begin(); it != _subMenus.end(); ++it)
    {
        (*it)->menu()._parentMenu = 0;
        delete *it;
    }
}


MenuShell* Menu::parentShell()
{ 
    return _parentShell; 
}


MenuShell& Menu::rootShell()
{    
    if( _parentMenu )
        return _parentMenu->rootShell();

    if( _parentShell )
        return *_parentShell;

    return *this;
}


void Menu::show(const Gfx::PointF& pos)
{
    Window::move(pos);
    Window::show();
}


void Menu::addItem(MenuItem& item)
{
    if(item._menu == this)
        return;

    _layout.add(item);
    
    item._menu = this;
    item.triggered() += Pt::slot(*this, &Menu::onItemTriggered);
    
    onContentChanged();
}


void Menu::removeItem(MenuItem& item)
{
    if(item._menu != this)
        return;

    item._menu = 0;
    item.triggered() -= Pt::slot(*this, &Menu::onItemTriggered);

    _layout.remove(item);

    onContentChanged();
}


void Menu::onItemTriggered(MenuItem&)
{
    rootShell().cancel();    
}


void Menu::onAddMenu(Menu& menu, const Pt::String& text)
{
    SubMenuItem* item = new SubMenuItem(menu, text); 
    item->triggered() += Pt::slot(*this, &Menu::onMenuTriggered);
     
    _subMenus.push_back(item);
    _layout.add(*item);

    menu._parentMenu = this;
    
    onContentChanged();
}


void Menu::onRemoveMenu(Menu& menu)
{
    std::vector<SubMenuItem*>::iterator it;
    for(it = _subMenus.begin(); it != _subMenus.end(); ++it)
    {
        if( &(*it)->menu() == &menu )
        {
            delete *it;
            _subMenus.erase(it);

            menu._parentMenu = 0;
            break;
        }
    }

    if(_currentMenu == &menu)
        _currentMenu = 0;

    onContentChanged();
}


void Menu::onMenuTriggered(MenuItem& m)
{
    SubMenuItem* item = static_cast<SubMenuItem*>(&m);
    Menu& menu = item->menu();

    // TODO: open menu on mouse enter and close menu on mouse leave
    //       possibly delayed by a 500ms timer
    
    if( ! menu.isVisible() )
    {
        Gfx::PointF topRight(item->size().width(), 0);
        Gfx::PointF wpos = item->toWindow(topRight);
        Gfx::PointF menuPos = item->window()->toScreen(wpos);

        menu.show(menuPos);
    }
    else
    {
       menu.close();
    }
}


MenuShell* Menu::onFindMenu(const Gfx::PointF& screenPos)
{ 
    if( ! isVisible() )
        return 0;

    Gfx::RectF rect( position(), size() );
    if( rect.contains(screenPos) )
        return this;

    if( ! _currentMenu)
        return 0;

    return _currentMenu->findMenu(screenPos);
}


void Menu::onOpenMenu(Menu& menu)
{
    _currentMenu = &menu;
}


void Menu::onCloseMenu(Menu& menu)
{
    if(_currentMenu == &menu)
        _currentMenu = 0;
}


void Menu::onCancel()
{
    close();
}


void Menu::onEnter()
{
    grabPointer();
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
    WindowBaseType::onPaintEvent(ev);
}


void Menu::onPaintBackground(const Gfx::RectF& rect)
{
    WindowBaseType::onPaintBackground(rect);

    Painter painter( surface() );
    painter.setClip(rect);

    //
    // icon strip on the left side
    //
    if(_iconWidth > 0)
    {
        Gfx::RectF iconStrip( Gfx::PointF(0, 0),
                              Gfx::SizeF(_layout.padding().left() + _iconWidth,
                                          size().height()) );
        
        // only the damaged region
        //iconStrip = iconStrip.intersect(rect);
        //Gfx::Brush brush = Pt::Gfx::Color(0.95f, 0.95f, 0.95f);

        // TODO: need painter clipping for gradient
        
         Gfx::Brush brush(Gfx::Color(65535* 0.90f, 65535*0.90f, 65535*0.91f),
                          Gfx::Color(65535*0.99f, 65535*0.99f, 65535*0.99f), 
                          Gfx::Brush::Vertical);

        painter.setBrush(brush);
        painter.fillRect(iconStrip);
    }

    //
    // menu border
    //
    Gfx::RectF borderRect(Gfx::PointF(0, 0), size());

    Gfx::Pen pen(Gfx::Color(65535*0.5f, 65535*0.5f, 65535*0.51f), 1 );
    painter.setPen(pen);
    painter.drawRect(borderRect);
}


void Menu::onMouseEvent(const MouseEvent& ev)
{
    WindowBaseType::onMouseEvent(ev);

    Gfx::RectF rect( Gfx::PointF(0,0), size() );
    if( rect.contains( ev.position() ) )
        return;

    Gfx::PointF screenPos = toScreen( ev.position() );
    MenuShell* menu = rootShell().findMenu(screenPos);   
    if(menu && menu != this)
    {   
        // navigate through menu chain
        releasePointer();
        menu->onEnter();
    }
    else if( ev.isPress() )
    {
        // cancel when clicked outside menu chain
        rootShell().cancel();          
    }
}


void Menu::onCloseEvent(const CloseEvent& ev)
{
    releasePointer();

    if( _currentMenu )
    {
        _currentMenu->close();
        _currentMenu = 0;
    }

    if( parentShell() )
        parentShell()->onCloseMenu(*this);
    
    WindowBaseType::onCloseEvent(ev);    
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
    WindowBaseType::onResizeEvent(ev);
}


void Menu::onShowEvent(const ShowEvent& ev)
{
    WindowBaseType::onShowEvent(ev);

    if( ! ev.visible() )
    {
        releasePointer();

        if( parentShell() )
            parentShell()->onCloseMenu(*this);
    }
    else
    {
        if( parentShell() )
            parentShell()->onOpenMenu(*this);

        if( ! parentShell() )
            grabPointer();
    }
}


void Menu::onEnterEvent( const EnterEvent& ev )
{
    WindowBaseType::onEnterEvent(ev);
    
    onEnter();
}


void Menu::onLeaveEvent( const LeaveEvent& ev )
{
    WindowBaseType::onLeaveEvent(ev);
}

} // namespace

} // namespace
