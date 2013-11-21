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
#ifndef Pt_Net_AddrInfo_H
#define Pt_Net_AddrInfo_H

#include <Pt/Net/Api.h>
#include <Pt/RefCounted.h>
#include <string>
#include <cassert>

namespace Pt {

namespace Net {

class EndpointImpl
{
    public:
        EndpointImpl();

        EndpointImpl(const std::string& ipaddr, unsigned short port);

        EndpointImpl(const EndpointImpl& ipaddr);

        ~EndpointImpl();

        EndpointImpl& operator=(const EndpointImpl& ipaddr);

        void init(Platform::String^ host, Platform::String^ service);

        void clear();

        std::string toString() const;

        // TODO: return Windows::Networking::HostName^
        const std::string& host() const
        { return _host; }

        // TODO: return Platform::String^
        const std::string& service() const
        { return _service; }

        static EndpointImpl* ip4Any(unsigned short port);

        static EndpointImpl* ip4Loopback(unsigned short port);

        static EndpointImpl* ip4Broadcast(unsigned short port);

        static EndpointImpl* ip6Any(unsigned short port);

        static EndpointImpl* ip6Loopback(unsigned short port);

    private:
        std::string _host; // TODO: Windows::Networking::HostName^
        std::string _service; // TODO: Platform::String^
};

} // namespace Net

} // nameace Pt

#endif
