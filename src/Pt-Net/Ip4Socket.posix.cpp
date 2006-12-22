/***************************************************************************
 *   Copyright (C) 2005 by Marc Boris Dürner                               *
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

#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include "Socket.posix.cpp"

#include "Pt/Net/Ip4Address.h"
using namespace Pt::Net;

#include <iostream>
using namespace std;


class PT_API Ip4SocketImpl : virtual public SocketImpl {
	public:
		Ip4SocketImpl()
		{}

		virtual ~Ip4SocketImpl()
		{}

		void connect(const Ip4Address& addr, Pt::Net::port_t port) throw(IO::IOError)
		{
			//cerr << "Connect: " << _handle << " " << addr.str() << ":" << port << endl;

			sockaddr_in sin;
			memset(&sin, 0, sizeof(sockaddr_in));
			sin.sin_family = AF_INET;
			sin.sin_port = htons(port);
			memcpy(&(sin.sin_addr), &addr.address(), sizeof(in_addr));

			int ret = ::connect(_handle, (sockaddr*)(&sin), sizeof(sockaddr_in));
			if(ret == -1)
				throw IO::IOError(errno, "Could not connect socket", PT_SOURCEINFO);

			SocketImpl::setTimeOut(_timeout);
		}

		void bind(const Ip4Address& addr, Pt::Net::port_t port) throw(IO::IOError)
		{
			//cerr << "Bind: " << _handle << " " << addr.str() << ":" << port << endl;

			sockaddr_in sin;
			memset(&sin, 0, sizeof(sockaddr_in));
			sin.sin_family = AF_INET;
			sin.sin_port = htons(port);
			memcpy(&(sin.sin_addr), &addr.address(), sizeof(in_addr));

			int ret = ::bind(_handle, (sockaddr*)(&sin), sizeof(sockaddr_in));
			if(ret == -1)
				throw IO::IOError(errno, "Could not bind address to socket", PT_SOURCEINFO);
		}
};
