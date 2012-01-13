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
#include "MainLoopImpl.h"
#include <Pt/System/SystemError.h>
#include <Pt/Net/TcpServer.h>
#include <Pt/Net/TcpSocket.h>
#include <sstream>
#include <cstring>
#include <cassert>

#define log_debug(x)

//#include <iostream>
//#define log_debug(x) std::cerr << x << std::endl;

namespace Pt {

namespace Net {

TcpSocketImpl::TcpSocketImpl(TcpSocket& socket)
: _connectResult(0)
, _sentry(0)
, _socket(socket)
, _fd(INVALID_SOCKET)
, _isConnected(false)
, _eventFlags(FD_CLOSE)
, _timeout(INFINITE) // Pt::System::EventLoop::WaitInfinite ?
{
    _currentEventHandle = INVALID_HANDLE_VALUE;
}


TcpSocketImpl::~TcpSocketImpl()
{
    if(_sentry)
        _sentry->detach();
}


void TcpSocketImpl::attachEvent(HANDLE ev, long events)
{
    if (WSAEventSelect(_fd, ev, events) == SOCKET_ERROR)
    {
        log_debug("set event failed: "<< WSAGetLastError());
        throw System::SystemError( PT_ERROR_MSG("attach event to socket failed") );
    }
}


void TcpSocketImpl::attach(System::EventLoop& loop)
{
    /*log_debug("attach to loop");
    HANDLE h = loop.impl().enable(_socket);
    _currentEventHandle = h;*/
}


void TcpSocketImpl::detach(System::EventLoop& loop)
{
    /*log_debug("detach from loop");

    loop.impl().disable(_socket);
    _currentEventHandle = INVALID_HANDLE_VALUE;*/
}


void TcpSocketImpl::cancel(System::EventLoop& loop)
{
    if(_currentEventHandle != INVALID_HANDLE_VALUE)
    {
        loop.impl().disableOverlapped(_socket);
        _currentEventHandle = INVALID_HANDLE_VALUE;
    }

    _eventFlags = FD_CLOSE;
    if( _fd != INVALID_SOCKET )
    {
        this->attachEvent(0, 0);
    }
}


void TcpSocketImpl::close()
{
    if( _fd == INVALID_SOCKET )
        return;

    /*if(_currentEventHandle != INVALID_HANDLE_VALUE)
        attachEvent(0, 0);

    if(loop)
        this->detach(*loop);*/

    ::closesocket(_fd);
    _fd = INVALID_SOCKET;
    _isConnected = false;
}


void TcpSocketImpl::accept(const TcpServer& server, unsigned flags)
{
    _fd = server.impl().accept();
    _isConnected = true;
    log_debug("accepted " << _fd);
}


void TcpSocketImpl::connect(const AddrInfo& addrinfo)
{
    throw std::logic_error("TcpSocketImpl::connect not implemented");
    //this->beginConnect(addrinfo);
    //this->endConnect();
}


const char* TcpSocketImpl::tryConnect()
{

    if (_addrInfoPtr == _addrInfo.impl()->end())
    {
        log_debug("no more address informations");
        return "invalid address information";
    }

    while (true)
    {
        while (true)
        {
            log_debug("create socket");
            _fd = WSASocket(_addrInfoPtr->ai_family, SOCK_STREAM, 0, NULL, 0, 0);

             if (_fd != INVALID_SOCKET)
                break;

             if (++_addrInfoPtr == _addrInfo.impl()->end())
                return "socket";
        }

        std::memmove(&_peeraddr, _addrInfoPtr->ai_addr, _addrInfoPtr->ai_addrlen);

        log_debug("created socket " << _fd);

        if( ::connect(_fd, _addrInfoPtr->ai_addr, _addrInfoPtr->ai_addrlen) == 0 )
        {
            _isConnected = true;
            log_debug("immediate connect ");
            break;
        }

        DWORD lastError = WSAGetLastError();
        if( lastError == WSAEWOULDBLOCK || lastError == WSAEINPROGRESS )
        {
            _eventFlags |= FD_CONNECT;
            log_debug("connect in progress");
            break;
        }

        log_debug("connect failed");
        close();

        if (++_addrInfoPtr == _addrInfo.impl()->end())
            return "connect";
    }

    return 0;
}


bool TcpSocketImpl::beginConnect(const AddrInfo& ai, System::EventLoop& loop)
{
    assert( ! _isConnected );
    log_debug("begin connect");

    if(_currentEventHandle == INVALID_HANDLE_VALUE)
    {
        HANDLE h = loop.impl().enableOverlapped(_socket);
        _currentEventHandle = h;
    }
    
    _addrInfo = ai;
    _addrInfoPtr = _addrInfo.impl()->begin();
    _connectResult = tryConnect();
    checkPendingError();

    if(_isConnected)
    {
        log_debug("connected " << _fd);
    }

    if( ! _isConnected)
        attachEvent(_currentEventHandle, _eventFlags);

    return _isConnected;
}


void TcpSocketImpl::checkPendingError()
{
    log_debug(_fd << " checkPendingError");

    if(_connectResult)
    {
        log_debug(_fd << "error msg: " << _connectResult);
        const char* p = _connectResult;
        _connectResult = 0;
        throw System::IOError(p);
    }
}


int TcpSocketImpl::checkConnect()
{
    int sockerr;

    socklen_t optlen = sizeof(sockerr);

    if( ::getsockopt(_fd, SOL_SOCKET, SO_ERROR, (char*)&sockerr, &optlen) != 0 )
    {
        //WSAINPROGRESS??
        close();
        throw System::SystemError("getsockopt");
    }
    else if(sockerr == 0)
    {
        _isConnected = true;
    }

    return sockerr;
}


void TcpSocketImpl::endConnect(System::EventLoop& loop)
{
    log_debug(_fd << " endConnect");

    _eventFlags &= ~FD_CONNECT;

    if(_currentEventHandle != INVALID_HANDLE_VALUE)
        attachEvent(_currentEventHandle, _eventFlags);

    checkPendingError();

    if( _isConnected )
        return;

    _eventFlags |= FD_CONNECT;
    attachEvent(_currentEventHandle, _eventFlags);

    log_debug(_fd << " force endConnect");
    try
    {
        while (true)
        {
            bool avail = this->wait(_timeout);

            if (avail)
            {
                // something has happened
                checkConnect();
                if (_isConnected)
                {
                    _eventFlags &= ~FD_CONNECT;
                    attachEvent(_currentEventHandle, _eventFlags);
                    return;
                }

                if (++_addrInfoPtr == _addrInfo.impl()->end())
                {
                    // no more addrInfo - propagate error
                    throw System::IOError("connect failed");
                }
            }
            else if (++_addrInfoPtr == _addrInfo.impl()->end())
            {
                // nothing has happened in time
                throw System::IOTimeout();
            }

            close();

            _connectResult = tryConnect();
            if (_isConnected)
            {
                _eventFlags &= ~FD_CONNECT;
                attachEvent(_currentEventHandle, _eventFlags);
                return;
            }
            checkPendingError();
        }
    }
    catch(...)
    {
        close();
        throw;
    }
}


bool TcpSocketImpl::runRead(System::EventLoop& loop)
{
    WSANETWORKEVENTS events;

    if( WSAEnumNetworkEvents(_fd, NULL, &events) == SOCKET_ERROR )
        throw System::SystemError("WSAEnumNetworkEvents failed");

    if( (events.lNetworkEvents & FD_CLOSE) == FD_CLOSE )
    {
       _isConnected = false;
       return true;
    }

    if( (events.lNetworkEvents & FD_READ) == FD_READ )
    {
        return true;
    }

    return false;
}

bool TcpSocketImpl::runWrite(System::EventLoop& loop)
{
    WSANETWORKEVENTS events;

    if( WSAEnumNetworkEvents(_fd, NULL, &events) == SOCKET_ERROR )
        throw System::SystemError("WSAEnumNetworkEvents failed");

    if( (events.lNetworkEvents & FD_CLOSE) == FD_CLOSE )
    {
       _isConnected = false;
       return true;
    }

    if( (events.lNetworkEvents & FD_WRITE) == FD_WRITE )
    {
       return true;
    }

    return false;
}


bool TcpSocketImpl::runConnect(System::EventLoop& loop)
{
    WSANETWORKEVENTS events;

    if( WSAEnumNetworkEvents(_fd, NULL, &events) == SOCKET_ERROR )
        throw System::SystemError("WSAEnumNetworkEvents failed");

    if( (events.lNetworkEvents & FD_CLOSE) == FD_CLOSE )
    {
       _isConnected = false;
       return true;
    }

    if((events.lNetworkEvents & FD_CONNECT) == FD_CONNECT && ! _isConnected)
    {
        //ResetEvent(_currentEventHandle);
        int s = FD_CONNECT_BIT;
        if(events.iErrorCode[s] != 0)
        {
            if (++_addrInfoPtr == _addrInfo.impl()->end())
            {
                // no more addrInfo - propagate error
                _connectResult = "connect failed";
                return true;
            }

            _connectResult = tryConnect();
            if (_isConnected)
            {
                return true;
            }
        }
        else
        {
            _isConnected = true;
            return true;
        }
    }

    return false;
}


/*bool TcpSocketImpl::run(System::EventLoop& loop)
{
    log_debug(_fd << " avail");

    DestructionSentry sentry(_sentry);

    WSANETWORKEVENTS events;

    if(WSAEnumNetworkEvents(_fd, NULL, &events) == SOCKET_ERROR)
        throw System::SystemError("WSAEnumNetworkEvents failed");

    bool ev = false;

    if((events.lNetworkEvents & FD_CONNECT) == FD_CONNECT && !_isConnected)
    {
        //ResetEvent(_currentEventHandle);
        int s = FD_CONNECT_BIT;
        if(events.iErrorCode[s] != 0)
        {
            if (++_addrInfoPtr == _addrInfo.impl()->end())
            {
                // no more addrInfo - propagate error
                _connectResult = "connect failed";
                _socket.connected.send(_socket);
                return true;
            }

            _connectResult = tryConnect(&loop);
            if (_isConnected)
            {
                _socket.connected.send(_socket);
                return true;
            }
        }
        else
        {
            ev = true;
            _isConnected = true;
            _socket.connected.send(_socket);

            if( ! _sentry )
               return ev;
        }
    }

    if(_socket.wavail() || ((events.lNetworkEvents & FD_WRITE) == FD_WRITE))
    {
       //ResetEvent(_currentEventHandle);
       ev = true;
       _socket.outputReady().send(_socket);

       if( ! _sentry )
           return ev;
    }

    if(_socket.ravail() || ((events.lNetworkEvents & FD_READ) == FD_READ))
    {
        //ResetEvent(_currentEventHandle);
        ev = true;
        _socket.inputReady().send(_socket);

        if( ! _sentry )
           return ev;
    }

    if((events.lNetworkEvents & FD_CLOSE) == FD_CLOSE)
    {
        //ResetEvent(_currentEventHandle);
         ev = true;
       _isConnected = false;

       if( ! _sentry )
           return ev;
    }

    return ev;
}*/


bool TcpSocketImpl::wait(std::size_t umsecs)
{
    log_debug(_fd << " wait " << umsecs);

    int msecs = umsecs;
    if(umsecs == Pt::System::EventLoop::WaitInfinite)
    {
        msecs = INFINITE;
    }
    else if( umsecs > static_cast<size_t>(std::numeric_limits<int>::max()))
    {
        msecs = std::numeric_limits<int>::max();
    }

    if( WSA_WAIT_TIMEOUT != WSAWaitForMultipleEvents(1, &_currentEventHandle, FALSE, msecs, FALSE) )
    {      
        return true;
    }

    return false;
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


// std::string TcpSocketImpl::getSockAddr() const
// {
//     SOCKADDR sockadr;
//     int l = sizeof(sockadr);
//     int ret = getsockname(_fd, &sockadr, &l);

//     SOCKADDR* saddr = const_cast<SOCKADDR*>(&sockadr);
//     DWORD len = 32;
//     TCHAR adr[32];
//     WSAAddressToString(saddr, sizeof(SOCKADDR), NULL, adr, &len);

//     std::string address(32, '\0');
//     for(unsigned n = 0; n < len; n++)
//     {
//         address.push_back( int(adr[n]) );
//     }

//     return adr;
// }


std::string TcpSocketImpl::getPeerAddr() const
{
	const sockaddr_in* sa = reinterpret_cast<const sockaddr_in*>(&_peeraddr);

	//char adr[15]; //TODO: Windows CE wchar_t
	//WSAAddressToString(sa, sizeof(sa), NULL, adr, 15);

    return inet_ntoa(sa->sin_addr);
	//return adr;
}


size_t TcpSocketImpl::beginRead(System::EventLoop& loop, char* buffer, size_t n, bool& eof)
{
    log_debug(_fd << " beginRead");

    if(_currentEventHandle == INVALID_HANDLE_VALUE)
    {
        HANDLE h = loop.impl().enableOverlapped(_socket);
        _currentEventHandle = h;
    }

    assert(buffer != 0);
    _eventFlags |= FD_READ;

    _receiveBuffer.buf = buffer;
    _receiveBuffer.len = n;

    // u_long argp = 0;
    // ::ioctlsocket(_fd, FIONREAD, &argp);
    // std::cerr << _fd << " FIONREAD: " << argp << std::endl;
    // if( argp == 2878)
    // {
    //     DWORD wr = WaitForSingleObject(_currentEventHandle, 0);
    //     std::cerr << _fd << " WAIT:  " << (wr == WAIT_OBJECT_0) << std::endl;
    // }

    attachEvent(_currentEventHandle, _eventFlags);

    // if( argp == 2878)
    // {
    //     DWORD wr = WaitForSingleObject(_currentEventHandle, 0);
    //     std::cerr << _fd << " WAIT2: " << (wr == WAIT_OBJECT_0) << std::endl;

    //     if(wr == WAIT_OBJECT_0)
    //     {
    //         std::cerr << _fd << " RESET" << std::endl;
    //         SetEvent(_currentEventHandle);
    //     }
    // }
    return 0;
}


size_t TcpSocketImpl::read(char* buffer, size_t count, bool& eof)
{
    return 0;
}


size_t TcpSocketImpl::endRead(System::EventLoop& loop, bool& eof)
{
    log_debug(_fd << " endRead");
    _eventFlags &= ~FD_READ;

    int len = ::recv(_fd, _receiveBuffer.buf, _receiveBuffer.len, 0);

    if( len == 0)
    {
        eof = true;
    }
    else if(len == -1)
    {
        int err = WSAGetLastError();
        if(err == WSAEWOULDBLOCK)
        {
            //Set socket to blocking mode
            attachEvent(0,0);
        
            u_long argp = 0;
            ::ioctlsocket(_fd, FIONBIO, &argp);
        
            len = ::recv(_fd, _receiveBuffer.buf, _receiveBuffer.len, 0);
        
            if( len == 0)
                eof = true;
        
            //Set socket to non-blocking mode
            argp = 1;
            ::ioctlsocket(_fd, FIONBIO, &argp);
        }
        else if(err == WSAECONNRESET)
        {
            eof = true;
        }
    }

    attachEvent(_currentEventHandle, _eventFlags);

    return len;
}


size_t TcpSocketImpl::beginWrite(System::EventLoop& loop, const char* buffer, size_t n)
{
    log_debug(_fd << " beginWrite");

    if(_currentEventHandle == INVALID_HANDLE_VALUE)
    {
        HANDLE h = loop.impl().enableOverlapped(_socket);
        _currentEventHandle = h;
    }

    _sendBuffer.buf = const_cast<char*>(buffer);
    _sendBuffer.len = n;

    DWORD numberOfBytesSent = 0;

    int rc = WSASend(_fd, &_sendBuffer, 1, &numberOfBytesSent, 0, NULL, NULL);

    if(rc == SOCKET_ERROR)
    {
        if(WSAGetLastError() == WSAEWOULDBLOCK)
        {
            _eventFlags |= FD_WRITE;
            attachEvent(_currentEventHandle, _eventFlags);
            return 0;
        }
    }

    log_debug(_fd << " beginWrite sent " << numberOfBytesSent << " of " << n << " bytes");

    //SetEvent(_currentEventHandle);
    return numberOfBytesSent;
}


size_t TcpSocketImpl::endWrite(System::EventLoop& loop)
{
    log_debug(_fd << " wndWrite");

    _eventFlags &= ~FD_WRITE;

    //Set socket to blocking mode
    attachEvent(0, 0);

    u_long argp = 0;
    ::ioctlsocket(_fd, FIONBIO, &argp);

    DWORD numberOfBytesSent = 0;

    int rc = WSASend(_fd, &_sendBuffer, 1, &numberOfBytesSent, 0, NULL, NULL);

    if(rc == SOCKET_ERROR)
        throw System::SystemError( PT_ERROR_MSG("beginWrite failed") );

    //Set socket to non-blocking mode
    argp = 1;
    ::ioctlsocket(_fd, FIONBIO, &argp);
    attachEvent(_currentEventHandle, _eventFlags);

    return  numberOfBytesSent;
}


size_t TcpSocketImpl::write(const char* buffer, size_t count)
{
    return 0;
}


/*bool TcpSocketImpl::setWaitHandle(HANDLE h, bool& avail)
{
    log_debug(_fd << " setWaitHandle");
    avail = _dataSends != 0;

    if( _currentEventHandle == h)
        return true;

    _currentEventHandle = h;

    if(_waitEvent != _currentEventHandle)
    {
        log_debug(_fd << " wait handle is selector");
    }

    attachEvent(_currentEventHandle, _eventFlags);

    return true;
}*/

/*bool TcpSocketImpl::checkEvent()
{
    log_debug(_fd << " checkEvent");

    DestructionSentry sentry(_sentry);

    if(_dataSends != 0 )
    {
       _socket.outputReady.send(_socket);
       return true;
    }

    WSANETWORKEVENTS events;

    if(WSAEnumNetworkEvents(_fd, NULL, &events) == SOCKET_ERROR)
        throw System::SystemError("WSAEnumNetworkEvents failed");

    bool ev = false;

    if((events.lNetworkEvents & FD_CONNECT) == FD_CONNECT && !_isConnected)
    {
        //ResetEvent(_currentEventHandle);
        int s = FD_CONNECT_BIT;
        if(events.iErrorCode[s] != 0)
        {
            if (++_addrInfoPtr == _addrInfo.impl()->end())
            {
                // no more addrInfo - propagate error
                _connectResult = "connect failed";
                _socket.connected.send(_socket);
                return true;
            }

            _connectResult = tryConnect();
            if (_isConnected)
            {
                _socket.connected.send(_socket);
                return true;
            }
        }
        else
        {
            ev = true;
            _isConnected = true;
            _socket.connected.send(_socket);

            if( ! _sentry )
               return ev;
        }
    }

    if((events.lNetworkEvents & FD_WRITE) == FD_WRITE)
    {
       //ResetEvent(_currentEventHandle);
       ev = true;
       _socket.outputReady.send(_socket);

       if( ! _sentry )
           return ev;
    }

    if((events.lNetworkEvents & FD_READ) == FD_READ)
    {
        //ResetEvent(_currentEventHandle);
        ev = true;
        _socket.inputReady.send(_socket);

        if( ! _sentry )
           return ev;
    }

    if((events.lNetworkEvents & FD_CLOSE) == FD_CLOSE)
    {
        //ResetEvent(_currentEventHandle);
         ev = true;
       _isConnected = false;

       if( ! _sentry )
           return ev;
    }

    return ev;
}*/

} // namespace Net

} // namespace Pt
