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

#ifndef _Pt_Net_TcpIp4Socket_h
#define _Pt_Net_TcpIp4Socket_h
 
#include <Pt/Export.h>
#include <Pt/Net/TcpSocket.h>
#include <Pt/Net/Ip4Socket.h>

class TcpIp4SocketImpl;


namespace Pt {

namespace Net {

	class PT_EXPORT TcpIp4ServerSocket : public Ip4ServerSocket, public TcpServerSocket {
		friend class TcpIp4Socket;

		public:
			TcpIp4ServerSocket() throw(IO::IOError);

			~TcpIp4ServerSocket() throw();

			//! Binds to a address and port as server.
			void bind(const Ip4Address& addr, port_t port) throw(IO::IOError);

			//! Listens for incoming connections.
			void listen() throw(IO::IOError);

			//! Waits for connects with a given timeout.
			bool wait(unsigned int timeout) throw(IO::IOError);

			void setTimeOut(unsigned int msec) throw(IO::IOError);

		private:
			TcpIp4SocketImpl* _impl;
	};


	class PT_EXPORT TcpIp4Socket : public Ip4Socket, public TcpSocket {
		public:
			TcpIp4Socket();

			TcpIp4Socket(const Ip4Address& addr, port_t port);

			//! deprectated
			TcpIp4Socket(TcpIp4Socket& socket);

			//! Accepts a incoming connection.
			TcpIp4Socket(TcpIp4ServerSocket& socket);

			~TcpIp4Socket() throw();

			//! Connects to a server with a given address and port.
			void connect(const Ip4Address& addr, port_t port) throw(IO::IOError);

			//! Binds to a address and port as server.
			void bind(const Ip4Address& addr, port_t port) throw(IO::IOError);

			//! Listens for incoming connections.
			void listen() throw(IO::IOError);

			//! Waits for connects with a given timeout.
			//bool waitClient(unsigned int timeout) throw(IO::IOError);

			//! Wait for data on the socket
			//bool waitData(unsigned int timeout, unsigned int timeout_usec=0);

			//! Wait for an event on the socket.
			bool wait(WaitMode mode, unsigned int msec) throw(IO::IOError);

			void setTimeOut(int msec) throw(IO::IOError);

		protected:
			//! Reimplemented from IODevice.
			void _close() throw(IO::IOError);

			//! Reimplemented from IODevice.
			bool _wait(WaitMode mode, unsigned int msec) throw(IO::IOError);

			//! Reimplemented from IODevice.
			size_t _read(char* buffer, size_t count) throw(IO::IOError);

			//! Reimplemented from IODevice.
			size_t _peek(char* buffer, size_t count) throw(IO::IOError);

			//! Reimplemented from IODevice.
			size_t _write(const char* buffer, size_t count) throw(IO::IOError);

		private:
			class TcpIp4SocketImpl* _impl;
			//friend class TcpIp4SocketImpl;
	};

} // !namespace Net

} // !namespace Pt

#endif
