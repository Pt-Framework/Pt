/*
  Copyright (C) 2026 Marc Boris Duerner

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
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the:
  Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
  Boston, MA 02110-1301 USA
*/

#ifndef PT_FORMS_API_MENUS_H
#define PT_FORMS_API_MENUS_H

/** @addtogroup Pt-Forms-Menus

    @brief Menu bars and popup menus for commands and context actions.

    Use menus to group related commands when the user chooses an action from
    a compact list. A %MenuBar is a horizontal strip of top-level entries.
    Choosing an entry opens its associated %Menu. A %Menu is a transient
    popup containing a vertical list of command items and submenu items.

    %MenuItem represents a command that the user can choose. It shows text,
    an optional icon, and an optional separator before the item. %MenuSubItem
    and %MenuMenuItem represent entries that open a nested %Menu. A
    %MenuBarItem is a top-level submenu entry for a %MenuBar.

    Applications create menu items and nested menus, add the items to a %Menu
    or %MenuBar, and keep them alive until they are removed or disassociated.
    %MenuSubItem and %MenuMenuItem associate a nested %Menu without taking
    ownership. Activating a command item emits its %MenuItemBase::triggered()
    signal. %Popup provides the transient window behavior used by %Menu and
    can also present other short interactions.

    Menu containers and items use the application style for their default
    appearance. %Menu, %MenuBar, and %MenuItemBase provide local brush, pen,
    color, and font overrides when an application needs a specific appearance.
*/

#endif