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

#ifndef Pt_Http_ClientImpl_h
#define Pt_Http_ClientImpl_h

#include "Connection.h"

#include <Pt/Http/Request.h>
#include <Pt/Http/Reply.h>
#include <Pt/Net/TcpServer.h>
#include <Pt/Net/TcpSocket.h>
#include <Pt/Ssl/StreamBuffer.h>
#include <Pt/System/Selectable.h>
#include <Pt/System/IOStream.h>
#include <Pt/System/IOBuffer.h>
#include <Pt/System/Timer.h>
#include <Pt/Connectable.h>
#include <Pt/Delegate.h>

#include <string>
#include <sstream>
#include <cstddef>

namespace Pt {

namespace Ssl {
  class Context;
}

namespace Http {

class Client;

class ClientImpl : public Connectable
{
    friend class ParseEvent;

    public:
        enum State
        {
            Idle                  = 0,
            OnConnect             = 1,
            OnConnectReceive      = 2,
            OnSslHandshake        = 3,
            OnSslHandshakeReceive = 4,
            OnRequest             = 5,
            OnChunkedRequest      = 6,
            OnRequestEnd          = 7,
            OnRequestComplete     = 8,
            OnReply               = 9,
            OnReplyComplete       = 10
        };

    public:
        ClientImpl();

        System::EventLoop* loop() const
        { return _conn.loop(); }

        void setActive(System::EventLoop& loop)
        { _conn.setActive(loop); }

        void setTimeout(std::size_t timeout)
        { _conn.setTimeout(timeout); }

        void setHost(const Net::Endpoint& addrinfo)
        { _conn.setHost(addrinfo); }

        void setHost(const Net::Endpoint& addrinfo, const Net::TcpSocketOptions& opts)
        { _conn.setHost(addrinfo, opts); }

        const Net::Endpoint& host() const
        { return _conn.host(); }

        void setSecure(Ssl::Context& ctx);

        void send(bool finished);

        std::istream& receive();

        void beginSend(bool finished);

        MessageProgress endSend();

        void beginReceive();

        MessageProgress endReceive();

        Request& request()
        { return _req; }

        const Request& request() const
        { return _req; }

        Reply& reply()
        { return _reply; }

        const Reply& reply() const
        { return _reply; }

        void cancel();

        State state() const
        { return _hstate; }

        Signal<Client&>& requestSent()
        { return _requestSent; }

        Signal<Client&>& replyReceived()
        { return _replyReceived; }

    private:
        void init();

    private:   
        Signal<Client&> _requestSent;
        Signal<Client&> _replyReceived;
        State _hstate;
        Connection _conn;
        Request _req;
        Reply _reply;
        unsigned _requestCount;
};

} // namespace Http

} // namespace Pt

#endif
