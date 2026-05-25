/* Copyright (C) 2026 Marc Boris Duerner

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
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
  MA 02110-1301 USA
*/

#ifndef Pt_Forms_StyleFlags_h
#define Pt_Forms_StyleFlags_h

#include <Pt/Forms/Api.h>

namespace Pt {

namespace Forms {

/** @brief Common state flags for widget rendering.

    Encapsulates combinable bit flags that describe the current
    interaction state of a widget.
*/
class StyleFlags
{
  public:
    enum Flag
    {
        Enabled     = 0x01,
        Disabled    = 0x02,
        Highlighted = 0x04,
        Focused     = 0x08
    };

    StyleFlags()
    : _value(0)
    {}

    bool has(Flag mask) const
    { return (_value & mask) != 0; }

    void set(Flag mask)
    { _value |= mask; }

  private:
    unsigned _value;
};


/** @brief State flags for button rendering.

    Extends StyleFlags with button-specific flags such as
    Pressed and Flat.
*/
class ButtonStyleFlags
{
  public:
    enum Flag
    {
        Pressed = 0x01,
        Flat    = 0x02
    };

    ButtonStyleFlags()
    : _state()
    , _value(0)
    {}

    ButtonStyleFlags(StyleFlags s)
    : _state(s)
    , _value(0)
    {}

    bool has(StyleFlags::Flag mask) const
    { return _state.has(mask); }

    bool has(Flag mask) const
    { return (_value & mask) != 0; }

    void set(StyleFlags::Flag mask)
    { _state.set(mask); }

    void set(Flag mask)
    { _value |= mask; }

  private:
    StyleFlags _state;
    unsigned _value;
};


/** @brief State flags for check box rendering.

    Extends StyleFlags with check-box-specific flags such as
    Checked.
*/
class CheckBoxStyleFlags
{
  public:
    enum Flag
    {
        Checked = 0x01
    };

    CheckBoxStyleFlags()
    : _state()
    , _value(0)
    {}

    CheckBoxStyleFlags(StyleFlags s)
    : _state(s)
    , _value(0)
    {}

    bool has(StyleFlags::Flag mask) const
    { return _state.has(mask); }

    bool has(Flag mask) const
    { return (_value & mask) != 0; }

    void set(StyleFlags::Flag mask)
    { _state.set(mask); }

    void set(Flag mask)
    { _value |= mask; }

  private:
    StyleFlags _state;
    unsigned _value;
};


/** @brief State flags for spin box rendering.

    Extends StyleFlags with spin-box-specific flags such as
    Editable.
*/
class SpinBoxStyleFlags
{
  public:
    enum Flag
    {
        Editable = 0x01
    };

    SpinBoxStyleFlags()
    : _state()
    , _value(0)
    {}

    SpinBoxStyleFlags(StyleFlags s)
    : _state(s)
    , _value(0)
    {}

    bool has(StyleFlags::Flag mask) const
    { return _state.has(mask); }

    bool has(Flag mask) const
    { return (_value & mask) != 0; }

    void set(StyleFlags::Flag mask)
    { _state.set(mask); }

    void set(Flag mask)
    { _value |= mask; }

  private:
    StyleFlags _state;
    unsigned _value;
};

/** @brief State flags for line edit rendering.

    Extends StyleFlags with line-edit-specific flags such as
    Editable and Placeholder.
*/
class LineEditStyleFlags
{
  public:
    enum Flag
    {
        Editable    = 0x01,
        Placeholder = 0x02
    };

    LineEditStyleFlags()
    : _state()
    , _value(0)
    {}

    LineEditStyleFlags(StyleFlags s)
    : _state(s)
    , _value(0)
    {}

    bool has(StyleFlags::Flag mask) const
    { return _state.has(mask); }

    bool has(Flag mask) const
    { return (_value & mask) != 0; }

    void set(StyleFlags::Flag mask)
    { _state.set(mask); }

    void set(Flag mask)
    { _value |= mask; }

  private:
    StyleFlags _state;
    unsigned _value;
};


/** @brief State flags for progress bar rendering.

    Extends StyleFlags for progress bars.
*/
class ProgressBarStyleFlags
{
  public:
    ProgressBarStyleFlags()
    : _state()
    , _value(0)
    {}

    ProgressBarStyleFlags(StyleFlags s)
    : _state(s)
    , _value(0)
    {}

    bool has(StyleFlags::Flag mask) const
    { return _state.has(mask); }

    void set(StyleFlags::Flag mask)
    { _state.set(mask); }

  private:
    StyleFlags _state;
    unsigned _value;
};


/** @brief State flags for slider rendering.

    Extends StyleFlags for sliders.
*/
class SliderStyleFlags
{
  public:
    SliderStyleFlags()
    : _state()
    , _value(0)
    {}

