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

#ifndef PT_FORMS_GALLERY_INPUTPAGES_H
#define PT_FORMS_GALLERY_INPUTPAGES_H

#include "GalleryPage.h"
#include <Pt/Forms/FlowLayout.h>
#include <Pt/Forms/LineEdit.h>
#include <Pt/Forms/SpinBox.h>
#include <Pt/Forms/ComboBox.h>
#include <Pt/Forms/ListBox.h>
#include <Pt/Forms/Slider.h>
#include <Pt/Forms/Label.h>
#include <Pt/Forms/CheckBox.h>
#include <Pt/Forms/ProgressBar.h>

namespace Pt {

namespace Forms {

namespace Gallery {

class LineEditPage : public GalleryPage
{
    public:
        LineEditPage();

    private:
        void onEdited(const String& text);

        void onReturn(const String& text);

        void onFinished(const String& text);

        void onEnableToggled();

    private:
        FlowLayout _body;
        LineEdit   _normal;
        LineEdit   _masked;
        LineEdit   _hidden;
        LineEdit   _disabled;
        CheckBox   _enableBox;
};

class SpinBoxPage : public GalleryPage
{
    public:
        SpinBoxPage();

    private:
        void onValue(int value);

        void onEditable();

    private:
        FlowLayout _body;
        SpinBox    _spin;
        CheckBox   _editable;
};

class ComboBoxPage : public GalleryPage
{
    public:
        ComboBoxPage();

    private:
        void onSelected(ListBoxItem& item);

        void onReturn(const String& text);

    private:
        FlowLayout  _body;
        ComboBox    _fixed;
        ComboBox    _editable;
        ListBoxItem _fixedItems[4];
        ListBoxItem _editItems[4];
};

class SliderPage : public GalleryPage
{
    public:
        SliderPage();

    private:
        void onPosition(int pos);

    private:
        FlowLayout  _body;
        Slider      _slider;
        Label       _value;
        ProgressBar _bar;
};

} // namespace

} // namespace

} // namespace

#endif
