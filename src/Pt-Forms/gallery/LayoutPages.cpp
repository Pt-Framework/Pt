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

#include "LayoutPages.h"
#include <Pt/Forms/Icon.h>
#include <Pt/Gfx/Color.h>
#include <sstream>

namespace Pt {

namespace Forms {

namespace Gallery {

FlowLayoutPage::FlowLayoutPage()
: _sample(Direction::Left)
, _buttons(Direction::Left)
{
    setPageTitle("FlowLayout");
    setPageDescription("Direction, center, and reverse.");

    const char* names[4] = { "A", "B", "C", "D" };
    const Gfx::Color colors[4] =
    {
        Gfx::Color(200, 120, 100),
        Gfx::Color(100, 200, 120),
        Gfx::Color(100, 120, 200),
        Gfx::Color(220, 200, 80)
    };

    for(int n = 0; n < 4; ++n)
    {
        _cells[n].setText(names[n]);
        _cells[n].setAlignment(Alignment::Center);
        _cells[n].setBackground(colors[n]);
        _cells[n].setMinimumSize(60, 40);
        _cells[n].setMargin(4);
        _sample.addItem(_cells[n]);
    }

    _sample.setPadding(8);
    _sample.setMinimumHeight(80);

    _left.setText("Left");
    _left.setMargin(4);
    _left.setPadding(4);
    _left.clicked() += Pt::slot(*this, &FlowLayoutPage::onLeft);

    _right.setText("Right");
    _right.setMargin(4);
    _right.setPadding(4);
    _right.clicked() += Pt::slot(*this, &FlowLayoutPage::onRight);

    _top.setText("Top");
    _top.setMargin(4);
    _top.setPadding(4);
    _top.clicked() += Pt::slot(*this, &FlowLayoutPage::onTop);

    _bottom.setText("Bottom");
    _bottom.setMargin(4);
    _bottom.setPadding(4);
    _bottom.clicked() += Pt::slot(*this, &FlowLayoutPage::onBottom);

    _center.setText("Center");
    _center.setMargin(4);
    _center.clicked() += Pt::slot(*this, &FlowLayoutPage::onCenter);

    _reverse.setText("Reverse");
    _reverse.setMargin(4);
    _reverse.clicked() += Pt::slot(*this, &FlowLayoutPage::onReverse);

    _buttons.addItem(_left);
    _buttons.addItem(_right);
    _buttons.addItem(_top);
    _buttons.addItem(_bottom);
    _buttons.addItem(_center);
    _buttons.addItem(_reverse);

    _body.addItem(_buttons, DockingLayout::Bottom);
    _body.addItem(_sample, DockingLayout::Fill);
    setBody(_body, false);
}


void FlowLayoutPage::onLeft()
{
    _sample.setDirection(Direction::Left);
    log("Left");
}


void FlowLayoutPage::onRight()
{
    _sample.setDirection(Direction::Right);
    log("Right");
}


void FlowLayoutPage::onTop()
{
    _sample.setDirection(Direction::Top);
    log("Top");
}


void FlowLayoutPage::onBottom()
{
    _sample.setDirection(Direction::Bottom);
    log("Bottom");
}


void FlowLayoutPage::onCenter()
{
    _sample.setCenter( _center.isChecked() );
    log("center");
}


void FlowLayoutPage::onReverse()
{
    _sample.setReverse( _reverse.isChecked() );
    log("reverse");
}


DockingLayoutPage::DockingLayoutPage()
{
    setPageTitle("DockingLayout");
    setPageDescription(
        "Fill + stacked Bottom docks, matching ChildW _childView. "
        "Resize this page or open the child window.");

    Icon icon = loadGalleryIcon();

    _fill.setName("FillLabel");
    _fill.setText(
        "Fill region. Resize the gallery window. "
        "Bottom docks stay packed. Fill takes leftover space.");
    _fill.setAlignment(Alignment::Center);
    _fill.setBackground( Gfx::Color(220, 230, 245) );
    _fill.setPadding(12);

    _iconLabel.setName("IconLabel");
    _iconLabel.setIcon(icon, icon.minimumSize());
    _iconLabel.setText("Bottom dock 1");
    _iconLabel.setAlignment(Alignment::Left);
    _iconLabel.setPadding(8);
    _iconLabel.setBackground( Gfx::Color(240, 220, 70) );

    _comboItems[0].setText("One");
    _comboItems[1].setText("Two");
    _comboItems[2].setText("Three");
    _combo.addItem(_comboItems[0]);
    _combo.addItem(_comboItems[1]);
    _combo.addItem(_comboItems[2]);
    _combo.setEditable(false);
    _combo.setMargin(5);

    _edit.setPlaceholderText("Input");
    _edit.setMargin(5);

    _check.setText("Option");
    _check.setMargin(5);

    _ok.setText("OK");
    _ok.setMargin(5);
    _ok.setPadding(5);

    _openWindow.setText("Open Fill + Bottom Window");
    _openWindow.setMargin(5);
    _openWindow.setPadding(5);
    _openWindow.clicked() += Pt::slot(*this, &DockingLayoutPage::onOpenWindow);

    _buttonBar.setName("ButtonBar");
    _buttonBar.setPadding(5);
    _buttonBar.addItem(_combo, DockingLayout::Top);
    _buttonBar.addItem(_edit, DockingLayout::Top);
    _buttonBar.addItem(_check, DockingLayout::Top);
    _buttonBar.addItem(_ok, DockingLayout::Top);
    _buttonBar.addItem(_openWindow, DockingLayout::Top);

    _body.setName("MainPanel");
    _body.setPadding(20);
    _body.addItem(_fill, DockingLayout::Fill);
    _body.addItem(_iconLabel, DockingLayout::Bottom);
    _body.addItem(_buttonBar, DockingLayout::Bottom);
    setBody(_body, false);

    _stressFill.setText(
        "Fill region in a child window. Drag-resize. "
        "Bottoms stay packed. Fill takes leftover space.");
    _stressFill.setAlignment(Alignment::Center);
    _stressFill.setBackground( Gfx::Color(220, 230, 245) );
    _stressFill.setPadding(12);

    _stressIcon.setIcon(icon, icon.minimumSize());
    _stressIcon.setText("Bottom dock 1");
    _stressIcon.setAlignment(Alignment::Left);
    _stressIcon.setPadding(8);
    _stressIcon.setBackground( Gfx::Color(240, 220, 70) );

    _stressEdit.setPlaceholderText("Input");
    _stressEdit.setMargin(5);

    _stressCheck.setText("Option");
    _stressCheck.setMargin(5);

    _stressOk.setText("Close");
    _stressOk.setMargin(5);
    _stressOk.setPadding(5);
    _stressOk.clicked() += Pt::slot(_stressWindow, &Window::close);

    _stressBar.setPadding(5);
    _stressBar.addItem(_stressEdit, DockingLayout::Top);
    _stressBar.addItem(_stressCheck, DockingLayout::Top);
    _stressBar.addItem(_stressOk, DockingLayout::Top);

    _stressView.setName("MainPanel");
    _stressView.setPadding(20);
    _stressView.addItem(_stressFill, DockingLayout::Fill);
    _stressView.addItem(_stressIcon, DockingLayout::Bottom);
    _stressView.addItem(_stressBar, DockingLayout::Bottom);

    _stressWindow.setTitle("Fill + Bottom");
    _stressWindow.setContent(&_stressView);
    _stressWindow.resize( Gfx::SizeF(360, 420) );
    _stressWindow.closed() += Pt::slot(*this, &DockingLayoutPage::onStressClosed);
}


void DockingLayoutPage::onOpenWindow()
{
    _stressWindow.show();
    _stressWindow.activate();
    log("Fill + Bottom window shown");
}


void DockingLayoutPage::onStressClosed()
{
    log("Fill + Bottom window closed");
}


GridLayoutPage::GridLayoutPage()
: _body(GridLayout::Vertical, 3)
{
    setPageTitle("GridLayout");
    setPageDescription("Vertical orientation, span 3. Equal cells.");

    const char* names[6] = { "1", "2", "3", "4", "5", "6" };
    for(int n = 0; n < 6; ++n)
    {
        _cells[n].setText(names[n]);
        _cells[n].setAlignment(Alignment::Center);
        _cells[n].setBackground( Gfx::Color(180, 200, 220) );
        _cells[n].setContour( Gfx::Color(80, 100, 120) );
        _cells[n].setFrame(true);
        _cells[n].setMargin(4);
        _cells[n].setMinimumSize(60, 40);
        _body.addItem(_cells[n]);
    }

    _body.setPadding(8);
    setBody(_body, false);
}


TableLayoutPage::TableLayoutPage()
{
    setPageTitle("TableLayout");
    setPageDescription("Rows and columns with Preferred, Fill, and Fixed modes.");

    _body.setColumn(0, TableLayout::Preferred);
    _body.setColumn(1, TableLayout::Fill);
    _body.setColumn(2, TableLayout::Fixed, 80);
    _body.setRow(0, TableLayout::Preferred);
    _body.setRow(1, TableLayout::Fill);

    const char* names[6] = { "A1", "A2 Fill", "A3 80px", "B1", "B2 Fill", "B3" };
    for(int n = 0; n < 6; ++n)
    {
        _cells[n].setText(names[n]);
        _cells[n].setAlignment(Alignment::Center);
        _cells[n].setBackground( Gfx::Color(210, 225, 210) );
        _cells[n].setContour( Gfx::Color(80, 110, 80) );
        _cells[n].setFrame(true);
        _cells[n].setMargin(4);
        _cells[n].setPadding(8);
        _body.addItem(_cells[n], n / 3, n % 3);
    }

    _body.setPadding(8);
    setBody(_body, false);
}


CanvasLayoutPage::CanvasLayoutPage()
{
    setPageTitle("CanvasLayout");
    setPageDescription("Absolute position and size.");

    _a.setText("A");
    _b.setText("B");
    _c.setText("C");
    _c.setAlignment(Alignment::Center);
    _c.setBackground( Gfx::Color(230, 220, 200) );
    _c.setFrame(true);

    _body.addItem(_a, Gfx::PointF(10, 10), Gfx::SizeF(80, 32));
    _body.addItem(_b, Gfx::PointF(110, 10), Gfx::SizeF(80, 32));
    _body.addItem(_c, Gfx::PointF(10, 60), Gfx::SizeF(180, 80));
    setBody(_body, false);
}


StackLayoutPage::StackLayoutPage()
: _buttons(Direction::Left)
{
    setPageTitle("StackLayout");
    setPageDescription("Stacked pages. Buttons change current.");

    _page0.setText("Stack page 0");
    _page0.setAlignment(Alignment::Center);
    _page0.setBackground( Gfx::Color(200, 120, 100) );

    _page1.setText("Stack page 1");
    _page1.setAlignment(Alignment::Center);
    _page1.setBackground( Gfx::Color(100, 200, 120) );

    _stack.addItem(_page0);
    _stack.addItem(_page1);
    _stack.setCurrent(0);
    _stack.currentChanged() += Pt::slot(*this, &StackLayoutPage::onChanged);

    _go0.setText("Page 0");
    _go0.setMargin(4);
    _go0.setPadding(4);
    _go0.clicked() += Pt::slot(*this, &StackLayoutPage::onPage0);

    _go1.setText("Page 1");
    _go1.setMargin(4);
    _go1.setPadding(4);
    _go1.clicked() += Pt::slot(*this, &StackLayoutPage::onPage1);

    _buttons.addItem(_go0);
    _buttons.addItem(_go1);

    _body.addItem(_buttons, DockingLayout::Bottom);
    _body.addItem(_stack, DockingLayout::Fill);
    setBody(_body, false);
}


void StackLayoutPage::onPage0()
{
    _stack.setCurrent(0);
}


void StackLayoutPage::onPage1()
{
    _stack.setCurrent(1);
}


void StackLayoutPage::onChanged(std::size_t n)
{
    std::ostringstream os;
    os << "current " << n;
    log( os.str().c_str() );
}

} // namespace

} // namespace

} // namespace
