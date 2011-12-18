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

#include "Pt/Net/AddrInfo.h"
#include "TcpSocketImpl.h"
#include "TcpServerImpl.h"
#include "MainLoopImpl.h"
#include "Pt/Net/TcpServer.h"
#include "Pt/Net/TcpSocket.h"
#include "Pt/System/SystemError.h"
#include "Pt/System/IOError.h"
#include <cerrno>
#include <cstring>
#include <cassert>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define log_debug(x) std::cerr << x << std::endl;
#define log_trace(x) std::cerr << x << std::endl;

namespace
{

    void formatIp(const sockaddr_storage& addr, std::string& str)
    {
#ifdef HAVE_INET_NTOP
        const sockaddr_in* sa = reinterpret_cast<const sockaddr_in*>(&addr);
        char strbuf[INET6_ADDRSTRLEN + 1];
        const char* p = inet_ntop(sa->sin_family, &sa->sin_addr, strbuf, sizeof(strbuf));
        str = (p == 0 ? "-" : strbuf);
#else
        static Pt::System::Mutex monitor;
        Pt::System::MutexLock lock(monitor);

        const sockaddr_in* sa = reinterpret_cast<const sockaddr_in*>(&addr);
        const char* p = inet_ntoa(sa->sin_addr);
        if (p)
            str = p;
        else
            str.clear();
#endif
    }
}

