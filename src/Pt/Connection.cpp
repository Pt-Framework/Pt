/*
 * Copyright (C) 2004-2008 by Marc Boris Duerner
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
