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

#include "Pt/Net/StreamSocket.h"
#include "Pt/Net/StreamServerSocket.h"
#include "StreamSocketImpl.h"
#include "StreamServerSocketImpl.h"
#include "Pt/Exception.h"


namespace Pt {

namespace Net {

StreamSocket::~StreamSocket()
{
  delete _impl;
}

void StreamSocket::setTimeout(ssize_t msec)
{
    Socket::setTimeout(msec);

    if (!_impl)
        _impl = new StreamSocketImpl();

    _impl->setTimeout(msec);
}


void StreamSocket::connect(const std::string& ipaddr, unsigned short int port)
{
    if (!_impl)
        _impl = new StreamSocketImpl();

    _impl->connect(ipaddr, port);
}


void StreamSocket::accept(const StreamServerSocket& server)
{
    if (_impl)
    {
        delete _impl;
        _impl = 0;
    }

    _impl = server._impl->accept();
}


size_t StreamSocket::_read(char* buffer, size_t count, bool& eof)
{
    if (!_impl)
        throw LogicError("socket is not connected", PT_SOURCEINFO);

    return _impl->read(buffer, count, eof);
}


size_t StreamSocket::_write(const char* buffer, size_t count)
{
    if (!_impl)
        throw LogicError("socket is not connected", PT_SOURCEINFO);

    return _impl->write(buffer, count);
}

void StreamSocket::_close()
{
  if (_impl)
    _impl->close();
  delete _impl;
  _impl = 0;
}

} // namespace Net

} // namespace Pt
