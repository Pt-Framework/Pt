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
#include "Ip4Socket.posix.cpp"
#include "TcpSocket.posix.cpp"



class PT_API TcpIp4SocketImpl : public TcpSocketImpl, public Ip4SocketImpl {
	public:
		TcpIp4SocketImpl(long handle)
		{
			_handle = handle;
			//cerr << "Opened: " << handle << endl;
		}

		TcpIp4SocketImpl() throw(IO::IOError)
		{
			int ret = ::socket(AF_INET, SOCK_STREAM, 0);
			if(ret == -1)
				throw IO::IOError(errno, "Could not open socket", PT_SOURCEINFO);

			_handle = ret;
			//cerr << "Opened: " << ret << endl;
		}

		~TcpIp4SocketImpl()
		{}

		TcpIp4SocketImpl* accept(Ip4Address& addr) throw(IO::IOError)
		{
			//cerr << "Accept: " << _handle << endl;
			socklen_t len = sizeof(sockaddr_in);
			sockaddr_in sin;

			long ret = ::accept(_handle, (sockaddr*)&sin, &len);
			if(ret == -1)
				throw IO::IOError(errno, "Could not accept connection on socket", PT_SOURCEINFO);

			//cerr << "Accepted: " << ret << endl;
			addr = Ip4Address(sin.sin_addr.s_addr);

			TcpIp4SocketImpl* impl = new TcpIp4SocketImpl(ret);
			return impl;
		}
};




