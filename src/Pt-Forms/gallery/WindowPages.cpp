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

#include "WindowPages.h"
#include <Pt/Gfx/Color.h>

namespace Pt {

namespace Forms {

namespace Gallery {

WindowPage::WindowPage()
: _body(Direction::Top)
{
    setPageTitle("Window");
    setPageDescription("Opens a child window with a title and close button.");

    _windowLabel.setText("Child window");
    _windowLabel.setAlignment(Alignment::Center);
    _windowLabel.setPadding(12);

    _close.setText("Close");
    _close.setMargin(5);
    _close.setPadding(5);
    _close.clicked() += Pt::slot(_window, &Window::close);

    _windowLayout.addItem(_windowLabel, DockingLayout::Fill);
    _windowLayout.addItem(_close, DockingLayout::Bottom);

    _window.setTitle("Sample Window");
    _window.setContent(&_windowLayout);
    _window.resize( Gfx::SizeF(280, 180) );
    _window.closed() += Pt::slot(*this, &WindowPage::onClosed);

    _open.setText("Open Window");
    _open.setMargin(5);
    _open.setPadding(5);
    _open.clicked() += Pt::slot(*this, &WindowPage::onOpen);

    _body.setPadding(8);
    _body.addItem(_open);
    setBody(_body);
}


void WindowPage::onOpen()
{
    _window.show();
    _window.activate();
    log("window shown");
}


void WindowPage::onClosed()
{
    log("window closed");
}


WorkspacePage::WorkspacePage()
{
    setPageTitle("Workspace");
    setPageDescription("Embedded workspace with two movable child windows.");

    _labelA.setText("Child A");
    _labelA.setAlignment(Alignment::Center);
    _labelA.setBackground( Gfx::Color(200, 120, 100) );
    _childA.setTitle("Child A");
    _childA.setContent(&_labelA);
    _childA.resize( Gfx::SizeF(200, 140) );
    _childA.move( Gfx::PointF(20, 20) );

    _labelB.setText("Child B");
    _labelB.setAlignment(Alignment::Center);
    _labelB.setBackground( Gfx::Color(100, 200, 120) );
    _childB.setTitle("Child B");
    _childB.setContent(&_labelB);
    _childB.resize( Gfx::SizeF(200, 140) );
    _childB.move( Gfx::PointF(80, 80) );

    _workspace.addWindow(_childA);
    _workspace.addWindow(_childB);
    _workspace.setMinimumSize( Gfx::SizeF(400, 300) );

    setBody(_workspace, false);

    _childA.show();
    _childB.show();
}

} // namespace

} // namespace

} // namespace
