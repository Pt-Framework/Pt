/*
 * Copyright (C) 2014 Marc Duerner
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

#include "AdapterInfo.h"
#include <Pt/System/SystemError.h>
#include <cstring>

namespace Pt {

namespace Net {

#ifdef PT_NET_WITH_GETIFADDRS

AdapterInfo::Iterator::Iterator(ifaddrs* ifa)
: _current(ifa)
{ }


AdapterInfo::Iterator& AdapterInfo::Iterator::operator++ ()
{ 
    _current = _current->ifa_next; 
    return *this; 
}


const char* AdapterInfo::Iterator::operator*() const
{ 
    return _current->ifa_name; 
}


int AdapterInfo::Iterator::index()
{
    return if_nametoindex(_current->ifa_name);
}


sockaddr* AdapterInfo::Iterator::addr()
{
    return _current->ifa_addr;
}


AdapterInfo::AdapterInfo(int)
{
    getifaddrs(&_adapters);
}


AdapterInfo::~AdapterInfo()
{
    freeifaddrs(_adapters);
}


#elif defined(PT_NET_WITH_IFCONF)


AdapterInfo::Iterator::Iterator(int sock, ifconf* ifc, ifreq* ifr)
: _sock(sock)
, _ifc(ifc)
, _offset(0)
, _current(ifr)
{ }


AdapterInfo::Iterator& AdapterInfo::Iterator::operator++()
{ 
#ifdef __linux__
    size_t len = sizeof(ifreq);
#else
    size_t len = _current->ifr_addr.sa_len;
    len += sizeof(_current->ifr_name);
    len = std::max(sizeof(ifreq), len);
#endif

    _offset += len;
                    
    if(_offset < _ifc->ifc_len)
        _current = (struct ifreq*)((char*)_current + len);
    else
        _current = 0;
                    
    return *this; 
}


const char* AdapterInfo::Iterator::operator*() const
{ 
    return _current->ifr_name; 
}


int AdapterInfo::Iterator::index()
{
    return if_nametoindex(_current->ifr_name);
}


sockaddr* AdapterInfo::Iterator::addr()
{
    std::strncpy(_ifr.ifr_name, _current->ifr_name, IFNAMSIZ);
                    
    ioctl( _sock, SIOCGIFADDR, &_ifr);
    return &_ifr.ifr_addr;
}


AdapterInfo::AdapterInfo(int sock)
: _sock(sock)
{
    std::memset(_ifr, 0, sizeof(_ifr));

    _ifc.ifc_len = sizeof(_ifr);
    _ifc.ifc_req = _ifr;

    if( ioctl(_sock, SIOCGIFCONF, &_ifc) == -1)
    {
        throw System::SystemError("ioctl SIOCGIFCONF failed");
    }
}


AdapterInfo::~AdapterInfo()
{
}


#else // default uses if_nameindex


AdapterInfo::Iterator::Iterator(struct if_nameindex* ifaces, int sock)
: _sock(sock)
, _current(ifaces)
{ }


AdapterInfo::Iterator& AdapterInfo::Iterator::operator++ ()
{ 
    _current++;
    if( ! _current->if_name )
        _current = 0;

    return *this; 
}


const char* AdapterInfo::Iterator::operator*() const
{ 
    return _current->if_name; 
}


int AdapterInfo::Iterator::index()
{
    return if_nametoindex(_current->if_name);
}


sockaddr* AdapterInfo::Iterator::addr()
{
    std::memset(&_ifr, 0, sizeof(_ifr));
    std::strncpy(_ifr.ifr_name, _current->if_name, IFNAMSIZ);
    ioctl(_sock, SIOCGIFADDR, &_ifr);
    return &_ifr.ifr_addr;
}


AdapterInfo::AdapterInfo(int sock)
: _sock(sock)
{
    _ifaces = if_nameindex();
}


AdapterInfo::~AdapterInfo()
{
    if_freenameindex(_ifaces);
}

#endif

} // namespace Net

} // namespace Pt
