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

#include "OverviewPage.h"

namespace Pt {

namespace Forms {

namespace Gallery {

OverviewPage::OverviewPage()
: _body(Direction::Top)
{
    setPageTitle("Overview");
    setPageDescription(
        "Catalog of Pt::Forms widgets and layouts. Select an entry on the left.");

    _line1.setText(
        "Each page shows live samples, typical variants, and an event log.");
    _line1.setAlignment(Alignment::Left);
    _line1.setPadding(8);

    _line2.setText(
        "Use Filter to search, View/Next and View/Previous to walk the catalog.");
    _line2.setAlignment(Alignment::Left);
    _line2.setPadding(8);

    _line3.setText("BUTTONS  PushButton, CheckBox");
    _line3.setAlignment(Alignment::Left);
    _line3.setPadding(8);

    _line4.setText(
        "DISPLAY  Label, Panel, ProgressBar    "
        "INPUT  LineEdit, SpinBox, ComboBox, Slider");
    _line4.setAlignment(Alignment::Left);
    _line4.setPadding(8);

    _line5.setText(
        "COLLECTIONS  ListBox, TabView, ScrollBar, ScrollView    "
        "MENUS  MenuBar, Popup    "
        "LAYOUTS  Flow, Docking, Grid, Table, Canvas, Stack    "
        "WINDOWS  Window, Workspace");
    _line5.setAlignment(Alignment::Left);
    _line5.setPadding(8);

    _body.setPadding(8);
    _body.addItem(_line1);
    _body.addItem(_line2);
    _body.addItem(_line3);
    _body.addItem(_line4);
    _body.addItem(_line5);

    setBody(_body);
}

} // namespace

} // namespace

} // namespace
