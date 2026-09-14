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
#ifndef PT_FORMS_MENUMENUITEM_H
#define PT_FORMS_MENUMENUITEM_H

#include <Pt/Forms/Api.h>
#include <Pt/Forms/MenuBase.h>
#include <Pt/Forms/MenuItemBase.h>
#include <vector>

namespace Pt {
namespace Forms {

class Menu;
class MenuBar;

/** @brief Represents an alternate menu item that manages a nested menu.

    %MenuMenuItem provides the same nested-menu association and keyboard
    delegation as %MenuSubItem while using the base item presentation.
    %setMenu() associates a nested menu without taking ownership.

    @ingroup Pt-Forms-Menus
*/
class PT_FORMS_API MenuMenuItem : public MenuItemBase
{
    friend class Menu;

    public:
        /** @brief Defines the inherited menu item type.
        */
        typedef MenuItemBase Base;

    public:
        /** @brief Creates an item without a nested menu.
        */
        MenuMenuItem();

        /** @brief Destroys the item.
        */
        virtual ~MenuMenuItem();

        /** @brief Associates @a menu with this item.

            Pass 0 to remove the current association. The item does not own
            @a menu, which must remain alive while it is associated.
        */
        void setMenu(Menu* menu);

        /** @brief Returns the nested menu, or 0 when none is assigned.
        */
        const Menu* menu() const
        {
            return _menu;
        }


        /** @brief Returns the nested menu, or 0 when none is assigned.
        */
        Menu* menu()
        {
            return _menu;
        }

        /** @brief Sets the containing menu coordinator.

            The item does not own @a p.
        */
        void setParentMenu(MenuBase* p)
        {
            _parentMenu = p;
        }

        /** @brief Cancels the nested menu interaction.
        */
        void cancel();


        /** @brief Closes the nested menu.
        */
        void closeMenu();

        /** @brief Opens the nested menu.
        */
        void openMenu();


        /** @brief Returns true if the nested menu is open.
        */
        bool isMenuOpen() const
        {
            return _isOpen;
        }

    protected:
        virtual const std::vector<Key> onGetShortcuts();

        virtual const std::vector<Pt::Char> onGetMnemonics();

        virtual void onShortcut(const Key& key);

        virtual void onMnemonic(Pt::Char m);

    private:
        MenuBase* _parentMenu;
        Menu* _menu;
        bool _isOpen;
};

}}

#endif