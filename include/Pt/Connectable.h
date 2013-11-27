/*
 * Copyright (C) 2005-2013 by Dr. Marc Boris Duerner
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

#ifndef Pt_Connectable_h
#define Pt_Connectable_h

#include <Pt/Api.h>
#include <Pt/Connection.h>
#include <list>
#include <cstddef>

namespace Pt {

/** @brief %Connection Management for Signal and Slot Objects.

    This class implements connection management for signal and slot
    objects. It makes sure that all connections where this object
    is involved are closed on destruction. Deriving classes can
    overload Connectable::opened and Connectable::closed to tune
    connection managenment.

    @ingroup sigslot
*/
class PT_API Connectable
{
    public:
        /** @brief Default constructor.
        */
        Connectable();

        /** @brief Closes all connections.

            When a %Connectable object is destroyed, it closes all its
            connections automatically.
        */
        virtual ~Connectable();

        //! @brief Closes all connections.
        void disconnectAll();

        /** @brief Registers a %Connection with the %Connectable.

            This function is called when a new Connection involving
            this object is opened. The default implementation adds
            the connection to a list, so the destructor can close it.

            @param c Connection being opened
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
        /** @brief Copy constructor.

            Connectables can be copied if the derived class provides a
            public copy constructor. This method will not attempt to copy
            any connections.
        */
        Connectable(const Connectable& c);

        /** @brief Assignment operator.

            Connectables can be copied if the derived class provides a
            public copy constructor. This method will not attempt to copy
            any connections.
        */
        Connectable& operator=(const Connectable& rhs);

        /** @brief Returns a list of all current connections.
        */
        const std::list<Connection>& connections() const
        { return _connections; }

        /** @brief Returns a list of all current connections.
        */
        std::list<Connection>& connections()
        { return _connections; }

    private:
        //! @internal @brief A list of all current connections
        std::list<Connection> _connections;
};
	
} // namespace Pt

#endif
