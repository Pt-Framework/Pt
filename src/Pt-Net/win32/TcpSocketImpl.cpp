/*
 * Copyright (C) 2009 Marc Boris Duerner, Tommi Maekitalo, 
 *                    Laurentiu-Gheorghe Crisan
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
#include <Pt/Net/AddrInfo.h>
#include "AddrInfoImpl.h"
#include "TcpServerImpl.h"
#include <Pt/System/SystemError.h>
#include <Pt/Net/TcpServer.h>
#include <Pt/Net/TcpSocket.h>
#include <sstream>
#define log_debug(x)

namespace Pt {

namespace Net {

TcpSocketImpl::TcpSocketImpl(TcpSocket& socket)
: _socket(socket)
, _fd(INVALID_SOCKET)
, _waitEvent(WSACreateEvent())
, _isConnected(false)
, _eventFlags(FD_CLOSE)
, _timeout(100)
, _events(0)
{
	_currentEventHandle = _waitEvent;
}

TcpSocketImpl::~TcpSocketImpl()
{
	WSAResetEvent(_waitEvent);

	if( _fd != INVALID_SOCKET)
		WSAEventSelect(_fd,_currentEventHandle,0);

	close();

    WSACloseEvent(_waitEvent);
    _waitEvent = INVALID_HANDLE_VALUE;
}

void TcpSocketImpl::create(int domain, int type, int protocol)
{
    log_debug("create socket");

	_fd = WSASocket(domain, type, protocol, NULL, 0, 0);

    if (_fd == INVALID_SOCKET)
    {
        log_debug("Error at socket(): "<< WSAGetLastError());
        throw System::SystemError(PT_ERROR_MSG("creating socket failed"));
    }

	//Close the socket on close no delay time
    struct linger ling;
    ling.l_onoff = 0;
    ling.l_linger = 0;

    ::setsockopt(_fd, SOL_SOCKET, SO_DONTLINGER, (char*)&ling, sizeof(ling));
}

void TcpSocketImpl::cancel()
{
    //TODO: canceling
}

void TcpSocketImpl::close()
{
    if( _fd == INVALID_SOCKET )
        return;

    ::closesocket(_fd);
    _fd = INVALID_SOCKET;
    _isConnected = false;
}

void TcpSocketImpl::connect(const AddrInfo& addrinfo)
{
	this->beginConnect(addrinfo);
    this->endConnect();
}

bool TcpSocketImpl::beginConnect(const AddrInfo& ai)
{	
	_isConnected = false;
	
    for (AddrInfoImpl::const_iterator it = ai.impl()->begin(); it != ai.impl()->end(); ++it)
    {
        try
        {
			this->create(it->ai_family, SOCK_STREAM, 0);
			_eventFlags |= FD_CONNECT;
			attachEvent(_currentEventHandle, _eventFlags);
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
				throw System::IOError("connect failed");
			}
		}

		std::memmove(&_peeraddr, it->ai_addr, it->ai_addrlen);
		_isConnected = true;
        return true;
    }

	close();
    throw System::SystemError("connect failed");
	return false;
}

void TcpSocketImpl::endConnect()
{
	_eventFlags &= ~FD_CONNECT;
	attachEvent(_currentEventHandle, _eventFlags);

	try
    {
		int sockerr;
		socklen_t optlen = sizeof(sockerr);
		if( ::getsockopt(_fd, SOL_SOCKET, SO_ERROR, (char*)&sockerr, &optlen) != 0 )
		{
			DWORD error = WSAGetLastError();
			
			if( error != WSAEINPROGRESS)
				throw System::SystemError("getsockopt");
		}
		else if(sockerr != 0)
		{
			throw System::IOError("connect");
		}	

    }
    catch(...)
    {
        close();
        throw;
	}


	if( _isConnected )
		return;


	HANDLE ev = WSACreateEvent();

	attachEvent(ev, FD_CONNECT);

	if(WaitForSingleObject( ev, 1000 ) == WAIT_TIMEOUT)
	{	
		close();
		throw System::IOTimeout();
	}

	WSACloseEvent(ev);

	_isConnected = true;

}

void TcpSocketImpl::detach(System::SelectorBase& sb)
{
}

void TcpSocketImpl::attach(System::SelectorBase& sb)
{

}

void TcpSocketImpl::attachEvent(HANDLE ev, long events)
{
    if (WSAEventSelect(_fd, ev, events) == SOCKET_ERROR)
    {
        log_debug("set event failed: "<< WSAGetLastError());
        throw System::SystemError( PT_ERROR_MSG("attach event to socket failed") );
    }
}

void TcpSocketImpl::accept(const TcpServer& server, bool closeOnExec)
{
    _fd = ::WSAAccept(server.impl().fd(), NULL, NULL, NULL, 0);
   
	if( _fd == SOCKET_ERROR)
    {
        log_debug("accept failed: "<< WSAGetLastError());
        throw System::SystemError( PT_ERROR_MSG("accept failed") );
    }
		
	//Close the socket on close no delay time
    struct linger ling;
    ling.l_onoff = 0;
    ling.l_linger = 0;

    ::setsockopt(_fd, SOL_SOCKET, SO_DONTLINGER, (char*)&ling, sizeof(ling));

    _isConnected = true;
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
		checkEvent();
		return true;
	}

    return false;
}

bool TcpSocketImpl::setWaitHandle(HANDLE h, bool& avail)
{
	avail = false;
	
	if( _currentEventHandle == h)
		return true;   

    _currentEventHandle = h;

    attachEvent(_currentEventHandle, _eventFlags);

	return true;    
}

void TcpSocketImpl::getWaitHandles(System::HandleMap& handles, bool& avail)
{
    //handles.add(_currentEventHandle, &_socket); 
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
	_eventFlags |= FD_READ;

	_receiveBuffer.buf = buffer;
    _receiveBuffer.len = n;

	if( _receiveBuffer.buf == 0)
		 throw System::SystemError( PT_ERROR_MSG("beginRead failed") );

	attachEvent(_currentEventHandle, _eventFlags);

	return 0;
}

size_t TcpSocketImpl::read(char* buffer, size_t count, bool& eof)
{
	return 0;
}

size_t TcpSocketImpl::endRead(bool& eof)
{
    _eventFlags &= ~FD_READ;
	attachEvent(_currentEventHandle, _eventFlags);

    int len =  ::recv(_fd, _receiveBuffer.buf, _receiveBuffer.len, 0);		

    if( len == -1)
    {
        int error = WSAGetLastError();

        switch(error)
        {
			case WSAEWOULDBLOCK:
			{
				HANDLE ev = WSACreateEvent();

				attachEvent(ev, FD_READ);

				if(WaitForSingleObject(ev,1000) == WAIT_TIMEOUT)
				{
					attachEvent(_currentEventHandle, _eventFlags);
					WSACloseEvent(ev);
					eof = true;
					return 0;
				}

				len =  ::recv(_fd, _receiveBuffer.buf, _receiveBuffer.len, 0);		    
				attachEvent(_currentEventHandle, _eventFlags);
				WSACloseEvent(ev);
			}
			break;
			case WSAECONNRESET:
				eof = true;
			break;
        }
		
    }

	if( len == 0)
		eof = true;

    return len;
}

size_t TcpSocketImpl::beginWrite(const char* buffer, size_t n)
{
	DWORD numberOfBytesSent = 0;
    DWORD flags = 0;

	_sendBuffer.buf = const_cast<char*>(buffer);
    _sendBuffer.len = n;   

    _eventFlags |= FD_WRITE;
    attachEvent(_currentEventHandle, _eventFlags);	

    numberOfBytesSent = ::send(_fd, _sendBuffer.buf, _sendBuffer.len, 0);	

    if(numberOfBytesSent == 0)
	    throw System::SystemError( PT_ERROR_MSG("beginWrite failed") );

    if( numberOfBytesSent == n )
    {
        _events = FD_WRITE;
        SetEvent(_currentEventHandle);        
    }

	return 0;
}

size_t TcpSocketImpl::endWrite()
{
	_eventFlags &= ~FD_WRITE;	
	attachEvent(_currentEventHandle, _eventFlags);
	return _sendBuffer.len;
}

size_t TcpSocketImpl::write(const char* buffer, size_t count)
{
	return 0;
}

bool TcpSocketImpl::checkEvent()
{
    log_debug("checkEvent");
    
    if( _events == 0)
    {
        WSANETWORKEVENTS events;
        if(WSAEnumNetworkEvents(_fd,_currentEventHandle, &events) == SOCKET_ERROR)
            throw System::SystemError("ask network events failed");
        
        _events = events.lNetworkEvents;
    }
	else
	{
		ResetEvent(_currentEventHandle);
	}

	bool ev = false;

    if((_events & FD_CONNECT) == FD_CONNECT)        
	{
		ev = true;		
		_isConnected = true;
		_socket.connected.send(_socket);
	}

	if((_events & FD_READ) == FD_READ)
	{
        ev = true;
		_socket.inputReady.send(_socket);	
	}

	if((_events & FD_WRITE) == FD_WRITE)
	{
       ev = true;
	   _socket.outputReady.send(_socket);	       
    }

	if((_events & FD_CLOSE) == FD_CLOSE)
	{
       _isConnected = false;
    }

    _events = 0;

    return ev;
}

} // namespace Net
} // namespace Pt
