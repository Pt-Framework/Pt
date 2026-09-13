/* Copyright (C) 2015 Marc Boris Duerner
   Copyright (C) 2015 Laurentiu-Gheorghe Crisan

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
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
  02110-1301  USA
*/

#ifndef PT_FORMS_WINDOWHOST_H
#define PT_FORMS_WINDOWHOST_H

#include <Pt/Forms/Api.h>
#include <Pt/Forms/Widget.h>
#include <Pt/Gfx/Point.h>
#include <Pt/Gfx/Size.h>
#include <Pt/Gfx/Rect.h>
#include <Pt/Signal.h>
#include <Pt/Connectable.h>

namespace Pt {

namespace Forms {

/** @brief Frame style of a window.

    %Default is a normal decorated window. %Borderless is a window without
    decoration.

    @ingroup Pt-Forms-Windows
*/
class WindowType
{
    public:
        /** @brief Defines a window frame style.
        */
        enum Type
        {
            /** @brief Uses the default decorated frame.
            */
            Default = 0,

            /** @brief Uses a frame without decoration.
            */
            Borderless = 1,

            /** @brief Alias for %Borderless.

                @deprecated Use %Borderless instead.
            */
            Frameless = Borderless,

            /** @brief Alias for %Borderless.

                @deprecated Use %Borderless instead.
            */
            Popup = Borderless
        };

        /** @brief Creates a window type with @a type.
        */
        WindowType(Type type = Default)
        : _type(type)
        {}

        /** @brief Sets the window type to @a type and returns this object.
        */
        WindowType& operator=(Type type)
        {
            _type = type;
            return *this;
        }

        /** @brief Returns the window type as an unsigned integer value.
        */
        operator Pt::uint32_t() const
        {
            return _type;
        }

    private:
        Pt::uint32_t _type;
};

/** @brief Presentation state of a window.

    A window is %Normal, %Minimized, or %Maximized.

    @ingroup Pt-Forms-Windows
*/
class WindowState
{
    public:
        /** @brief Defines a window presentation state.
        */
        enum State
        {
            /** @brief Uses the normal window state.
            */
            Normal = 0,

            /** @brief Uses the minimized window state.
            */
            Minimized = 1,

            /** @brief Uses the maximized window state.
            */
            Maximized = 2
        };

        /** @brief Creates a window state with @a state.
        */
        WindowState(State state = Normal)
        : _state(state)
        {}

        /** @brief Sets the window state to @a state and returns this object.
        */
        WindowState& operator=(State state)
        {
            _state = state;
            return *this;
        }

        /** @brief Returns the window state as an unsigned integer value.
        */
        operator Pt::uint32_t() const
        {
            return _state;
        }

    private:
        Pt::uint32_t _state;
};

} // namespace

} // namespace

#endif // include guard
