/* Copyright (C) 2015 by Laurentiu-Gheorghe Crisan
   Copyright (C) 2026 by Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
*/

#ifndef PT_HTTP_WEBSOCKETSERVICE_H
#define PT_HTTP_WEBSOCKETSERVICE_H

#include <Pt/Http/Api.h>
#include <Pt/Http/Service.h>
#include <Pt/Http/Message.h>
#include <Pt/Http/WebSocketSession.h>
#include <Pt/Allocator.h>
#include <Pt/Connectable.h>
#include <Pt/System/Mutex.h>
#include <string>
#include <vector>
#include <cstddef>

namespace Pt {

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
    the session owner are different objects. The setters are
    endpoint policy of this service.

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
    with 405 and Allow: GET when the method is not GET, with 400 when
    the HTTP version is below 1.1, with 426 when the request neither
    lists upgrade in Connection nor has an Upgrade value, with 400
    when only one of the two is present or Upgrade does not list
    websocket, with 503 when no servlet is attached or the session
    limit is reached, with 400 when Sec-WebSocket-Key is missing or
    invalid, with 426 and Sec-WebSocket-Version: 13 when
    Sec-WebSocket-Version is not 13, with 400 when Origin is empty,
    or with 403 when a configured Origin list rejects the Origin
    value.
    The responder is not the session. %addProtocol() names one
    Sec-WebSocket-Protocol value this endpoint accepts. Several calls
    name several accepted values. An empty list accepts
    the upgrade and selects no name. A non-empty list selects the
    first offered name that is also accepted, and a request that
    offers none of them is answered with 400. %addOrigin() names one
    Origin this endpoint accepts. Several calls name several values.
    An empty list does not check Origin. A non-empty list accepts
    one Origin value that matches an entry. The comparison ignores
    ASCII case and uses the whole stored value. The literal null is
    accepted only when it is an entry. A missing value or a value
    that matches no entry is answered with 403. An empty Origin value
    is answered with 400. %setMaxSockets(),
    %setIdleTimeout(), %setPingInterval(), %setPingTimeout(),
    %setCloseTimeout() and %setMaxMessageSize() are endpoint policy
    of this service.
    The handshake takes a place from the session limit only after all
    other checks have passed, so an invalid handshake never takes a
    place and is answered with its own status. The handshake responder holds that place until
    the server releases it, which is after the server has handed over
    the stream. Between %getSession() and that release the upgrade is
    counted twice, so the limit is reached early and never late. The
    counters, the servlet, the protocol list, the Origin list and
    the policy values
    are guarded by a mutex, because the handshake may run in a server
    thread while %getSession() and %releaseSession() run in the
    thread of the server loop. The session base
    copies the idle, ping-interval, ping and close timeouts and the
    data-message limit onto the connection before the derived
    constructor runs. A missing
    servlet is still 503 when %maxSockets() is zero. One service
    has one active %WebSocketServlet. Constructing a second servlet
    throws %std::logic_error.

    After a finished 101 the HTTP server keeps the connection and
    delivers the upgraded stream to this service. This class
    implements that delivery and asks the servlet to create a session
    through %getSession() with the stream, the opening request, and
    the opening reply. %getSession() calls
    %onGetSession() and counts a non-null session. The session binds
    the stream in its constructor, which accepts the upgrade. A null
    session leaves the stream unbound, and the HTTP server closes it.
    A null return is not counted. No attached servlet is a 503 from
    the handshake. The null check here covers only the window after a
    finished 101 and before the stream is delivered.

    %BasicWebSocketService is this factory for one session type.
    Derive this class when the session type depends on the upgrade,
    or when the session constructor needs more than the service, the
    stream, and the opening reply.

    @code
    Pt::Http::WebSocketSession* ChatService::onGetSession(
        Pt::Http::Stream& stream,
        const Pt::Http::Request& request,
        const Pt::Http::Reply& reply)
    {
        const char* user = request.header().get("X-User");
        return new ChatSession(*this, stream, reply, _rooms,
                               user ? user : "");
    }

    void ChatService::onReleaseSession(Pt::Http::WebSocketSession* session)
    {
        delete session;
    }
    @endcode

    %onHandshake() runs before the 101 is written. Return false there
    to reject the opening request with an HTTP status. The handshake
    responder sends that reply and closes the connection. No stream
    is opened. %onGetSession() does not run.

