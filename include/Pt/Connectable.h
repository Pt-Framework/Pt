/***************************************************************************
 *   Copyright (C) 2004-2007 by Dr. Marc Boris Duerner                     *
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
#ifndef Pt_Connectable_h
#define Pt_Connectable_h

#include <Pt/Api.h>
#include <Pt/Connection.h>
#include <list>
#include <cstddef>

namespace Pt {

    /** @brief %Connection management for signal and slot objects
        @ingroup sigslot

        This class implements connection management for signal and slot
        objects. It makes sure that all connections where this object
        is involved are closed on destruction. Deriving classes can
        overload Connectable::opened and Connectable::closed to tune
        connection managenment.
    */
    class PT_API Connectable
    {
        public:
            /** @brief Default constructor.

                Creates an empty %Connectable.
            */
            Connectable();

            /** @brief Closes all connections.

                When a %Connectable object is destroyed, it closes all its
                connections automatically.
            */
            virtual ~Connectable();

            /** @brief Registers a Connection with the %Connectable.

                This function is called when a new Connection involving
                this object is opened. The default implementation adds
                the connection to a list, so the destructor can close it.

                @param c Connection being opened
                @return True if the Connection was accepted
            */
            virtual void onConnectionOpen(const Connection& c);

            /** @brief Unregisters a Connection from the %Connectable.

                This function is called when a new Connection involving
                this object is closed. The default implementation removes
                the connection from its list of connections.

                @param c Connection being opened
            */
            virtual void onConnectionClose(const Connection& c);

            //! @internal @brief For unit tests only.
            std::size_t connectionCount() const
            { return _connections.size(); }

        protected:
            /** @brief Copy constructor

                @sa Connectable::operator=()
            */
            Connectable(const Connectable& c);

            /** @brief Assignment operator

                Connectables can be copy constructed if the derived class
                provides a public copy constructor. Copying a %Connectable
                will not change its connections.
            */
            Connectable& operator=(const Connectable& rhs);

            /** @brief Returns a list of all current connections
            */
            const std::list<Connection>& connections() const
            { return _connections; }

            /** @brief Returns a list of all current connections
            */
            std::list<Connection>& connections()
            { return _connections; }

        protected:
            /** @brief A list of all current connections
            */
            mutable std::list<Connection> _connections;

            //! @internal
            void clear();
    };

	

inline ConnectionData::ConnectionData(Connection& c, Connectable& sender, Slot* slot)
: _refs(1)
, _valid(true)
, _slot(slot)
, _sender(&sender)
{ 
	sender.onConnectionOpen(c);
	slot->onConnect(c);
}

inline void Connection::close()
{
    if( !this->valid() )
        return;

    _data->slot().onDisconnect( *this );
    // We set the valid flag here to false since the call above may 
    // fail for any reason. If setting the valid flag before, a
    // connection may pretend to be closed but it is not and it 
    // may reside e.g. in the list of connections of the 
    // Connectable class and then provoke an infinite loop.
    _data->setValid(false);
    _data->sender().onConnectionClose( *this );
}

	
} // namespace Pt


#endif
