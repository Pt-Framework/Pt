/*
 * Copyright (C) 2006 by Marc Boris Duerner, Tommi Maekitalo
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

#include "Pt/Net/TcpSocket.h"
#include "Pt/Net/TcpServerSocket.h"
#include "TcpSocketImpl.h"
#include "TcpServerSocketImpl.h"
#include "Pt/SourceInfo.h"
#include <stdexcept>


namespace Pt {

namespace Net {

TcpSocket::~TcpSocket()
{
  delete _impl;
}

void TcpSocket::setTimeout(ssize_t msec)
{
    Socket::setTimeout(msec);

    if (!_impl)
        _impl = new TcpSocketImpl();

    _impl->setTimeout(msec);
}


void TcpSocket::connect(const std::string& ipaddr, unsigned short int port)
{
    if (!_impl)
        _impl = new TcpSocketImpl();

    _impl->connect(ipaddr, port);
}


void TcpSocket::accept(const TcpServerSocket& server)
{
    if (_impl)
    {
        delete _impl;
        _impl = 0;
    }

    _impl = server._impl->accept();
}


unsigned long TcpSocket::availableBytes(void)
{
    if (!_impl)
        _impl = new TcpSocketImpl();

    return _impl->availableBytes();
}


size_t TcpSocket::onRead(char* buffer, size_t count, bool& eof)
{
    if (!_impl)
        throw std::logic_error("socket is not connected" + PT_SOURCEINFO);

    return _impl->read(buffer, count, eof);
}


size_t TcpSocket::onWrite(const char* buffer, size_t count)
{
    if (!_impl)
        throw std::logic_error("socket is not connected" + PT_SOURCEINFO);

    return _impl->write(buffer, count);
}


bool TcpSocket::_wait(Socket::WaitMode mode, unsigned int msec)
{
    if (!_impl)
        throw std::logic_error("socket is not connected" + PT_SOURCEINFO);

    return _impl->wait(mode, msec);
}


void TcpSocket::onClose()
{
  if (_impl)
    _impl->close();
  delete _impl;
  _impl = 0;
}

} // namespace Net

} // namespace Pt
