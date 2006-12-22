/***************************************************************************
 *   Copyright (C) 2005 by Marc Boris Dürner                               *
 *                                                                         *
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

#ifndef _Pt_Net_Ip4Socket_h
#define _Pt_Net_Ip4Socket_h

#include <Pt/Export.h>
#include <Pt/Net/Socket.h>
#include <Pt/Net/Ip4Address.h>

#include <iostream>

namespace Pt {

namespace Net {

	class PT_API Ip4Socket : virtual public Socket {
		public:
			Ip4Socket()
			{}

			~Ip4Socket() throw()
			{ }

			//! Returns the address of the connected peer.
			const Ip4Address& peerAddress() const
			{ return _peerAddress;}

			//! Connects to a server with a given address and port.
			virtual void connect(const Ip4Address& addr, port_t port)  throw(IO::IOError) = 0;

			//! Binds to a address and port as server.
			virtual void bind(const Ip4Address& addr, port_t port) throw(IO::IOError) = 0;

		protected:
			Ip4Address _peerAddress;
	};


	class PT_API Ip4ServerSocket : virtual public ServerSocket {
		public:
			Ip4ServerSocket()
			{}

			//! Binds to a address and port as server.
			virtual void bind(const Ip4Address& addr, port_t port) throw(IO::IOError) = 0;

		protected:
			Ip4Address _peerAddress;
	};

} // !namespace Net

} // !namespace Pt

#endif
