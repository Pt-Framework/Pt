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

#include "Socket.posix.cpp"

#include "Pt/Net/Ip6Address.h"
using namespace Pt::Net;

#include <iostream>
using namespace std;


class PT_API Ip6SocketImpl : virtual public SocketImpl {
	public:
		Ip6SocketImpl()
		{}

		virtual ~Ip6SocketImpl()
		{}

		void connect(const Ip6Address& addr, Pt::Net::port_t port) throw(IO::IOError)
		{
			cerr << "Connecting to: " << addr.str() << endl;
			std::vector<char> address = addr.data();

			sockaddr_in6 sin;
			memset(&sin, 0, sizeof(sockaddr_in6));
			sin.sin6_family = AF_INET6;
			sin.sin6_port = htons(port);
			memcpy(&(sin.sin6_addr), &(address[0]), sizeof(in6_addr));

			int ret = ::connect(_handle, (sockaddr*)(&sin), sizeof(sockaddr_in6));
			if(ret == -1)
				throw IO::IOError(errno, "Could not connect socket", PT_SOURCEINFO);
		}

		void bind(const Ip6Address& addr, Pt::Net::port_t port) throw(IO::IOError)
		{
			cerr << "Binding to: " << addr.str() << endl;
			std::vector<char> address = addr.data();

			sockaddr_in6 sin;
			memset(&sin, 0, sizeof(sockaddr_in6));
			sin.sin6_family = AF_INET6;
			sin.sin6_port = htons(port);
			memcpy(&(sin.sin6_addr), &(address[0]), sizeof(in6_addr));

			int ret = ::bind(_handle, (sockaddr*)(&sin), sizeof(sockaddr_in6));
			if(ret == -1)
				throw IO::IOError(errno, "Could not bind address to socket", PT_SOURCEINFO);
		}
};
