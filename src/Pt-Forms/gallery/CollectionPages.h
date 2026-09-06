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

#ifndef PT_FORMS_GALLERY_COLLECTIONPAGES_H
#define PT_FORMS_GALLERY_COLLECTIONPAGES_H

#include "GalleryPage.h"
#include <Pt/Forms/FlowLayout.h>
#include <Pt/Forms/ListBox.h>
#include <Pt/Forms/TabView.h>
#include <Pt/Forms/Label.h>
#include <Pt/Forms/ScrollBar.h>
#include <Pt/Forms/ScrollView.h>
#include <Pt/Forms/PushButton.h>
#include <Pt/Forms/SizePolicy.h>

namespace Pt {

namespace Forms {

namespace Gallery {

class ListBoxPage : public GalleryPage
{
    public:
        ListBoxPage();

    private:
        void onSelected(ListBoxItem& item);

    private:
        FlowLayout  _body;
        ListBox     _list;
        ListBoxItem _items[8];
};

class TabViewPage : public GalleryPage
{
    public:
        TabViewPage();

    private:
        void onTab0();

        void onTab1();

        void onTab2();

    private:
        FlowLayout _body;
        TabView    _tabs;
        Label      _tab0;
        Label      _tab1;
        Label      _tab2;
        PushButton _go0;
        PushButton _go1;
        PushButton _go2;
};

class ScrollBarPage : public GalleryPage
{
    public:
        ScrollBarPage();

    private:
        void onH(double pos);

        void onV(double pos);

    private:
        FlowLayout _body;
        ScrollBar  _h;
        ScrollBar  _v;
        Label      _hLabel;
        Label      _vLabel;
};

class ScrollViewPage : public GalleryPage
{
    public:
        ScrollViewPage();

    private:
        FlowLayout _body;
        ScrollView _view;
        FlowLayout _content;
        PushButton _buttons[16];
};

} // namespace

} // namespace

} // namespace

#endif
