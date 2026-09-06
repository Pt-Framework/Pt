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

#ifndef PT_FORMS_GALLERY_LAYOUTPAGES_H
#define PT_FORMS_GALLERY_LAYOUTPAGES_H

#include "GalleryPage.h"
#include <Pt/Forms/FlowLayout.h>
#include <Pt/Forms/DockingLayout.h>
#include <Pt/Forms/GridLayout.h>
#include <Pt/Forms/TableLayout.h>
#include <Pt/Forms/CanvasLayout.h>
#include <Pt/Forms/StackLayout.h>
#include <Pt/Forms/Label.h>
#include <Pt/Forms/PushButton.h>
#include <Pt/Forms/CheckBox.h>
#include <Pt/Forms/LineEdit.h>
#include <Pt/Forms/ComboBox.h>
#include <Pt/Forms/ListBox.h>
#include <Pt/Forms/Window.h>
#include <Pt/Forms/Panel.h>

namespace Pt {

namespace Forms {

namespace Gallery {

class FlowLayoutPage : public GalleryPage
{
    public:
        FlowLayoutPage();

    private:
        void onLeft();

        void onRight();

        void onTop();

        void onBottom();

        void onCenter();

        void onReverse();

    private:
        DockingLayout _body;
        FlowLayout    _sample;
        FlowLayout    _buttons;
        Label         _cells[4];
        PushButton    _left;
        PushButton    _right;
        PushButton    _top;
        PushButton    _bottom;
        CheckBox      _center;
        CheckBox      _reverse;
};

class DockingLayoutPage : public GalleryPage
{
    public:
        DockingLayoutPage();

    private:
        void onOpenWindow();

        void onStressClosed();

    private:
        DockingLayout _body;
        Label         _fill;
        Label         _iconLabel;
        DockingLayout _buttonBar;
        PushButton    _openWindow;
        LineEdit      _edit;
        ComboBox      _combo;
        ListBoxItem   _comboItems[3];
        CheckBox      _check;
        PushButton    _ok;

        Window        _stressWindow;
        DockingLayout _stressView;
        Label         _stressFill;
        Label         _stressIcon;
        DockingLayout _stressBar;
        PushButton    _stressOk;
        LineEdit      _stressEdit;
        CheckBox      _stressCheck;
};

class GridLayoutPage : public GalleryPage
{
    public:
        GridLayoutPage();

    private:
        GridLayout _body;
        Label      _cells[6];
};

class TableLayoutPage : public GalleryPage
{
    public:
        TableLayoutPage();

    private:
        TableLayout _body;
        Label       _cells[6];
};

class CanvasLayoutPage : public GalleryPage
{
    public:
        CanvasLayoutPage();

    private:
        CanvasLayout _body;
        PushButton   _a;
        PushButton   _b;
        Label        _c;
};

class StackLayoutPage : public GalleryPage
{
    public:
        StackLayoutPage();

    private:
        void onPage0();

        void onPage1();

        void onChanged(std::size_t n);

    private:
        DockingLayout _body;
        StackLayout   _stack;
        Label         _page0;
        Label         _page1;
        FlowLayout    _buttons;
        PushButton    _go0;
        PushButton    _go1;
};

} // namespace

} // namespace

} // namespace

#endif
