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

#include "Pt/Net/TcpServerSocket.h"
#include "TcpServerSocketImpl.h"


namespace Pt {

namespace Net {

TcpServerSocket::~TcpServerSocket()
{
	delete _impl;
}


void TcpServerSocket::listen(const std::string& ipaddr, unsigned short int port, unsigned backlog)
{
	if(!_impl)
	{
		_impl = new TcpServerSocketImpl;
	}
	
	_impl->bind(ipaddr, port);
	_impl->listen(backlog);
}

} // namespace Net

} // namespace Pt
