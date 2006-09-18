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

#include "TcpIp4Socket.posix.cpp"

#include "Pt/Net/TcpIp4Socket.h"
using namespace Pt::Net;

#include <iostream>
using namespace std;


TcpIp4Socket::TcpIp4Socket()
: Ip4Socket()
{
	_impl = new ::TcpIp4SocketImpl();
	IO::IODevice::setValid(true);
}


TcpIp4Socket::TcpIp4Socket(const Ip4Address& addr, port_t port)
: Socket()
{
	_impl = new ::TcpIp4SocketImpl();
	IO::IODevice::setValid(true);

	this->connect(addr, port);
	_peerAddress = addr;
}


TcpIp4Socket::TcpIp4Socket(TcpIp4Socket& socket)
{
	_impl = socket._impl->accept(_peerAddress);
	IO::IODevice::setValid(true);
}


TcpIp4Socket::TcpIp4Socket(TcpIp4ServerSocket& socket)
{
	_impl = socket._impl->accept(_peerAddress);
	IO::IODevice::setValid(true);
}


TcpIp4Socket::~TcpIp4Socket() throw()
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


void TcpIp4Socket::connect(const Ip4Address& addr, port_t port) throw(IO::IOError)
{
	_impl->connect(addr, port);
}


void TcpIp4Socket::bind(const Ip4Address& addr, port_t port) throw(IO::IOError)
{
	_impl->bind(addr, port);
}


void TcpIp4Socket::listen() throw(IO::IOError)
{
	_impl->listen();
}


/*bool TcpIp4Socket::waitClient(unsigned int timeout) throw(IO::IOError)
{
	return _impl->wait(timeout);
}*/


/*bool TcpIp4Socket::waitData(unsigned int timeout, unsigned int timeout_usec)
{
	return _impl->waitData(timeout, timeout_usec);
}*/


bool TcpIp4Socket::wait(WaitMode mode, unsigned int msec) throw(IO::IOError)
{
	return _impl->wait(mode, msec);
}


void TcpIp4Socket::setTimeOut(int msec) throw(IO::IOError)
{
	_impl->setTimeOut(msec);
}


void TcpIp4Socket::_close() throw(IO::IOError)
{
	_impl->close();
}


bool TcpIp4Socket::_wait(WaitMode mode, unsigned int msec) throw(IO::IOError)
{
	return _impl->wait(mode, msec);
}


size_t TcpIp4Socket::_read(char* buffer, size_t count) throw(IO::IOError)
{
	return _impl->read(buffer, count);
}


size_t TcpIp4Socket::_peek(char* buffer, size_t count) throw(IO::IOError)
{
	return 0;
}


size_t TcpIp4Socket::_write(const char* buffer, size_t count) throw(IO::IOError)
{
	return _impl->write(buffer, count);
}






TcpIp4ServerSocket::TcpIp4ServerSocket() throw(IO::IOError)
: Ip4ServerSocket(),
  TcpServerSocket()
{
	_impl = new ::TcpIp4SocketImpl();
}


TcpIp4ServerSocket::~TcpIp4ServerSocket() throw()
{
	_impl->close();
	delete _impl;
}


void TcpIp4ServerSocket::bind(const Ip4Address& addr, port_t port) throw(IO::IOError)
{
	_impl->bind(addr, port);
}


void TcpIp4ServerSocket::listen() throw(IO::IOError)
{
	_impl->listen();
}


bool TcpIp4ServerSocket::wait(unsigned int timeout) throw(IO::IOError)
{
	return _impl->wait(Socket::WaitInput, timeout);
}
