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
#include "AddrInfoImpl.h"

namespace Pt {

namespace Net {

AddrInfoImpl::AddrInfoImpl()
: _listen(false)
{
}


AddrInfoImpl::AddrInfoImpl(const std::string& ipaddr, unsigned short port, bool listen)
: _listen(listen)
{
    init(ipaddr, port);
}


AddrInfoImpl::~AddrInfoImpl()
{
    clear();
}


AddrInfoImpl* AddrInfoImpl::ip4Any(unsigned short port)
{
    AddrInfoImpl* impl = new AddrInfoImpl();
    impl->initIp4Any(port);
    return impl;
}


AddrInfoImpl* AddrInfoImpl::ip4Loopback(unsigned short port)
{
    AddrInfoImpl* impl = new AddrInfoImpl();
    impl->initIp4Loopback(port);
    return impl;
}

AddrInfoImpl* AddrInfoImpl::ip4Broadcast(unsigned short port)
{
    AddrInfoImpl* impl = new AddrInfoImpl();
    impl->initIp4Broadcast(port);
    return impl;
}


AddrInfoImpl* AddrInfoImpl::ip6Any(unsigned short port)
{
    AddrInfoImpl* impl = new AddrInfoImpl();
    impl->initIp6Any(port);
    return impl;
}


AddrInfoImpl* AddrInfoImpl::ip6Loopback(unsigned short port)
{
    AddrInfoImpl* impl = new AddrInfoImpl();
    impl->initIp6Loopback(port);
    return impl;
}


void AddrInfoImpl::clear()
{  
    _host.clear();
    _port = 0;
}


void AddrInfoImpl::init(const std::string& ipaddr, unsigned short port)
{
    clear();
    
    _host = ipaddr;
    _port = port;
}


void AddrInfoImpl::initIp4Any(unsigned short port)
{  
    clear();

    // Stream-/Datagramockets must use BindServiceNameAsync if hostname is empty
    _host = "";
    _port = port;
}


void AddrInfoImpl::initIp4Loopback(unsigned short port)
{  
    clear();

    _host = "127.0.0.1";
    _port = port;
}


void AddrInfoImpl::initIp4Broadcast(unsigned short port)
{  
    clear();

    _host = "255.255.255.255";
    _port = port;
}


void AddrInfoImpl::initIp6Any(unsigned short port)
{  
    clear();

    // Stream-/Datagramockets must use BindServiceNameAsync if hostname is empty
    _host = "";
    _port = port;
}


void AddrInfoImpl::initIp6Loopback(unsigned short port)
{  
    clear();

    _host = "0:0:0:0:0:0:0:1";
    _port = port;
}

}

}
