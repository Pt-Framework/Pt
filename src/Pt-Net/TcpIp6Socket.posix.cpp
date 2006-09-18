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
#include "Ip6Socket.posix.cpp"
#include "TcpSocket.posix.cpp"


class PT_EXPORT TcpIp6SocketImpl : public TcpSocketImpl, public Ip6SocketImpl {
	public:
		TcpIp6SocketImpl(long handle)
		{
			_handle = handle;
			//cerr << "Opened: " << handle << endl;
		}

		TcpIp6SocketImpl() throw(IO::IOError)
		{
			int ret = ::socket(AF_INET6, SOCK_STREAM, 0);
			if(ret == -1)
				throw IO::IOError(errno, "Could not open socket", PT_SOURCEINFO);

			_handle = ret;
			//cerr << "Opened: " << ret << endl;
		}

		~TcpIp6SocketImpl()
		{}

		TcpIp6SocketImpl* accept(Ip6Address& addr) throw(IO::IOError)
		{
			socklen_t len = sizeof(sockaddr_in6);
			sockaddr_in6 sin;

			long ret = ::accept(_handle, (sockaddr*)&sin, &len);
			if(ret == -1)
				throw IO::IOError(errno, "Could not accept connection on socket", PT_SOURCEINFO);

			//cerr << "Accepted: " << ret << endl;
			addr = Ip6Address(sin.sin6_addr.s6_addr);

			TcpIp6SocketImpl* impl = new TcpIp6SocketImpl(ret);
			return impl;
		}
};




