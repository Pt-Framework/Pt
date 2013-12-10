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

#ifndef PT_NET_ENDPOINT_H
#define PT_NET_ENDPOINT_H

#include <Pt/Net/Api.h>
#include <string>

namespace Pt {

namespace Net {

class EndpointImpl;

/** @brief Represents a Network Host
    
    Endpoints are constructed from a hostname and service name such as
    an IP port number.
 */
class PT_NET_API Endpoint
{
    public:
        Endpoint();

        Endpoint(const std::string& host, unsigned short port);

        Endpoint(const char* host, unsigned short port);

        Endpoint(const Endpoint& src);

        ~Endpoint();

        Endpoint& operator=(const Endpoint& src);

        void clear();

        std::string toString() const;

        static Endpoint ip4Any(unsigned short port);

        static Endpoint ip4Loopback(unsigned short port);

        static Endpoint ip4Broadcast(unsigned short port);

        static Endpoint ip6Any(unsigned short port);

        static Endpoint ip6Loopback(unsigned short port);

        //! @internal
        explicit Endpoint(EndpointImpl* impl);

        //! @internal
        EndpointImpl* impl()
        { return _impl; }

        //! @internal
        const EndpointImpl* impl() const
        { return _impl; }

    private:
        EndpointImpl* _impl;
};

} // namespace Net

} // namespace Pt

#endif // PT_NET_ENDPOINT_H
