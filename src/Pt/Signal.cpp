/***************************************************************************
 *   Copyright (C) 2005 by Dr. Marc Boris Duerner                          *
 *   Copyright (C) 2005 Stephan Beal                                       *
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
#include "Pt/Signal.h"

namespace Pt {

SignalBase::Sentry::Sentry(const SignalBase* signal)
: _signal(signal)
{
    _signal->_sentry = this;
    _signal->_sending = true;
    _signal->_dirty = false;
}


SignalBase::Sentry::~Sentry()
{
    if( _signal )
        this->detach();
}


void SignalBase::Sentry::detach()
{
    _signal->_sending = false;

    if( _signal->_dirty == false )
    {
        _signal->_sentry = 0;
        _signal = 0;
        return;
    }

    std::list<Connection>::iterator it = _signal->_connections.begin();
    while( it != _signal->_connections.end() )
    {
        if( it->valid() )
        {
            ++it;
        }
        else
        {
            it = _signal->_connections.erase(it);
        }
    }

    _signal->_dirty = false;
    _signal->_sentry = 0;
    _signal = 0;
}


SignalBase::SignalBase()
: _sentry(0)
, _sending(false)
{ }


SignalBase::~SignalBase()
{
    if(_sentry)
    {
        _sentry->detach();
    }
}


SignalBase& SignalBase::operator=(const SignalBase& other)
{
    this->clear();

    std::list<Connection>::const_iterator it = other.connections().begin();
    std::list<Connection>::const_iterator end = other.connections().end();

    for( ; it != end; ++it)
    {
        const IConnectable& signal = it->sender();
        if( &signal == &other)
        {
            const Slot& slot = it->slot();
            Connection connection( *this, slot.clone()  );
        }
    }

    return *this;
}


bool SignalBase::opened(const Connection& c)
{
    return Connectable::opened(c);
}


void SignalBase::closed(const Connection& c)
{
    // if the signal is currently calling its slots, do not
    // remove the connection now, but only set the cleanup flag
    // Any invalid connection objects will be removed after
    // the signal has finished calling its slots by the Sentry.
    if( _sending )
    {
        _dirty = true;
    }
    else
    {
        Connectable::closed(c);
    }
}


void SignalBase::disconnectSlot(const Slot& slot)
{
    std::list<Connection>::iterator it = Connectable::connections().begin();
    std::list<Connection>::iterator end = Connectable::connections().end();

    for(; it != end; ++it)
    {
        if( it->slot().equals(slot) )
        {
            it->close();
            return;
        }
    }
}

}
