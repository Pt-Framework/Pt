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

#include "GalleryPage.h"
#include <Pt/Forms/Application.h>
#include <Pt/System/Path.h>

namespace Pt {

namespace Forms {

namespace Gallery {

Icon loadGalleryIcon()
{
    System::Path appdir( Application::instance().argv()[0] );
    appdir = appdir.dirName();

    Icon icon;
    icon.addImage(16, 16, appdir / "test-16x16.png");
    icon.addImage(24, 24, appdir / "test-24x24.png");
    icon.addImage(32, 32, appdir / "test-32x32.png");
    icon.addImage(48, 48, appdir / "test-48x48.png");
    icon.addImage(64, 64, appdir / "test-64x64.png");
    return icon;
}


GalleryPage::GalleryPage()
: _header(Direction::Top)
{
    _title.setName("PageTitle");
    _title.setAlignment(Alignment::Left);
    _title.setPadding(8);

    _description.setName("PageDescription");
    _description.setAlignment(Alignment::Left);
    _description.setPadding(8);

    _header.setName("PageHeader");
    _header.addItem(_title);
    _header.addItem(_description);

    _log.setName("PageLog");
    _log.setText("Ready.");
    _log.setPadding(8);
    _log.setMinimumHeight(28);

    _pageLayout.setName("PageLayout");
    _pageLayout.addItem(_header, DockingLayout::Top);
    _pageLayout.addItem(_log, DockingLayout::Bottom);

    setContent(&_pageLayout);
    setPadding(4);
}


GalleryPage::~GalleryPage()
{
}


ListBoxItem& GalleryPage::navItem()
{
    return _navItem;
}


const String& GalleryPage::pageTitle() const
{
    return _title.text();
}


void GalleryPage::setPageTitle(const String& title)
{
    _title.setText(title);
    _navItem.setText(title);
}


void GalleryPage::setPageDescription(const String& text)
{
    _description.setText(text);
}


void GalleryPage::setBody(Control& body, bool scroll)
{
    if(scroll)
    {
        _scroll.setName("PageScroll");
        _scroll.setScrollBars(true);
        _scroll.setContent(body);
        _pageLayout.addItem(_scroll, DockingLayout::Fill);
    }
    else
    {
        _pageLayout.addItem(body, DockingLayout::Fill);
    }
}


void GalleryPage::log(const String& text)
{
    _log.setText(text);
}


void GalleryPage::log(const char* text)
{
    _log.setText(text);
}

} // namespace

} // namespace

} // namespace
