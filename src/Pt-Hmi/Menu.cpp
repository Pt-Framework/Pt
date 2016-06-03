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

Menu::Menu()
: _impl(0)
{
    _impl = new MenuImpl(*this);    
}


Menu::~Menu()
{
    if( parentShell() )
        parentShell()->removeMenu(*this);

    delete _impl;
}


void Menu::setName(const std::string& name)
{ 
    _impl->setName(name); 
}


void Menu::addItem(MenuItem& item)
{
    _impl->addItem(item);
}


void Menu::removeItem(MenuItem& item)
{
    _impl->removeItem(item);
}


void Menu::show(const Gfx::PointF& pos)
{
    _impl->move(pos);
    _impl->show();
}


bool Menu::isVisible() const
{
    return _impl->isVisible();
}


void Menu::onAddMenu(Menu& menu, const Pt::String& text)
{
    _impl->onAddMenu(menu, text);
}


void Menu::onRemoveMenu(Menu& menu)
{
    _impl->onRemoveMenu( menu );
}


void Menu::close()
{
    _impl->close();
}


void Menu::onCloseMenu(Menu& menu)
{
    _impl->onCloseMenu(menu);
}


void Menu::onCancel()
{
    _impl->onCancel();
}


MenuShell* Menu::onFindMenu(const Gfx::PointF& screenPos)
{
    return _impl->onFindMenu(screenPos);
}


MenuImpl* Menu::impl()
{
    return _impl;
}

} // namespace

} // namespace
