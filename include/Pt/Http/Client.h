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
*/
class PT_HTTP_API Client : public Connectable
                         , private NonCopyable
{
    public:
        Client();
        
        explicit Client(const Net::Endpoint& ep);

        explicit Client(System::EventLoop& loop);

        Client(System::EventLoop& loop, const Net::Endpoint& ep);

        ~Client();

        System::EventLoop* loop() const;

        void setActive(System::EventLoop& loop);

        /** @brief Set timeout for I/O operations.
        */
        void setTimeout(std::size_t timeout);

        void setSecure(Ssl::Context& ctx);

        /** @brief Set host to connect to.
        */
        void setHost(const Net::Endpoint& ep);

        void setHost(const Net::Endpoint& ep, const Net::TcpSocketOptions& opts);

        const Net::Endpoint& host() const;

        Request& request();

        const Request& request() const;

        Reply& reply();

        const Reply& reply() const;

        void beginSend(bool finished = true);

        MessageProgress endSend();

        /** @brief Signals that a part of the request was sent.
        */
        Signal<Client&>& requestSent();

        void beginReceive();

        MessageProgress endReceive();

        /** @brief Signals that a part of the reply was received.
        */
        Signal<Client&>& replyReceived();

        void cancel();

        /** @brief Blocks until request is sent.
        */
        void send(bool finished = true);

        /** @brief Blocks until reply is received.
        */
        std::istream& receive();

    protected:
        void onRequestSent(Request& r);

        void onReplyReceived(Reply& r);

    private:
        void init();

    private:
        class ClientImpl* _impl;
};

} // namespace Http

} // namespace Pt

#endif // Pt_Http_Client_h
