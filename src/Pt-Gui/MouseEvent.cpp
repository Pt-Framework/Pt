/*
 * Copyright (C) 2006 Marc Boris Dürner
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * As a special exception, you may use this file as part of a free
 * software library without restriction. Specifically, if other files
 * instantiate templates or use macros or inline functions from this
 * file, or you compile this file and link it with other files to
 * produce an executable, this file does not by itself cause the
 * resulting executable to be covered by the GNU General Public
 * License. This exception does not however invalidate any other
 * reasons why the executable file might be covered by the GNU Library
 * General Public License.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "Pt/Gui/MouseEvent.h"
#include "Pt/Gui/Widget.h"

#include <string>
#include <iostream>
using namespace std;


namespace Pt {

namespace Gui {


const type_info& MouseEvent::TYPE_INFO = typeid(MouseEvent);


MouseEvent::MouseEvent(Widget& widget, size_t x, size_t y, const Button& button, const Action& action, unsigned int modifiers)
: Event(widget), _x(x), _y(y), _button(button), _action(action), _modifiers(modifiers)
{}


MouseEvent::~MouseEvent()
{}


size_t MouseEvent::x() const
{
    return _x;
}


size_t MouseEvent::y() const
{
    return _y;
}


MouseEvent::Button MouseEvent::button() const
{
    return _button;
}


MouseEvent::Action MouseEvent::action() const
{
    return _action;
}

unsigned int MouseEvent::modifiers() const
{
    return _modifiers;
}


const std::type_info& MouseEvent::typeInfo() const
{
    static const std::type_info& ti = typeid(MouseEvent);
    return ti;
}

} // namespace Gui

} // namespace Pt
