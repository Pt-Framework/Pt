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

#include <Pt/Hmi/MenuShell.h>
#include <Pt/Hmi/Menu.h>
#include <algorithm>

namespace Pt {

namespace Hmi {

MenuShell::MenuShell()
{
}


MenuShell::~MenuShell()
{
    std::vector<Menu*>::iterator it;
    for(it = _menus.begin(); it != _menus.end(); ++it)
    {
        (*it)->_parentShell = 0;
        delete (*it);
    }
}


void MenuShell::addMenu(Menu& menu, const Pt::String& text)
{
    if(menu.parentShell() == this)
        return;

    onAddMenu(menu, text);
    menu._parentShell = this;

    _menus.push_back(&menu);
}


void MenuShell::removeMenu(Menu& menu)
{
    if(menu.parentShell() != this)
        return;

    onRemoveMenu(menu);
    menu._parentShell = 0;

    std::vector<Menu*>::iterator it = std::remove(_menus.begin(), _menus.end(), &menu);
    _menus.erase(it, _menus.end());
}


Visual* MenuShell::findMenu(const Gfx::PointF& screenPos)
{
    return onFindMenu(screenPos);
}


void MenuShell::cancel()
{
    this->onCancel();
}

} // namespace

} // namespace
