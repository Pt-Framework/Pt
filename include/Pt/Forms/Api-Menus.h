/* Copyright (C) 2026 Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
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