/*
 * Copyright (C) 2012 by Marc Boris Duerner
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

#ifndef Pt_Http_Client_h
#define Pt_Http_Client_h

#include <Pt/Http/Api.h>
#include <Pt/Signal.h>
#include <Pt/NonCopyable.h>
#include <string>
#include <iosfwd>
#include <cstddef>

namespace Pt {

namespace System {
class EventLoop;
}

namespace Net {
class Endpoint;
class TcpSocketOptions;
}

namespace Ssl {
class Context;
}

namespace Http {

/** @brief An HTTP client.

    A connection will be persistent (keep-alive), if the request headers
    contains the keep-alive header fields. A persistent connection is needed
    for HTTP request pipelining. Once all requests have been made, the client
    should be closed, otherwise it may run into the servers keep-alive
    timeout, if the client is used again later.

    @code
        Pt::Http::Client client;
        client.request().header().setKeepAlive();

        // use client for multiple requests
        ...

        // close the persistent connection
        client.close();

    @endcode
*/
class PT_HTTP_API Client : public Connectable
                         , private NonCopyable
{
    public:
        /** @brief Default Constructor.
        */
        Client();
        
        /** @brief Construct with host to connect to.
        */
        explicit Client(const Net::Endpoint& ep);

        /** @brief Construct with event loop.
        */
        explicit Client(System::EventLoop& loop);

        /** @brief Construct with loop and host to connect to.
        */
        Client(System::EventLoop& loop, const Net::Endpoint& ep);

        /** @brief Destructor.
        */
        ~Client();

        /** @brief Returns the used event loop.
        */
        System::EventLoop* loop() const;

        /** @brief Sets the event loop to use.
        */
        void setActive(System::EventLoop& loop);

        /** @brief Set timeout for I/O operations.
        */
        void setTimeout(std::size_t timeout);

        /** @brief Enable to use HTTPS.
        */
        void setSecure(Ssl::Context& ctx);

        /** @brief Set expected SSL peer name.
        */
        void setPeerName(const std::string& peer);

        /** @brief Set host to connect to.
        */
        void setHost(const Net::Endpoint& ep);

        /** @brief Set host to connect to.
        */
        void setHost(const Net::Endpoint& ep, const Net::TcpSocketOptions& opts);

        /** @brief Returns the host to connect to.
        */
        const Net::Endpoint& host() const;

        /** @brief Returns the request to send.
        */
        Request& request();

        /** @brief Returns the request to send.
        */
        const Request& request() const;

        /** @brief Returns the received reply.
        */
        Reply& reply();

        /** @brief Returns the received reply.
        */
        const Reply& reply() const;

        /** @brief Begin sending the request.
        */
        void beginSend(bool finished = true);

        /** @brief End sending the request.
        */
        MessageProgress endSend();

        /** @brief Signals that a part of the request was sent.
        */
        Signal<Client&>& requestSent();

        /** @brief Begin receiving the reply.
        */
        void beginReceive();

        /** @brief End receiving the reply.
        */
        MessageProgress endReceive();

        /** @brief Signals that a part of the reply was received.
        */
        Signal<Client&>& replyReceived();

        // TODO: remove in later version
        void cancel();

        /** @brief Closes the connection and cancels all operations.
        */
        void close();

        /** @brief Blocks until request is sent.
        */
        void send(bool finished = true);

        /** @brief Blocks until reply is received.
        */
        std::istream& receive();

    protected:
        //! @internal
        void onRequestSent(Request& r);
        
        //! @internal
        void onReplyReceived(Reply& r);

    private:
        //! @internal
        void init();

    private:
        class ClientImpl* _impl;
};

} // namespace Http

} // namespace Pt

#endif // Pt_Http_Client_h
