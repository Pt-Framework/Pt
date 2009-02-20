/*
 * Copyright (C) 2009 Marc Boris Duerner, Tommi Maekitalo
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * As a special exception, you may use this file as part of a free
 * software library without restriction. Specifically, if other files
 * instantiate templates or use macros or inline functions from this
 * file, or you compile this file and link it with other files to
 * produce an executable, this file does not by itself cause the
 * resulting executable to be covered by the GNU General Public
 * License. This exception does not however invalidate any other
 * reasons why the executable file might be covered by the GNU Library
 * General Public License.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */
#include "TcpSocketImpl.h"
#include "AddrInfo.h"
#include "TcpServerImpl.h"
#include <Pt/System/SystemError.h>
#include <Pt/Net/TcpServer.h>
#include <Pt/Net/TcpSocket.h>
//#include <Mswsock.h>

#define log_debug(x)

namespace Pt {

namespace Net {

TcpSocketImpl::TcpSocketImpl(TcpSocket& socket)
: _socket(socket)
, _fd(INVALID_SOCKET)
, _waitEvent(WSACreateEvent())
{
	memset(&_receiveOverlapped, 0, sizeof(WSAOVERLAPPED));
	memset(&_sendOverlapped, 0, sizeof(WSAOVERLAPPED));

	_currentEventHandle		  = _waitEvent;
	_sendOverlapped.hEvent	  = _currentEventHandle;
	_receiveOverlapped.hEvent = _currentEventHandle;
}

TcpSocketImpl::~TcpSocketImpl()
{
    WSACloseEvent(_waitEvent);
    _waitEvent = INVALID_HANDLE_VALUE;
	close();
}

void TcpSocketImpl::create(int domain, int type, int protocol)
{
    log_debug("create socket");

	_fd = WSASocket(domain, type, protocol, NULL, 0, WSA_FLAG_OVERLAPPED);

    if (_fd == INVALID_SOCKET)
    {
        log_debug("Error at socket(): "<< WSAGetLastError());
        throw System::SystemError(PT_ERROR_MSG("creating socket failed"));
    }

	_sendOverlapped.hEvent    = _currentEventHandle;
	_receiveOverlapped.hEvent = _currentEventHandle;
}

void TcpSocketImpl::close()
{
    if( _fd == INVALID_SOCKET )
        return;

    ::closesocket(_fd);
    _fd = INVALID_SOCKET;
}

void TcpSocketImpl::connect(const std::string& ipaddr, unsigned short int port)
{
    bool isConnected = this->beginConnect(ipaddr, port);

    if( !isConnected )
    {
        this->wait(_timeout);
        this->endConnect();
    }
}

bool TcpSocketImpl::beginConnect(const std::string& ipaddr, unsigned short int port)
{	
    AddrInfo ai(ipaddr, port);

    for (AddrInfo::const_iterator it = ai.begin(); it != ai.end(); ++it)
    {
        try
        {
			this->create(it->ai_family, SOCK_STREAM, 0);
			attachEvent(_currentEventHandle, FD_CONNECT);
        }
        catch (const System::SystemError&)
        {
          continue;
        }

		std::memmove(&_peeraddr, it->ai_addr, it->ai_addrlen);		

		if( ::connect(_fd, it->ai_addr, (int)it->ai_addrlen) == SOCKET_ERROR)
		{
			if( WSAGetLastError() == WSAEWOULDBLOCK)
			{
				std::memmove(&_peeraddr, it->ai_addr, it->ai_addrlen);
				return false;
			}
			else
			{
				close();
				throw System::SystemError("connect failed");
			}
		}

		// _isConnected = true;
		std::memmove(&_peeraddr, it->ai_addr, it->ai_addrlen);
        return true;
    }

	close();
    throw System::SystemError("connect failed");
	return false;
}

void TcpSocketImpl::endConnect()
{
    // FD_CONNECT abschalten

	//if( ! _isConnected )
    {
		// auf FD_CONNECT warten aussed checkEvent hat das schon getan
	
		int sockerr;
		socklen_t optlen = sizeof(sockerr);
		if( ::getsockopt(_fd, SOL_SOCKET, SO_ERROR, (char*)&sockerr, &optlen) != 0 )
		{
			close();
			throw System::SystemError("getsockopt");
		}

		if(sockerr != 0)
		{
		    close();
			throw System::SystemError("connect");
		}
		
		//_isConnected = true;
	}
}

void TcpSocketImpl::detach(System::SelectorBase& sb)
{
	if( _currentEventHandle != _waitEvent)
		attachEvent(_currentEventHandle,0);

	_currentEventHandle = _waitEvent;
	attachEvent();
}

void TcpSocketImpl::attachEvent()
{
	if( _socket.rbuf() == 0 && _socket.wbuf() == 0)
		attachEvent(_currentEventHandle, FD_CONNECT);
	else if(_socket.rbuf() != 0 && _socket.wbuf() == 0)
		attachEvent(_currentEventHandle, FD_READ);
	else if(_socket.rbuf() == 0 && _socket.wbuf() != 0)
		attachEvent(_currentEventHandle, FD_WRITE);	
}

void TcpSocketImpl::attachEvent(HANDLE ev, long events)
{
    if (WSAEventSelect(_fd, ev, events) == SOCKET_ERROR)
    {
        log_debug("set event failed: "<< WSAGetLastError());
        throw System::SystemError( PT_ERROR_MSG("attach event to socket failed") );
    }
}

void TcpSocketImpl::accept(TcpServer& server)
{
	_fd =  WSAAccept(server.impl().fd(), NULL, NULL, NULL, 0);

	if( _fd == INVALID_SOCKET)
	      throw System::SystemError("accept");	
}

bool TcpSocketImpl::wait(std::size_t umsecs)
{
    log_debug("wait " << msecs);

    int msecs = umsecs;
	if(umsecs == Pt::System::SelectorBase::WaitInfinite) 
    {
        msecs = INFINITE;
    }
    else if( umsecs > static_cast<size_t>(std::numeric_limits<int>::max()))
    {
        msecs = std::numeric_limits<int>::max();
    }

	if(WSAWaitForMultipleEvents(1, &_currentEventHandle, FALSE, msecs, FALSE) != WSA_WAIT_TIMEOUT)
	{
		WSAResetEvent(_currentEventHandle);
		checkEvent();
		return true;
	}

    return false;
}

bool TcpSocketImpl::setWaitHandle(HANDLE h, bool& avail)
{
    log_debug("setWaitHandle");	
	
	if( _fd != INVALID_SOCKET)
	{
		_currentEventHandle = h;

		attachEvent();
	
		_sendOverlapped.hEvent = _currentEventHandle;
		_receiveOverlapped.hEvent = _currentEventHandle;

		avail = checkEvent();
	}
	else
	{
		avail = false;
	}

    return true;
}

void TcpSocketImpl::getWaitHandles(System::HandleMap& handles, bool& avail)
{
    log_debug("getWaitHandles");
}

std::string TcpSocketImpl::getSockAddr() const
{
	SOCKADDR sockadr;
	int len = sizeof(sockadr);
	int ret = getsockname(_fd, &sockadr, &len);

	if( ret == SOCKET_ERROR)
		throw System::SystemError( PT_ERROR_MSG("getSockAddr failed") );

	const sockaddr_in* sa = reinterpret_cast<const sockaddr_in*>(&sockadr);
    return inet_ntoa(sa->sin_addr);
}

std::string TcpSocketImpl::getPeerAddr() const
{
	const sockaddr_in* sa = reinterpret_cast<const sockaddr_in*>(&_peeraddr);
    return inet_ntoa(sa->sin_addr);
}

size_t TcpSocketImpl::beginRead(char* buffer, size_t n, bool& eof)
{
	attachEvent(_currentEventHandle, FD_READ);
	return 0;
}

size_t TcpSocketImpl::read(char* buffer, size_t count, bool& eof)
{
	return 0;
}

size_t TcpSocketImpl::endRead(bool& eof)
{
	DWORD bytes = 0;
	DWORD flags = 0;

	WSABUF receiveBuffer;
		
	receiveBuffer.buf = _socket.rbuf();
    receiveBuffer.len = _socket.rbuflen();

	if( receiveBuffer.buf == 0)
		 throw System::SystemError( PT_ERROR_MSG("endRead failed") );

	if(WSARecv(_fd, &receiveBuffer, 1, &bytes, &flags, &_receiveOverlapped, NULL) == SOCKET_ERROR)
		throw System::SystemError( PT_ERROR_MSG("endRead failed") );

	bytes = checkReceiveResult(eof);

	if(bytes > 0)
	{
		WSAResetEvent(_receiveOverlapped.hEvent);
		return bytes;
	}

	if(!this->wait(_timeout))
	{
		eof = true;
		return 0;
	}
	
	bytes = checkReceiveResult(eof);

	if(bytes == 0)
		eof = true;

	WSAResetEvent(_receiveOverlapped.hEvent);

	return bytes;
}

size_t TcpSocketImpl::checkReceiveResult(bool& eof)
{
	DWORD bytes = 0;
	DWORD flags = 0;

	BOOL rc = WSAGetOverlappedResult(_fd, &_receiveOverlapped, &bytes, FALSE, &flags);

	if (rc == FALSE) 
	{
		int err = WSAGetLastError();
		
		if( err == WSAECONNRESET)
			eof = true;
		else
			throw System::SystemError( PT_ERROR_MSG("endRead failed") );
	}

	return bytes;
}

size_t TcpSocketImpl::beginWrite(const char* buffer, size_t n)
{
	_sendBuffer.buf = const_cast<char*>(buffer);
	_sendBuffer.len = n;
	DWORD numberOfBytesSent = 0;

	int rc = WSASend( _fd, &_sendBuffer, 1, &numberOfBytesSent, 0, &_sendOverlapped, NULL);

	if ((rc == SOCKET_ERROR) && (WSA_IO_PENDING != WSAGetLastError())) 
	{
		throw System::SystemError( PT_ERROR_MSG("WSASend failed") );
	}

	return numberOfBytesSent;
}

size_t TcpSocketImpl::endWrite()
{
	size_t bytes = checkSendResult();
	if( bytes > 0)
	{
		WSAResetEvent(_sendOverlapped.hEvent);
		return bytes;
	}

	this->wait(_timeout);
	WSAResetEvent(_sendOverlapped.hEvent);

	return checkSendResult();
}

size_t TcpSocketImpl::checkSendResult()
{
	DWORD sendBytes;
	DWORD flags;
    BOOL rc = WSAGetOverlappedResult(_fd, &_sendOverlapped, &sendBytes, FALSE, &flags);
    
	if (rc == FALSE) 
		throw System::SystemError( PT_ERROR_MSG("endWrite failed") );

	return sendBytes;
}

size_t TcpSocketImpl::write(const char* buffer, size_t count)
{
	return 0;
}

bool TcpSocketImpl::checkEvent()
{
    log_debug("checkEvent");

    WSANETWORKEVENTS events;

    if(WSAEnumNetworkEvents(_fd,_currentEventHandle, &events) == SOCKET_ERROR)
        throw System::SystemError("ask network events failed");

	bool ev = false;

    if((events.lNetworkEvents & FD_CONNECT) == FD_CONNECT)        
	{
		ev = true;
		_socket.connected.send(_socket);
	}

	if((events.lNetworkEvents & FD_READ) == FD_READ)
	{
        ev = true;
		_socket.inputReady.send(_socket);	
	}

	if((events.lNetworkEvents & FD_WRITE) == FD_WRITE)
	{
       ev = true;
	   _socket.outputReady.send(_socket);	
    }

    return ev;
}

} // namespace Net
} // namespace Pt
