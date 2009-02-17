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
#include <Mswsock.h>

#define log_debug(x)

namespace Pt {

namespace Net {

TcpSocketImpl::TcpSocketImpl(TcpSocket& socket)
: _socket(socket)
, _fd(INVALID_SOCKET)
, _waitEvent(WSACreateEvent())
{
	memset(&_connectOverlapped, 0, sizeof(WSAOVERLAPPED));
	memset(&_receiveOverlapped, 0, sizeof(WSAOVERLAPPED));
	memset(&_sendOverlapped, 0, sizeof(WSAOVERLAPPED));

	_currentEventHandle = _waitEvent;
	_sendOverlapped.hEvent	  = _currentEventHandle;
	_receiveOverlapped.hEvent = _currentEventHandle;
	_sendOverlapped.hEvent	  = _currentEventHandle;
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

	attachEvent(_waitEvent, FD_CONNECT|FD_WRITE|FD_READ);
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

    if( ! isConnected )
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

			_sendOverlapped.hEvent    = _currentEventHandle;
			_receiveOverlapped.hEvent = _currentEventHandle;
			_connectOverlapped.hEvent = _currentEventHandle;

			attachEvent(_currentEventHandle, FD_CONNECT|FD_READ|FD_WRITE);
        }
        catch (const System::SystemError&)
        {
          continue;
        }

  	    DWORD dwBytesSent = 0;

		// Load ConnectEx
		GUID GuidConnectEx = WSAID_CONNECTEX;
		LPFN_CONNECTEX lpfnConnectEx = NULL;
		DWORD dwBytes = 0;

		if( WSAIoctl(_fd, SIO_GET_EXTENSION_FUNCTION_POINTER, &GuidConnectEx, sizeof(GuidConnectEx), &lpfnConnectEx, sizeof(lpfnConnectEx), &dwBytes, NULL, NULL ) == SOCKET_ERROR )
		{
			close();
			throw System::SystemError("connect failed");
		}

		// Connect to server.
		if( lpfnConnectEx(_fd, it->ai_addr, (int)it->ai_addrlen, NULL, 0, &dwBytesSent, &_connectOverlapped) == FALSE)
		{
			if( WSAGetLastError() != ERROR_IO_PENDING)
			{
				close();
				throw System::SystemError("connect failed");
			}
		}		

/* NOTE: das klapt 
		if( ::connect(_fd, it->ai_addr, (int)it->ai_addrlen) == SOCKET_ERROR)
		{
			if( WSAGetLastError() != WSAEWOULDBLOCK)
			{
				close();
				throw System::SystemError("connect failed");
			}
		}
*/		
		 std::memmove(&_addr, it->ai_addr, it->ai_addrlen);
         return true;
    }

	close();
    throw System::SystemError("bind");
	return false;
}

void TcpSocketImpl::endConnect()
{
	DWORD bytes;
	DWORD flags;
    BOOL rc = WSAGetOverlappedResult(_fd, &_connectOverlapped, &bytes, FALSE, &flags);
    
	if (rc == FALSE) 
		throw System::SystemError( PT_ERROR_MSG("endWrite failed") );

	WSAResetEvent(_connectOverlapped.hEvent);
}

void TcpSocketImpl::detach(System::SelectorBase& sb)
{
	if( _currentEventHandle != _waitEvent)
		attachEvent(_currentEventHandle,0);

	_currentEventHandle = _waitEvent;
	attachEvent(_currentEventHandle, FD_CONNECT|FD_READ|FD_WRITE);	
}

void TcpSocketImpl::attachEvent(HANDLE ev, long events)
{
    if (WSAEventSelect(_fd, ev, events) == SOCKET_ERROR)
    {
        log_debug("Set event failed: "<< WSAGetLastError());
        throw System::SystemError( PT_ERROR_MSG("attach event to socket failed") );
    }
}

void TcpSocketImpl::accept(TcpServer& server)
{
	_fd =  WSAAccept(server.impl().fd(), NULL,NULL,NULL,0);
//	_fd = ::accept(server.impl().fd(), NULL,NULL);

	if( _fd == INVALID_SOCKET)
	      throw System::SystemError("accept");

	attachEvent(_currentEventHandle, FD_CONNECT|FD_READ|FD_WRITE);	
}

bool TcpSocketImpl::wait(std::size_t umsecs)
{
    log_debug("wait " << msecs);

    int msecs = umsecs;
	if(umsecs == Pt::System::SelectorBase::WaitInfinite) 
    {
        msecs = INFINITE;
    }
    else if( umsecs > std::numeric_limits<int>::max() )
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

		attachEvent(_currentEventHandle, FD_CONNECT|FD_READ|FD_WRITE);
		
		_sendOverlapped.hEvent = _currentEventHandle;
		_receiveOverlapped.hEvent = _currentEventHandle;
		_connectOverlapped.hEvent = _currentEventHandle;

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
	return _addr.sa_data;
}

size_t TcpSocketImpl::beginRead(char* buffer, size_t n, bool& eof)
{
	DWORD numberOfBytesRecvd = 0;
	DWORD flags = 0;

	_receiveBuffer.buf = buffer;
	_receiveBuffer.len = n;

	int rc = WSARecv(_fd, &_receiveBuffer, 1, &numberOfBytesRecvd, &flags, &_receiveOverlapped, NULL);
	
	if (rc == SOCKET_ERROR && (WSA_IO_PENDING == WSAGetLastError()))
		return numberOfBytesRecvd;

	if( rc == SOCKET_ERROR)
		throw System::SystemError( PT_ERROR_MSG("WSARecv failed") );

	return numberOfBytesRecvd;
}

size_t TcpSocketImpl::read(char* buffer, size_t count, bool& eof)
{
	int numberOfBytesRecvd = recv(_fd, buffer, count, 0);
	
	if( numberOfBytesRecvd == SOCKET_ERROR )
		throw System::SystemError( PT_ERROR_MSG("recv failed") );

	return static_cast<size_t>(numberOfBytesRecvd);
}

size_t TcpSocketImpl::endRead(bool& eof)
{
	DWORD bytes;
	DWORD flags;
    BOOL rc = WSAGetOverlappedResult(_fd, &_receiveOverlapped, &bytes, FALSE, &flags);
    
	if (rc == FALSE) 
		throw System::SystemError( PT_ERROR_MSG("endWrite failed") );

	WSAResetEvent(_receiveOverlapped.hEvent);
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
	DWORD sendBytes;
	DWORD flags;
    BOOL rc = WSAGetOverlappedResult(_fd, &_sendOverlapped, &sendBytes, FALSE, &flags);
    
	if (rc == FALSE) 
		throw System::SystemError( PT_ERROR_MSG("endWrite failed") );

	WSAResetEvent(_sendOverlapped.hEvent);
	return sendBytes;
}

size_t TcpSocketImpl::write(const char* buffer, size_t count)
{
	int numberOfBytesSend = send( _fd, buffer, static_cast<int>(count), 0);
	
	if( numberOfBytesSend == SOCKET_ERROR )
		throw System::SystemError( PT_ERROR_MSG("recv failed") );

	return static_cast<size_t>(numberOfBytesSend);
}

bool TcpSocketImpl::checkEvent()
{
    log_debug("checkEvent");

    WSANETWORKEVENTS events;

    if(WSAEnumNetworkEvents(_fd, 0, &events) == SOCKET_ERROR)
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
