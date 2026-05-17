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
    struct Flag { unsigned value; };

    static constexpr Flag Enabled     = {0x01};
    static constexpr Flag Disabled    = {0x02};
    static constexpr Flag Highlighted = {0x04};
    static constexpr Flag Focused     = {0x08};
    static constexpr Flag Checked     = {0x10};
    static constexpr Flag ReadOnly    = {0x20};
    static constexpr Flag Selected    = {0x40};
    static constexpr Flag Expanded    = {0x80};

    StyleFlags()
    : _value(0)
    {}

    bool has(Flag mask) const
    { return (_value & mask.value) != 0; }

    void set(Flag mask)
    { _value |= mask.value; }

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
    struct Flag { unsigned value; };

    static constexpr Flag Pressed = {0x01};
    static constexpr Flag Flat    = {0x02};

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
    { return (_value & mask.value) != 0; }

    void set(StyleFlags::Flag mask)
    { _state.set(mask); }

    void set(Flag mask)
    { _value |= mask.value; }

  private:
    StyleFlags _state;
    unsigned _value;
};

} // namespace Forms

} // namespace Pt

#endif
