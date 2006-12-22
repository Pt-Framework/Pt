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

#ifndef _Pt_Net_UdpIp4Socket_h
#define _Pt_Net_UdpIp4Socket_h

#include <Pt/Export.h>
#include <Pt/Net/UdpSocket.h>
#include <Pt/Net/Ip4Socket.h>

class UdpIp4SocketImpl;


namespace Pt {

namespace Net {

	class PT_API UdpIp4Sender : virtual public UdpSender, virtual public Ip4Socket {
		public:
			UdpIp4Sender(const Ip4Address& addr, port_t port);
			~UdpIp4Sender() throw();

			//! Udp connect to a server with address and port.
			void connect(const Ip4Address& addr, port_t port) throw(IO::IOError);

			//! Wait for an event on the socket.
			bool wait(WaitMode mode, unsigned int msec) throw(IO::IOError);

			void setTimeOut(int msec) throw(IO::IOError);

		protected:
			//! Reimplemented from IODevice.
			size_t _write(const char* buffer, size_t count) throw(IO::IOError);

		private:
			UdpIp4SocketImpl* _impl;
	};


	class PT_API UdpIp4Receiver : virtual public UdpReceiver, virtual public Ip4ServerSocket {
		public:
			UdpIp4Receiver(const Ip4Address& addr, port_t port);
			~UdpIp4Receiver() throw();

			//! Binds to a address and port as server.
			void bind(const Ip4Address& addr, port_t port) throw(IO::IOError);

		protected:
			//! Reimplemented from IODevice.
			size_t _read(char* buffer, size_t count) throw(IO::IOError);

		private:
			UdpIp4SocketImpl* _impl;
	};

} // !namespace Net

} // !namespace Pt

#endif
