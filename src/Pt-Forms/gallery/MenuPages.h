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

#ifndef PT_FORMS_GALLERY_MENUPAGES_H
#define PT_FORMS_GALLERY_MENUPAGES_H

#include "GalleryPage.h"
#include <Pt/Forms/FlowLayout.h>
#include <Pt/Forms/DockingLayout.h>
#include <Pt/Forms/MenuBar.h>
#include <Pt/Forms/Menu.h>
#include <Pt/Forms/MenuItem.h>
#include <Pt/Forms/MenuItemBase.h>
#include <Pt/Forms/MenuSubItem.h>
#include <Pt/Forms/MenuBarItem.h>
#include <Pt/Forms/Popup.h>
#include <Pt/Forms/PushButton.h>
#include <Pt/Forms/Label.h>
#include <Pt/Forms/Key.h>

namespace Pt {

namespace Forms {

namespace Gallery {

class MenuBarPage : public GalleryPage
{
    public:
        MenuBarPage();

    private:
        void onTriggered(MenuItemBase& item);

    private:
        DockingLayout _body;
        MenuBar       _bar;
        Menu          _fileMenu;
        Menu          _editMenu;
        Menu          _subMenu;
        MenuBarItem   _fileItem;
        MenuBarItem   _editItem;
        MenuItem      _newItem;
        MenuItem      _openItem;
        MenuSubItem   _recentItem;
        MenuItem      _recent1;
        MenuItem      _recent2;
        MenuItem      _cutItem;
        MenuItem      _copyItem;
        MenuItem      _pasteItem;
        Label         _hint;
        Key           _newKey;
};

class PopupPage : public GalleryPage
{
    public:
        PopupPage();

    private:
        void onShowPopup();

        void onPopupClosed();

    private:
        FlowLayout _body;
        PushButton _show;
        Popup      _popup;
        Label      _popupLabel;
};

} // namespace

} // namespace

} // namespace

#endif
