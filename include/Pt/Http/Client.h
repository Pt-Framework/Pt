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

/** @brief HTTP user agent.

    %Client is the HTTP user agent in the client model. It holds one
    %Request and one %Reply. Fill the request, send it, and read the
    reply. The client opens a TCP connection to its host when a send
    needs one, so there is no separate connect method. The host is an
    %Endpoint passed to a constructor or to %setHost().

    Asynchronous work needs an %EventLoop, passed to a constructor or
    to %setActive(). The loop does not own the client; keep the client
    alive while an operation is still waiting on the loop. %setTimeout()
    bounds I/O. %send() and %receive() are the blocking forms of the
    same exchange.

    The request is %request(). Set the URL, the method, query
    parameters and header fields before the send starts, and write the
    body with %request().body(). The default method is GET. The reply
    is %reply() after a receive step has made it available.

    Asynchronous receive is %beginReceive() and %endReceive().
    %replyReceived() is emitted when a step has completed. The slot
    calls %endReceive(), which returns %MessageProgress. If the header
    is available, the status can be read; if the body is available, it
    can be read from %reply().body(); if the reply is not finished,
    %beginReceive() continues the same reply. A short reply often
    completes in one step.

    The example is an asynchronous GET. The client is constructed with
    the loop and the host, the request URL is set, and %beginReceive()
    starts the exchange. The slot ends each receive step and exits the
    loop when the reply is finished.

    @code
    void onReplyReceived(Pt::Http::Client& client)
    {
        Pt::Http::MessageProgress progress = client.endReceive();
        Pt::Http::Reply& reply = client.reply();

        if( progress.header() )
        {
            std::cout << reply.statusCode() << ' '
                      << reply.statusText() << std::endl;
        }

        if( progress.body() )
        {
            while( reply.body().rdbuf()->in_avail() )
                std::cout << reply.body().get();
        }

        if( progress.finished() )
        {
            client.loop()->exit();
            return;
        }

        client.beginReceive();
    }

    Pt::System::MainLoop loop;
    Pt::Net::Endpoint ep("www.example.com", 80);
    Pt::Http::Client client(loop, ep);

    client.request().setUrl("/index.html");
    client.replyReceived() += Pt::slot(onReplyReceived);
    client.beginReceive();
    loop.run();
    @endcode

    @par Pipelining

    Pipelining sends several requests before receiving the matching
    replies, which needs a persistent connection. Set the keep-alive
    header on the request, connect %requestSent() as well as
    %replyReceived(), and start with %beginSend() rather than
    %beginReceive(). The send slot calls %endSend(); if that send is
    not finished, %beginSend() continues it, and if it is finished,
    the next request can be filled and sent. When no further request
    will be pipelined, %beginReceive() starts reading the replies.
    Identify each reply by order or by application state, because
    %Reply does not store the request URL.

    @code
    void onRequestSent(Pt::Http::Client& client)
    {
        Pt::Http::MessageProgress progress = client.endSend();
        if( ! progress.finished() )
        {
            client.beginSend();
            return;
        }

        if( client.request().url() == "/cat.png" )
        {
            client.request().setUrl("/dog.png");
            client.beginSend();
            return;
        }

        client.beginReceive();
    }
    @endcode

    @par Chunked bodies

    A chunked request body is sent with %beginSend(false) until the
    last chunk, so the completion flag is false while more body data
    will be written. %endSend() reports whether the current chunk has
    left the socket, not whether the whole request is complete. When
    a chunk has finished and more data remains, write it to
    %request().body() and call %beginSend(false) again. When no more
    chunks remain, %beginReceive() finishes the request correctly. To
    pipeline another chunked request after this one, call
    %beginSend(true) so the request body is terminated.

    @par Persistent connections and HTTPS

    A keep-alive header on the request asks for a persistent
    connection, which pipelining needs and which the server may still
    close. %close() ends the connection. Leave it open only while the
    next request will reuse it; otherwise the server keep-alive
    timeout may close it before the client is used again. A later send
    on a closed or timed-out connection opens a new one.

    %setSecure() assigns a %Pt::Ssl::Context so further connections
    are HTTPS. %setPeerName() sets the name expected in the peer
    certificate. Send and receive are otherwise unchanged. Certificate
    and handshake details live in the SSL module.

    @ingroup Pt-Http-Clients
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

        /** @brief Enables HTTPS with @a ctx.
        */
        void setSecure(Ssl::Context& ctx);

        /** @brief Sets the expected SSL peer name.
        */
        void setPeerName(const std::string& peer);

        /** @brief Sets the host to connect to.
        */
        void setHost(const Net::Endpoint& ep);

        /** @brief Sets the host to connect to.
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

        /** @brief Begins sending the request.

            @a finished is true when this is the last chunk of the body.
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
