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
#include <Pt/Http/MessageHeader.h>
#include <Pt/Signal.h>
#include <Pt/NonCopyable.h>
#include <string>

namespace Pt {

namespace System {
    class EventLoop;
}

namespace Net {
    class AddrInfo;
}

namespace Ssl {
    class Context;
}

namespace Http {

class Authorization;
class Reply;
class Request;

class PT_HTTP_API Client : public Connectable
                         , private NonCopyable
{
    public:
        Client();
        
        explicit Client(System::EventLoop& loop);

        Client(System::EventLoop& loop, const std::string& host, unsigned short int port);
        
        Client(System::EventLoop& loop, const Net::AddrInfo& addrinfo);

        ~Client();

        System::EventLoop* loop() const;

        void setActive(System::EventLoop& loop);

        void setHost(const Net::AddrInfo& addrinfo);
        
        void setHost(const std::string& host, unsigned short int port);

        const Net::AddrInfo& host() const;

        void setSecure(Ssl::Context& ctx);

        void setAuthorization(Authorization& auth);

        void clearAuthorization();

        void beginSend();

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

        Request& request();

        const Request& request() const;

        Reply& reply();

        const Reply& reply() const;

        /** @brief Set timeout for I/O operations.
        */
        void setTimeout(std::size_t timeout);

        /** @brief Blocks until request is sent.
        */
        void send();

        /** @brief Blocks until reply is received.
        */
        std::istream& receive();

    protected:
        void onRequestSent(Request& r);

        void onReplyReceived(Reply& r);

    private:
        class ClientImpl* _impl;
};

} // namespace Http

} // namespace Pt

#endif
