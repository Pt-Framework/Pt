/***************************************************************************
 *   Copyright (C) 2008 Marc Boris Duerner                                 *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this program; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include "Pt/System/Selectable.h"

namespace Pt {

namespace System {

Selectable::~Selectable()
{
}


void Selectable::setSelector(SelectorBase* parent)
{
    if(_parent)
    {
        if( this->enabled() )
            _parent->onRemove(*this);

        this->onDetach(*_parent);
    }

    if(parent)
    {
        if( this->enabled() )
            parent->onAdd(*this);

        this->onAttach(*parent);
    }

    _parent = parent;
}


SelectorBase* Selectable::selector()
{
    return _parent;
}


const SelectorBase* Selectable::selector() const
{
    return _parent;
}


void Selectable::close()
{
    if( this->enabled() )
    {
        this->setEnabled(false);
        this->onClose();
    }
}

bool Selectable::wait(unsigned int msecs)
{
    return this->onWait(msecs); 
}


bool Selectable::enabled() const
{
    return _state != Disabled;
}


bool Selectable::idle() const
{
    return _state == Idle;
}


bool Selectable::busy() const
{
    return _state == Busy;
}


bool Selectable::avail() const
{
    return _state == Avail;
}


Selectable::Selectable()
: _parent(0)
, _state(Disabled)
{ }


void Selectable::setEnabled(bool isEnabled)
{
    if(_parent)
    {
        if(isEnabled)
            _parent->onAdd(*this);
        else
            _parent->onRemove(*this);
    }

    if(_state != Disabled)
        _state = Idle;
}


void Selectable::setState(State state)
{
    _state = state;
    if(_parent)
    {
        _parent->onChanged(*this);
    }
}

}

}
