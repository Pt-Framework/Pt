/***************************************************************************
 *   Copyright (C) 2004-2008 by Marc Boris Duerner                         *
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
#include <memory>

namespace Pt {

Connection::Connection(Connectable& sender, Slot* slot)
: _data(0)
{
	_data = new ConnectionData(sender, slot);

	try 
	{
		sender.onConnectionOpen(*this);
		slot->onConnect(*this);
	}
	catch(...)
	{
	    delete _data;
		throw;
	}
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
}


void Connection::close()
{
    if( this->valid() )
    {
	    _data->slot().onDisconnect( *this );
	    // We set the valid flag here to false since the call above may 
	    // fail for any reason. If setting the valid flag before, a
	    // connection may pretend to be closed but it is not and it 
	    // may reside e.g. in the list of connections of the 
	    // Connectable class and then provoke an infinite loop.
	    _data->setValid(false);
	    _data->sender().onConnectionClose( *this );
	}
}

} //namespace Pt