namespace Pt {

namespace Net {

TcpSocketImpl::TcpSocketImpl(TcpSocket& socket)
: System::IODeviceImpl(socket)
, _socket(socket)
, _isConnected(false)
{
}


TcpSocketImpl::~TcpSocketImpl()
{
    //assert(_rfds == 0);
    //assert(_wfds == 0);
}


void TcpSocketImpl::close()
{
    log_debug("close socket " << _fd);
    System::IODeviceImpl::close();
    _isConnected = false;
}


// TODO enable sockets after an open call
void TcpSocketImpl::enable(System::EventLoop& loop)
{
    IODeviceImpl::enable(loop);

    if( ! _isConnected )
    {
        std::cerr << "IODeviceImpl::beginConnect on handle " << std::endl;
        loop.impl().beginWrite( _iohandle );
    }
}


std::string TcpSocketImpl::getSockAddr() const
{
    struct sockaddr_storage addr;

    socklen_t slen = sizeof(addr);
    if (::getsockname(fd(), reinterpret_cast<struct sockaddr*>(&addr), &slen) < 0)
        throw System::SystemError("getsockname");

    std::string ret;
    formatIp(addr, ret);
    return ret;
}


std::string TcpSocketImpl::getPeerAddr() const
{
    std::string ret;
    formatIp(_peeraddr, ret);
    return ret;
}


void TcpSocketImpl::connect(const AddrInfo& addrInfo)
{
    log_debug("connect");
    this->beginConnect(addrInfo);
    this->endConnect();
}


int TcpSocketImpl::checkConnect()
{
    log_trace("checkConnect");

    int sockerr;
    socklen_t optlen = sizeof(sockerr);

    // check for socket error
    if( ::getsockopt(this->fd(), SOL_SOCKET, SO_ERROR, &sockerr, &optlen) != 0 )
    {
        // getsockopt failed
        close();
        throw System::SystemError("getsockopt");
    }

    if (sockerr == 0)
    {
        log_debug("connected successfully");
        _isConnected = true;
    }

    return sockerr;
}

void TcpSocketImpl::checkPendingError()
{
    if (_connectResult)
    {
        const char* p = _connectResult;
        _connectResult = 0;
        throw System::IOError(p);
    }
}


const char* TcpSocketImpl::tryConnect()
{
    log_trace("tryConnect");

    assert(_fd == -1);

    if (_addrInfoPtr == _addrInfo.impl()->end())
    {
        log_debug("no more address informations");
        return "invalid address information";
    }

    while (true)
    {
        int fd;
        while (true)
        {
            log_debug("create socket");
            fd = ::socket(_addrInfoPtr->ai_family, SOCK_STREAM, 0);
            if (fd >= 0)
                break;

            if (++_addrInfoPtr == _addrInfo.impl()->end())
                return "socket";
        }

        IODeviceImpl::open(fd, true, false);

        std::memmove(&_peeraddr, _addrInfoPtr->ai_addr, _addrInfoPtr->ai_addrlen);

        log_debug("created socket " << _fd << " max: " << FD_SETSIZE);

        if( ::connect(this->fd(), _addrInfoPtr->ai_addr, _addrInfoPtr->ai_addrlen) == 0 )
        {
            _isConnected = true;
            log_debug("connected successfully");
            break;
        }

        if (errno == EINPROGRESS)
        {
            log_debug("connect in progress");
            break;
        }

        close();
        if (++_addrInfoPtr == _addrInfo.impl()->end())
            return "connect";
    }

    return 0;
}


bool TcpSocketImpl::beginConnect(const AddrInfo& addrInfo)
{
    log_trace("begin connect");
    std::cerr << "########### BEGIN CONNECT" << std::endl;

    assert(!_isConnected);

    _addrInfo = addrInfo;
    _addrInfoPtr = _addrInfo.impl()->begin();
    _connectResult = tryConnect();
    checkPendingError();
    std::cerr << "###########" << _isConnected << std::endl;
    return _isConnected;
}


void TcpSocketImpl::endConnect()
{
    log_trace("ending connect");

    System::EventLoop* loop = _device.parent();
    if( loop && _iohandle && ! _socket.wbuf() )
    {
        loop->impl().endWrite( _iohandle );
    }

//  if(_wfds && ! _socket.wbuf() )
//  {
//      FD_CLR( this->fd(), _wfds );
//  }

    checkPendingError();

    if( _isConnected )
        return;

    try
    {
        while (true)
        {
            fd_set wfds;
            FD_ZERO(&wfds);
            FD_SET(this->fd(), &wfds);

            bool avail = this->wait(timeout(), 0, &wfds, 0);

            if (avail)
            {
                // something has happened
                checkConnect();
                if (_isConnected)
                    return;

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
                return;
            checkPendingError();
        }
    }
    catch(...)
    {
        close();
        throw;
    }
}


void TcpSocketImpl::accept(const TcpServer& server, unsigned flags)
{
    bool inherit = (flags & TcpSocket::INHERIT) != 0;
    socklen_t peeraddr_len = sizeof(_peeraddr);

    log_debug( "accept " << server.impl().fd() );
    _fd = ::accept(server.impl().fd(), reinterpret_cast <struct sockaddr*>(&_peeraddr), &peeraddr_len);
    if( _fd < 0 )
      throw System::SystemError("accept");

    System::IODeviceImpl::open(_fd, true, inherit);
    //TODO ECONNABORTED EINTR EPERM

    _isConnected = true;
    log_debug( "accepted " << server.impl().fd() << " => " << _fd );
}


void TcpSocketImpl::initWait(fd_set& rfds, fd_set& wfds, fd_set& efds)
{
    System::IODeviceImpl::initWait(rfds, wfds, efds);

    if( this->fd() > 0 )
    {
        if( ! _isConnected )
        {
            FD_SET(this->fd(), &wfds);
            FD_SET(this->fd(), &efds);
        }
    }
}


bool TcpSocketImpl::avail()
{
    std::cerr << "########### AVAIL" << std::endl;

    if( _isConnected )
        return System::IODeviceImpl::avail();

    System::EventLoopImpl& impl = _device.parent()->impl();

    if( impl.isError(_iohandle) )
    {
        AddrInfoImpl::const_iterator ptr = _addrInfoPtr;
        if (++ptr == _addrInfo.impl()->end())
        {
            // not really connected but error
            // end of addrinfo list means that no working addrinfo was found
            _socket.connected.send(_socket);
            std::cerr << "########### CONNECT FAIL" << std::endl;
            return true;
        }
        else
        {
            _addrInfoPtr = ptr;

            close();
            _connectResult = tryConnect();

            if (_isConnected || _connectResult)
                // immediate success or error
                _socket.connected.send(_socket);
            else
                // by closing the previous file handle _pfd is set to 0.
                // creating a new socket in tryConnect may also change the value of fd.
                // TODO: handle this differently, without loosing the socket fd...
                throw std::logic_error("posix tcp socket impl reconnect not implemented");

            return _isConnected;
        }
    }
    else if( impl.isWritable(_iohandle)  )
    {
        std::cerr << "########### WRITABLE" << std::endl;
        int sockerr = checkConnect();
        if (_isConnected)
        {
            _socket.connected.send(_socket);
            return true;
        }

        // something went wrong - look for next addrInfo
        log_debug("sockerr is " << sockerr << " try next");
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

    return false;
}


/*int TcpSocketImpl::checkEvent(fd_set& rfds, fd_set& wfds, fd_set& efds)
{
    log_debug("TcpSocketImpl::checkEvent");

    if( _isConnected )
        return System::IODeviceImpl::checkEvent(rfds, wfds, efds);

    if (FD_ISSET(this->fd(), &efds) )
    {
        AddrInfoImpl::const_iterator ptr = _addrInfoPtr;
        if (++ptr == _addrInfo.impl()->end())
        {
            // not really connected but error
            // end of addrinfo list means that no working addrinfo was found
            _socket.connected.send(_socket);
            return true;
        }
        else
        {
            _addrInfoPtr = ptr;

            close();
            _connectResult = tryConnect();

            if (_isConnected || _connectResult)
                // immediate success or error
                _socket.connected.send(_socket);
            else
                // by closing the previous file handle _pfd is set to 0.
                // creating a new socket in tryConnect may also change the value of fd.
                initSelect(rfds, wfds, efds);

            return _isConnected;
        }
    }
    else if (FD_ISSET(this->fd(), &wfds) )
    {
        int sockerr = checkConnect();
        if (_isConnected)
        {
            _socket.connected.send(_socket);
            return 1;
        }

        // something went wrong - look for next addrInfo
        log_debug("sockerr is " << sockerr << " try next");
        if (++_addrInfoPtr == _addrInfo.impl()->end())
        {
            // no more addrInfo - propagate error
            _connectResult = "connect failed";
            _socket.connected.send(_socket);
            return 1;
        }

        _connectResult = tryConnect();
        if (_isConnected)
        {
            _socket.connected.send(_socket);
            return 1;
        }
    }

    return 0;
}*/

} // namespace Net

} // namespace Pt
