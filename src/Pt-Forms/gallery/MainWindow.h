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

#ifndef PT_FORMS_GALLERY_MAINWINDOW_H
#define PT_FORMS_GALLERY_MAINWINDOW_H

#include "GalleryPage.h"
#include "OverviewPage.h"
#include "ButtonPages.h"
#include "DisplayPages.h"
#include "InputPages.h"
#include "CollectionPages.h"
#include "MenuPages.h"
#include "LayoutPages.h"
#include "WindowPages.h"

#include <Pt/Forms/Window.h>
#include <Pt/Forms/MenuBar.h>
#include <Pt/Forms/Menu.h>
#include <Pt/Forms/MenuItem.h>
#include <Pt/Forms/MenuItemBase.h>
#include <Pt/Forms/MenuBarItem.h>
#include <Pt/Forms/DockingLayout.h>
#include <Pt/Forms/Panel.h>
#include <Pt/Forms/LineEdit.h>
#include <Pt/Forms/ListBox.h>
#include <Pt/Forms/StackLayout.h>
#include <Pt/Forms/Label.h>
#include <Pt/Forms/PushButton.h>
#include <Pt/String.h>
#include <vector>

namespace Pt {

namespace Forms {

namespace Gallery {

class MainWindow : public Window
{
    public:
        MainWindow();

        virtual ~MainWindow();

    private:
        void addCategory(ListBoxItem& item, const char* name);

        void addPage(GalleryPage& page, bool indent);

        void onNavSelected(ListBoxItem& item);

        void onFilter(const String& text);

        void onNext(MenuItemBase& item);

        void onPrevious(MenuItemBase& item);

        void onAbout(MenuItemBase& item);

        void onExit(MenuItemBase& item);

        void selectNav(std::size_t index);

        void stepNav(int delta);

    private:
        struct NavEntry
        {
            ListBoxItem* item;
            GalleryPage* page;
            String       title;
        };

        MenuBar     _menuBar;
        Menu        _fileMenu;
        Menu        _viewMenu;
        Menu        _helpMenu;
        MenuBarItem _fileMenuItem;
        MenuBarItem _viewMenuItem;
        MenuBarItem _helpMenuItem;
        MenuItem    _exitItem;
        MenuItem    _nextItem;
        MenuItem    _prevItem;
        MenuItem    _aboutItem;

        DockingLayout _mainLayout;
        Panel         _navPanel;
        DockingLayout _navLayout;
        LineEdit      _filter;
        ListBox       _navList;
        StackLayout   _pages;

        ListBoxItem _catButtons;
        ListBoxItem _catDisplay;
        ListBoxItem _catInput;
        ListBoxItem _catCollections;
        ListBoxItem _catMenus;
        ListBoxItem _catLayouts;
        ListBoxItem _catWindows;

        OverviewPage     _overview;
        PushButtonPage   _pushButton;
        CheckBoxPage     _checkBox;
        LabelPage        _label;
        PanelPage        _panel;
        ProgressBarPage  _progressBar;
        LineEditPage     _lineEdit;
        SpinBoxPage      _spinBox;
        ComboBoxPage     _comboBox;
        SliderPage       _slider;
        ListBoxPage      _listBox;
        TabViewPage      _tabView;
        ScrollBarPage    _scrollBar;
        ScrollViewPage   _scrollView;
        MenuBarPage      _menuBarPage;
        PopupPage        _popup;
        FlowLayoutPage   _flowLayout;
        DockingLayoutPage _dockingLayout;
        GridLayoutPage   _gridLayout;
        TableLayoutPage  _tableLayout;
        CanvasLayoutPage _canvasLayout;
        StackLayoutPage  _stackLayout;
        WindowPage       _windowPage;
        WorkspacePage    _workspacePage;

        Window        _about;
        DockingLayout _aboutLayout;
        Label         _aboutLabel;
        PushButton    _aboutClose;

        std::vector<NavEntry> _nav;
};

} // namespace

} // namespace

} // namespace

#endif
