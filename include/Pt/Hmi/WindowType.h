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

#ifndef PT_HMI_WINDOWHOST_H
#define PT_HMI_WINDOWHOST_H

#include <Pt/Hmi/Api.h>
#include <Pt/Hmi/Visual.h>
#include <Pt/Gfx/Point.h>
#include <Pt/Gfx/Size.h>
#include <Pt/Gfx/Rect.h>
#include <Pt/Signal.h>
#include <Pt/Connectable.h>

namespace Pt {

namespace Hmi {

class WindowType
{
    public:
        enum Type
        {
            Default = 0,      // default window style
            Borderless = 1,   // no decoration
            Frameless = Borderless, // TODO: deprecated
            Popup = Borderless      // TODO: deprecated
        };

        WindowType(Type t = Default)
        : _type(t)
        {}

        WindowType& operator=(Type t)
        {
            _type = t;
            return *this;
        }

        operator Pt::uint32_t() const
        { 
            return _type; 
        }

    private:
        Pt::uint32_t _type;
};

class WindowState
{
    public:
        enum State
        {
            Normal = 0,
            Minimized = 1,
            Maximized = 2
        };

        WindowState(State t = Normal)
        : _state(t)
        {}

        WindowState& operator=(State t)
        {
            _state = t;
            return *this;
        }

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
