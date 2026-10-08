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

/** @brief Mapped service and factory for WebSocket sessions.

    %WebSocketService is the server-side factory. Map it with an HTTP
    servlet like any other service, so the handshake request reaches
    it. This service owns the endpoint policy and the allocator. A
    separate %WebSocketServlet owns the live sessions. Destroy that
    servlet before this service. Its destruction releases every
    remaining session while the derived service is still fully
    constructed. This destructor does not release sessions. By then
    the derived destructor has already run, and the virtual release
    call and the allocator are gone. This service keeps a non-owning
    pointer to its one active servlet.

    The example is the usual server setup. The mapping servlet and
    the session owner are different objects. The three setters are
    endpoint policy of this service. They apply before a servlet is
    constructed.

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
    with 503 when no servlet is attached or the accepted-session
    limit is already reached, or with 404 when the request is not a
    WebSocket upgrade. The responder is released before the server
    opens the stream. It is not the session. %addProtocol() names one
    Sec-WebSocket-Protocol value this endpoint accepts. Several calls
    name several values, in preference order. An empty list accepts
    the upgrade and selects no name. A non-empty list selects the
    first offered name that is also accepted, and a request that
    offers none of them is answered with 400. %setMaxSockets(),
    %setIdleTimeout() and %setMaxMessageSize() are endpoint policy
    of this service. They apply before a servlet is constructed.
    The handshake reads the session limit here. The session base
    copies the idle timeout and the data-message limit onto the
    connection before the derived constructor runs. A missing
    servlet is still 503 when %maxSockets() is zero. One service
    has one active %WebSocketServlet. Constructing a second servlet
    throws %std::logic_error.

    After a finished 101 the HTTP server keeps the connection and
    delivers the upgraded stream to this service. This class
    implements that delivery and asks the servlet to create a session
    through %getSession() with the loop of the stream, the stream, the
    opening request, and the opening reply. %getSession() calls
    %onGetSession() and counts a non-null session. The session binds
    the stream in its constructor, which accepts the upgrade. A null
    session leaves the stream unbound, and the HTTP server closes it.
    A null return is not counted. No attached servlet is a 503 from
    the handshake. The null check here covers only the window after a
    finished 101 and before the stream is delivered.

    %BasicWebSocketService is this factory for one session type.
    Derive this class when the session type depends on the upgrade,
    or when the session constructor needs more than the service, the
    loop, the stream, and the opening reply.

    @code
    Pt::Http::WebSocketSession* ChatService::onGetSession(
        Pt::System::EventLoop& loop,
        Pt::Http::Stream& stream,
        const Pt::Http::Request& request,
        const Pt::Http::Reply& reply)
    {
        const char* user = request.header().get("X-User");
        return new ChatSession(*this, loop, stream, reply, _rooms,
                               user ? user : "");
    }

    void ChatService::onReleaseSession(Pt::Http::WebSocketSession* session)
    {
        delete session;
    }
    @endcode

    %getSession() is the factory call. It calls %onGetSession() and
    counts a non-null session. %releaseSession() calls
    %onReleaseSession() and drops that count. %onGetSession() creates
    the session and %onReleaseSession() destroys it. The request
    argument is the opening request. The reply argument is the opening
    reply. Both are valid for that call. Copy any header the session
    must keep. The session does not store the request or the reply.
    The two methods must use the same allocator. A pool, or any other
    detach, lives in the derived service. Return null from
    %onGetSession() to decline the upgrade. The servlet owns the live
    sessions only. This service owns the endpoint policy, the
    allocator, and the count of sessions %getSession() returned and
    %releaseSession() has not yet released. This service does not keep
    a session list. %socketCount() is that count.

    @ingroup Pt-Http-WebSocket-Server
*/
class PT_HTTP_API WebSocketService : public Service
                                   , public Connectable
{
    friend class WebSocketSession;
    friend class WebSocketServlet;

    public:
        /** @brief Default constructor.
        */
        WebSocketService();

        /** @brief Destructor.

            Does not release sessions. The active %WebSocketServlet
            releases them before this destructor runs.
        */
        ~WebSocketService();

        /** @brief Returns accepted Sec-WebSocket-Protocol names in preference order.
        */
        const std::vector<std::string>& protocols() const
        { return _protocols; }

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

        /** @brief Returns the maximum number of live sessions.

            Zero means no limit. This is endpoint policy of this
            service. It applies before a servlet is constructed.
        */
        std::size_t maxSockets() const
        { return _maxSockets; }

        /** @brief Sets the maximum number of live sessions.

            A handshake above this limit is answered with 503. Zero
            means no limit. A missing servlet is still 503 when this
            limit is zero. This is endpoint policy of this service.
            It applies before a servlet is constructed.
        */
        void setMaxSockets(std::size_t n)
        { _maxSockets = n; }

        /** @brief Returns the idle timeout in milliseconds.

            Zero means the idle timeout is disabled. This is endpoint
            policy of this service. It applies before a servlet is
            constructed.
        */
        std::size_t idleTimeout() const
        { return _idleTimeout; }

        /** @brief Sets the idle timeout in milliseconds.

            An accepted socket is closed after @a ms without a finished
            transfer. A finished send or receive restarts the timeout.
            Zero disables it. This is endpoint policy of this service.
            It applies before a servlet is constructed.
        */
        void setIdleTimeout(std::size_t ms)
        { _idleTimeout = ms; }

        /** @brief Returns the maximum data message size in bytes.

            Zero means the limit is disabled. This is endpoint policy
            of this service. It applies before a servlet is constructed.
        */
        std::size_t maxMessageSize() const
        { return _maxMessageSize; }

        /** @brief Sets the maximum data message size in bytes.

            A larger data message closes the socket. Zero disables the
            limit. The count is the declared payload from the first
            data opcode to FIN. This is endpoint policy of this
            service. It applies before a servlet is constructed.
        */
        void setMaxMessageSize(std::size_t n)
        { _maxMessageSize = n; }

        /** @brief True when a servlet is attached.

            A missing servlet is a separate state from a zero
            %maxSockets().
        */
        bool hasServlet() const
        { return _servlet != 0; }

        /** @brief Returns the number of counted sessions.

            A session is counted when %getSession() returns it, and
            dropped when %releaseSession() releases it. A null return
            is not counted. Zero is not a reached limit. A missing
            servlet is a separate state, read with %hasServlet().
        */
        std::size_t socketCount() const
        { return _socketCount; }

        /** @brief Creates a session and counts a non-null result.

            Calls %onGetSession(). @a request is the opening request
            and @a reply is the opening reply. Both are valid until
            this call returns. A null return declines the upgrade and
            is not counted.
        */
        WebSocketSession* getSession(System::EventLoop& loop,
                                     Stream& stream,
                                     const Request& request,
                                     const Reply& reply);

        /** @brief Releases a session created by %getSession().

            Calls %onReleaseSession() and drops the count. A null
            @a session is ignored. Called while this service is still
            fully constructed.
        */
        void releaseSession(WebSocketSession* session);

    protected:
        /** @brief Creates the handshake responder.
        */
        virtual Responder* onGetResponder(const Request&);

        /** @brief Destroys the handshake responder.
        */
        virtual void onReleaseResponder(Responder* responder);

        /** @brief Accepts or declines the upgraded stream.

            Implemented by this class. A derived service uses
            %onGetSession() instead.
        */
        virtual void onUpgrade(Stream& stream,
                               const Request& request,
                               const Reply& reply) final;

    protected:
        /** @brief Creates the session for @a stream.

            Called by %getSession(). The active %WebSocketServlet owns
            a non-null result. @a loop serializes @a stream.
            @a request is the opening request and @a reply is the
            opening reply. Both are valid for this call. Copy any
            header the session must keep. Return null to decline the
            upgrade. A null return is not counted.
        */
        virtual WebSocketSession* onGetSession(System::EventLoop& loop,
                                               Stream& stream,
                                               const Request& request,
                                               const Reply& reply) = 0;

        /** @brief Destroys a session created by %onGetSession().

            Called by %releaseSession() while this service is still
            fully constructed. The servlet has already removed the
            session from its list.
        */
        virtual void onReleaseSession(WebSocketSession* session) = 0;

    private:
        void attach(WebSocketServlet& servlet);

        void detach(WebSocketServlet& servlet);

        WebSocketServlet* servlet()
        { return _servlet; }

        /** @brief Releases @a session through the attached servlet.

            Returns when no servlet is attached. The servlet removes
            the session from its list and calls %releaseSession().
        */
        void close(WebSocketSession& session);

    private:
        WebSocketServlet*        _servlet;
        std::size_t              _socketCount;
        std::size_t              _maxSockets;
        std::size_t              _idleTimeout;
        std::size_t              _maxMessageSize;
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
        virtual WebSocketSession* onGetSession(System::EventLoop& loop,
                                               Stream& stream,
                                               const Request& /*request*/,
                                               const Reply& reply)
        {
            void* memory = _alloc.allocate(sizeof(S));
            return new(memory) S(*this, loop, stream, reply);
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
