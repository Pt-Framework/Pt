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

#include "MainWindow.h"
#include <Pt/Forms/Application.h>
#include <Pt/Forms/SizePolicy.h>

namespace Pt {

namespace Forms {

namespace Gallery {

MainWindow::MainWindow()
{
    setName("Gallery");
    setTitle("Pt Forms Gallery");
    resize( Gfx::SizeF(1000, 700) );

    _exitItem.setText("Exit");
    _exitItem.triggered() += Pt::slot(*this, &MainWindow::onExit);
    _fileMenu.addItem(_exitItem);
    _fileMenuItem.setText("File");
    _fileMenuItem.setMenu(&_fileMenu);
    _menuBar.addItem(_fileMenuItem);

    _nextItem.setText("Next");
    _nextItem.triggered() += Pt::slot(*this, &MainWindow::onNext);
    _viewMenu.addItem(_nextItem);

    _prevItem.setText("Previous");
    _prevItem.triggered() += Pt::slot(*this, &MainWindow::onPrevious);
    _viewMenu.addItem(_prevItem);

    _viewMenuItem.setText("View");
    _viewMenuItem.setMenu(&_viewMenu);
    _menuBar.addItem(_viewMenuItem);

    _aboutItem.setText("About");
    _aboutItem.triggered() += Pt::slot(*this, &MainWindow::onAbout);
    _helpMenu.addItem(_aboutItem);
    _helpMenuItem.setText("Help");
    _helpMenuItem.setMenu(&_helpMenu);
    _menuBar.addItem(_helpMenuItem);

    _filter.setName("Filter");
    _filter.setPlaceholderText("Filter...");
    _filter.setMargin(4);
    _filter.setPadding(4);
    _filter.textEdited() += Pt::slot(*this, &MainWindow::onFilter);

    _navList.setName("Catalog");
    _navList.setScrollBars(true);

    _navLayout.addItem(_filter, DockingLayout::Top);
    _navLayout.addItem(_navList, DockingLayout::Fill);
    _navPanel.setContent(&_navLayout);

    SizePolicy navPolicy(SizePolicy::Fixed, SizePolicy::Any);
    navPolicy.setWidth(220);
    _navPanel.setSizePolicy(navPolicy);

    addPage(_overview, false);

    addCategory(_catButtons, "BUTTONS");
    addPage(_pushButton, true);
    addPage(_checkBox, true);

    addCategory(_catDisplay, "DISPLAY");
    addPage(_label, true);
    addPage(_panel, true);
    addPage(_progressBar, true);

    addCategory(_catInput, "INPUT");
    addPage(_lineEdit, true);
    addPage(_spinBox, true);
    addPage(_comboBox, true);
    addPage(_slider, true);

    addCategory(_catCollections, "COLLECTIONS");
    addPage(_listBox, true);
    addPage(_tabView, true);
    addPage(_scrollBar, true);
    addPage(_scrollView, true);

    addCategory(_catMenus, "MENUS");
    addPage(_menuBarPage, true);
    addPage(_popup, true);

    addCategory(_catLayouts, "LAYOUTS");
    addPage(_flowLayout, true);
    addPage(_dockingLayout, true);
    addPage(_gridLayout, true);
    addPage(_tableLayout, true);
    addPage(_canvasLayout, true);
    addPage(_stackLayout, true);

    addCategory(_catWindows, "WINDOWS");
    addPage(_windowPage, true);
    addPage(_workspacePage, true);

    _pages.setCurrent(0);
    _overview.navItem().setSelected(true);

    _mainLayout.addItem(_menuBar, DockingLayout::Top);
    _mainLayout.addItem(_navPanel, DockingLayout::Left);
    _mainLayout.addItem(_pages, DockingLayout::Fill);
    setContent(&_mainLayout);

    _aboutLabel.setText("Pt Forms Gallery");
    _aboutLabel.setAlignment(Alignment::Center);
    _aboutLabel.setPadding(16);

    _aboutClose.setText("Close");
    _aboutClose.setMargin(8);
    _aboutClose.setPadding(5);
    _aboutClose.clicked() += Pt::slot(_about, &Window::close);

    _aboutLayout.addItem(_aboutLabel, DockingLayout::Fill);
    _aboutLayout.addItem(_aboutClose, DockingLayout::Bottom);
    _about.setTitle("About");
    _about.setContent(&_aboutLayout);
    _about.resize( Gfx::SizeF(280, 140) );

    closed() += Pt::slot(Application::instance(), &Application::exit);
}


MainWindow::~MainWindow()
{
}


void MainWindow::addCategory(ListBoxItem& item, const char* name)
{
    item.setText(name);
    item.setSelectable(false);
    _navList.addItem(item);

    NavEntry entry;
    entry.item = &item;
    entry.page = 0;
    entry.title = name;
    _nav.push_back(entry);
}


void MainWindow::addPage(GalleryPage& page, bool indent)
{
    if(indent)
    {
        String text("  ");
        text += page.navItem().text();
        page.navItem().setText(text);
    }

    page.navItem().selected() += Pt::slot(*this, &MainWindow::onNavSelected);
    _navList.addItem(page.navItem());
    _pages.addItem(page);

    NavEntry entry;
    entry.item = &page.navItem();
    entry.page = &page;
    entry.title = page.pageTitle();
    _nav.push_back(entry);
}


void MainWindow::onNavSelected(ListBoxItem& item)
{
    for(std::size_t n = 0; n < _nav.size(); ++n)
    {
        if(_nav[n].item == &item && _nav[n].page)
        {
            const std::size_t index = _pages.indexOf(*_nav[n].page);
            _pages.setCurrent(index);
            break;
        }
    }
}


void MainWindow::onFilter(const String& text)
{
    for(std::size_t n = 0; n < _nav.size(); ++n)
    {
        if( ! _nav[n].page )
            continue;

        const bool match = text.empty() ||
                           _nav[n].title.find(text) != String::npos;
        _nav[n].item->show(match);
    }

    for(std::size_t n = 0; n < _nav.size(); ++n)
    {
        if(_nav[n].page)
            continue;

        bool any = text.empty();
        for(std::size_t k = n + 1; k < _nav.size(); ++k)
        {
            if( ! _nav[k].page )
                break;

            if(_nav[k].item->isVisible())
                any = true;
        }

        _nav[n].item->show(any);
    }
}


void MainWindow::selectNav(std::size_t index)
{
    if(index >= _nav.size() || ! _nav[index].page)
        return;

    _nav[index].item->setSelected(true);
    const std::size_t pageIndex = _pages.indexOf(*_nav[index].page);
    _pages.setCurrent(pageIndex);
}


void MainWindow::stepNav(int delta)
{
    std::size_t current = 0;
    const std::size_t pageIndex = _pages.current();
    for(std::size_t n = 0; n < _nav.size(); ++n)
    {
        if(_nav[n].page && _pages.indexOf(*_nav[n].page) == pageIndex)
        {
            current = n;
            break;
        }
    }

    const int count = static_cast<int>(_nav.size());
    int index = static_cast<int>(current);
    for(int step = 0; step < count; ++step)
    {
        index += delta;
        if(index < 0)
            index = count - 1;
        else if(index >= count)
            index = 0;

        const std::size_t n = static_cast<std::size_t>(index);
        if(_nav[n].page && _nav[n].item->isVisible())
        {
            selectNav(n);
            return;
        }
    }
}


void MainWindow::onNext(MenuItemBase& /*item*/)
{
    stepNav(1);
}


void MainWindow::onPrevious(MenuItemBase& /*item*/)
{
    stepNav(-1);
}


void MainWindow::onAbout(MenuItemBase& /*item*/)
{
    _about.show();
    _about.activate();
}


void MainWindow::onExit(MenuItemBase& /*item*/)
{
    Application::instance().exit();
}

} // namespace

} // namespace

} // namespace
