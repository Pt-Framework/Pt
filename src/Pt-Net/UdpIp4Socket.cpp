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
#include "UdpIp4Socket.posix.cpp"

#include "Pt/Net/UdpIp4Socket.h"
using namespace Pt::Net;

#include <iostream>
using namespace std;


UdpIp4Sender::UdpIp4Sender(const Ip4Address& addr, port_t port)
{
	_impl = new ::UdpIp4SocketImpl();
	IO::IODevice::setValid(true);
	IO::IODevice::setEof(false);

	this->connect(addr, port);
	Ip4Socket::_peerAddress = addr;
}


UdpIp4Sender::~UdpIp4Sender() throw()
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


void UdpIp4Sender::connect(const Ip4Address& addr, port_t port) throw(IO::IOError)
{
	_impl->connect(addr, port);
}


bool UdpIp4Sender::wait(WaitMode mode, unsigned int msec) throw(IO::IOError)
{
	return _impl->wait(mode, msec);
}


void UdpIp4Sender::setTimeOut(int msec) throw(IO::IOError)
{
	_impl->setTimeOut(msec);
}


size_t UdpIp4Sender::_write(const char* buffer, size_t count) throw(IO::IOError)
{
	return _impl->write(buffer, count);
}




UdpIp4Receiver::UdpIp4Receiver(const Ip4Address& addr, port_t port)
{
	_impl = new ::UdpIp4SocketImpl();
	IO::IODevice::setValid(true);
	IO::IODevice::setEof(false);

	this->bind(addr, port);
	Ip4ServerSocket::_peerAddress = addr;
}


UdpIp4Receiver::~UdpIp4Receiver() throw()
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


void UdpIp4Receiver::bind(const Ip4Address& addr, port_t port) throw(IO::IOError)
{
	_impl->bind(addr, port);
}


size_t UdpIp4Receiver::_read(char* buffer, size_t count) throw(IO::IOError)
{
	return _impl->read(buffer, count);
}



