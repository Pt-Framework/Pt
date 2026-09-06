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

#include "CollectionPages.h"
#include <Pt/Forms/Icon.h>
#include <Pt/Gfx/Color.h>
#include <sstream>

namespace Pt {

namespace Forms {

namespace Gallery {

ListBoxPage::ListBoxPage()
: _body(Direction::Top)
{
    setPageTitle("ListBox");
    setPageDescription("Text items, icons, selection, and scrollbars.");

    Icon icon = loadGalleryIcon();
    const char* names[8] =
    {
        "Mercury", "Venus", "Earth", "Mars",
        "Jupiter", "Saturn", "Uranus", "Neptune"
    };

    for(int n = 0; n < 8; ++n)
    {
        _items[n].setText(names[n]);
        if(n % 2 == 0)
            _items[n].setIcon(icon, icon.minimumSize());
        _list.addItem(_items[n]);
    }

    _list.setName("ListBox");
    _list.setScrollBars(true);
    _list.setFrame(true);
    _list.setMinimumHeight(220);
    _list.setMargin(5);
    _list.selected() += Pt::slot(*this, &ListBoxPage::onSelected);

    _body.setPadding(8);
    _body.addItem(_list);
    setBody(_body);
}


void ListBoxPage::onSelected(ListBoxItem& item)
{
    log( item.text() );
}


TabViewPage::TabViewPage()
: _body(Direction::Top)
{
    setPageTitle("TabView");
    setPageDescription("Three tabs. Buttons change the current tab.");

    _tab0.setText("First tab");
    _tab0.setAlignment(Alignment::Center);
    _tab0.setBackground( Gfx::Color(200, 120, 100) );

    _tab1.setText("Second tab");
    _tab1.setAlignment(Alignment::Center);
    _tab1.setBackground( Gfx::Color(100, 200, 120) );

    _tab2.setText("Third tab");
    _tab2.setAlignment(Alignment::Center);
    _tab2.setBackground( Gfx::Color(100, 120, 200) );

    _tabs.addTab(_tab0, "Tab 1");
    _tabs.addTab(_tab1, "Tab 2");
    _tabs.addTab(_tab2, "Tab 3");
    _tabs.setCurrent(0);
    _tabs.setMinimumHeight(180);
    _tabs.setMargin(5);

    _go0.setText("Tab 1");
    _go0.setMargin(4);
    _go0.setPadding(4);
    _go0.clicked() += Pt::slot(*this, &TabViewPage::onTab0);

    _go1.setText("Tab 2");
    _go1.setMargin(4);
    _go1.setPadding(4);
    _go1.clicked() += Pt::slot(*this, &TabViewPage::onTab1);

    _go2.setText("Tab 3");
    _go2.setMargin(4);
    _go2.setPadding(4);
    _go2.clicked() += Pt::slot(*this, &TabViewPage::onTab2);

    _body.setPadding(8);
    _body.addItem(_tabs);
    _body.addItem(_go0);
    _body.addItem(_go1);
    _body.addItem(_go2);
    setBody(_body);
}


void TabViewPage::onTab0()
{
    _tabs.setCurrent(0);
    log("tab 0");
}


void TabViewPage::onTab1()
{
    _tabs.setCurrent(1);
    log("tab 1");
}


void TabViewPage::onTab2()
{
    _tabs.setCurrent(2);
    log("tab 2");
}


ScrollBarPage::ScrollBarPage()
: _body(Direction::Top)
, _h(ScrollBar::Horizontal)
, _v(ScrollBar::Vertical)
{
    setPageTitle("ScrollBar");
    setPageDescription("Horizontal and vertical bars. ScrollLayout powers ScrollView.");

    _h.setRange(0, 100);
    _h.setStepping(1, 10);
    _h.setPosition(0);
    _h.setMinimumHeight(20);
    _h.setMargin(5);
    _h.changed() += Pt::slot(*this, &ScrollBarPage::onH);

    _v.setRange(0, 100);
    _v.setStepping(1, 10);
    _v.setPosition(0);
    _v.setMinimumWidth(20);
    _v.setMinimumHeight(120);
    _v.setMargin(5);
    _v.changed() += Pt::slot(*this, &ScrollBarPage::onV);

    _hLabel.setText("H 0");
    _hLabel.setPadding(8);
    _vLabel.setText("V 0");
    _vLabel.setPadding(8);

    _body.setPadding(8);
    _body.addItem(_h);
    _body.addItem(_hLabel);
    _body.addItem(_v);
    _body.addItem(_vLabel);
    setBody(_body);
}


void ScrollBarPage::onH(double pos)
{
    std::ostringstream os;
    os << "H " << static_cast<int>(pos);
    _hLabel.setText( os.str().c_str() );
    log( os.str().c_str() );
}


void ScrollBarPage::onV(double pos)
{
    std::ostringstream os;
    os << "V " << static_cast<int>(pos);
    _vLabel.setText( os.str().c_str() );
    log( os.str().c_str() );
}


ScrollViewPage::ScrollViewPage()
: _body(Direction::Top)
, _content(Direction::Top)
{
    setPageTitle("ScrollView");
    setPageDescription(
        "Content larger than the viewport. ScrollLayout is the engine behind this.");

    SizePolicy policy(SizePolicy::Any, SizePolicy::Fixed);
    policy.setHeight(36);

    for(int n = 0; n < 16; ++n)
    {
        std::ostringstream os;
        os << "Row " << (n + 1);
        _buttons[n].setText( os.str().c_str() );
        _buttons[n].setSizePolicy(policy);
        _buttons[n].setMargin(4);
        _buttons[n].setPadding(4);
        _content.addItem(_buttons[n]);
    }

    _content.setPadding(5);

    SizePolicy contentPolicy(SizePolicy::Preferred, SizePolicy::Preferred);
    _content.setSizePolicy(contentPolicy);

    _view.setScrollBars(true);
    _view.setContent(_content);
    _view.setMinimumHeight(240);
    _view.setMargin(5);

    _body.setPadding(8);
    _body.addItem(_view);
    setBody(_body);
}

} // namespace

} // namespace

} // namespace
