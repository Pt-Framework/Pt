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

#include "ButtonPages.h"
#include <Pt/Forms/Icon.h>

namespace Pt {

namespace Forms {

namespace Gallery {

PushButtonPage::PushButtonPage()
: _body(Direction::Top)
, _shortcut(Key::Control, Key::P)
{
    setPageTitle("PushButton");
    setPageDescription(
        "Default, toggle, flat, icon, mnemonic, shortcut, enabled/disabled.");

    _defaultBtn.setName("DefaultButton");
    _defaultBtn.setText("&Default");
    _defaultBtn.setMargin(5);
    _defaultBtn.setPadding(5);
    _defaultBtn.clicked() += Pt::slot(*this, &PushButtonPage::onDefaultClicked);

    _toggleBtn.setName("ToggleButton");
    _toggleBtn.setText("&Toggle Me");
    _toggleBtn.setToggle(true);
    _toggleBtn.setMargin(5);
    _toggleBtn.setPadding(5);
    _toggleBtn.clicked() += Pt::slot(*this, &PushButtonPage::onToggleClicked);

    _flatBtn.setName("FlatButton");
    _flatBtn.setText("Flat");
    _flatBtn.setFlat(true);
    _flatBtn.setMargin(5);
    _flatBtn.setPadding(5);

    Icon icon = loadGalleryIcon();
    _iconBtn.setName("IconButton");
    _iconBtn.setText("Icon [Ctrl+P]");
    _iconBtn.setIcon(icon, icon.minimumSize());
    _iconBtn.setLayout(Direction::Left);
    _iconBtn.setShortcut(&_shortcut);
    _iconBtn.setMargin(5);
    _iconBtn.setPadding(5);
    _iconBtn.clicked() += Pt::slot(*this, &PushButtonPage::onIconClicked);

    _enableBox.setName("EnableBox");
    _enableBox.setText("Enable icon button");
    _enableBox.setState(CheckBox::Checked);
    _enableBox.setMargin(5);
    _enableBox.clicked() += Pt::slot(*this, &PushButtonPage::onEnableToggled);

    _body.setPadding(8);
    _body.addItem(_defaultBtn);
    _body.addItem(_toggleBtn);
    _body.addItem(_flatBtn);
    _body.addItem(_iconBtn);
    _body.addItem(_enableBox);
    setBody(_body);
}


void PushButtonPage::onDefaultClicked()
{
    log("Default clicked");
}


void PushButtonPage::onToggleClicked()
{
    if(_toggleBtn.isPressed())
        log("Toggle pressed");
    else
        log("Toggle released");
}


void PushButtonPage::onIconClicked()
{
    log("Icon button clicked");
}


void PushButtonPage::onEnableToggled()
{
    const bool on = _enableBox.isChecked();
    _iconBtn.enable(on);
    if(on)
        log("Icon button enabled");
    else
        log("Icon button disabled");
}


CheckBoxPage::CheckBoxPage()
: _body(Direction::Top)
{
    setPageTitle("CheckBox");
    setPageDescription("Unchecked, checked, unspecified, and disabled states.");

    _unchecked.setName("UncheckedBox");
    _unchecked.setText("Unchecked");
    _unchecked.setState(CheckBox::Unchecked);
    _unchecked.setMargin(5);
    _unchecked.clicked() += Pt::slot(*this, &CheckBoxPage::onBoxClicked);

    _checked.setName("CheckedBox");
    _checked.setText("Checked");
    _checked.setState(CheckBox::Checked);
    _checked.setMargin(5);
    _checked.clicked() += Pt::slot(*this, &CheckBoxPage::onBoxClicked);

    _unspecified.setName("UnspecifiedBox");
    _unspecified.setText("Unspecified");
    _unspecified.setState(CheckBox::Unspecified);
    _unspecified.setMargin(5);
    _unspecified.clicked() += Pt::slot(*this, &CheckBoxPage::onBoxClicked);

    _disabled.setName("DisabledBox");
    _disabled.setText("Disabled");
    _disabled.setState(CheckBox::Checked);
    _disabled.enable(false);
    _disabled.setMargin(5);

    _body.setPadding(8);
    _body.addItem(_unchecked);
    _body.addItem(_checked);
    _body.addItem(_unspecified);
    _body.addItem(_disabled);
    setBody(_body);
}


void CheckBoxPage::onBoxClicked()
{
    log("CheckBox clicked");
}

} // namespace

} // namespace

} // namespace
