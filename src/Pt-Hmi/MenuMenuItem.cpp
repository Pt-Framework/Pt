/* Copyright (C) 2015 Marc Boris Duerner
   Copyright (C) 2015 Laurentiu-Gheorghe Crisan

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
#include <Pt/Hmi/MenuMenuItem.h>
#include <Pt/Hmi/Menu.h>

namespace Pt {
namespace Hmi {

MenuMenuItem::MenuMenuItem()
: _menu(0)
, _isOpen(false)
, _parentMenu(0)
{
}

MenuMenuItem::~MenuMenuItem()
{
}

void MenuMenuItem::closeMenu()
{
    _isOpen = false;
    if (_parentMenu)
        _parentMenu->closeMenu(*this);

}

void MenuMenuItem::openMenu()
{
    _isOpen = true;

    if (_parentMenu)
        _parentMenu->openMenu(*this);
}


void MenuMenuItem::setMenu(Menu* menu)
{
    if (_menu)
    {
        _menu->setParentItem(0);

        if(_parentMenu)
            _parentMenu->addMenu(*this);
    }

    _menu = menu;

    if( _menu)
    {
        _menu->setParentItem(this);   
        
        if (_parentMenu)
            _parentMenu->removeMenu(*this);
    }    
}

const std::vector<Key> MenuMenuItem::onGetShortcuts()
{
    std::vector<Key> sck = MenuBaseItem::onGetShortcuts();
    
    if(_menu == 0)
        return sck;

    std::map<Key, Control*>::const_iterator  it = _menu->shortcuts().begin();

    for( ;it != _menu->shortcuts().end(); ++it)
        sck.push_back(it->first);

    return sck;
}

const std::vector<Pt::Char> MenuMenuItem::onGetMnemonics()
{
    std::vector<Pt::Char> mns = MenuBaseItem::onGetMnemonics();

    if (_menu == 0)
        return mns;

    std::map<Pt::Char, Control*>::const_iterator  it = _menu->mnemonics().begin();

    for (; it != _menu->mnemonics().end(); ++it)
        mns.push_back(it->first);

    return mns;
}

void MenuMenuItem::onMnemonic(Pt::Char m)
{
    const Char* myMn = mnemonic();

    if (myMn)
    {
        if (m == *myMn)
        {
            MenuBaseItem::onMnemonic(m);
            return;
        }
    }

    std::map<Pt::Char, Control*>::const_iterator  it = _menu->mnemonics().begin();

    for (; it != _menu->mnemonics().end(); ++it)
    {
        if (it->first == m)
        {
            it->second->processMnemonic(m);
            break;
        }
    }
}

void MenuMenuItem::onShortcut(const Key& key)
{
    const Key* myKey = shortcut();

    if(myKey)
    {
        if( key == *myKey)
        {
            MenuBaseItem::onShortcut(key);
            return;
        }
    }

    std::map<Key, Control*>::const_iterator  it = _menu->shortcuts().begin();

    for (; it != _menu->shortcuts().end(); ++it)
    {
        if(it->first == key)
        {
            it->second->processShortcut(key);
            break;
        }
    }
}


void MenuMenuItem::cancel()
{
    if(_menu)
        _menu->cancel();
}

}}
