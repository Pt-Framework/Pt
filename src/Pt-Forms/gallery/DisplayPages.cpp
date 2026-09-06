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

#include "DisplayPages.h"
#include <Pt/Forms/Icon.h>
#include <Pt/Gfx/Color.h>
#include <sstream>

namespace Pt {

namespace Forms {

namespace Gallery {

LabelPage::LabelPage()
: _body(Direction::Top)
{
    setPageTitle("Label");
    setPageDescription("Alignment, icon, background, and frame.");

    _left.setText("Left aligned");
    _left.setAlignment(Alignment::Left);
    _left.setPadding(8);
    _left.setMargin(4);

    _center.setText("Center aligned");
    _center.setAlignment(Alignment::Center);
    _center.setBackground( Gfx::Color(240, 220, 70) );
    _center.setPadding(8);
    _center.setMargin(4);
    _center.setMinimumHeight(40);

    _right.setText("Right aligned");
    _right.setAlignment(Alignment::Right);
    _right.setPadding(8);
    _right.setMargin(4);

    Icon icon = loadGalleryIcon();
    _icon.setIcon(icon, icon.minimumSize());
    _icon.setText("With icon");
    _icon.setAlignment(Alignment::Left);
    _icon.setPadding(8);
    _icon.setMargin(4);

    _framed.setText("Framed");
    _framed.setAlignment(Alignment::Center);
    _framed.setContour( Gfx::Color(120, 50, 50) );
    _framed.setFrame(true);
    _framed.setPadding(8);
    _framed.setMargin(4);
    _framed.setMinimumHeight(40);

    _body.setPadding(8);
    _body.addItem(_left);
    _body.addItem(_center);
    _body.addItem(_right);
    _body.addItem(_icon);
    _body.addItem(_framed);
    setBody(_body);
}


PanelPage::PanelPage()
: _body(Direction::Top)
{
    setPageTitle("Panel");
    setPageDescription("Background, frame, and nested content.");

    _plain.setBackground( Gfx::Color(220, 230, 240) );
    _plain.setMinimumHeight(60);
    _plain.setMargin(4);

    _framed.setBackground( Gfx::Color(240, 230, 220) );
    _framed.setContour( Gfx::Color(120, 80, 60) );
    _framed.setFrame(true);
    _framed.setMinimumHeight(60);
    _framed.setMargin(4);

    _nestedLabel.setText("Nested label");
    _nestedLabel.setAlignment(Alignment::Center);
    _nested.setContent(&_nestedLabel);
    _nested.setBackground( Gfx::Color(220, 240, 220) );
    _nested.setFrame(true);
    _nested.setMinimumHeight(60);
    _nested.setMargin(4);

    _body.setPadding(8);
    _body.addItem(_plain);
    _body.addItem(_framed);
    _body.addItem(_nested);
    setBody(_body);
}


ProgressBarPage::ProgressBarPage()
: _body(Direction::Top)
{
    setPageTitle("ProgressBar");
    setPageDescription("Range and value. The slider drives the bar.");

    _bar.setName("ProgressBar");
    _bar.setRange(0, 100);
    _bar.setValue(40);
    _bar.setMargin(5);
    _bar.setPadding(5);
    _bar.setMinimumHeight(24);

    _slider.setName("ProgressSlider");
    _slider.setRange(0, 100);
    _slider.setPosition(40);
    _slider.setMargin(5);
    _slider.setPadding(5);
    _slider.positionChanged() += Pt::slot(*this, &ProgressBarPage::onSlider);

    _reset.setText("Reset");
    _reset.setMargin(5);
    _reset.setPadding(5);
    _reset.clicked() += Pt::slot(*this, &ProgressBarPage::onReset);

    _body.setPadding(8);
    _body.addItem(_bar);
    _body.addItem(_slider);
    _body.addItem(_reset);
    setBody(_body);
}


void ProgressBarPage::onSlider(int value)
{
    _bar.setValue(value);

    std::ostringstream os;
    os << "progress " << value;
    log( os.str().c_str() );
}


void ProgressBarPage::onReset()
{
    _bar.reset();
    _slider.setPosition(_bar.value());
    log("reset");
}

} // namespace

} // namespace

} // namespace