    SliderStyleFlags(StyleFlags s)
    : _state(s)
    , _value(0)
    {}

    bool has(StyleFlags::Flag mask) const
    { return _state.has(mask); }

    void set(StyleFlags::Flag mask)
    { _state.set(mask); }

  private:
    StyleFlags _state;
    unsigned _value;
};


/** @brief State flags for scroll bar rendering.

    Extends StyleFlags for scroll bars.
*/
class ScrollBarStyleFlags
{
  public:
    enum Flag
    {
        HandleHovered = 0x01,
        HandlePressed = 0x02
    };

    ScrollBarStyleFlags()
    : _state()
    , _value(0)
    {}

    ScrollBarStyleFlags(StyleFlags s)
    : _state(s)
    , _value(0)
    {}

    bool has(StyleFlags::Flag mask) const
    { return _state.has(mask); }

    bool has(Flag mask) const
    { return (_value & mask) != 0; }

    void set(StyleFlags::Flag mask)
    { _state.set(mask); }

    void set(Flag mask)
    { _value |= mask; }

  private:
    StyleFlags _state;
    unsigned _value;
};


/** @brief State flags for list box container rendering.

    Extends StyleFlags for list boxes.
*/
class ListBoxStyleFlags
{
  public:
    ListBoxStyleFlags()
    : _state()
    , _value(0)
    {}

    ListBoxStyleFlags(StyleFlags s)
    : _state(s)
    , _value(0)
    {}

    bool has(StyleFlags::Flag mask) const
    { return _state.has(mask); }

    void set(StyleFlags::Flag mask)
    { _state.set(mask); }

  private:
    StyleFlags _state;
    unsigned _value;
};


/** @brief State flags for list item rendering.

    Extends StyleFlags with item-specific flags such as
    Selected.
*/
class ListItemStyleFlags
{
  public:
    enum Flag
    {
        Selected = 0x01
    };

    ListItemStyleFlags()
    : _state()
    , _value(0)
    {}

    ListItemStyleFlags(StyleFlags s)
    : _state(s)
    , _value(0)
    {}

    bool has(StyleFlags::Flag mask) const
    { return _state.has(mask); }

    bool has(Flag mask) const
    { return (_value & mask) != 0; }

    void set(StyleFlags::Flag mask)
    { _state.set(mask); }

    void set(Flag mask)
    { _value |= mask; }

  private:
    StyleFlags _state;
    unsigned _value;
};

/** @brief State flags for combo box rendering.

    Extends StyleFlags with combo-box-specific flags such as
    Editable and PopupVisible.
*/
class ComboBoxStyleFlags
{
  public:
    enum Flag
    {
        Editable     = 0x01,
        PopupVisible = 0x02
    };

    ComboBoxStyleFlags()
    : _state()
    , _value(0)
    {}

    ComboBoxStyleFlags(StyleFlags s)
    : _state(s)
    , _value(0)
    {}

    bool has(StyleFlags::Flag mask) const
    { return _state.has(mask); }

    bool has(Flag mask) const
    { return (_value & mask) != 0; }

    void set(StyleFlags::Flag mask)
    { _state.set(mask); }

    void set(Flag mask)
    { _value |= mask; }

  private:
    StyleFlags _state;
    unsigned _value;
};

/** @brief State flags for tab view container rendering.

    Extends StyleFlags with container-level tab view state.
*/
class TabViewStyleFlags
{
  public:
    TabViewStyleFlags()
    : _state()
    {}

    TabViewStyleFlags(StyleFlags s)
    : _state(s)
    {}

    bool has(StyleFlags::Flag mask) const
    { return _state.has(mask); }

    void set(StyleFlags::Flag mask)
    { _state.set(mask); }

  private:
    StyleFlags _state;
};

/** @brief State flags for individual tab item rendering.

    Conveys per-tab state such as Active, Highlighted, or Pressed.
*/
class TabItemStyleFlags
{
  public:
    enum Flag
    {
        Active      = 0x01,
        Highlighted = 0x02,
        Pressed     = 0x04
    };

    TabItemStyleFlags()
    : _state()
    , _value(0)
    {}

    TabItemStyleFlags(StyleFlags s)
    : _state(s)
    , _value(0)
    {}

    bool has(StyleFlags::Flag mask) const
    { return _state.has(mask); }

    bool has(Flag mask) const
    { return (_value & mask) != 0; }

    void set(StyleFlags::Flag mask)
    { _state.set(mask); }

    void set(Flag mask)
    { _value |= mask; }

  private:
    StyleFlags _state;
    unsigned _value;
};

} // namespace Forms

} // namespace Pt

#endif
