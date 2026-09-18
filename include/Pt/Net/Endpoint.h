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

/** @brief Host and service address.

    %Endpoint is the copyable address value that TCP and UDP operations
    use: it names a host and a service port, but it does not open a
    socket, and constructing one does not start a connection or a
    listen.

    A host string and a port number construct it, and the host may be a
    DNS name or a numeric IPv4 or IPv6 address. Name resolution happens
    when a socket uses the endpoint, not when the value is created, so
    an invalid name fails on listen, bind, or connect rather than on
    construction.

    Factory functions build the IPv4 and IPv6 any and loopback
    addresses, and the IPv4 broadcast address, for a port, and those
    addresses do not perform name resolution. The any-address is the
    local wildcard used to listen or bind on every local interface,
    loopback is the host itself, and the IPv4 broadcast address is the
    send destination for UDP broadcast.

    %toString() formats the endpoint for display, %clear() resets it to
    an empty address, and copy and assignment duplicate the address,
    not a socket.

    The example constructs a named host-and-port endpoint and an IPv4
    any-address for a port, so the first is a remote or named host and
    the second is a local wildcard.

    @code
    Pt::Net::Endpoint host("example.com", 80);
    Pt::Net::Endpoint any = Pt::Net::Endpoint::ip4Any(8080);
    @endcode

    @ingroup Pt-Net
*/
class PT_NET_API Endpoint
{
    public:
        /** @brief Creates an empty endpoint.
        */
        Endpoint();

        /** @brief Creates an endpoint for @a host and @a port.
        */
        Endpoint(const std::string& host, unsigned short port);

        /** @brief Creates an endpoint for @a host and @a port.
        */
        Endpoint(const char* host, unsigned short port);

        /** @brief Copies the endpoint.
        */
        Endpoint(const Endpoint& src);

        /** @brief Destroys the endpoint.
        */
        ~Endpoint();

        /** @brief Assigns the endpoint.
        */
        Endpoint& operator=(const Endpoint& src);

        /** @brief Clears the endpoint.
        */
        void clear();

        /** @brief Returns the endpoint as a string.
        */
        std::string toString() const;

        /** @brief Creates the IPv4 any-address for @a port.
        */
        static Endpoint ip4Any(unsigned short port);

        /** @brief Creates the IPv4 loopback address for @a port.
        */
        static Endpoint ip4Loopback(unsigned short port);

        /** @brief Creates the IPv4 broadcast address for @a port.
        */
        static Endpoint ip4Broadcast(unsigned short port);

        /** @brief Creates the IPv6 any-address for @a port.
        */
        static Endpoint ip6Any(unsigned short port);

        /** @brief Creates the IPv6 loopback address for @a port.
        */
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
