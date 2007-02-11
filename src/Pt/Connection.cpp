/***************************************************************************
 *   Copyright (C) 2004-2006 by Marc Boris Dürner                               *
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
 **************************************************************************/

#include "Pt/Connectable.h"
#include "Pt/Callable.h"

#include <iostream>
using namespace std;


namespace Pt {


Connection::Connection()
{
    _data = new ConnectionData();
}


Connection::Connection(Connectable& sender, Slot* slot)
{
    _data = new ConnectionData(sender, slot);
    sender.opened(*this);
    slot->opened(*this);
}


Connection::Connection(const Connection& connection)
{
    _data = connection._data;
    _data->ref();
}


Connection::~Connection()
{
    if( _data->unref() > 0) {
        return;
    }

    // close the connection if its still valid
    if( this->valid() ) {
        this->close();
    }

    // delete the shared data
    delete _data;
    _data = 0;
}


void Connection::close()
{
    _data->setValid(false);
    _data->slot().closed( *this );
    _data->sender().closed( *this );
}


Connection& Connection::operator=(const Connection& connection)
{
    if( 0 == _data->unref() && this->valid() ) {
        this->close();
    }

    delete _data;

    _data = connection._data;
    _data->ref();
    return (*this);
}


bool Connection::operator==(const Connection& connection) const
{
    // compare pointers or callable?
    return _data == connection._data;
}

} //namespace Pt


