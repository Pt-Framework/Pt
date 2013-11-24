/*
 * Copyright (C) 2006-2009 by Marc Boris Duerner
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
#include "EndpointImpl.h"
#include <sstream>

using namespace Platform;

namespace Pt {

namespace Net {

EndpointImpl::EndpointImpl()
{
}


EndpointImpl::EndpointImpl(const std::string& ipaddr, unsigned short port)
: _host(ipaddr)
{
    std::stringstream ss;
    ss << port;
    ss >> _service;
}


EndpointImpl::EndpointImpl(const EndpointImpl& ep)
: _host(ep._host)
, _service(ep._service)
{
}


EndpointImpl::~EndpointImpl()
{
}


void EndpointImpl::clear()
{  
    _host.clear();
    _service.clear();
}


void EndpointImpl::init(Platform::String^ host, Platform::String^ service)
{
    std::wstring waddr;
    std::wstring wport;

    if(host)
		waddr = host->Data();

    if(service)
        wport = service->Data();

    _host.assign( waddr.begin(), waddr.end() );
    _service.assign( wport.begin(), wport.end() );
}


EndpointImpl& EndpointImpl::operator=(const EndpointImpl& ainfo)
{
    _host = ainfo._host;
    _service = ainfo._service;
    return *this;
}


std::string EndpointImpl::toString() const
{ 
    std::string str;

    str += _host;
    str += ':';
    str += _service;
    
    return str; 
}


EndpointImpl* EndpointImpl::ip4Any(unsigned short port)
{
	EndpointImpl* impl = new EndpointImpl();

    // Stream-/Datagramockets must use BindServiceNameAsync if hostname is empty
    impl->_host.clear();

    std::stringstream ss;
    ss << port;
    ss >> impl->_service;
    
    return impl;
}


EndpointImpl* EndpointImpl::ip4Loopback(unsigned short port)
{
	EndpointImpl* impl = new EndpointImpl();
    
    impl->_host = "127.0.0.1";
    
    std::stringstream ss;
    ss << port;
    ss >> impl->_service;
    
    return impl;
}

EndpointImpl* EndpointImpl::ip4Broadcast(unsigned short port)
{
	EndpointImpl* impl = new EndpointImpl();

    impl->_host = "255.255.255.255";
    
    std::stringstream ss;
    ss << port;
    ss >> impl->_service;
    
    return impl;
}


EndpointImpl* EndpointImpl::ip6Any(unsigned short port)
{
	EndpointImpl* impl = new EndpointImpl();
    
    // Stream-/Datagramockets must use BindServiceNameAsync if hostname is empty
    impl->_host.clear();

    std::stringstream ss;
    ss << port;
    ss >> impl->_service;
    
    return impl;
}


EndpointImpl* EndpointImpl::ip6Loopback(unsigned short port)
{
	EndpointImpl* impl = new EndpointImpl();

    impl->_host = "0:0:0:0:0:0:0:1";
    
    std::stringstream ss;
    ss << port;
    ss >> impl->_service;
    
    return impl;
}

} // namespace Net

} // namespace Pt
