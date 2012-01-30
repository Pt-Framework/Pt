/*
 * Copyright (C) 2005,2009 Tommi Maekitalo
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
 * y
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include <Pt/Net/AddrInfo.h>
#include <string.h>
#include "AddrInfoImpl.h"
#include <sstream>

namespace Pt {

namespace Net {

namespace
{
  std::string AddressInUseMsg(const std::string& ipaddr, unsigned short int port)
  {
    std::ostringstream msg;
    msg << "address " << ipaddr << ':' << port << " in use";
    return msg.str();
  }
}


AddressInUse::AddressInUse()
: IOError("address in use")
{ }


AddressInUse::AddressInUse(const std::string& ipaddr, unsigned short int port)
    : IOError(AddressInUseMsg(ipaddr, port))
{
}


AddrInfo::AddrInfo(AddrInfoImpl* impl)
: _impl(impl)
{
    _impl->addRef();
}


AddrInfo::AddrInfo(const std::string& host, unsigned short port, bool passive)
: _impl(0)
{
    /*struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_socktype = SOCK_STREAM;
    if (listen)
        hints.ai_flags |= AI_PASSIVE;

    _impl = new AddrInfoImpl(host, port, hints);
    _impl->addRef();*/

    _impl = new AddrInfoImpl(host, port, passive);
    _impl->addRef();
}


AddrInfo::AddrInfo(const AddrInfo& src)
: _impl(src._impl)
{
    _impl->addRef();
}

AddrInfo::~AddrInfo()
{
    if (_impl)
        _impl->release();
}


AddrInfo& AddrInfo::operator= (const AddrInfo& src)
{
    if (src._impl != _impl)
    {
        if (_impl)
            _impl->release();

        _impl = src._impl;

        if (_impl)
            _impl->addRef();
    }

    return *this;
}


const std::string& AddrInfo::host() const
{
    return _impl->host();
}


unsigned short AddrInfo::port() const
{
    return _impl->port();
}

}

}

