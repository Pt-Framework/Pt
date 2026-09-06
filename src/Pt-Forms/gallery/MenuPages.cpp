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

#include "MenuPages.h"

namespace Pt {

namespace Forms {

namespace Gallery {

MenuBarPage::MenuBarPage()
: _newKey(Key::Control, Key::N)
{
    setPageTitle("MenuBar");
    setPageDescription(
        "File and Edit menus, shortcuts, submenu, separator. "
        "The main window bar is another live sample.");

    _newItem.setText("New");
    _newItem.setShortcut(&_newKey);
    _newItem.triggered() += Pt::slot(*this, &MenuBarPage::onTriggered);
    _fileMenu.addItem(_newItem);

    _openItem.setText("Open");
    _openItem.setSeperator(true);
    _openItem.triggered() += Pt::slot(*this, &MenuBarPage::onTriggered);
    _fileMenu.addItem(_openItem);

    _recent1.setText("File A");
    _recent1.triggered() += Pt::slot(*this, &MenuBarPage::onTriggered);
    _subMenu.addItem(_recent1);

    _recent2.setText("File B");
    _recent2.triggered() += Pt::slot(*this, &MenuBarPage::onTriggered);
    _subMenu.addItem(_recent2);

    _recentItem.setText("Recent");
    _recentItem.setMenu(&_subMenu);
    _fileMenu.addItem(_recentItem);

    _fileItem.setText("File");
    _fileItem.setMenu(&_fileMenu);
    _bar.addItem(_fileItem);

    _cutItem.setText("Cut");
    _cutItem.triggered() += Pt::slot(*this, &MenuBarPage::onTriggered);
    _editMenu.addItem(_cutItem);

    _copyItem.setText("Copy");
    _copyItem.triggered() += Pt::slot(*this, &MenuBarPage::onTriggered);
    _editMenu.addItem(_copyItem);

    _pasteItem.setText("Paste");
    _pasteItem.triggered() += Pt::slot(*this, &MenuBarPage::onTriggered);
    _editMenu.addItem(_pasteItem);

    _editItem.setText("Edit");
    _editItem.setMenu(&_editMenu);
    _bar.addItem(_editItem);

    _hint.setText("Open File or Edit on the bar above.");
    _hint.setAlignment(Alignment::Left);
    _hint.setPadding(8);

    _body.addItem(_bar, DockingLayout::Top);
    _body.addItem(_hint, DockingLayout::Fill);
    setBody(_body, false);
}


void MenuBarPage::onTriggered(MenuItemBase& item)
{
    log( item.text() );
}


PopupPage::PopupPage()
: _body(Direction::Top)
{
    setPageTitle("Popup");
    setPageDescription("Anchored popup window. Click the button to show it.");

    _popupLabel.setText("Popup content");
    _popupLabel.setAlignment(Alignment::Center);
    _popupLabel.setPadding(12);

    _popup.setAnchor(&_show);
    _popup.setContent(&_popupLabel);
    _popup.setTitle("Popup");
    _popup.resize( Gfx::SizeF(200, 80) );
    _popup.closed() += Pt::slot(*this, &PopupPage::onPopupClosed);

    _show.setText("Show Popup");
    _show.setMargin(5);
    _show.setPadding(5);
    _show.clicked() += Pt::slot(*this, &PopupPage::onShowPopup);

    _body.setPadding(8);
    _body.addItem(_show);
    setBody(_body);
}


void PopupPage::onShowPopup()
{
    _popup.show();
    log("popup shown");
}


void PopupPage::onPopupClosed()
{
    log("popup closed");
}

} // namespace

} // namespace

} // namespace
