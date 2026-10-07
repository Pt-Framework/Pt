/* Copyright (C) 2015 by Laurentiu-Gheorghe Crisan
   Copyright (C) 2026 by Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
*/

#ifndef PT_HTTP_WEBSOCKETSERVICE_H
#define PT_HTTP_WEBSOCKETSERVICE_H

#include <Pt/Http/Api.h>
#include <Pt/Http/Service.h>
#include <Pt/Http/WebSocketSession.h>
#include <Pt/Allocator.h>
#include <Pt/Connectable.h>
#include <string>
#include <vector>
#include <cstddef>

namespace Pt {

namespace System {
class EventLoop;
}

namespace Http {

class Request;
class Reply;
class Responder;
class Stream;
class WebSocketServlet;

/** @brief Factory and endpoint policy for WebSocket sessions.

    %WebSocketService is the server-side factory. Map it with an HTTP
    servlet like any other service, so the handshake request reaches
    it. This type does not keep the sessions it creates. A separate
    release scope does, and that scope is destroyed before this
    service so the factory is still fully constructed when the last
    session is released. The service destructor does not release
    sessions. By then the derived destructor has already run, and the
    virtual release call and the allocator are gone.

    The example is the usual server setup. The mapping servlet and
    the release scope are different objects.

    @code
    typedef Pt::Http::BasicWebSocketService<EchoSession> EchoService;

    EchoService service;
    service.setMaxSockets(100);
    service.setIdleTimeout(60000);
    service.setMaxMessageSize(1 << 20);
    Pt::Http::WebSocketServlet sockets(service);
    Pt::Http::MapUrl mapUrl("/ws", service);
    server.addServlet(mapUrl);
    @endcode

    The handshake responder answers a WebSocket upgrade with 101,
    with 503 when the accepted-session limit is already reached, or
    with 404 when the request is not a WebSocket upgrade. The responder
    is released before the server opens the stream. It is not the
    session. %addProtocol() names one Sec-WebSocket-Protocol value
    this endpoint accepts. Several calls name several values, in
    preference order. An empty list accepts the upgrade and selects
    no name. A non-empty list selects the first offered name that
    is also accepted, and a request that offers none of them is
    answered with 400. %setMaxSockets(), %setIdleTimeout() and
    %setMaxMessageSize() are endpoint policy. The handshake reads
    the session limit. The session base copies the idle timeout and
    the data-message limit onto the connection before the derived
    constructor runs. One service has one registered
    %WebSocketServlet. A second registration replaces it. The
    previous servlet keeps the sessions it already holds and
    releases them itself.

    After a finished 101 the HTTP server keeps the connection and
    delivers the upgraded stream to this service. This class
    implements that delivery and does not pass it on. It forwards
    the stream to the registered servlet. The servlet asks the
    factory for a session and passes the loop of the stream, the
    stream, the opening request, and the opening reply. The session
    binds the stream in its constructor, which accepts the upgrade.
    A null session, or no registered servlet, leaves the stream
    unbound, and the HTTP server closes it.

    %BasicWebSocketService is this factory for one session type.
    Derive this class when the session type depends on the upgrade,
    or when the session constructor needs more than the servlet, the
    loop, the stream, and the opening reply.

    @code
    Pt::Http::WebSocketSession* ChatService::onGetSession(
        Pt::Http::WebSocketServlet& servlet,
        Pt::System::EventLoop& loop,
        Pt::Http::Stream& stream,
        const Pt::Http::Request& request,
        const Pt::Http::Reply& reply)
    {
        const char* user = request.header().get("X-User");
        return new ChatSession(servlet, loop, stream, reply, _rooms,
                               user ? user : "");
    }

    void ChatService::onReleaseSession(Pt::Http::WebSocketSession* session)
    {
        delete session;
    }
    @endcode

    %onGetSession() creates the session and %onReleaseSession()
    destroys it. The request argument is the opening request. The
    reply argument is the opening reply. Both are valid for that
    call. Copy any header the session must keep. The session does
    not store the request or the reply. The two methods must use the
    same allocator. A pool, or any other detach, lives in the derived
    service. Return null from %onGetSession() to decline the upgrade.

    @ingroup Pt-Http-WebSocket-Server
*/
class PT_HTTP_API WebSocketService : public Service
                                   , public Connectable
{
    friend class WebSocketResponder;

    public:
        /** @brief Default constructor.
        */
        WebSocketService();

        /** @brief Destructor.

            Does not release sessions. The %WebSocketServlet of this
            service does that, and it is destroyed first.
        */
        ~WebSocketService();

        /** @brief Returns the maximum number of live sessions.

            Zero means no limit.
        */
        std::size_t maxSockets() const;

        /** @brief Sets the maximum number of live sessions.

            A handshake above this limit is answered with 503. Zero
            means no limit.
        */
        void setMaxSockets(std::size_t n);

        /** @brief Returns the idle timeout in milliseconds.
        */
        std::size_t idleTimeout() const;

        /** @brief Sets the idle timeout in milliseconds.

            An accepted socket is closed after @a ms without a finished
            transfer. A finished send or receive restarts the timeout.
            Zero disables it.
        */
        void setIdleTimeout(std::size_t ms);

        /** @brief Returns the maximum data message size in bytes.
        */
        std::size_t maxMessageSize() const;

        /** @brief Sets the maximum data message size in bytes.

            A larger data message closes the socket. Zero disables the
            limit. The count is the declared payload from the first
            data opcode to FIN.
        */
        void setMaxMessageSize(std::size_t n);

        /** @brief Adds one accepted Sec-WebSocket-Protocol name.

            Names are matched in the order they were added. An empty
            list accepts the upgrade and selects no name.

            @throw %std::invalid_argument if @a name is empty or is
            not a single protocol token.
        */
        void addProtocol(const std::string& name);

        /** @brief Drops every accepted protocol name.
        */
        void clearProtocols();

    protected:
        /** @brief Creates the handshake responder.
        */
        virtual Responder* onGetResponder(const Request&);

        /** @brief Destroys the handshake responder.
        */
        virtual void onReleaseResponder(Responder* responder);

        /** @brief Creates the session for @a stream.

            @a servlet holds the returned session. @a loop serializes
            @a stream. @a request is the opening request and @a reply
            is the opening reply. Both are valid for this call. Copy
            any header the session must keep. Return null to decline
            the upgrade.
        */
        virtual WebSocketSession* onGetSession(WebSocketServlet& servlet,
                                               System::EventLoop& loop,
                                               Stream& stream,
                                               const Request& request,
                                               const Reply& reply) = 0;

        /** @brief Destroys a session created by %onGetSession().

            Called by the %WebSocketServlet that holds the session,
            while this service is still fully constructed.
        */
        virtual void onReleaseSession(WebSocketSession* session) = 0;

        /** @brief Accepts or declines the upgraded stream.

            Implemented by this class. A derived service uses
            %onGetSession() instead.
        */
        virtual void onUpgrade(Stream& stream,
                               const Request& request,
                               const Reply& reply) final;

    private:
        friend class WebSocketServlet;
        friend class WebSocketResponder;

        /** @internal Live sessions of the registered servlet.
        */
        std::size_t sessionCount() const;

        const std::vector<std::string>& protocols() const
        { return _protocols; }

        void registerServlet(WebSocketServlet& servlet);

        void unregisterServlet(WebSocketServlet& servlet);

        WebSocketServlet* _servlet;
        std::size_t _maxSockets;
        std::size_t _idleTimeout;
        std::size_t _maxMessageSize;
        std::vector<std::string> _protocols;
};

/** @brief WebSocket service for one session type.

    @ingroup Pt-Http-WebSocket-Server
*/
template <typename S, typename Alloc = Allocator>
class BasicWebSocketService : public WebSocketService
{
    public:
        /** @brief Default constructor.
        */
        BasicWebSocketService()
        { }

        /** @brief Destructor.
        */
        ~BasicWebSocketService()
        { }

    protected:
        virtual WebSocketSession* onGetSession(WebSocketServlet& servlet,
                                               System::EventLoop& loop,
                                               Stream& stream,
                                               const Request& /*request*/,
                                               const Reply& reply)
        {
            void* memory = _alloc.allocate(sizeof(S));
            return new(memory) S(servlet, loop, stream, reply);
        }

        virtual void onReleaseSession(WebSocketSession* session)
        {
            session->~WebSocketSession();
            _alloc.deallocate(session, sizeof(S));
        }

    private:
        Alloc _alloc;
};

} // namespace Http

} // namespace Pt

#endif