    %getSession() is the factory call. It calls %onGetSession() and
    counts a non-null session. %releaseSession() calls
    %onReleaseSession() and drops that count. %onGetSession() creates
    the session and %onReleaseSession() destroys it. The request
    argument is the opening request. The reply argument is the opening
    reply. Both are valid for that call. Copy any header the session
    must keep. The session does not store the request or the reply.
    The two methods must use the same allocator. A pool, or any other
    detach, lives in the derived service. A null return from
    %onGetSession() closes the stream after the 101 and carries no
    HTTP status. A rejection that needs a status uses %onHandshake().
    The servlet owns the live sessions only. This service owns the
    endpoint policy, the allocator, and the count of sessions
    %getSession() returned and %releaseSession() has not yet released.
    This service does not keep a session list. %socketCount() is that
    count.

    @ingroup Pt-Http-WebSocket-Server
*/
class PT_HTTP_API WebSocketService : public Service
                                   , public Connectable
{
    friend class WebSocketSession;
    friend class WebSocketServlet;
    friend class WebSocketResponder;

    public:
        /** @brief Default constructor.
        */
        WebSocketService();

        /** @brief Destructor.

            Does not release sessions. The active %WebSocketServlet
            releases them before this destructor runs.
        */
        ~WebSocketService();

        /** @brief Returns the accepted Sec-WebSocket-Protocol names.

            Returns a copy, because the list may change in another
            thread while a handshake reads it.
        */
        std::vector<std::string> protocols() const;

        /** @brief Adds one accepted Sec-WebSocket-Protocol name.

            An empty list accepts the upgrade and selects no name.

            @throw %std::invalid_argument if @a name is empty or is
            not a single protocol token.
        */
        void addProtocol(const std::string& name);

        /** @brief Drops every accepted protocol name.
        */
        void clearProtocols();

        /** @brief Returns accepted Origin values.

            Returns a copy, because the list may change in another
            thread while a handshake reads it. An empty list does not
            check Origin.
        */
        std::vector<std::string> origins() const;

        /** @brief Adds one accepted Origin value.

            The handshake compares @a origin to the stored Origin
            value and ignores ASCII case. An empty list does not check
            Origin. A missing value is rejected. The literal null is
            accepted only when it was added.

            @throw %std::invalid_argument if @a origin is empty.
        */
        void addOrigin(const std::string& origin);

        /** @brief Drops every accepted Origin value.
        */
        void clearOrigins();

        /** @brief Returns the maximum number of live sessions.

            Zero means no limit. The limit covers live sessions and
            handshakes that are about to become one.
        */
        std::size_t maxSockets() const;

        /** @brief Sets the maximum number of live sessions.

            A handshake above this limit is answered with 503.
            The limit covers live sessions and handshakes that are
            about to become one. Zero means no limit. A missing
            servlet is still 503 when this limit is zero.
        */
        void setMaxSockets(std::size_t n);

        /** @brief Returns the idle timeout in milliseconds.

            Zero means the idle timeout is disabled.
        */
        std::size_t idleTimeout() const;

        /** @brief Sets the idle timeout in milliseconds.

            An accepted socket is closed after @a ms without an
            incoming frame. Incoming data, a received ping, and a
            received pong restart it. Sending does not. Zero disables
            it.
        */
        void setIdleTimeout(std::size_t ms);

        /** @brief Returns the ping interval in milliseconds.

            Zero means the engine does not write keepalive pings.
        */
        std::size_t pingInterval() const;

        /** @brief Sets the ping interval in milliseconds.

            An accepted socket writes a keepalive ping after @a ms
            without an incoming frame. Incoming data, a received ping,
            and a received pong restart it. Sending does not. Zero
            disables it.
        */
        void setPingInterval(std::size_t ms);

        /** @brief Returns the ping timeout in milliseconds.

            Zero means an unanswered ping has no deadline.
        */
        std::size_t pingTimeout() const;

        /** @brief Sets the ping timeout in milliseconds.

            An accepted socket is closed if a ping is unanswered for
            @a ms. The wait starts at %beginPing() and at a keepalive
            ping. Zero disables it.
        */
        void setPingTimeout(std::size_t ms);

        /** @brief Returns the close timeout in milliseconds.

            Zero leaves a close handshake unbounded. The default is
            ten seconds.
        */
        std::size_t closeTimeout() const;

        /** @brief Sets the close timeout in milliseconds.

            An accepted socket is closed if a close handshake is
            unanswered for @a ms. Zero leaves the wait unbounded.
            The default is ten seconds.
        */
        void setCloseTimeout(std::size_t ms);

        /** @brief Returns the maximum data message size in bytes.

            Zero means the limit is disabled.
        */
        std::size_t maxMessageSize() const;

        /** @brief Sets the maximum data message size in bytes.

            A larger received data message closes the socket. Zero
            disables the limit. The count is the declared payload from
            the first data opcode to FIN. The limit applies to received
            messages only. Sent messages are not limited.
        */
        void setMaxMessageSize(std::size_t n);

        /** @brief True when a servlet is attached.

            A missing servlet is a separate state from a zero
            %maxSockets().
        */
        bool hasServlet() const;

        /** @brief Returns the number of counted sessions.

            A session is counted when %getSession() returns it, and
            dropped when %releaseSession() releases it. A null return
            is not counted. A handshake that has not yet become a
            session is not counted. Zero is not a reached limit. A
            missing servlet is a separate state, read with
            %hasServlet().
        */
        std::size_t socketCount() const;

        /** @brief Creates a session and counts a non-null result.

            Calls %onGetSession(). @a request is the opening request
            and @a reply is the opening reply. Both are valid until
            this call returns. A null return declines the upgrade and
            is not counted.
        */
        WebSocketSession* getSession(Stream& stream,
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
        virtual bool onUpgrade(Stream& stream,
                               const Request& request,
                               const Reply& reply) final;

    protected:
        /** @brief Accepts or rejects the opening handshake.

            Runs on a server thread, before the 101 is written, and
            only after the field checks have passed and a place has
            been taken from the session limit. It does not run on the
            loop of the server. It is synchronous and must not block.
            A check that needs I/O has no place here. The responder
            does not hold the service mutex during this call. Protect
            shared state the way the Origin list and the socket count
            are protected.

            @a request and @a reply are valid until this call returns.
            There is no stream, no event loop, and no servlet. The
            reply starts at status 200 with an empty header and an
            empty body.

            Return true to accept. Extra header fields, such as
            Set-Cookie, are kept. A status or a body set here is
            discarded. The responder writes status 101, Upgrade,
            Connection, and Sec-WebSocket-Accept afterwards.

            Return false to reject. Describe the reply completely:
            status, reason, header, and body. A status of 300 or
            higher is sent, and the connection is closed. No stream
            is opened. A status below 300, including the default 200
            and a 101 set here, is a broken contract and is sent as
            500 without the header or body set here. The writer
            determines Content-Length and Transfer-Encoding. A value
            set here is not sent.

            An exception does not send the reply prepared here.
            %HttpError becomes 400. Any other exception ends the
            exchange without that reply. The reserved place is
            released when the responder is destroyed. Return false to
            send a status.

            The default returns true and leaves @a reply unchanged.
        */
        virtual bool onHandshake(const Request& request, Reply& reply);

        /** @brief Creates the session for @a stream.

            Called by %getSession() after a finished 101. The active
            %WebSocketServlet owns a non-null result. @a request is
            the opening request and @a reply is the opening reply.
            Both are valid for this call. Copy any header the session
            must keep. Return null to close the stream. A null return
            carries no HTTP status and is not counted. A rejection
            that needs a status uses %onHandshake() before the 101 is
            written.
        */
        virtual WebSocketSession* onGetSession(Stream& stream,
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

        WebSocketServlet* servlet() const;

        /** @brief Releases @a session through the attached servlet.

            Returns when no servlet is attached. The servlet removes
            the session from its live list when present.
        */
        void onSessionClosed(WebSocketSession& session);

        /** @brief Takes a place for a handshake.

            Called before the request is checked, so a rejected
            handshake keeps the place until its responder is
            destroyed. Returns false when no servlet is attached or
            the sessions and pending handshakes already reach
            %maxSockets().
        */
        bool reservePending();

        /** @brief Gives back a place taken by %reservePending().
        */
        void releasePending();

        /** @brief Selects the first offered name that is accepted.

            Reads the Sec-WebSocket-Protocol field of @a header.
            Returns false when names are configured and none of them
            is offered, which includes a missing field. Otherwise
            returns true, and @a selected is the match, or empty when
            no names are configured. The comparison is case-sensitive.
        */
        bool selectProtocol(const MessageHeader& header, std::string& selected) const;

        /** @brief Returns true when @a origin is accepted.

            An empty list accepts every value. Otherwise the comparison
            ignores ASCII case. A null @a origin is not an entry.
        */
        bool acceptsOrigin(const char* origin) const;

    private:
        mutable System::Mutex    _mutex;
        WebSocketServlet*        _servlet;
        std::size_t              _socketCount;
        std::size_t              _pending;
        std::size_t              _maxSockets;
        std::size_t              _idleTimeout;
        std::size_t              _pingInterval;
        std::size_t              _pingTimeout;
        std::size_t              _closeTimeout;
        std::size_t              _maxMessageSize;
        std::vector<std::string> _protocols;
        std::vector<std::string> _origins;
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
        virtual WebSocketSession* onGetSession(Stream& stream,
                                               const Request& /*request*/,
                                               const Reply& reply)
        {
            void* memory = _alloc.allocate(sizeof(S));
            return new(memory) S(*this, stream, reply);
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
