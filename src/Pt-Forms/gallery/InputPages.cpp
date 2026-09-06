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

#include "InputPages.h"
#include <Pt/Forms/Icon.h>
#include <Pt/Gfx/Color.h>
#include <sstream>

namespace Pt {

namespace Forms {

namespace Gallery {

LineEditPage::LineEditPage()
: _body(Direction::Top)
{
    setPageTitle("LineEdit");
    setPageDescription(
        "Placeholder, Normal/Masked/Hidden echo, accepted length, disabled.");

    _normal.setName("NormalEdit");
    _normal.setPlaceholderText("Type three letters...");
    _normal.setMargin(5);
    _normal.setPadding(5);
    _normal.textEdited() += Pt::slot(*this, &LineEditPage::onEdited);
    _normal.returnPressed() += Pt::slot(*this, &LineEditPage::onReturn);
    _normal.editingFinished() += Pt::slot(*this, &LineEditPage::onFinished);

    _masked.setName("MaskedEdit");
    _masked.setPlaceholderText("Password (masked)");
    _masked.setEchoMode(LineEdit::Masked);
    _masked.setMargin(5);
    _masked.setPadding(5);

    _hidden.setName("HiddenEdit");
    _hidden.setPlaceholderText("Hidden echo");
    _hidden.setEchoMode(LineEdit::Hidden);
    _hidden.setMargin(5);
    _hidden.setPadding(5);

    _disabled.setName("DisabledEdit");
    _disabled.setText("Disabled");
    _disabled.enable(false);
    _disabled.setMargin(5);
    _disabled.setPadding(5);

    _enableBox.setText("Enable first editor");
    _enableBox.setState(CheckBox::Checked);
    _enableBox.setMargin(5);
    _enableBox.clicked() += Pt::slot(*this, &LineEditPage::onEnableToggled);

    _body.setPadding(8);
    _body.addItem(_normal);
    _body.addItem(_masked);
    _body.addItem(_hidden);
    _body.addItem(_disabled);
    _body.addItem(_enableBox);
    setBody(_body);
}


void LineEditPage::onEdited(const String& text)
{
    _normal.setAccepted(text.size() == 3);

    if( ! _normal.isAccepted() )
        _normal.setTextColor( Gfx::Color(255, 0, 0) );
    else
        _normal.setTextColor( Gfx::Color(0, 0, 0) );

    log("textEdited");
}


void LineEditPage::onReturn(const String& /*text*/)
{
    log("returnPressed");
}


void LineEditPage::onFinished(const String& /*text*/)
{
    log("editingFinished");
}


void LineEditPage::onEnableToggled()
{
    _normal.enable( _enableBox.isChecked() );
}


SpinBoxPage::SpinBoxPage()
: _body(Direction::Top)
{
    setPageTitle("SpinBox");
    setPageDescription("Range, editable, accepted even values.");

    _spin.setName("SpinBox");
    _spin.setRange(0, 20);
    _spin.setValue(4);
    _spin.setMargin(5);
    _spin.setPadding(5);
    _spin.valueEdited() += Pt::slot(*this, &SpinBoxPage::onValue);

    _editable.setText("Editable");
    _editable.setState(CheckBox::Checked);
    _editable.setMargin(5);
    _editable.clicked() += Pt::slot(*this, &SpinBoxPage::onEditable);

    _body.setPadding(8);
    _body.addItem(_spin);
    _body.addItem(_editable);
    setBody(_body);

    onValue(_spin.value());
}


void SpinBoxPage::onEditable()
{
    onValue(_spin.value());
}


void SpinBoxPage::onValue(int /*value*/)
{
    _spin.setEditable( _editable.isChecked() );
    _spin.setAccepted( _spin.value() % 2 == 0 );

    if( ! _spin.isAccepted() )
        _spin.setTextColor( Gfx::Color(255, 0, 0) );
    else
        _spin.setTextColor( Gfx::Color(0, 0, 0) );

    std::ostringstream os;
    os << "value " << _spin.value();
    log( os.str().c_str() );
}


ComboBoxPage::ComboBoxPage()
: _body(Direction::Top)
{
    setPageTitle("ComboBox");
    setPageDescription("Non-editable and editable, items with icons.");

    Icon icon = loadGalleryIcon();

    const char* names[4] = { "Alpha", "Beta", "Gamma", "Delta" };
    for(int n = 0; n < 4; ++n)
    {
        _fixedItems[n].setText(names[n]);
        _fixedItems[n].setIcon(icon, icon.minimumSize());
        _fixed.addItem(_fixedItems[n]);

        _editItems[n].setText(names[n]);
        _editable.addItem(_editItems[n]);
    }

    _fixed.setName("FixedCombo");
    _fixed.setEditable(false);
    _fixed.setMaxHeight(160);
    _fixed.setMargin(5);
    _fixed.setPadding(5);
    _fixed.selected() += Pt::slot(*this, &ComboBoxPage::onSelected);

    _editable.setName("EditableCombo");
    _editable.setEditable(true);
    _editable.setText("");
    _editable.setMaxHeight(160);
    _editable.setMargin(5);
    _editable.setPadding(5);
    _editable.returnPressed() += Pt::slot(*this, &ComboBoxPage::onReturn);

    _body.setPadding(8);
    _body.addItem(_fixed);
    _body.addItem(_editable);
    setBody(_body);
}


void ComboBoxPage::onSelected(ListBoxItem& item)
{
    log( item.text() );
}


void ComboBoxPage::onReturn(const String& text)
{
    log(text);
}


SliderPage::SliderPage()
: _body(Direction::Top)
{
    setPageTitle("Slider");
    setPageDescription("Range and positionChanged. Drives a label and a bar.");

    _slider.setName("Slider");
    _slider.setRange(0, 100);
    _slider.setPosition(50);
    _slider.setMargin(5);
    _slider.setPadding(5);
    _slider.positionChanged() += Pt::slot(*this, &SliderPage::onPosition);

    _value.setText("50");
    _value.setAlignment(Alignment::Center);
    _value.setPadding(8);

    _bar.setRange(0, 100);
    _bar.setValue(50);
    _bar.setMargin(5);
    _bar.setMinimumHeight(24);

    _body.setPadding(8);
    _body.addItem(_slider);
    _body.addItem(_value);
    _body.addItem(_bar);
    setBody(_body);
}


void SliderPage::onPosition(int pos)
{
    _bar.setValue(pos);

    std::ostringstream os;
    os << pos;
    _value.setText( os.str().c_str() );
    log( os.str().c_str() );
}

} // namespace

} // namespace

} // namespace
