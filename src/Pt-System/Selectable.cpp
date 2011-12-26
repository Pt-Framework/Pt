/*
 * Copyright (C) 2008 Marc Boris Duerner
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

#include "Pt/System/Selectable.h"
#include <cassert>

namespace Pt {

namespace System {

Selectable::Selectable()
: _parent(0)
, _state(Idle)
{ 
}


Selectable::~Selectable()
{
    assert(_state != Avail);

    if(_parent)
    {    
        _parent->onDetach(*this);
    }
}


void Selectable::setParent(EventLoop* parent)
{
    //TODO: exception safety...
    //      may need to split this function in attach/detach...

    if(_parent)
    {
        if(_state == Avail)
            _parent->onIdle(*this);

        // TODO: should call cancel to make sure we are not changing 
        //       loops while operation is running
        //this->cancel();
        this->onDetach(*_parent);
        _parent->onDetach(*this);
        _parent = 0;
    }

    if(parent)
    {
        this->onAttach(*parent);
        parent->onAttach(*this);

        if(_state == Avail)
            parent->onAvail(*this);
    }

    _parent = parent;
}


EventLoop* Selectable::parent() const
{
    return _parent;
}


void Selectable::close()
{
    // TODO: should we also cancel?

    this->onClose();

    if(_parent)
    {
       if(_state == Avail)
       {
            _parent->onIdle(*this);
            _state = Idle;
       }
    }
}


void Selectable::cancel()
{
    // this should block until the operation is cancelled
    this->onCancel();

    // signal that an operation was cancelled
    this->setIdle();
}


void Selectable::setAvail(bool isAvail)
{
    if(isAvail)
    {
        if(_parent)
            _parent->onAvail(*this); 
    
        _state = Avail;
    }
    else
    {
        if(_parent)
            _parent->onIdle(*this);
    
        _state = Idle;
    }
}

void Selectable::setIdle()
{
    if(_parent)
        _parent->onIdle(*this);

    _state = Idle;
}



/*bool Selectable::wait(std::size_t msecs)
{
    return this->onWait(msecs);
}*/


/*bool Selectable::avail() const
{
    return this->isAvail();
}*/


/*bool Selectable::enabled() const
{
    return _state != Disabled;
}*/


/*void Selectable::setEnabled(bool isEnabled)
{
    if(isEnabled)
    {
        //if(_state == Disabled)
        //    _state = Active;
    }
    else // disable
    {
        if(_parent)
        {
           if(_state == Avail)
           {
                _parent->onIdle(*this);
                _state = Idle;
           }
        }

        //_state = Disabled;
    }
}*/


/*bool Selectable::idle() const
{
    return _state == Idle;
}*/


/*void Selectable::setIdle()
{ 
    if(_parent)
        _parent->onIdle(*this);
    
    _state = Idle; 
}*/


/*void Selectable::setActive()
{ 
    if(_parent)
        _parent->onActive(*this); 

    _state = Busy;
}*/


/*void Selectable::setAvail()
{ 
    if(_parent)
        _parent->onAvail(*this); 

    _state = Avail;
}*/

/*void Selectable::setState(State state)
{
    if(state == Disabled)
    {
        if(_parent)
        {
            _parent->onDisable(*this);
        }
    }
    else if(_state == Disabled)
    {
        if(_parent)
        {
            _parent->onEnable(*this);
        }
    }

    //State prev = _state;
    _state = state;

    // TODO: rename onChanged to onAvail
    // TODO: Disabled and Idle are somewhat the same state
    if(_parent)
    {
        if( _state == Avail)
        {
            //_parent->onChanged(*this);
        }
        else if(_state == Idle || _state == Busy)
        {
            //_parent->onChanged(*this);
        }
    }
}*/

}

}
