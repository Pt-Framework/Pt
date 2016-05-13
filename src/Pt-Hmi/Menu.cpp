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

#include <Pt/Hmi/Menu.h>

namespace Pt {

namespace Hmi {

Menu::Menu()
: _iconWidth(0)
{
    setBorder(false);

    _layout.setAlignment(FlowLayout::Top);
    setMainWidget(&_layout);
}


Menu::~Menu()
{
}

void Menu::add(MenuItem& item)
{
    // TODO: have virtual onRemove() in Widget to notify derived classes

    _layout.add(item);
    onContentChanged();
}


void Menu::remove(MenuItem& item)
{
    // TODO: have virtual onRemove() in Widget to notify derived classes

    _layout.remove(item);
    onContentChanged();
}

/* TODO: 
this happens when item->resize() is called in onContentChanged

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

    double itemsWidth = 0;
    double menuHeight = 0;

    // determine menu size
    for(std::size_t i = 0; i < _layout.widgets().size(); ++i)
    {
        MenuItem* item = static_cast<MenuItem*>(_layout.widgets().at(i));

        // item size with margin
        Gfx::SizeF itemSize = item->preferredSize();
        itemSize.addWidth( item->margin().leftRight() );
        itemSize.addHeight( item->margin().topBottom() );

        // the width of the items is the width of the widest item
        itemsWidth = std::max( itemsWidth, itemSize.width() );

        // the height of the menu is the sum of the item heights
        menuHeight += itemSize.height();

        _iconWidth = std::max(item->icon().width(), _iconWidth);
    }
    
    int iconPadding = 4;
    int menuPadding = 4;

    if(_iconWidth > 0)
        _iconWidth += 2 * iconPadding;

    // left padding is for the icon strip
    _layout.setPadding( Spacing(menuPadding + _iconWidth, 
                                menuPadding, 
                                menuPadding, 
                                menuPadding) );

    Gfx::SizeF size(_iconWidth + itemsWidth, menuHeight);
    size.addWidth(2 * menuPadding);
    size.addHeight(2 * menuPadding);

    resize(size);
}


void Menu::onPaintEvent(const PaintEvent& ev)
{
    Painter painter( surface() );
    
    //
    // icon strip on the left side
    //
    if(_iconWidth > 0)
    {
        Gfx::RectF iconStrip( Gfx::PointF(0, 0),
                              Gfx::SizeF(_layout.padding().left(),
                                          size().height()) );

        Gfx::Brush brush = Pt::Gfx::Color(0.95f, 0.95f, 0.95f);
        painter.setBrush(brush);
        painter.fillRect(iconStrip);

        // draw icon centered for each menu item
        for(std::size_t i = 0; i < _layout.widgets().size(); ++i)
        {
            MenuItem* item = static_cast<MenuItem*>(_layout.widgets().at(i));

            double iconX = (iconStrip.width() - item->icon().width()) / 2;

            double iconY = item->position().y();
            iconY += (item->size().height() - item->icon().height()) / 2;

            Gfx::PointF iconPos(iconX, iconY);
            painter.drawImage(iconPos, item->icon());
        }
    }
    
    //
    // menu border
    //
    Gfx::RectF borderRect(Gfx::PointF(0, 0), size());

    Gfx::Pen pen(1, Gfx::Color(0.5f, 0.5f, 0.51f) );
    painter.setPen(pen);
    painter.drawRect(borderRect);

    BaseType::onPaintEvent(ev);
}


void Menu::onActivateEvent(const ActivateEvent& ev)
{
    BaseType::onActivateEvent(ev);

    if( ! ev.isActive() )
        close();
}


void Menu::onMoveEvent(const MoveEvent& ev)
{
    BaseType::onMoveEvent(ev);
}


void Menu::onResizeEvent(const ResizeEvent& ev)
{
    for(std::size_t i = 0; i < _layout.widgets().size(); ++i)
    {
        MenuItem* item = static_cast<MenuItem*>(_layout.widgets().at(i));

        Gfx::SizeF itemSize = item->preferredSize();
        item->resize(itemSize);
    }

    // _layout positions the items now in OnResizeEvent
    // TODO: our overall design should make this clearer
    BaseType::onResizeEvent(ev);
}

} // namespace

} // namespace
