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
#include "TcpIp6Socket.posix.cpp"

#include "Pt/Net/TcpIp6Socket.h"
using namespace Pt::Net;

#include <iostream>
using namespace std;


TcpIp6Socket::TcpIp6Socket()
: Ip6Socket()
{
	_impl = new ::TcpIp6SocketImpl();
	this->setValid(true);
	IO::IODevice::setEof(false);
}


TcpIp6Socket::TcpIp6Socket(const Ip6Address& addr, port_t port)
: Socket()
{
	_impl = new ::TcpIp6SocketImpl();
	this->setValid(true);
	IO::IODevice::setEof(false);

	this->connect(addr, port);
	_peerAddress = addr;
}


TcpIp6Socket::TcpIp6Socket(TcpIp6Socket& socket)
{
	_impl = socket._impl->accept(_peerAddress);
	this->setValid(true);
	IO::IODevice::setEof(false);
}


TcpIp6Socket::~TcpIp6Socket() throw()
{
	if( this->valid() ) {
		try {
			this->close();
		}
		catch(...)
		{}
	}

	delete _impl;
}


void TcpIp6Socket::connect(const Ip6Address& addr, port_t port)
{
	_impl->connect(addr, port);
}


void TcpIp6Socket::bind(const Ip6Address& addr, port_t port) throw(IO::IOError)
{
	_impl->bind(addr, port);
}


void TcpIp6Socket::listen() throw(IO::IOError)
{
	_impl->listen();
}


bool TcpIp6Socket::wait(WaitMode mode, unsigned int msec) throw(IO::IOError)
{
	return _impl->wait(mode, msec);
}


void TcpIp6Socket::setTimeOut(int msec) throw(IO::IOError)
{
	_impl->setTimeOut(msec);
}


void TcpIp6Socket::_close() throw(IO::IOError)
{
	_impl->close();
}


size_t TcpIp6Socket::_read(char* buffer, size_t count) throw(IO::IOError)
{
	return _impl->read(buffer, count);
}


size_t TcpIp6Socket::_peek(char* buffer, size_t count) throw(IO::IOError)
{
	return 0;
}


size_t TcpIp6Socket::_write(const char* buffer, size_t count) throw(IO::IOError)
{
	return _impl->write(buffer, count);
}



