/*
 * Copyright (C) 2008-2013 Marc Boris Duerner
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

#include <Pt/System/Selectable.h>
#include <Pt/System/EventLoop.h>
#include <stdexcept>
#include <cassert>

namespace Pt {

namespace System {

Selectable::Selectable()
: _parent(0)
, _prev(0)
, _next(0)
{ 
}


Selectable::~Selectable()
{
    if(_parent)
    {    
        _parent->onDetachSelectable(*this);
    }

    assert(_prev == 0);
    assert(_next == 0);
}


void Selectable::setActive(EventLoop& parent)
{
    if(_parent == &parent)
        return;

    if(_parent)
        throw std::logic_error("selectable already active");

    assert(_prev == 0);
    assert(_next == 0);

    this->onAttach(parent);
    parent.onAttachSelectable(*this);
    _parent = &parent;
}


void Selectable::detach()
{
    if(_parent)
    {
        this->cancel();

        this->onDetach(*_parent);
        _parent->onDetachSelectable(*this);
        _parent = 0;
    }

    assert(_prev == 0);
    assert(_next == 0);
}


void Selectable::cancel()
{ 
    // cancel all async actions
    this->onCancel(); 

    // remove any ready notifications from the parent loop
    if( _parent )
    {
        _parent->onCancel(*this);
    }
}


bool Selectable::run()
{ 
    return this->onRun(); 
}

} // namespace System

} // namespace Pt
