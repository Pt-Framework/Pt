/*
 * Copyright (C) 2013 Marc Duerner
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

#include "EndpointImpl.h"
#include <Pt/Net/Endpoint.h>

namespace Pt {

namespace Net {

Endpoint::Endpoint()
: _impl(0)
{ 
    _impl = new EndpointImpl();
}


Endpoint::Endpoint(EndpointImpl* impl)
: _impl(impl)
{
}


Endpoint::Endpoint(const std::string& host, unsigned short port)
: _impl(0)
{
    _impl = new EndpointImpl(host, port);
}


Endpoint::Endpoint(const char* host, unsigned short port)
: _impl(0)
{
    _impl = new EndpointImpl(host, port);
}


Endpoint::Endpoint(const Endpoint& src)
: _impl(0)
{
    _impl = new EndpointImpl( *src._impl );
}


Endpoint::~Endpoint()
{
    delete _impl;
}


Endpoint& Endpoint::operator= (const Endpoint& src)
{
    *_impl = *src._impl;
    return *this;
}

void Endpoint::clear()
{
    return _impl->clear();
}


std::string Endpoint::toString() const
{
    return _impl->toString();
}


Endpoint Endpoint::ip4Any(unsigned short port)
{
    return Endpoint( EndpointImpl::ip4Any(port) );
}


Endpoint Endpoint::ip4Loopback(unsigned short port)
{
    return Endpoint( EndpointImpl::ip4Loopback(port) );
}


Endpoint Endpoint::ip4Broadcast(unsigned short port)
{
    return Endpoint( EndpointImpl::ip4Broadcast(port) );
}


Endpoint Endpoint::ip6Any(unsigned short port)
{
    return Endpoint( EndpointImpl::ip6Any(port) );
}


Endpoint Endpoint::ip6Loopback(unsigned short port)
{
    return Endpoint( EndpointImpl::ip6Loopback(port) );
}

} // namespace Net

} // namespace Pt
