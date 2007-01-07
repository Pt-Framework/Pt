/***************************************************************************
 *   Copyright (C) 2006 by Marc Boris Duerner, Tommi Maekitalo             *
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

#include "SocketImpl.h"
#include <Pt/Net/Timeout.h>
#include <Pt/Exception.h>

#include <winsock2.h>


namespace Pt {

namespace Net {

SocketImpl::SocketImpl() 
: _sd(INVALID_SOCKET) 
{ }


SocketImpl::~SocketImpl()
{
    if (_sd != INVALID_SOCKET)
        close();
}


void SocketImpl::create(int domain, int type, int protocol)
{
	// TODO: concurrency
	{ 
		WSADATA wsadata;
		WSAStartup(0x101, &wsadata);
	}
	
    _sd = ::socket(domain, type, protocol);
    if (_sd < 0)
      throw RuntimeError("cannot create socket", PT_SOURCEINFO); // TODO change exceptiontype
}


void SocketImpl::close()
{
	::shutdown(_sd, SHUT_RDWR);
	::closesocket(_sd);
    _sd = INVALID_SOCKET;
}


bool SocketImpl::wait(SocketImpl::WaitMode mode, int msec) const
{
	fd_set rfds;
	FD_ZERO(&rfds);
	FD_SET(_sd, &rfds);

	struct timeval tv;
	tv.tv_sec = msec / 1000;
	tv.tv_usec = (msec % 1000) * 1000;

	_select:
	int ret = -1;

	switch(mode) 
	{
		case SocketImpl::WaitInput:
			ret = select(_sd + 1, &rfds, 0, 0, &tv);
			break;

		case SocketImpl::WaitOutput:
			ret = select(_sd + 1, 0, &rfds, 0, &tv);
			break;
	}

	// error
	if(ret == -1) 
	{
		if(errno == WSAEINTR)
			goto _select;

		throw Exception("Could not select on socket", PT_SOURCEINFO); //TODO
	}

	// data available
	if(ret == 1)
		return true;
		
	// no data available
	return false;
}

} // namespace Net

} // namespace Pt
