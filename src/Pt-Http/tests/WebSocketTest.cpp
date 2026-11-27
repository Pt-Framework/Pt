/* Copyright (C) 2015-2026 by Laurentiu-Gheorghe Crisan
   Copyright (C) 2026 by Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
*/

#include "Pt/Unit/Assertion.h"
#include "Pt/Unit/TestSuite.h"
#include "Pt/Unit/RegisterTest.h"
#include "Pt/Http/Server.h"
#include "Pt/Http/Servlet.h"
#include "Pt/Http/Service.h"
#include "Pt/Http/Responder.h"
#include "Pt/Http/WebSocket.h"
#include "Pt/Http/WebSocketMessage.h"
#include "Pt/Http/WebSocketService.h"
#include "Pt/Http/WebSocketServlet.h"
#include "Pt/Http/WebSocketSession.h"
#include "Pt/Http/WebSocketError.h"
#include "Pt/Http/Client.h"
#include "Pt/Http/HttpError.h"
#include "Pt/Http/Request.h"
#include "Pt/Http/Reply.h"
#include "Pt/Net/Endpoint.h"
#include "Pt/Net/TcpSocket.h"
#include "Pt/System/MainLoop.h"
#include "Pt/System/IOError.h"
#include "Pt/System/Timer.h"
#include <Pt/Base64Codec.h>
#include <Pt/TextStream.h>
#include "Sha1.h"

#include <iterator>
#include <sstream>
#include <stdexcept>
#include <string>

class DeclineResponder : public Pt::Http::Responder
{
    public:
        explicit DeclineResponder(Pt::Http::Service& s)
        : Pt::Http::Responder(s)
        {}

    protected:
        virtual void onBeginRequest(Pt::Http::Request&, Pt::Http::Reply&, Pt::System::EventLoop&)
        {
            setReady(false);
        }

        virtual void onReadRequest(Pt::Http::Request&, Pt::Http::Reply&, Pt::System::EventLoop&)
        {
            setReady(false);
        }

        virtual void onBeginReply(const Pt::Http::Request& request, Pt::Http::Reply& reply, Pt::System::EventLoop& loop)
        {
            onWriteReply(request, reply, loop);
        }

        virtual void onWriteReply(const Pt::Http::Request& request, Pt::Http::Reply& reply,
                                  Pt::System::EventLoop&)
        {
            if(request.header().has("X-Test-Status"))
            {
                reply.setStatus(200, "OK");
                setReady(true);
                return;
            }

            reply.setStatus(101, "Switching Protocols");
            reply.header().setUpgrade();
            reply.header().set("Upgrade", "websocket");
            const char* accept = request.header().get("X-Test-Accept");
            if(accept)
                reply.header().set("Sec-WebSocket-Accept", accept);
            setReady(true);
        }
};

typedef Pt::Http::BasicService<DeclineResponder> DeclineService;


class SilentResponder : public Pt::Http::Responder
{
    public:
        explicit SilentResponder(Pt::Http::Service& service)
        : Pt::Http::Responder(service)
        {}

    protected:
        virtual void onBeginRequest(Pt::Http::Request&, Pt::Http::Reply&,
                                    Pt::System::EventLoop&)
        {
            setReady(false);
        }

        virtual void onReadRequest(Pt::Http::Request&, Pt::Http::Reply&,
                                   Pt::System::EventLoop&)
        {
            setReady(false);
        }

        virtual void onBeginReply(const Pt::Http::Request&, Pt::Http::Reply&,
                                  Pt::System::EventLoop&)
        {}

        virtual void onWriteReply(const Pt::Http::Request&, Pt::Http::Reply&,
                                  Pt::System::EventLoop&)
        {}
};

typedef Pt::Http::BasicService<SilentResponder> SilentService;


class HandshakeObserver : public Pt::Connectable
{
    public:
        explicit HandshakeObserver(Pt::System::EventLoop& loop, bool throwOnReady = false)
        : _loop(&loop)
        , _throwOnReady(throwOnReady)
        , _count(0)
        {}

        std::size_t count() const
        { return _count; }

        void onConnected(Pt::Http::WebSocket&)
        {
            ++_count;
            if(_throwOnReady)
                throw std::runtime_error("connected slot failed");

            _loop->exit();
        }

        void onReply(Pt::Http::Client& client)
        {
            Pt::Http::MessageProgress progress = client.endReceive();
            if( ! progress.finished() )
            {
                client.beginReceive();
                return;
            }

            ++_count;
            _loop->exit();
        }

    private:
        Pt::System::EventLoop* _loop;
        bool _throwOnReady;
        std::size_t _count;
};


class ReconnectObserver : public Pt::Connectable
{
    public:
        explicit ReconnectObserver(Pt::System::EventLoop& loop)
        : _loop(&loop)
        , _inputs(0)
        , _connects(0)
        {}

        std::size_t inputs() const
        { return _inputs; }

        std::size_t connects() const
        { return _connects; }

        void onConnected(Pt::Http::WebSocket& socket)
        {
            socket.endConnect();
            ++_connects;
            socket.beginReceive();
        }

        void onInputReady(Pt::Http::WebSocket& socket)
        {
            Pt::Http::WebSocketProgress progress = socket.endReceive();
            PT_UNIT_ASSERT(progress.finished());
            ++_inputs;

            std::string body;
            body.assign( std::istreambuf_iterator<char>( socket.incoming().body() ),
                         std::istreambuf_iterator<char>() );
            PT_UNIT_ASSERT_EQUALS(body, "feed");

            if(_inputs == 1)
            {
                socket.close();
                socket.beginConnect("/ws");
                return;
            }

            _loop->exit();
        }

    private:
        Pt::System::EventLoop* _loop;
        std::size_t _inputs;
        std::size_t _connects;
};


class RecordSession : public Pt::Http::WebSocketSession
{
    public:
        RecordSession(Pt::Http::WebSocketService& service,
                      Pt::Http::Stream& stream,
                      const Pt::Http::Reply& reply,
                      std::string& message,
                      Pt::Http::WebSocketMessage::Type& type,
                      bool& received,
                      bool& closedFlag,
                      Pt::System::EventLoop& exitLoop,
                      unsigned* closeCode = 0)
        : Pt::Http::WebSocketSession(service, stream, reply)
        , _message(&message)
        , _type(&type)
        , _received(&received)
        , _closed(&closedFlag)
        , _exitLoop(&exitLoop)
        , _closeCode(closeCode)
        {
            inputReady() += Pt::slot(*this, &RecordSession::onInputReady);
            outputReady() += Pt::slot(*this, &RecordSession::onOutputReady);
            peerClosed() += Pt::slot(*this, &RecordSession::onClosed);
            beginReceive();
        }

    private:
        void onInputReady(Pt::Http::WebSocketSession&)
        {
            endReceive();

            _message->assign( std::istreambuf_iterator<char>( incoming().body() ),
                              std::istreambuf_iterator<char>() );
            *_type = incoming().type();
            *_received = true;
            _exitLoop->exit();
        }

        void onOutputReady(Pt::Http::WebSocketSession&)
        {
            endSend();
        }

        void onClosed(Pt::Http::WebSocketSession&)
        {
            if(_closeCode)
                *_closeCode = closeCode();

            *_closed = true;
            close();
        }

    private:
        std::string* _message;
        Pt::Http::WebSocketMessage::Type* _type;
        bool* _received;
        bool* _closed;
        Pt::System::EventLoop* _exitLoop;
        unsigned* _closeCode;
};

class RecordService : public Pt::Http::WebSocketService
{
    public:
        RecordService(std::string& message,
                      Pt::Http::WebSocketMessage::Type& type,
                      bool& received,
                      bool& closed,
                      Pt::System::EventLoop& loop,
                      unsigned* closeCode = 0)
        : _message(&message)
        , _type(&type)
        , _received(&received)
        , _closed(&closed)
        , _loop(&loop)
        , _closeCode(closeCode)
        , _opened(0)
        , _released(0)
        {}

        std::size_t opened() const
        { return _opened; }

        std::size_t released() const
        { return _released; }

        std::size_t live() const
        { return _opened - _released; }

    protected:
        virtual Pt::Http::WebSocketSession* onGetSession(Pt::Http::Stream& stream,
                                                         const Pt::Http::Request&,
                                                         const Pt::Http::Reply& reply)
        {
            ++_opened;
            return new RecordSession(*this, stream, reply,
                                     *_message, *_type, *_received, *_closed, *_loop,
                                     _closeCode);
        }

        virtual void onReleaseSession(Pt::Http::WebSocketSession* session)
        {
            ++_released;
            delete session;
            _loop->exit();
        }

    private:
        std::string* _message;
        Pt::Http::WebSocketMessage::Type* _type;
        bool* _received;
        bool* _closed;
        Pt::System::EventLoop* _loop;
        unsigned* _closeCode;
        std::size_t _opened;
        std::size_t _released;
};

class CollectSession : public Pt::Http::WebSocketSession
{
    public:
        CollectSession(Pt::Http::WebSocketService& service,
                       Pt::Http::Stream& stream,
                       const Pt::Http::Reply& reply,
                       std::string& message,
                       Pt::Http::WebSocketMessage::Type& type,
                       bool& received,
                       Pt::System::EventLoop& exitLoop)
        : Pt::Http::WebSocketSession(service, stream, reply)
        , _message(&message)
        , _type(&type)
        , _received(&received)
        , _exitLoop(&exitLoop)
        {
            inputReady() += Pt::slot(*this, &CollectSession::onInputReady);
            outputReady() += Pt::slot(*this, &CollectSession::onOutputReady);
            beginReceive();
        }

    private:
        void onInputReady(Pt::Http::WebSocketSession&)
        {
            Pt::Http::WebSocketProgress progress = endReceive();
            PT_UNIT_ASSERT(progress.header() || progress.continuation());
            std::string chunk;
            chunk.assign( std::istreambuf_iterator<char>( incoming().body() ),
                          std::istreambuf_iterator<char>() );
            *_message += chunk;
            *_type = incoming().type();

            if( progress.finished() && chunk.empty() )
            {
                PT_UNIT_ASSERT(progress.header());
                PT_UNIT_ASSERT( ! progress.continuation() );
                PT_UNIT_ASSERT( ! progress.body() );
            }

            if( ! progress.finished() )
            {
                incoming().discard();
                beginReceive();
                return;
            }

            *_received = true;
            _exitLoop->exit();
        }

        void onOutputReady(Pt::Http::WebSocketSession&)
        {
            endSend();
        }

    private:
        std::string* _message;
        Pt::Http::WebSocketMessage::Type* _type;
        bool* _received;
        Pt::System::EventLoop* _exitLoop;
};

class CollectService : public Pt::Http::WebSocketService
{
    public:
        CollectService(std::string& message,
                       Pt::Http::WebSocketMessage::Type& type,
                       bool& received,
                       Pt::System::EventLoop& loop)
        : _message(&message)
        , _type(&type)
        , _received(&received)
        , _loop(&loop)
        {}

    protected:
        virtual Pt::Http::WebSocketSession* onGetSession(Pt::Http::Stream& stream,
                                                         const Pt::Http::Request&,
                                                         const Pt::Http::Reply& reply)
        {
            return new CollectSession(*this, stream, reply,
                                      *_message, *_type, *_received, *_loop);
        }

        virtual void onReleaseSession(Pt::Http::WebSocketSession* session)
        {
            delete session;
        }

    private:
        std::string* _message;
        Pt::Http::WebSocketMessage::Type* _type;
        bool* _received;
        Pt::System::EventLoop* _loop;
};

class PulseSession : public Pt::Http::WebSocketSession
{
    public:
        PulseSession(Pt::Http::WebSocketService& service,
                     Pt::Http::Stream& stream,
                     const Pt::Http::Reply& reply,
                     Pt::System::EventLoop& loop)
        : Pt::Http::WebSocketSession(service, stream, reply)
        , _sending(false)
        {
            inputReady() += Pt::slot(*this, &PulseSession::onInputReady);
            outputReady() += Pt::slot(*this, &PulseSession::onOutputReady);
            beginReceive();
            outgoing().setType(Pt::Http::WebSocketMessage::Text);
            outgoing().body() << "x";
            beginSend();
            _sending = true;
            _timer.setActive(loop);
            _timer.timeout() += Pt::slot(*this, &PulseSession::onPulse);
            _timer.start(40);
        }

        ~PulseSession()
        {
            _timer.detach();
        }

    private:
        void onInputReady(Pt::Http::WebSocketSession&)
        {
            endReceive();
            incoming().clear();
            beginReceive();
        }

        void onOutputReady(Pt::Http::WebSocketSession&)
        {
            endSend();
            _sending = false;
        }

        void onPulse()
        {
            if(_sending)
                return;

            outgoing().setType(Pt::Http::WebSocketMessage::Text);
            outgoing().body() << "x";
            beginSend();
            _sending = true;
        }

    private:
        Pt::System::Timer _timer;
        bool _sending;
};

class PulseService : public Pt::Http::WebSocketService
{
    public:
        explicit PulseService(Pt::System::EventLoop& loop)
        : _loop(&loop)
        {}

    protected:
        virtual Pt::Http::WebSocketSession* onGetSession(Pt::Http::Stream& stream,
                                                         const Pt::Http::Request&,
                                                         const Pt::Http::Reply& reply)
        {
            return new PulseSession(*this, stream, reply, *_loop);
        }

        virtual void onReleaseSession(Pt::Http::WebSocketSession* session)
        {
            delete session;
        }

    private:
        Pt::System::EventLoop* _loop;
};


class IdleSession : public Pt::Http::WebSocketSession
{
    public:
        IdleSession(Pt::Http::WebSocketService& service,
                    Pt::Http::Stream& stream,
                    const Pt::Http::Reply& reply,
                    Pt::System::EventLoop& exitLoop,
                    bool exitOnAccept)
        : Pt::Http::WebSocketSession(service, stream, reply)
        , _exitLoop(&exitLoop)
        {
            if(exitOnAccept)
                _exitLoop->exit();
        }

    private:
        Pt::System::EventLoop* _exitLoop;
};

class IdleService : public Pt::Http::WebSocketService
{
    public:
        IdleService(Pt::System::EventLoop& loop, bool exitOnAccept = true)
        : _loop(&loop)
        , _exitOnAccept(exitOnAccept)
        , _opened(0)
        , _released(0)
        {}

        std::size_t live() const
        { return _opened - _released; }

        std::size_t opened() const
        { return _opened; }

    protected:
        virtual Pt::Http::WebSocketSession* onGetSession(Pt::Http::Stream& stream,
                                                         const Pt::Http::Request&,
                                                         const Pt::Http::Reply& reply)
        {
            ++_opened;
            return new IdleSession(*this, stream, reply, *_loop, _exitOnAccept);
        }

        virtual void onReleaseSession(Pt::Http::WebSocketSession* session)
        {
            ++_released;
            delete session;
        }

    private:
        Pt::System::EventLoop* _loop;
        bool _exitOnAccept;
        std::size_t _opened;
        std::size_t _released;
};


class DeferredCloseSession : public Pt::Http::WebSocketSession
{
    public:
        DeferredCloseSession(Pt::Http::WebSocketService& service,
                             Pt::Http::Stream& stream,
                             const Pt::Http::Reply& reply,
                             std::string& message,
                             Pt::Http::WebSocketMessage::Type& type,
                             bool& received,
                             bool& closedFlag,
                             Pt::System::EventLoop& exitLoop)
        : Pt::Http::WebSocketSession(service, stream, reply)
        , _message(&message)
        , _type(&type)
        , _received(&received)
        , _closed(&closedFlag)
        , _exitLoop(&exitLoop)
        {
            inputReady() += Pt::slot(*this, &DeferredCloseSession::onInputReady);
            outputReady() += Pt::slot(*this, &DeferredCloseSession::onOutputReady);
            peerClosed() += Pt::slot(*this, &DeferredCloseSession::onClosed);
        }

        void beginDeferredReceive()
        {
            beginReceive();
        }

    private:
        void onInputReady(Pt::Http::WebSocketSession&)
        {
            Pt::Http::WebSocketProgress progress = endReceive();
            _message->append( std::istreambuf_iterator<char>( incoming().body() ),
                              std::istreambuf_iterator<char>() );
            *_type = incoming().type();

            if( ! progress.finished() )
            {
                incoming().discard();
                beginReceive();
                return;
            }

            *_received = true;
            incoming().clear();
            beginReceive();
        }

        void onOutputReady(Pt::Http::WebSocketSession&)
        {
            endSend();
        }

        void onClosed(Pt::Http::WebSocketSession&)
        {
            *_closed = true;
            _exitLoop->exit();
            close();
        }

    private:
        std::string* _message;
        Pt::Http::WebSocketMessage::Type* _type;
        bool* _received;
        bool* _closed;
        Pt::System::EventLoop* _exitLoop;
};


class DeferredCloseService : public Pt::Http::WebSocketService
{
    public:
        DeferredCloseService(std::string& message,
                             Pt::Http::WebSocketMessage::Type& type,
                             bool& received,
                             bool& closed,
                             Pt::System::EventLoop& loop)
        : _message(&message)
        , _type(&type)
        , _received(&received)
        , _closed(&closed)
        , _loop(&loop)
        , _session(0)
        , _receiveRequested(false)
        {}

        void beginDeferredReceive()
        {
            _receiveRequested = true;

            if(_session)
                _session->beginDeferredReceive();
        }

    protected:
        virtual Pt::Http::WebSocketSession* onGetSession(Pt::Http::Stream& stream,
                                                         const Pt::Http::Request&,
                                                         const Pt::Http::Reply& reply)
        {
            _session = new DeferredCloseSession(*this, stream, reply,
                                                *_message, *_type, *_received, *_closed, *_loop);

            if(_receiveRequested)
                _session->beginDeferredReceive();

            return _session;
        }

        virtual void onReleaseSession(Pt::Http::WebSocketSession* session)
        {
            _session = 0;
            delete session;
        }

    private:
        std::string* _message;
        Pt::Http::WebSocketMessage::Type* _type;
        bool* _received;
        bool* _closed;
        Pt::System::EventLoop* _loop;
        DeferredCloseSession* _session;
        bool _receiveRequested;
};


class PartialCloseSession : public Pt::Http::WebSocketSession
{
    public:
        PartialCloseSession(Pt::Http::WebSocketService& service,
                            Pt::Http::Stream& stream,
                            const Pt::Http::Reply& reply,
                            bool& partial,
                            bool& finished,
                            bool& closedFlag,
                            Pt::System::EventLoop& exitLoop)
        : Pt::Http::WebSocketSession(service, stream, reply)
        , _partial(&partial)
        , _finished(&finished)
        , _closed(&closedFlag)
        , _exitLoop(&exitLoop)
        {
            inputReady() += Pt::slot(*this, &PartialCloseSession::onInputReady);
            outputReady() += Pt::slot(*this, &PartialCloseSession::onOutputReady);
            peerClosed() += Pt::slot(*this, &PartialCloseSession::onClosed);
            beginReceive();
        }

    private:
        void onInputReady(Pt::Http::WebSocketSession&)
        {
            Pt::Http::WebSocketProgress progress = endReceive();
            if( ! progress.finished() )
            {
                *_partial = true;
                incoming().discard();
                beginReceive();
                return;
            }

            *_finished = true;
            incoming().clear();
            beginReceive();
        }

        void onOutputReady(Pt::Http::WebSocketSession&)
        {
            endSend();
        }

        void onClosed(Pt::Http::WebSocketSession&)
        {
            *_closed = true;
            _exitLoop->exit();
            close();
        }

    private:
        bool* _partial;
        bool* _finished;
        bool* _closed;
        Pt::System::EventLoop* _exitLoop;
};


class PartialCloseService : public Pt::Http::WebSocketService
{
    public:
        PartialCloseService(bool& partial,
                            bool& finished,
                            bool& closed,
                            Pt::System::EventLoop& loop)
        : _partial(&partial)
        , _finished(&finished)
        , _closed(&closed)
        , _loop(&loop)
        {}

    protected:
        virtual Pt::Http::WebSocketSession* onGetSession(Pt::Http::Stream& stream,
                                                         const Pt::Http::Request&,
                                                         const Pt::Http::Reply& reply)
        {
            return new PartialCloseSession(*this, stream, reply,
                                           *_partial, *_finished, *_closed, *_loop);
        }

        virtual void onReleaseSession(Pt::Http::WebSocketSession* session)
        {
            delete session;
        }

    private:
        bool* _partial;
        bool* _finished;
        bool* _closed;
        Pt::System::EventLoop* _loop;
};


class HandshakePolicyService : public Pt::Http::WebSocketService
{
    public:
        enum Decision
        {
            Accept,
            Unauthorized,
            NotFound,
            Unavailable,
            Untouched,
            Switching,
            Framed,
            Throw
        };

        explicit HandshakePolicyService(Decision decision)
        : _decision(decision)
        , _asked(0)
        , _follow(0)
        {}

        std::size_t asked() const
        { return _asked; }

        void follow(Pt::Http::WebSocket& socket)
        { _follow = &socket; }

    protected:
        virtual bool onHandshake(const Pt::Http::Request&, Pt::Http::Reply& reply)
        {
            ++_asked;

            if(_decision == Unauthorized)
            {
                reply.setStatus(Pt::Http::Reply::Unauthorized, "Unauthorized");
                reply.header().set("WWW-Authenticate", "Bearer");
                return false;
            }

            if(_decision == NotFound)
            {
                reply.setStatus(Pt::Http::Reply::NotFound, "Not Found");
                return false;
            }

            if(_decision == Unavailable)
            {
                reply.setStatus(Pt::Http::Reply::ServiceUnavailable, "Service Unavailable");
                reply.header().set("Retry-After", "120");
                reply.body() << "later";
                return false;
            }

            if(_decision == Untouched)
            {
                reply.header().set("WWW-Authenticate", "Bearer");
                reply.body() << "kept";
                return false;
            }

            if(_decision == Switching)
            {
                reply.setStatus(Pt::Http::Reply::SwitchingProtocols, "Switching Protocols");
                reply.header().set("Sec-WebSocket-Accept", "not-a-digest");
                reply.header().setUpgrade();
                return false;
            }

            if(_decision == Framed)
            {
                reply.header().set("Content-Length", "3");
                reply.header().set("Transfer-Encoding", "chunked");
                reply.setStatus(Pt::Http::Reply::Forbidden, "Forbidden");
                reply.body() << "no";
                return false;
            }

            if(_decision == Throw)
            {
                reply.setStatus(Pt::Http::Reply::NotFound, "Not Found");
                throw std::runtime_error("handshake failed");
            }

            return true;
        }

        virtual Pt::Http::WebSocketSession* onGetSession(Pt::Http::Stream& stream,
                                                         const Pt::Http::Request&,
                                                         const Pt::Http::Reply& reply)
        {
            if(_follow)
            {
                Pt::Http::WebSocket* socket = _follow;
                _follow = 0;
                socket->beginConnect("/ws");
            }

            return new Pt::Http::WebSocketSession(*this, stream, reply);
        }

        virtual void onReleaseSession(Pt::Http::WebSocketSession* session)
        {
            delete session;
        }

    private:
        Decision _decision;
        std::size_t _asked;
        Pt::Http::WebSocket* _follow;
};


class HandshakeHeaderSession : public Pt::Http::WebSocketSession
{
    public:
        HandshakeHeaderSession(Pt::Http::WebSocketService& service,
                               Pt::Http::Stream& stream,
                               const Pt::Http::Reply& reply,
                               std::string& message,
                               bool& received,
                               Pt::System::EventLoop& loop)
        : Pt::Http::WebSocketSession(service, stream, reply)
        , _message(&message)
        , _received(&received)
        , _loop(&loop)
        {
            inputReady() += Pt::slot(*this, &HandshakeHeaderSession::onInputReady);
            beginReceive();
        }

    private:
        void onInputReady(Pt::Http::WebSocketSession&)
        {
            endReceive();
            _message->assign( std::istreambuf_iterator<char>( incoming().body() ),
                              std::istreambuf_iterator<char>() );
            *_received = true;
            _loop->exit();
        }

    private:
        std::string* _message;
        bool* _received;
        Pt::System::EventLoop* _loop;
};


class HandshakeHeaderService : public Pt::Http::WebSocketService
{
    protected:
        virtual bool onHandshake(const Pt::Http::Request&, Pt::Http::Reply& reply)
        {
            reply.setStatus(Pt::Http::Reply::OK, "OK");
            reply.header().set("Set-Cookie", "session=1");
            reply.header().set("Content-Length", "4");
            reply.header().set("Transfer-Encoding", "chunked");
            reply.body() << "body";
            return true;
        }

        virtual Pt::Http::WebSocketSession* onGetSession(Pt::Http::Stream& stream,
                                                         const Pt::Http::Request&,
                                                         const Pt::Http::Reply& reply)
        {
            return new HandshakeHeaderSession(*this, stream, reply,
                                              _message, _received, *_loop);
        }

        virtual void onReleaseSession(Pt::Http::WebSocketSession* session)
        {
            delete session;
        }

    public:
        HandshakeHeaderService(std::string& message,
                               bool& received,
                               Pt::System::EventLoop& loop)
        : _message(message)
        , _received(received)
        , _loop(&loop)
        {}

    private:
        std::string& _message;
        bool& _received;
        Pt::System::EventLoop* _loop;
};


class NullService : public Pt::Http::WebSocketService
{
    protected:
        virtual Pt::Http::WebSocketSession* onGetSession(Pt::Http::Stream&,
                                                         const Pt::Http::Request&,
                                                         const Pt::Http::Reply&)
        {
            return 0;
        }

        virtual void onReleaseSession(Pt::Http::WebSocketSession*)
        {}
};

class ThrowSession : public Pt::Http::WebSocketSession
{
    public:
        ThrowSession(Pt::Http::WebSocketService& service,
                     Pt::Http::Stream& stream,
                     const Pt::Http::Reply& reply)
        : Pt::Http::WebSocketSession(service, stream, reply)
        {
            throw std::runtime_error("session construction failed");
        }
};

class ThrowService : public Pt::Http::WebSocketService
{
    protected:
        virtual Pt::Http::WebSocketSession* onGetSession(Pt::Http::Stream& stream,
                                                         const Pt::Http::Request&,
                                                         const Pt::Http::Reply& reply)
        {
            return new ThrowSession(*this, stream, reply);
        }

        virtual void onReleaseSession(Pt::Http::WebSocketSession* session)
        {
            delete session;
        }
};

class PushSession : public Pt::Http::WebSocketSession
{
    public:
        PushSession(Pt::Http::WebSocketService& service,
                    Pt::Http::Stream& stream,
                    const Pt::Http::Reply& reply)
        : Pt::Http::WebSocketSession(service, stream, reply)
        {
            outputReady() += Pt::slot(*this, &PushSession::onOutputReady);
            outgoing().setType(Pt::Http::WebSocketMessage::Text);
            outgoing().body() << "feed";
            beginSend();
        }

    private:
        void onOutputReady(Pt::Http::WebSocketSession&)
        {
            endSend();
        }
};

typedef Pt::Http::BasicWebSocketService<PushSession> PushService;


class LongPushSession : public Pt::Http::WebSocketSession
{
    public:
        LongPushSession(Pt::Http::WebSocketService& service,
                        Pt::Http::Stream& stream,
                        const Pt::Http::Reply& reply)
        : Pt::Http::WebSocketSession(service, stream, reply)
        {
            outputReady() += Pt::slot(*this, &LongPushSession::onOutputReady);
            outgoing().setType(Pt::Http::WebSocketMessage::Text);
            outgoing().body() << std::string(64, 'x');
            beginSend();
        }

    private:
        void onOutputReady(Pt::Http::WebSocketSession&)
        {
            endSend();
        }
};

typedef Pt::Http::BasicWebSocketService<LongPushSession> LongPushService;


class FeedSession : public Pt::Http::WebSocketSession
{
    public:
        FeedSession(Pt::Http::WebSocketService& service,
                    Pt::Http::Stream& stream,
                    const Pt::Http::Reply& reply)
        : Pt::Http::WebSocketSession(service, stream, reply)
        {
            inputReady() += Pt::slot(*this, &FeedSession::onInputReady);
            outputReady() += Pt::slot(*this, &FeedSession::onOutputReady);
            outgoing().setType(Pt::Http::WebSocketMessage::Text);
            outgoing().body() << "feed";
            beginSend();
            beginReceive();
        }

    private:
        void onInputReady(Pt::Http::WebSocketSession&)
        {
            endReceive();
        }

        void onOutputReady(Pt::Http::WebSocketSession&)
        {
            endSend();
        }
};

typedef Pt::Http::BasicWebSocketService<FeedSession> FeedService;


class TwoFeedSession : public Pt::Http::WebSocketSession
{
    public:
        TwoFeedSession(Pt::Http::WebSocketService& service,
                       Pt::Http::Stream& stream,
                       const Pt::Http::Reply& reply)
        : Pt::Http::WebSocketSession(service, stream, reply)
        , _second(false)
        {
            outputReady() += Pt::slot(*this, &TwoFeedSession::onOutputReady);
            outgoing().setType(Pt::Http::WebSocketMessage::Text);
            outgoing().body() << "one";
            beginSend();
        }

    private:
        void onOutputReady(Pt::Http::WebSocketSession&)
        {
            Pt::Http::WebSocketProgress progress = endSend();
            if( ! progress.finished() )
            {
                beginSend();
                return;
            }

            if(_second)
                return;

            _second = true;
            outgoing().clear();
            outgoing().setType(Pt::Http::WebSocketMessage::Text);
            outgoing().body() << "two";
            beginSend();
        }

    private:
        bool _second;
};

typedef Pt::Http::BasicWebSocketService<TwoFeedSession> TwoFeedService;


class DataThenCloseSession : public Pt::Http::WebSocketSession
{
    public:
        DataThenCloseSession(Pt::Http::WebSocketService& service,
                             Pt::Http::Stream& stream,
                             const Pt::Http::Reply& reply)
        : Pt::Http::WebSocketSession(service, stream, reply)
        {
            outputReady() += Pt::slot(*this, &DataThenCloseSession::onOutputReady);
            shutdownReady() += Pt::slot(*this, &DataThenCloseSession::onShutdownReady);
            outgoing().setType(Pt::Http::WebSocketMessage::Text);
            outgoing().body() << "hello";
            beginSend();
        }

    private:
        void onOutputReady(Pt::Http::WebSocketSession&)
        {
            Pt::Http::WebSocketProgress progress = endSend();
            if( ! progress.finished() )
            {
                beginSend();
                return;
            }

            beginShutdown();
        }

        void onShutdownReady(Pt::Http::WebSocketSession&)
        {
            endShutdown();
        }
};

typedef Pt::Http::BasicWebSocketService<DataThenCloseSession> DataThenCloseService;


class TwoFeedThenCloseSession : public Pt::Http::WebSocketSession
{
    public:
        TwoFeedThenCloseSession(Pt::Http::WebSocketService& service,
                                Pt::Http::Stream& stream,
                                const Pt::Http::Reply& reply)
        : Pt::Http::WebSocketSession(service, stream, reply)
        , _second(false)
        {
            outputReady() += Pt::slot(*this, &TwoFeedThenCloseSession::onOutputReady);
            shutdownReady() += Pt::slot(*this, &TwoFeedThenCloseSession::onShutdownReady);
            outgoing().setType(Pt::Http::WebSocketMessage::Text);
            outgoing().body() << "one";
            beginSend();
        }

    private:
        void onOutputReady(Pt::Http::WebSocketSession&)
        {
            Pt::Http::WebSocketProgress progress = endSend();
            if( ! progress.finished() )
            {
                beginSend();
                return;
            }

            if( ! _second )
            {
                _second = true;
                outgoing().clear();
                outgoing().setType(Pt::Http::WebSocketMessage::Text);
                outgoing().body() << "two";
                beginSend();
                return;
            }

            beginShutdown();
        }

        void onShutdownReady(Pt::Http::WebSocketSession&)
        {
            endShutdown();
        }

    private:
        bool _second;
};

typedef Pt::Http::BasicWebSocketService<TwoFeedThenCloseSession> TwoFeedThenCloseService;


class PongSession : public Pt::Http::WebSocketSession
{
    public:
        PongSession(Pt::Http::WebSocketService& service,
                    Pt::Http::Stream& stream,
                    const Pt::Http::Reply& reply,
                    bool& pongFlag,
                    bool& pingReadyFlag,
                    bool& inputReadyFlag,
                    std::string& payload,
                    Pt::System::EventLoop& exitLoop)
        : Pt::Http::WebSocketSession(service, stream, reply)
        , _pong(&pongFlag)
        , _pingReady(&pingReadyFlag)
        , _inputReady(&inputReadyFlag)
        , _payload(&payload)
        , _exitLoop(&exitLoop)
        {
            inputReady() += Pt::slot(*this, &PongSession::onInputReady);
            pingReady() += Pt::slot(*this, &PongSession::onPingReady);
            pong() += Pt::slot(*this, &PongSession::onPong);
            beginPing("session-ping", 12);
            beginReceive();
        }

    private:
        void onInputReady(Pt::Http::WebSocketSession&)
        {
            *_inputReady = true;
            _exitLoop->exit();
        }

        void onPingReady(Pt::Http::WebSocketSession&)
        {
            endPing();
            *_pingReady = true;
            finish();
        }

        void onPong(Pt::Http::WebSocketSession&, const std::string& payload)
        {
            *_payload = payload;
            *_pong = true;
            finish();
        }

    private:
        void finish()
        {
            if(*_pong && *_pingReady)
                _exitLoop->exit();
        }

    private:
        bool* _pong;
        bool* _pingReady;
        bool* _inputReady;
        std::string* _payload;
        Pt::System::EventLoop* _exitLoop;
};


class PongService : public Pt::Http::WebSocketService
{
    public:
        PongService(bool& pong,
                    bool& pingReady,
                    bool& inputReady,
                    std::string& payload,
                    Pt::System::EventLoop& loop)
        : _pong(&pong)
        , _pingReady(&pingReady)
        , _inputReady(&inputReady)
        , _payload(&payload)
        , _loop(&loop)
        {}

    protected:
        virtual Pt::Http::WebSocketSession* onGetSession(Pt::Http::Stream& stream,
                                                         const Pt::Http::Request&,
                                                         const Pt::Http::Reply& reply)
        {
            return new PongSession(*this, stream, reply, *_pong, *_pingReady, *_inputReady,
                                   *_payload, *_loop);
        }

        virtual void onReleaseSession(Pt::Http::WebSocketSession* session)
        {
            delete session;
        }

    private:
        bool* _pong;
        bool* _pingReady;
        bool* _inputReady;
        std::string* _payload;
        Pt::System::EventLoop* _loop;
};


class ShutdownSession : public Pt::Http::WebSocketSession
{
    public:
        ShutdownSession(Pt::Http::WebSocketService& service,
                        Pt::Http::Stream& stream,
                        const Pt::Http::Reply& reply,
                        bool& shutdownReadyFlag,
                        bool& closedFlag,
                        Pt::System::EventLoop& exitLoop)
        : Pt::Http::WebSocketSession(service, stream, reply)
        , _shutdownReady(&shutdownReadyFlag)
        , _closed(&closedFlag)
        , _exitLoop(&exitLoop)
        {
            shutdownReady() += Pt::slot(*this, &ShutdownSession::onShutdownReady);
            beginShutdown();
        }

    private:
        void onShutdownReady(Pt::Http::WebSocketSession&)
        {
            endShutdown();
            *_shutdownReady = true;
            *_closed = true;
            _exitLoop->exit();
        }

    private:
        bool* _shutdownReady;
        bool* _closed;
        Pt::System::EventLoop* _exitLoop;
};


class ShutdownService : public Pt::Http::WebSocketService
{
    public:
        ShutdownService(bool& shutdownReady,
                        bool& closed,
                        Pt::System::EventLoop& loop)
        : _shutdownReady(&shutdownReady)
        , _closed(&closed)
        , _loop(&loop)
        {}

    protected:
        virtual Pt::Http::WebSocketSession* onGetSession(Pt::Http::Stream& stream,
                                                         const Pt::Http::Request&,
                                                         const Pt::Http::Reply& reply)
        {
            return new ShutdownSession(*this, stream, reply, *_shutdownReady, *_closed, *_loop);
        }

        virtual void onReleaseSession(Pt::Http::WebSocketSession* session)
        {
            delete session;
        }

    private:
        bool* _shutdownReady;
        bool* _closed;
        Pt::System::EventLoop* _loop;
};


class PeerCloseSession : public Pt::Http::WebSocketSession
{
    public:
        PeerCloseSession(Pt::Http::WebSocketService& service,
                         Pt::Http::Stream& stream,
                         const Pt::Http::Reply& reply,
                         unsigned code,
                         const std::string& reason)
        : Pt::Http::WebSocketSession(service, stream, reply)
        {
            shutdownReady() += Pt::slot(*this, &PeerCloseSession::onShutdownReady);
            beginShutdown(code, reason);
        }

    private:
        void onShutdownReady(Pt::Http::WebSocketSession&)
        {
            endShutdown();
        }
};


class PeerCloseService : public Pt::Http::WebSocketService
{
    public:
        PeerCloseService(unsigned code, const std::string& reason)
        : _code(code)
        , _reason(reason)
        {}

    protected:
        virtual Pt::Http::WebSocketSession* onGetSession(Pt::Http::Stream& stream,
                                                         const Pt::Http::Request&,
                                                         const Pt::Http::Reply& reply)
        {
            return new PeerCloseSession(*this, stream, reply, _code, _reason);
        }

        virtual void onReleaseSession(Pt::Http::WebSocketSession* session)
        {
            delete session;
        }

    private:
        unsigned _code;
        std::string _reason;
};


class ProtocolSession : public Pt::Http::WebSocketSession
{
    public:
        ProtocolSession(Pt::Http::WebSocketService& service,
                        Pt::Http::Stream& stream,
                        const Pt::Http::Reply& reply,
                        std::string& selected,
                        std::string& offered,
                        std::string& copied,
                        bool& handshake,
                        bool& upgraded,
                        Pt::System::EventLoop& exitLoop)
        : Pt::Http::WebSocketSession(service, stream, reply)
        , _selected(&selected)
        , _handshake(&handshake)
        , _upgraded(&upgraded)
        , _exitLoop(&exitLoop)
        {
            *_selected = protocol();
            *_upgraded = true;
            if(*_handshake)
                _exitLoop->exit();
        }

        const std::string& copiedHeader() const
        { return _copied; }

    private:
        std::string* _selected;
        bool* _handshake;
        bool* _upgraded;
        Pt::System::EventLoop* _exitLoop;
        std::string _copied;
};

class ProtocolService : public Pt::Http::WebSocketService
{
    public:
        ProtocolService(std::string& selected,
                        std::string& offered,
                        std::string& copied,
                        bool& handshake,
                        bool& upgraded,
                        Pt::System::EventLoop& loop)
        : _selected(&selected)
        , _offered(&offered)
        , _copied(&copied)
        , _handshake(&handshake)
        , _upgraded(&upgraded)
        , _loop(&loop)
        , _opened(0)
        {}

        std::size_t opened() const
        { return _opened; }

    protected:
        virtual Pt::Http::WebSocketSession* onGetSession(Pt::Http::Stream& stream,
                                                         const Pt::Http::Request& request,
                                                         const Pt::Http::Reply& reply)
        {
            ++_opened;
            const char* field = request.header().get("Sec-WebSocket-Protocol");
            *_offered = field ? field : "";
            const char* origin = request.header().get("Origin");
            *_copied = origin ? origin : "";
            ProtocolSession* session = new ProtocolSession(*this, stream, reply,
                                                           *_selected, *_offered, *_copied,
                                                           *_handshake, *_upgraded, *_loop);
            return session;
        }

        virtual void onReleaseSession(Pt::Http::WebSocketSession* session)
        {
            delete session;
        }

    private:
        std::string* _selected;
        std::string* _offered;
        std::string* _copied;
        bool* _handshake;
        bool* _upgraded;
        Pt::System::EventLoop* _loop;
        std::size_t _opened;
};

class AcceptResponder : public DeclineResponder
{
    public:
        explicit AcceptResponder(Pt::Http::WebSocketService& service)
        : DeclineResponder(service)
        {}

    protected:
        virtual void onWriteReply(const Pt::Http::Request& request, Pt::Http::Reply& reply,
                                  Pt::System::EventLoop&)
        {
            reply.setStatus(101, "Switching Protocols");
            reply.header().setUpgrade();
            reply.header().set("Upgrade", "websocket");
            std::string challenge(request.header().get("Sec-WebSocket-Key"));
            challenge += "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
            Pt::Http::Sha1 sha1(challenge.data(), challenge.size());
            const std::vector<uint8_t>& digest = sha1.calc();
            std::ostringstream encoded;
            Pt::BasicTextOStream<char, char> base64(encoded, new Pt::Base64Codec());
            base64.write(reinterpret_cast<const char*>(digest.data()), digest.size());
            base64.flush();
            std::string accept = encoded.str();

            const char* field = request.header().get("X-Test-Reply");
            const std::string mode = field ? field : "";
            if(mode == "missing")
            {
                setReady(true);
                return;
            }
            if(mode == "case")
            {
                for(std::size_t index = 0; index < accept.size(); ++index)
                {
                    if(accept[index] >= 'A' && accept[index] <= 'Z')
                    {
                        accept[index] += 'a' - 'A';
                        break;
                    }
                    if(accept[index] >= 'a' && accept[index] <= 'z')
                    {
                        accept[index] -= 'a' - 'A';
                        break;
                    }
                }
            }
            else if(mode == "ows")
                accept = " \t" + accept + "\t ";
            else if(mode == "list")
                accept += ", " + accept;
            else if(mode == "upgrade-other")
                reply.header().set("Upgrade", "h2c");
            else if(mode == "upgrade-missing")
                reply.header().remove("Upgrade");
            else if(mode == "connection-missing")
                reply.header().remove("Connection");
            else if(mode == "extensions")
                reply.header().set("Sec-WebSocket-Extensions", "permessage-deflate");
            else if(mode == "extensions-empty")
                reply.header().set("Sec-WebSocket-Extensions", "");
            else if(mode == "status")
            {
                reply.setStatus(503, "Service Unavailable");
                reply.header().set("Retry-After", "120");
                reply.header().set("Set-Cookie", "retry=1");
                setReady(true);
                return;
            }

            reply.header().set("Sec-WebSocket-Accept", accept.c_str());
            setReady(true);
        }
};


class AcceptService : public ProtocolService
{
    public:
        using ProtocolService::ProtocolService;

    protected:
        virtual Pt::Http::Responder* onGetResponder(const Pt::Http::Request&)
        {
            return new AcceptResponder(*this);
        }

        virtual Pt::Http::WebSocketSession* onGetSession(Pt::Http::Stream& stream,
                                                        const Pt::Http::Request& request,
                                                        const Pt::Http::Reply& reply)
        {
            if(request.header().has("X-Custom"))
            {
                PT_UNIT_ASSERT(request.header().isSet("Authorization", "Bearer test"));
                PT_UNIT_ASSERT(request.header().isSet("Cookie", "session=test"));
                PT_UNIT_ASSERT_EQUALS(request.method(), "GET");
                PT_UNIT_ASSERT(request.qparams().empty());
                PT_UNIT_ASSERT_EQUALS(request.available(), static_cast<std::size_t>(0));

                std::size_t count = 0;
                for(Pt::Http::MessageHeader::FieldIterator field = request.header().begin();
                    field != request.header().end(); ++field)
                {
                    if(std::string(field->name()) == "X-Custom")
                    {
                        const std::string expected = count == 0 ? "first" : "second";
                        PT_UNIT_ASSERT_EQUALS(std::string(field->value()), expected);
                        ++count;
                    }
                }
                PT_UNIT_ASSERT_EQUALS(count, static_cast<std::size_t>(2));
            }

            return ProtocolService::onGetSession(stream, request, reply);
        }
};


class FailSession : public Pt::Http::WebSocketSession
{
    public:
        FailSession(Pt::Http::WebSocketService& service,
                    Pt::Http::Stream& stream,
                    const Pt::Http::Reply& reply,
                    bool& failed,
                    bool& closedFlag,
                    unsigned& code,
                    Pt::System::EventLoop& exitLoop)
        : Pt::Http::WebSocketSession(service, stream, reply)
        , _failed(&failed)
        , _closed(&closedFlag)
        , _code(&code)
        , _exitLoop(&exitLoop)
        {
            inputReady() += Pt::slot(*this, &FailSession::onInputReady);
            beginReceive();
        }

    private:
        void onInputReady(Pt::Http::WebSocketSession&)
        {
            try
            {
                endReceive();
            }
            catch(const std::exception&)
            {
                *_failed = true;
                *_code = closeCode();
                *_closed = true;
                _exitLoop->exit();
                close();
            }
        }

    private:
        bool* _failed;
        bool* _closed;
        unsigned* _code;
        Pt::System::EventLoop* _exitLoop;
};


class FailService : public Pt::Http::WebSocketService
{
    public:
        FailService(bool& failed,
                    bool& closed,
                    unsigned& code,
                    Pt::System::EventLoop& loop)
        : _failed(&failed)
        , _closed(&closed)
        , _code(&code)
        , _loop(&loop)
        {}

    protected:
        virtual Pt::Http::WebSocketSession* onGetSession(Pt::Http::Stream& stream,
                                                         const Pt::Http::Request&,
                                                         const Pt::Http::Reply& reply)
        {
            return new FailSession(*this, stream, reply,
                                   *_failed, *_closed, *_code, *_loop);
        }

        virtual void onReleaseSession(Pt::Http::WebSocketSession* session)
        {
            delete session;
        }

    private:
        bool* _failed;
        bool* _closed;
        unsigned* _code;
        Pt::System::EventLoop* _loop;
};


class CloseNowSession : public Pt::Http::WebSocketSession
{
    public:
        CloseNowSession(Pt::Http::WebSocketService& service,
                        Pt::Http::Stream& stream,
                        const Pt::Http::Reply& reply)
        : Pt::Http::WebSocketSession(service, stream, reply)
        {
            close();
        }
};

class CloseNowService : public Pt::Http::WebSocketService
{
    protected:
        virtual Pt::Http::WebSocketSession* onGetSession(Pt::Http::Stream& stream,
                                                         const Pt::Http::Request&,
                                                         const Pt::Http::Reply& reply)
        {
            return new CloseNowSession(*this, stream, reply);
        }

        virtual void onReleaseSession(Pt::Http::WebSocketSession* session)
        {
            delete session;
        }
};


class DropTcpSession : public Pt::Http::WebSocketSession
{
    public:
        DropTcpSession(Pt::Http::WebSocketService& service,
                       Pt::Http::Stream& stream,
                       const Pt::Http::Reply& reply,
                       Pt::System::EventLoop& loop)
        : Pt::Http::WebSocketSession(service, stream, reply)
        {
            _drop.setActive(loop);
            _drop.timeout() += Pt::slot(*this, &DropTcpSession::onDrop);
            _drop.start(50);
        }

        ~DropTcpSession()
        {
            _drop.detach();
        }

    private:
        void onDrop()
        {
            close();
        }

        Pt::System::Timer _drop;
};

class DropTcpService : public Pt::Http::WebSocketService
{
    public:
        explicit DropTcpService(Pt::System::EventLoop& loop)
        : _loop(&loop)
        {}

    protected:
        virtual Pt::Http::WebSocketSession* onGetSession(Pt::Http::Stream& stream,
                                                         const Pt::Http::Request&,
                                                         const Pt::Http::Reply& reply)
        {
            return new DropTcpSession(*this, stream, reply, *_loop);
        }

        virtual void onReleaseSession(Pt::Http::WebSocketSession* session)
        {
            delete session;
        }

    private:
        Pt::System::EventLoop* _loop;
};


class OversizedSendSession : public Pt::Http::WebSocketSession
{
    public:
        OversizedSendSession(Pt::Http::WebSocketService& service,
                             Pt::Http::Stream& stream,
                             const Pt::Http::Reply& reply,
                             Pt::System::EventLoop& loop)
        : Pt::Http::WebSocketSession(service, stream, reply)
        {
            outputReady() += Pt::slot(*this, &OversizedSendSession::onOutputReady);
            _send.setActive(loop);
            _send.timeout() += Pt::slot(*this, &OversizedSendSession::onSend);
            _send.start(50);
        }

        ~OversizedSendSession()
        {
            _send.detach();
        }

    private:
        void onSend()
        {
            outgoing().setType(Pt::Http::WebSocketMessage::Text);
            outgoing().body() << std::string(64, 'x');
            beginSend();
        }

        void onOutputReady(Pt::Http::WebSocketSession&)
        {
            endSend();
        }

        Pt::System::Timer _send;
};

class InvalidUtf8SendSession : public Pt::Http::WebSocketSession
{
    public:
        InvalidUtf8SendSession(Pt::Http::WebSocketService& service,
                               Pt::Http::Stream& stream,
                               const Pt::Http::Reply& reply,
                               Pt::System::EventLoop& loop)
        : Pt::Http::WebSocketSession(service, stream, reply)
        {
            outputReady() += Pt::slot(*this, &InvalidUtf8SendSession::onOutputReady);
            _send.setActive(loop);
            _send.timeout() += Pt::slot(*this, &InvalidUtf8SendSession::onSend);
            _send.start(50);
        }

        ~InvalidUtf8SendSession()
        {
            _send.detach();
        }

    private:
        void onSend()
        {
            outgoing().setType(Pt::Http::WebSocketMessage::Text);
            const char bad = '\xff';
            outgoing().body().write(&bad, 1);
            beginSend();
        }

        void onOutputReady(Pt::Http::WebSocketSession&)
        {
            endSend();
        }

        Pt::System::Timer _send;
};

class InvalidUtf8SendService : public Pt::Http::WebSocketService
{
    public:
        explicit InvalidUtf8SendService(Pt::System::EventLoop& loop)
        : _loop(&loop)
        {}

    protected:
        virtual Pt::Http::WebSocketSession* onGetSession(Pt::Http::Stream& stream,
                                                         const Pt::Http::Request&,
                                                         const Pt::Http::Reply& reply)
        {
            return new InvalidUtf8SendSession(*this, stream, reply, *_loop);
        }

        virtual void onReleaseSession(Pt::Http::WebSocketSession* session)
        {
            delete session;
        }

    private:
        Pt::System::EventLoop* _loop;
};


class OversizedSendService : public Pt::Http::WebSocketService
{
    public:
        explicit OversizedSendService(Pt::System::EventLoop& loop)
        : _loop(&loop)
        {}

    protected:
        virtual Pt::Http::WebSocketSession* onGetSession(Pt::Http::Stream& stream,
                                                         const Pt::Http::Request&,
                                                         const Pt::Http::Reply& reply)
        {
            return new OversizedSendSession(*this, stream, reply, *_loop);
        }

        virtual void onReleaseSession(Pt::Http::WebSocketSession* session)
        {
            delete session;
        }

    private:
        Pt::System::EventLoop* _loop;
};


class ReceiveSession : public Pt::Http::WebSocketSession
{
    public:
        ReceiveSession(Pt::Http::WebSocketService& service,
                       Pt::Http::Stream& stream,
                       const Pt::Http::Reply& reply)
        : Pt::Http::WebSocketSession(service, stream, reply)
        {
            inputReady() += Pt::slot(*this, &ReceiveSession::onInputReady);
            outputReady() += Pt::slot(*this, &ReceiveSession::onOutputReady);
            beginReceive();
        }

    private:
        void onInputReady(Pt::Http::WebSocketSession&)
        {
            endReceive();
            incoming().clear();
            beginReceive();
        }

        void onOutputReady(Pt::Http::WebSocketSession&)
        {
            endSend();
        }
};

class ReceiveService : public Pt::Http::WebSocketService
{
    public:
        ReceiveService()
        : _opened(0)
        , _released(0)
        {}

        std::size_t live() const
        { return _opened - _released; }

    protected:
        virtual Pt::Http::WebSocketSession* onGetSession(Pt::Http::Stream& stream,
                                                         const Pt::Http::Request&,
                                                         const Pt::Http::Reply& reply)
        {
            ++_opened;
            return new ReceiveSession(*this, stream, reply);
        }

        virtual void onReleaseSession(Pt::Http::WebSocketSession* session)
        {
            ++_released;
            delete session;
        }

    private:
        std::size_t _opened;
        std::size_t _released;
};


class ReadyExceptionService : public Pt::Http::WebSocketService
{
    public:
        explicit ReadyExceptionService(Pt::System::EventLoop& loop)
        : _loop(&loop)
        , _opened(0)
        , _released(0)
        {}

        std::size_t live() const
        { return _opened - _released; }

    protected:
        virtual Pt::Http::WebSocketSession* onGetSession(Pt::Http::Stream& stream,
                                                         const Pt::Http::Request&,
                                                         const Pt::Http::Reply& reply)
        {
            ++_opened;
            return new ReceiveSession(*this, stream, reply);
        }

        virtual void onReleaseSession(Pt::Http::WebSocketSession* session)
        {
            ++_released;
            delete session;
            _loop->exit();
        }

    private:
        Pt::System::EventLoop* _loop;
        std::size_t _opened;
        std::size_t _released;
};


class CloseFromReceiveSession : public Pt::Http::WebSocketSession
{
    public:
        CloseFromReceiveSession(Pt::Http::WebSocketService& service,
                                Pt::Http::Stream& stream,
                                const Pt::Http::Reply& reply)
        : Pt::Http::WebSocketSession(service, stream, reply)
        {
            inputReady() += Pt::slot(*this, &CloseFromReceiveSession::onInputReady);
            beginReceive();
        }

    private:
        void onInputReady(Pt::Http::WebSocketSession&)
        {
            endReceive();
            close();
        }
};

class CloseFromReceiveService : public Pt::Http::WebSocketService
{
    public:
        explicit CloseFromReceiveService(Pt::System::EventLoop& loop)
        : _loop(&loop)
        , _opened(0)
        , _released(0)
        {}

        std::size_t live() const
        { return _opened - _released; }

    protected:
        virtual Pt::Http::WebSocketSession* onGetSession(Pt::Http::Stream& stream,
                                                         const Pt::Http::Request&,
                                                         const Pt::Http::Reply& reply)
        {
            ++_opened;
            return new CloseFromReceiveSession(*this, stream, reply);
        }

        virtual void onReleaseSession(Pt::Http::WebSocketSession* session)
        {
            ++_released;
            delete session;
            _loop->exit();
        }

    private:
        Pt::System::EventLoop* _loop;
        std::size_t _opened;
        std::size_t _released;
};


class CloseFromPongSession : public Pt::Http::WebSocketSession
{
    public:
        CloseFromPongSession(Pt::Http::WebSocketService& service,
                             Pt::Http::Stream& stream,
                             const Pt::Http::Reply& reply)
        : Pt::Http::WebSocketSession(service, stream, reply)
        {
            pong() += Pt::slot(*this, &CloseFromPongSession::onPong);
            beginPing();
            beginReceive();
        }

    private:
        void onPong(Pt::Http::WebSocketSession&, const std::string&)
        {
            close();
        }
};

class CloseFromPongService : public Pt::Http::WebSocketService
{
    public:
        explicit CloseFromPongService(Pt::System::EventLoop& loop)
        : _loop(&loop)
        , _opened(0)
        , _released(0)
        {}

        std::size_t live() const
        { return _opened - _released; }

    protected:
        virtual Pt::Http::WebSocketSession* onGetSession(Pt::Http::Stream& stream,
                                                         const Pt::Http::Request&,
                                                         const Pt::Http::Reply& reply)
        {
            ++_opened;
            return new CloseFromPongSession(*this, stream, reply);
        }

        virtual void onReleaseSession(Pt::Http::WebSocketSession* session)
        {
            ++_released;
            delete session;
            _loop->exit();
        }

    private:
        Pt::System::EventLoop* _loop;
        std::size_t _opened;
        std::size_t _released;
};


class CloseFromShutdownSession : public Pt::Http::WebSocketSession
{
    public:
        CloseFromShutdownSession(Pt::Http::WebSocketService& service,
                                 Pt::Http::Stream& stream,
                                 const Pt::Http::Reply& reply)
        : Pt::Http::WebSocketSession(service, stream, reply)
        {
            shutdownReady() += Pt::slot(*this, &CloseFromShutdownSession::onShutdownReady);
            beginShutdown();
        }

    private:
        void onShutdownReady(Pt::Http::WebSocketSession&)
        {
            endShutdown();
            close();
            close();
        }
};

class CloseFromShutdownService : public Pt::Http::WebSocketService
{
    public:
        explicit CloseFromShutdownService(Pt::System::EventLoop& loop)
        : _loop(&loop)
        , _opened(0)
        , _released(0)
        {}

        std::size_t live() const
        { return _opened - _released; }

    protected:
        virtual Pt::Http::WebSocketSession* onGetSession(Pt::Http::Stream& stream,
                                                         const Pt::Http::Request&,
                                                         const Pt::Http::Reply& reply)
        {
            ++_opened;
            return new CloseFromShutdownSession(*this, stream, reply);
        }

        virtual void onReleaseSession(Pt::Http::WebSocketSession* session)
        {
            ++_released;
            delete session;
            _loop->exit();
        }

    private:
        Pt::System::EventLoop* _loop;
        std::size_t _opened;
        std::size_t _released;
};


class CloseFromClosedSession : public Pt::Http::WebSocketSession
{
    public:
        CloseFromClosedSession(Pt::Http::WebSocketService& service,
                               Pt::Http::Stream& stream,
                               const Pt::Http::Reply& reply)
        : Pt::Http::WebSocketSession(service, stream, reply)
        {
            peerClosed() += Pt::slot(*this, &CloseFromClosedSession::onClosed);
            beginReceive();
        }

    private:
        void onClosed(Pt::Http::WebSocketSession&)
        {
            close();
        }
};

class CloseFromClosedService : public Pt::Http::WebSocketService
{
    public:
        explicit CloseFromClosedService(Pt::System::EventLoop& loop)
        : _loop(&loop)
        , _opened(0)
        , _released(0)
        {}

        std::size_t live() const
        { return _opened - _released; }

    protected:
        virtual Pt::Http::WebSocketSession* onGetSession(Pt::Http::Stream& stream,
                                                         const Pt::Http::Request&,
                                                         const Pt::Http::Reply& reply)
        {
            ++_opened;
            return new CloseFromClosedSession(*this, stream, reply);
        }

        virtual void onReleaseSession(Pt::Http::WebSocketSession* session)
        {
            ++_released;
            delete session;
            _loop->exit();
        }

    private:
        Pt::System::EventLoop* _loop;
        std::size_t _opened;
        std::size_t _released;
};


class CloseTwiceSession : public Pt::Http::WebSocketSession
{
    public:
        CloseTwiceSession(Pt::Http::WebSocketService& service,
                          Pt::Http::Stream& stream,
                          const Pt::Http::Reply& reply)
        : Pt::Http::WebSocketSession(service, stream, reply)
        {
            inputReady() += Pt::slot(*this, &CloseTwiceSession::onInputReady);
            beginReceive();
        }

    private:
        void onInputReady(Pt::Http::WebSocketSession&)
        {
            endReceive();
            close();
            close();
        }
};

class CloseTwiceService : public Pt::Http::WebSocketService
{
    public:
        explicit CloseTwiceService(Pt::System::EventLoop& loop)
        : _loop(&loop)
        , _opened(0)
        , _released(0)
        {}

        std::size_t live() const
        { return _opened - _released; }

    protected:
        virtual Pt::Http::WebSocketSession* onGetSession(Pt::Http::Stream& stream,
                                                         const Pt::Http::Request&,
                                                         const Pt::Http::Reply& reply)
        {
            ++_opened;
            return new CloseTwiceSession(*this, stream, reply);
        }

        virtual void onReleaseSession(Pt::Http::WebSocketSession* session)
        {
            ++_released;
            delete session;
            _loop->exit();
        }

    private:
        Pt::System::EventLoop* _loop;
        std::size_t _opened;
        std::size_t _released;
};


class DestroyOnReceive : public Pt::Connectable
{
    public:
        DestroyOnReceive(Pt::Http::WebSocket*& socket,
                         Pt::System::EventLoop& loop,
                         bool& received)
        : _socket(&socket)
        , _loop(&loop)
        , _received(&received)
        {}

        void onConnected(Pt::Http::WebSocket& socket)
        {
            socket.endConnect();
            socket.beginReceive();
        }

        void onInputReady(Pt::Http::WebSocket& socket)
        {
            socket.endReceive();
            *_received = true;
            delete *_socket;
            *_socket = 0;
            _loop->exit();
        }

    private:
        Pt::Http::WebSocket** _socket;
        Pt::System::EventLoop* _loop;
        bool* _received;
};

class WebSocketTest : public Pt::Unit::TestSuite
                    , public Pt::Connectable
{
    public:
        WebSocketTest()
        : Pt::Unit::TestSuite("Pt::Http::WebSocketTest")
        , _loop(0)
        , _received(false)
        , _declined(false)
        , _closed(false)
        , _partial(false)
        , _finished(false)
        , _sendOpened(false)
        , _pong(false)
        , _inputReady(false)
        , _pingReady(false)
        , _shutdownReady(false)
        , _handshake(false)
        , _upgraded(false)
        , _rejected(false)
        , _status(0)
        , _type(Pt::Http::WebSocketMessage::Unknown)
        , _limitSocket(0)
        , _limitClient(0)
        , _deferredCloseService(0)
        , _ioWait(0)
        , _server(0)
        {
            _waitTimer.timeout() += Pt::slot(*this, &WebSocketTest::onHttpTimeoutWait);
            _deferTimer.timeout() += Pt::slot(*this, &WebSocketTest::onDeferredWait);

            registerMethod("Text", *this, &WebSocketTest::Text);
            registerMethod("Accepted", *this, &WebSocketTest::Accepted);
            registerMethod("NoScope", *this, &WebSocketTest::NoScope);
            registerMethod("Scope", *this, &WebSocketTest::Scope);
            registerMethod("SecondScope", *this, &WebSocketTest::SecondScope);
            registerMethod("BindLoop", *this, &WebSocketTest::BindLoop);
            registerMethod("DetachReleases", *this, &WebSocketTest::DetachReleases);
            registerMethod("PeerClose", *this, &WebSocketTest::PeerClose);
            registerMethod("AutoRelease", *this, &WebSocketTest::AutoRelease);
            registerMethod("DataBeforeClose", *this, &WebSocketTest::DataBeforeClose);
            registerMethod("ClosePartialReceive", *this, &WebSocketTest::ClosePartialReceive);
            registerMethod("Decline", *this, &WebSocketTest::Decline);
            registerMethod("NullSession", *this, &WebSocketTest::NullSession);
            registerMethod("ConstructFails", *this, &WebSocketTest::ConstructFails);
            registerMethod("Push", *this, &WebSocketTest::Push);
            registerMethod("UnreadIncoming", *this, &WebSocketTest::UnreadIncoming);
            registerMethod("DeferredReceive", *this, &WebSocketTest::DeferredReceive);
            registerMethod("UnreadBeforeClose", *this, &WebSocketTest::UnreadBeforeClose);
            registerMethod("DeferredReceiveClose", *this, &WebSocketTest::DeferredReceiveClose);
            registerMethod("Pong", *this, &WebSocketTest::Pong);
            registerMethod("SessionPong", *this, &WebSocketTest::SessionPong);
            registerMethod("SessionShutdown", *this, &WebSocketTest::SessionShutdown);
            registerMethod("PingPayload", *this, &WebSocketTest::PingPayload);
            registerMethod("Limit", *this, &WebSocketTest::Limit);
            registerMethod("Binary", *this, &WebSocketTest::Binary);
            registerMethod("Empty", *this, &WebSocketTest::Empty);
            registerMethod("Large", *this, &WebSocketTest::Large);
            registerMethod("CloseHandshake", *this, &WebSocketTest::CloseHandshake);
            registerMethod("CloseHandshakePeerCode", *this, &WebSocketTest::CloseHandshakePeerCode);
            registerMethod("Close", *this, &WebSocketTest::Close);
            registerMethod("CloseReceive", *this, &WebSocketTest::CloseReceive);
            registerMethod("CloseFromReceive", *this, &WebSocketTest::CloseFromReceive);
            registerMethod("DestroyFromReceive", *this, &WebSocketTest::DestroyFromReceive);
            registerMethod("CloseFromPong", *this, &WebSocketTest::CloseFromPong);
            registerMethod("CloseFromShutdown", *this, &WebSocketTest::CloseFromShutdown);
            registerMethod("CloseFromClosed", *this, &WebSocketTest::CloseFromClosed);
            registerMethod("CloseTwice", *this, &WebSocketTest::CloseTwice);
            registerMethod("CloseDestructor", *this, &WebSocketTest::CloseDestructor);
            registerMethod("CloseNoFrame", *this, &WebSocketTest::CloseNoFrame);
            registerMethod("BeginTwice", *this, &WebSocketTest::BeginTwice);
            registerMethod("EndBeforeReady", *this, &WebSocketTest::EndBeforeReady);
            registerMethod("CloseCodes", *this, &WebSocketTest::CloseCodes);
            registerMethod("HandshakeControl", *this, &WebSocketTest::HandshakeControl);
            registerMethod("HandshakeTimeout", *this, &WebSocketTest::HandshakeTimeout);
            registerMethod("HandshakePending", *this, &WebSocketTest::HandshakePending);
            registerMethod("ConnectedThrows", *this, &WebSocketTest::ConnectedThrows);
            registerMethod("FailedConnectedThrows", *this, &WebSocketTest::FailedConnectedThrows);
            registerMethod("HandshakeRetry", *this, &WebSocketTest::HandshakeRetry);
            registerMethod("HandshakeReconnect", *this, &WebSocketTest::HandshakeReconnect);
            registerMethod("ConnectWhileOpen", *this, &WebSocketTest::ConnectWhileOpen);
            registerMethod("ProtocolClear", *this, &WebSocketTest::ProtocolClear);
            registerMethod("Protocol", *this, &WebSocketTest::Protocol);
            registerMethod("ProtocolNone", *this, &WebSocketTest::ProtocolNone);
            registerMethod("ProtocolRequired", *this, &WebSocketTest::ProtocolRequired);
            registerMethod("ProtocolRejected", *this, &WebSocketTest::ProtocolRejected);
            registerMethod("ProtocolHeader", *this, &WebSocketTest::ProtocolHeader);
            registerMethod("HandshakeHeaders", *this, &WebSocketTest::HandshakeHeaders);
            registerMethod("HandshakeKey", *this, &WebSocketTest::HandshakeKey);
            registerMethod("AcceptMissing", *this, &WebSocketTest::AcceptMissing);
            registerMethod("AcceptEmpty", *this, &WebSocketTest::AcceptEmpty);
            registerMethod("AcceptWrong", *this, &WebSocketTest::AcceptWrong);
            registerMethod("AcceptCase", *this, &WebSocketTest::AcceptCase);
            registerMethod("AcceptWhitespace", *this, &WebSocketTest::AcceptWhitespace);
            registerMethod("AcceptList", *this, &WebSocketTest::AcceptList);
            registerMethod("AcceptRetry", *this, &WebSocketTest::AcceptRetry);
            registerMethod("AcceptVector", *this, &WebSocketTest::AcceptVector);
            registerMethod("HandshakeKeyMissing", *this, &WebSocketTest::HandshakeKeyMissing);
            registerMethod("HandshakeKeyInvalid", *this, &WebSocketTest::HandshakeKeyInvalid);
            registerMethod("HandshakeVersionMissing", *this, &WebSocketTest::HandshakeVersionMissing);
            registerMethod("HandshakeVersionOther", *this, &WebSocketTest::HandshakeVersionOther);
            registerMethod("HandshakeMethodOther", *this, &WebSocketTest::HandshakeMethodOther);
            registerMethod("HandshakeVersionOld", *this, &WebSocketTest::HandshakeVersionOld);
            registerMethod("HandshakeNoUpgrade", *this, &WebSocketTest::HandshakeNoUpgrade);
            registerMethod("HandshakeNoUpgradeKeepAlive", *this, &WebSocketTest::HandshakeNoUpgradeKeepAlive);
            registerMethod("HandshakeConnectionOnly", *this, &WebSocketTest::HandshakeConnectionOnly);
            registerMethod("HandshakeUpgradeOnly", *this, &WebSocketTest::HandshakeUpgradeOnly);
            registerMethod("HandshakeUpgradeProtocol", *this, &WebSocketTest::HandshakeUpgradeProtocol);
            registerMethod("OriginAccepted", *this, &WebSocketTest::OriginAccepted);
            registerMethod("OriginRejected", *this, &WebSocketTest::OriginRejected);
            registerMethod("OriginMissing", *this, &WebSocketTest::OriginMissing);
            registerMethod("OriginUnchecked", *this, &WebSocketTest::OriginUnchecked);
            registerMethod("OriginNullRejected", *this, &WebSocketTest::OriginNullRejected);
            registerMethod("OriginNullAccepted", *this, &WebSocketTest::OriginNullAccepted);
            registerMethod("HandshakeStatus", *this, &WebSocketTest::HandshakeStatus);
            registerMethod("HandshakeUpgradeOther", *this, &WebSocketTest::HandshakeUpgradeOther);
            registerMethod("HandshakeUpgradeMissing", *this, &WebSocketTest::HandshakeUpgradeMissing);
            registerMethod("HandshakeConnectionMissing", *this, &WebSocketTest::HandshakeConnectionMissing);
            registerMethod("HandshakeExtensions", *this, &WebSocketTest::HandshakeExtensions);
            registerMethod("HandshakeExtensionsEmpty", *this, &WebSocketTest::HandshakeExtensionsEmpty);
            registerMethod("HandshakeExtensionsRequested", *this, &WebSocketTest::HandshakeExtensionsRequested);
            registerMethod("HandshakeExtensionsEchoed", *this, &WebSocketTest::HandshakeExtensionsEchoed);
            registerMethod("HandshakeReply", *this, &WebSocketTest::HandshakeReply);
            registerMethod("HandshakeReplyUnfinished", *this, &WebSocketTest::HandshakeReplyUnfinished);
            registerMethod("EndAfterClosed", *this, &WebSocketTest::EndAfterClosed);
            registerMethod("PingAfterShutdown", *this, &WebSocketTest::PingAfterShutdown);
            registerMethod("IdleBeforeConnect", *this, &WebSocketTest::IdleBeforeConnect);
            registerMethod("HttpTimeoutAfterUpgrade", *this, &WebSocketTest::HttpTimeoutAfterUpgrade);
            registerMethod("IdleNotResetBySend", *this, &WebSocketTest::IdleNotResetBySend);
            registerMethod("PingTimeoutNoReply", *this, &WebSocketTest::PingTimeoutNoReply);
            registerMethod("PingTimeoutClearedByPong", *this, &WebSocketTest::PingTimeoutClearedByPong);
            registerMethod("CloseTimeoutNoReply", *this, &WebSocketTest::CloseTimeoutNoReply);
            registerMethod("CloseTimeoutHandshake", *this, &WebSocketTest::CloseTimeoutHandshake);
            registerMethod("ReceiveDuringShutdown", *this, &WebSocketTest::ReceiveDuringShutdown);
            registerMethod("ProtocolFailure", *this, &WebSocketTest::ProtocolFailure);
            registerMethod("ProtocolFailureDuringShutdown", *this, &WebSocketTest::ProtocolFailureDuringShutdown);
            registerMethod("ProtocolFailureDuringShutdownUtf8", *this, &WebSocketTest::ProtocolFailureDuringShutdownUtf8);
            registerMethod("ProtocolFailureReceiveDuringShutdown", *this, &WebSocketTest::ProtocolFailureReceiveDuringShutdown);
            registerMethod("ReadyExceptionReleases", *this, &WebSocketTest::ReadyExceptionReleases);
            registerMethod("IdleTimeoutTwice", *this, &WebSocketTest::IdleTimeoutTwice);
            registerMethod("PingIntervalPong", *this, &WebSocketTest::PingIntervalPong);
            registerMethod("PingIntervalTimeoutNoReply", *this, &WebSocketTest::PingIntervalTimeoutNoReply);
            registerMethod("PingIntervalWithoutReceive", *this, &WebSocketTest::PingIntervalWithoutReceive);
            registerMethod("PingIntervalResetByInbound", *this, &WebSocketTest::PingIntervalResetByInbound);
            registerMethod("PingIntervalYieldsToBeginPing", *this, &WebSocketTest::PingIntervalYieldsToBeginPing);
            registerMethod("InputReadyThrows", *this, &WebSocketTest::InputReadyThrows);
            registerMethod("OutputReadyThrows", *this, &WebSocketTest::OutputReadyThrows);
            registerMethod("ReceiveErrorLeavesLoop", *this, &WebSocketTest::ReceiveErrorLeavesLoop);
            registerMethod("ShutdownWriteError", *this, &WebSocketTest::ShutdownWriteError);
            registerMethod("DeferredReceiveOneSignal", *this, &WebSocketTest::DeferredReceiveOneSignal);
            registerMethod("PeerCloseClosedOnce", *this, &WebSocketTest::PeerCloseClosedOnce);
            registerMethod("ServerCancelReleases", *this, &WebSocketTest::ServerCancelReleases);
            registerMethod("LimitNullSessionReleases", *this, &WebSocketTest::LimitNullSessionReleases);
            registerMethod("LimitConstructFailsReleases", *this, &WebSocketTest::LimitConstructFailsReleases);
            registerMethod("LimitInvalidHandshake", *this, &WebSocketTest::LimitInvalidHandshake);
            registerMethod("HandshakeUnauthorized", *this, &WebSocketTest::HandshakeUnauthorized);
            registerMethod("HandshakeNotFound", *this, &WebSocketTest::HandshakeNotFound);
            registerMethod("HandshakeUnavailable", *this, &WebSocketTest::HandshakeUnavailable);
            registerMethod("HandshakeUntouched", *this, &WebSocketTest::HandshakeUntouched);
            registerMethod("HandshakeSwitching", *this, &WebSocketTest::HandshakeSwitching);
            registerMethod("HandshakeCookie", *this, &WebSocketTest::HandshakeCookie);
            registerMethod("HandshakeFraming", *this, &WebSocketTest::HandshakeFraming);
            registerMethod("HandshakeLimitSkips", *this, &WebSocketTest::HandshakeLimitSkips);
            registerMethod("HandshakeThrows", *this, &WebSocketTest::HandshakeThrows);
        }

        void setUp()
        {
            _loop = new Pt::System::MainLoop();
            _received = false;
            _declined = false;
            _closed = false;
            _partial = false;
            _finished = false;
            _sendOpened = false;
            _pong = false;
            _inputReady = false;
            _pingReady = false;
            _shutdownReady = false;
            _handshake = false;
            _upgraded = false;
            _rejected = false;
            _status = 0;
            _limitSocket = 0;
            _limitClient = 0;
            _deferredCloseService = 0;
            _ioWait = 0;
            _server = 0;
            _message.clear();
            _offered.clear();
            _copied.clear();
            _type = Pt::Http::WebSocketMessage::Unknown;

            _exitTimer.setActive(*_loop);
            _exitTimer.timeout() += Pt::slot(*_loop, &Pt::System::EventLoop::exit);
            _exitTimer.start(5000);
        }

        void tearDown()
        {
            _exitTimer.detach();
            _waitTimer.detach();
            _deferTimer.detach();
            delete _loop;
            _loop = 0;
        }

    protected:
        void Text()
        {
            Pt::Net::Endpoint ep("127.0.0.1", 8011);

            Pt::Http::Server server(*_loop, ep);
            RecordService service(_message, _type, _received, _closed, *_loop);
            Pt::Http::WebSocketServlet sockets(service);

            Pt::Http::MapUrl mapUrl("/ws", service);
            server.addServlet(mapUrl);

            Pt::Http::Client http(*_loop, ep);
            Pt::Http::WebSocket socket(http);
            socket.connected() += Pt::slot(*this, &WebSocketTest::onConnected);
            socket.beginConnect("/ws");

            _loop->run();

            PT_UNIT_ASSERT(_received);
            PT_UNIT_ASSERT_EQUALS(_message, "hello");
            PT_UNIT_ASSERT_EQUALS(_type, Pt::Http::WebSocketMessage::Text);
        }

        void onConnected(Pt::Http::WebSocket& socket)
        {
            socket.endConnect();
            socket.outgoing().setType(Pt::Http::WebSocketMessage::Text);
            socket.outgoing().body() << "hello";
            socket.beginSend();
        }

    protected:
    protected:
        void Accepted()
        {
            Pt::Net::Endpoint ep("127.0.0.1", 8012);

            Pt::Http::Server server(*_loop, ep);
            IdleService service(*_loop);
            Pt::Http::WebSocketServlet sockets(service);

            Pt::Http::MapUrl mapUrl("/ws", service);
            server.addServlet(mapUrl);

            Pt::Http::Client http(*_loop, ep);
            Pt::Http::WebSocket socket(http);
            socket.connected() += Pt::slot(*this, &WebSocketTest::onConnectedIdle);
            socket.beginConnect("/ws");

            _loop->run();

            PT_UNIT_ASSERT_EQUALS(sockets.size(), static_cast<std::size_t>(1));
        }

        void onConnectedIdle(Pt::Http::WebSocket& socket)
        {
            socket.endConnect();
        }

    protected:
        void NoScope()
        {
            Pt::Net::Endpoint ep("127.0.0.1", 8031);

            Pt::Http::Server server(*_loop, ep);
            IdleService service(*_loop, false);

            Pt::Http::MapUrl mapUrl("/ws", service);
            server.addServlet(mapUrl);

            Pt::Http::Client http(*_loop, ep);
            _limitClient = &http;
            Pt::Http::WebSocket socket(http);
            socket.connected() += Pt::slot(*this, &WebSocketTest::onLimitRejected);
            socket.beginConnect("/ws");

            _loop->run();

            PT_UNIT_ASSERT_EQUALS(_status, 503u);
            PT_UNIT_ASSERT_EQUALS(service.opened(), static_cast<std::size_t>(0));
            _limitClient = 0;
        }

    protected:
        void Scope()
        {
            Pt::Net::Endpoint ep("127.0.0.1", 8032);

            Pt::Http::Server server(*_loop, ep);
            IdleService service(*_loop);
            Pt::Http::Client http(*_loop, ep);
            Pt::Http::WebSocket socket(http);

            {
                Pt::Http::WebSocketServlet sockets(service);
                Pt::Http::MapUrl mapUrl("/ws", service);
                server.addServlet(mapUrl);

                socket.connected() += Pt::slot(*this, &WebSocketTest::onConnectedIdle);
                socket.beginConnect("/ws");

                _loop->run();

                PT_UNIT_ASSERT_EQUALS(sockets.size(), static_cast<std::size_t>(1));
                PT_UNIT_ASSERT_EQUALS(service.live(), static_cast<std::size_t>(1));
            }

            PT_UNIT_ASSERT_EQUALS(service.live(), static_cast<std::size_t>(0));
        }

    protected:
        void SecondScope()
        {
            IdleService service(*_loop, false);
            Pt::Http::WebSocketServlet sockets(service);

            PT_UNIT_ASSERT_THROW(createScope(service), std::logic_error);
        }

        void createScope(Pt::Http::WebSocketService& service)
        {
            Pt::Http::WebSocketServlet scope(service);
        }

    protected:
        void BindLoop()
        {
            Pt::Net::Endpoint ep("127.0.0.1", 8202);

            Pt::Http::Server server(*_loop, ep);
            IdleService service(*_loop);
            Pt::Http::WebSocketServlet sockets(service);

            PT_UNIT_ASSERT( ! sockets.loop() );
            PT_UNIT_ASSERT( service.hasServlet() );

            Pt::Http::MapUrl mapUrl("/ws", service);
            server.addServlet(mapUrl);

            Pt::Http::Client http(*_loop, ep);
            Pt::Http::WebSocket socket(http);
            socket.connected() += Pt::slot(*this, &WebSocketTest::onConnectedIdle);
            socket.beginConnect("/ws");

            _loop->run();

            PT_UNIT_ASSERT(sockets.loop() == _loop);
            PT_UNIT_ASSERT_EQUALS(sockets.size(), static_cast<std::size_t>(1));
        }

        void DetachReleases()
        {
            Pt::Net::Endpoint ep("127.0.0.1", 8203);

            Pt::Http::Server server(*_loop, ep);
            IdleService service(*_loop);
            Pt::Http::WebSocketServlet sockets(service);

            Pt::Http::MapUrl mapUrl("/ws", service);
            server.addServlet(mapUrl);

            Pt::Http::Client http(*_loop, ep);
            Pt::Http::WebSocket socket(http);
            socket.connected() += Pt::slot(*this, &WebSocketTest::onConnectedIdle);
            socket.beginConnect("/ws");

            _loop->run();

            PT_UNIT_ASSERT_EQUALS(sockets.size(), static_cast<std::size_t>(1));

            sockets.detach();

            PT_UNIT_ASSERT( ! sockets.loop() );
            PT_UNIT_ASSERT_EQUALS(sockets.size(), static_cast<std::size_t>(0));
            PT_UNIT_ASSERT_EQUALS(service.live(), static_cast<std::size_t>(0));
        }

    protected:
        void PeerClose()
        {
            Pt::Net::Endpoint ep("127.0.0.1", 8013);

            Pt::Http::Server server(*_loop, ep);
            RecordService service(_message, _type, _received, _closed, *_loop);
            Pt::Http::WebSocketServlet sockets(service);

            Pt::Http::MapUrl mapUrl("/ws", service);
            server.addServlet(mapUrl);

            Pt::Http::Client http(*_loop, ep);
            Pt::Http::WebSocket socket(http);
            socket.connected() += Pt::slot(*this, &WebSocketTest::onConnectedClose);
            socket.shutdownReady() += Pt::slot(*this, &WebSocketTest::onShutdownReady);
            socket.beginConnect("/ws");

            _loop->run();

            PT_UNIT_ASSERT(_closed);
            PT_UNIT_ASSERT(_shutdownReady);
            PT_UNIT_ASSERT_EQUALS(service.live(), static_cast<std::size_t>(0));
            PT_UNIT_ASSERT_EQUALS(sockets.size(), static_cast<std::size_t>(0));
        }

        void AutoRelease()
        {
            Pt::Net::Endpoint ep("127.0.0.1", 8059);

            Pt::Http::Server server(*_loop, ep);
            ReceiveService service;

            {
                Pt::Http::WebSocketServlet sockets(service);
                Pt::Http::MapUrl mapUrl("/ws", service);
                server.addServlet(mapUrl);

                Pt::Http::Client http(*_loop, ep);
                Pt::Http::WebSocket socket(http);
                socket.connected() += Pt::slot(*this, &WebSocketTest::onConnectedClose);
                socket.shutdownReady() += Pt::slot(*this, &WebSocketTest::onAutoReleaseShutdown);
                socket.beginConnect("/ws");

                _loop->run();

                PT_UNIT_ASSERT(_shutdownReady);
                PT_UNIT_ASSERT_EQUALS(sockets.size(), static_cast<std::size_t>(0));
            }

            PT_UNIT_ASSERT_EQUALS(service.live(), static_cast<std::size_t>(0));
        }

        void onAutoReleaseShutdown(Pt::Http::WebSocket& socket)
        {
            socket.endShutdown();
            socket.close();
            _shutdownReady = true;
            _loop->exit();
        }

        void onConnectedClose(Pt::Http::WebSocket& socket)
        {
            socket.endConnect();
            socket.beginShutdown();
        }

        void onShutdownReady(Pt::Http::WebSocket& socket)
        {
            socket.endShutdown();
            socket.close();
            _shutdownReady = true;
        }

    protected:
        void DataBeforeClose()
        {
            Pt::Net::Endpoint ep("127.0.0.1", 8036);

            Pt::Http::Server server(*_loop, ep);
            DeferredCloseService service(_message, _type, _received, _closed, *_loop);
            Pt::Http::WebSocketServlet sockets(service);

            Pt::Http::MapUrl mapUrl("/ws", service);
            server.addServlet(mapUrl);

            Pt::Http::Client http(*_loop, ep);
            Pt::Http::WebSocket socket(http);
            socket.connected() += Pt::slot(*this, &WebSocketTest::onConnectedDataBeforeClose);
            socket.outputReady() += Pt::slot(*this, &WebSocketTest::onDataBeforeCloseOutput);
            socket.shutdownReady() += Pt::slot(*this, &WebSocketTest::onDataBeforeCloseShutdown);
            _deferredCloseService = &service;
            socket.beginConnect("/ws");

            _loop->run();

            PT_UNIT_ASSERT(_closed);
            PT_UNIT_ASSERT(_received);
            PT_UNIT_ASSERT_EQUALS(_message, "ignored");
            PT_UNIT_ASSERT_EQUALS(_type, Pt::Http::WebSocketMessage::Text);
            _deferredCloseService = 0;
        }

        void onConnectedDataBeforeClose(Pt::Http::WebSocket& socket)
        {
            socket.endConnect();
            socket.outgoing().setType(Pt::Http::WebSocketMessage::Text);
            socket.outgoing().body() << "ignored";
            socket.beginSend();
        }

        void onDataBeforeCloseOutput(Pt::Http::WebSocket& socket)
        {
            Pt::Http::WebSocketProgress progress = socket.endSend();
            if( ! progress.finished() )
            {
                socket.beginSend();
                return;
            }

            socket.beginShutdown();
            _deferredCloseService->beginDeferredReceive();
        }

        void onDataBeforeCloseShutdown(Pt::Http::WebSocket& socket)
        {
            socket.endShutdown();
            socket.close();
            _closed = true;
            _loop->exit();
        }

    protected:
        void ClosePartialReceive()
        {
            Pt::Net::Endpoint ep("127.0.0.1", 8037);

            Pt::Http::Server server(*_loop, ep);
            PartialCloseService service(_partial, _finished, _closed, *_loop);
            Pt::Http::WebSocketServlet sockets(service);

            Pt::Http::MapUrl mapUrl("/ws", service);
            server.addServlet(mapUrl);

            Pt::Http::Client http(*_loop, ep);
            Pt::Http::WebSocket socket(http);
            socket.connected() += Pt::slot(*this, &WebSocketTest::onConnectedPartialClose);
            socket.outputReady() += Pt::slot(*this, &WebSocketTest::onPartialCloseOutput);
            socket.beginConnect("/ws");

            _loop->run();

            PT_UNIT_ASSERT(_partial);
            PT_UNIT_ASSERT(! _finished);
            PT_UNIT_ASSERT(_closed);
        }

        void onConnectedPartialClose(Pt::Http::WebSocket& socket)
        {
            socket.endConnect();
            socket.outgoing().setType(Pt::Http::WebSocketMessage::Text);
            socket.outgoing().body() << std::string(5000, 'a');
            socket.beginSend();
        }

        void onPartialCloseOutput(Pt::Http::WebSocket& socket)
        {
            Pt::Http::WebSocketProgress progress = socket.endSend();
            if( ! progress.finished() )
                socket.beginShutdown();
        }

    protected:
        void Decline()
        {
            Pt::Net::Endpoint ep("127.0.0.1", 8014);

            Pt::Http::Server server(*_loop, ep);
            DeclineService service;

            Pt::Http::MapUrl mapUrl("/ws", service);
            server.addServlet(mapUrl);

            Pt::Http::Client client(*_loop, ep);
            Pt::Http::WebSocket socket(client);
            socket.connected() += Pt::slot(*this, &WebSocketTest::onConnectedProtocolFailed);
            socket.beginConnect("/ws");

            _loop->run();

            PT_UNIT_ASSERT(_declined);
        }

        void onDeclineConnected(Pt::Http::WebSocket& socket)
        {
            socket.endConnect();
            socket.closed() += Pt::slot(*this, &WebSocketTest::onDeclineClosed);
            socket.beginReceive();
        }

        void onDeclineClosed(Pt::Http::WebSocket& socket)
        {
            socket.close();
            _declined = true;
            _loop->exit();
        }

    protected:
        void NullSession()
        {
            Pt::Net::Endpoint ep("127.0.0.1", 8015);

            Pt::Http::Server server(*_loop, ep);
            NullService service;
            Pt::Http::WebSocketServlet sockets(service);

            Pt::Http::MapUrl mapUrl("/ws", service);
            server.addServlet(mapUrl);

            Pt::Http::Client client(*_loop, ep);
            Pt::Http::WebSocket socket(client);
            socket.connected() += Pt::slot(*this, &WebSocketTest::onDeclineConnected);
            socket.beginConnect("/ws");

            _loop->run();

            PT_UNIT_ASSERT(_declined);
        }

    protected:
        void ConstructFails()
        {
            Pt::Net::Endpoint ep("127.0.0.1", 8016);

            Pt::Http::Server server(*_loop, ep);
            ThrowService service;
            Pt::Http::WebSocketServlet sockets(service);

            Pt::Http::MapUrl mapUrl("/ws", service);
            server.addServlet(mapUrl);

            Pt::Http::Client client(*_loop, ep);
            Pt::Http::WebSocket socket(client);
            socket.connected() += Pt::slot(*this, &WebSocketTest::onDeclineConnected);
            socket.beginConnect("/ws");

            _loop->run();

            PT_UNIT_ASSERT(_declined);
        }

    protected:
        void Push()
        {
            Pt::Net::Endpoint ep("127.0.0.1", 8017);

            Pt::Http::Server server(*_loop, ep);
            PushService service;
            Pt::Http::WebSocketServlet sockets(service);

            Pt::Http::MapUrl mapUrl("/ws", service);
            server.addServlet(mapUrl);

            Pt::Http::Client client(*_loop, ep);
            Pt::Http::WebSocket socket(client);
            socket.connected() += Pt::slot(*this, &WebSocketTest::onPushConnected);
            socket.inputReady() += Pt::slot(*this, &WebSocketTest::onPushInput);
            socket.beginConnect("/ws");

            _loop->run();

            PT_UNIT_ASSERT(_received);
            PT_UNIT_ASSERT_EQUALS(_message, "feed");
            PT_UNIT_ASSERT_EQUALS(_type, Pt::Http::WebSocketMessage::Text);
        }

        void onPushConnected(Pt::Http::WebSocket& socket)
        {
            socket.endConnect();
            socket.beginReceive();
        }

        void onPushInput(Pt::Http::WebSocket& socket)
        {
            socket.endReceive();
            _message.assign( std::istreambuf_iterator<char>( socket.incoming().body() ),
                             std::istreambuf_iterator<char>() );
            _type = socket.incoming().type();
            _received = true;
            _loop->exit();
        }

    protected:
        void UnreadIncoming()
        {
            Pt::Net::Endpoint ep("127.0.0.1", 8061);

            Pt::Http::Server server(*_loop, ep);
            PushService service;
            Pt::Http::WebSocketServlet sockets(service);

            Pt::Http::MapUrl mapUrl("/ws", service);
            server.addServlet(mapUrl);

            Pt::Http::Client client(*_loop, ep);
            Pt::Http::WebSocket socket(client);
            socket.connected() += Pt::slot(*this, &WebSocketTest::onPushConnected);
            socket.inputReady() += Pt::slot(*this, &WebSocketTest::onUnreadIncomingInput);
            socket.beginConnect("/ws");

            _loop->run();

            PT_UNIT_ASSERT(_received);
            _message.assign( std::istreambuf_iterator<char>( socket.incoming().body() ),
                             std::istreambuf_iterator<char>() );
            PT_UNIT_ASSERT_EQUALS(_message, "feed");
            PT_UNIT_ASSERT_EQUALS(socket.incoming().type(),
                                  Pt::Http::WebSocketMessage::Text);
        }

        void onUnreadIncomingInput(Pt::Http::WebSocket& socket)
        {
            socket.endReceive();
            _received = true;
            _loop->exit();
        }

    protected:
        void DeferredReceive()
        {
            Pt::Net::Endpoint ep("127.0.0.1", 8062);

            Pt::Http::Server server(*_loop, ep);
            TwoFeedService service;
            Pt::Http::WebSocketServlet sockets(service);

            Pt::Http::MapUrl mapUrl("/ws", service);
            server.addServlet(mapUrl);

            Pt::Http::Client client(*_loop, ep);
            Pt::Http::WebSocket socket(client);
            _limitSocket = &socket;
            socket.connected() += Pt::slot(*this, &WebSocketTest::onDeferredReceiveConnected);
            socket.inputReady() += Pt::slot(*this, &WebSocketTest::onDeferredReceiveInput);
            socket.beginConnect("/ws");

            _loop->run();

            PT_UNIT_ASSERT(_received);
            PT_UNIT_ASSERT(_finished);
            PT_UNIT_ASSERT_EQUALS(_message, "onetwo");
            _limitSocket = 0;
        }

        void onDeferredReceiveConnected(Pt::Http::WebSocket& socket)
        {
            socket.endConnect();
            _deferTimer.setActive(*_loop);
            _deferTimer.start(100);
        }

        void onDeferredReceiveInput(Pt::Http::WebSocket& socket)
        {
            socket.endReceive();
            _message.append( std::istreambuf_iterator<char>( socket.incoming().body() ),
                             std::istreambuf_iterator<char>() );
            socket.incoming().clear();

            if( ! _received )
            {
                _received = true;
                _deferTimer.start(10);
                return;
            }

            _finished = true;
            _loop->exit();
        }

    protected:
        void UnreadBeforeClose()
        {
            Pt::Net::Endpoint ep("127.0.0.1", 8063);

            Pt::Http::Server server(*_loop, ep);
            DataThenCloseService service;
            Pt::Http::WebSocketServlet sockets(service);

            Pt::Http::MapUrl mapUrl("/ws", service);
            server.addServlet(mapUrl);

            Pt::Http::Client client(*_loop, ep);
            Pt::Http::WebSocket socket(client);
            _limitSocket = &socket;
            socket.connected() += Pt::slot(*this, &WebSocketTest::onDeferredReceiveConnected);
            socket.inputReady() += Pt::slot(*this, &WebSocketTest::onUnreadBeforeCloseInput);
            socket.closed() += Pt::slot(*this, &WebSocketTest::onUnreadBeforeCloseClosed);
            socket.beginConnect("/ws");

            _loop->run();

            PT_UNIT_ASSERT(_received);
            PT_UNIT_ASSERT(_closed);
            PT_UNIT_ASSERT_EQUALS(_message, "hello");
            PT_UNIT_ASSERT_EQUALS(_type, Pt::Http::WebSocketMessage::Text);
            _limitSocket = 0;
        }

        void onUnreadBeforeCloseInput(Pt::Http::WebSocket& socket)
        {
            socket.endReceive();
            _received = true;
            _deferTimer.start(10);
        }

        void onUnreadBeforeCloseClosed(Pt::Http::WebSocket& socket)
        {
            socket.close();
            _closed = true;
            _loop->exit();
        }

    protected:
        void DeferredReceiveClose()
        {
            Pt::Net::Endpoint ep("127.0.0.1", 8064);

            Pt::Http::Server server(*_loop, ep);
            TwoFeedThenCloseService service;
            Pt::Http::WebSocketServlet sockets(service);

            Pt::Http::MapUrl mapUrl("/ws", service);
            server.addServlet(mapUrl);

            Pt::Http::Client client(*_loop, ep);
            Pt::Http::WebSocket socket(client);
            _limitSocket = &socket;
            _partial = true;
            socket.connected() += Pt::slot(*this, &WebSocketTest::onDeferredReceiveConnected);
            socket.inputReady() += Pt::slot(*this, &WebSocketTest::onDeferredReceiveCloseInput);
            socket.closed() += Pt::slot(*this, &WebSocketTest::onUnreadBeforeCloseClosed);
            socket.beginConnect("/ws");

            _loop->run();

            PT_UNIT_ASSERT(_received);
            PT_UNIT_ASSERT(_finished);
            PT_UNIT_ASSERT(_closed);
            PT_UNIT_ASSERT_EQUALS(_copied, "one");
            PT_UNIT_ASSERT_EQUALS(_message, "two");
            _limitSocket = 0;
        }

        void onDeferredReceiveCloseInput(Pt::Http::WebSocket& socket)
        {
            socket.endReceive();

            if( ! _received )
                _received = true;
            else
                _finished = true;

            _deferTimer.start(10);
        }

        void onDeferredWait()
        {
            _deferTimer.stop();

            if( ! _limitSocket )
                return;

            if(_received)
            {
                Pt::Http::WebSocketMessage& incoming = _limitSocket->incoming();
                if( incoming.available() > 0 )
                {
                    std::string body;
                    body.assign( std::istreambuf_iterator<char>( incoming.body() ),
                                 std::istreambuf_iterator<char>() );
                    _type = incoming.type();
                    incoming.clear();

                    if(_partial && _copied.empty())
                        _copied = body;
                    else
                        _message = body;
                }

                if( ! _closed )
                    _limitSocket->beginReceive();

                return;
            }

            _limitSocket->beginReceive();
        }

    protected:
        void Pong()
        {
            Pt::Net::Endpoint ep("127.0.0.1", 8039);

            Pt::Http::Server server(*_loop, ep);
            RecordService service(_message, _type, _received, _closed, *_loop);
            Pt::Http::WebSocketServlet sockets(service);

            Pt::Http::MapUrl mapUrl("/ws", service);
            server.addServlet(mapUrl);

            Pt::Http::Client http(*_loop, ep);
            Pt::Http::WebSocket socket(http);
            socket.connected() += Pt::slot(*this, &WebSocketTest::onConnectedPong);
            socket.inputReady() += Pt::slot(*this, &WebSocketTest::onUnexpectedInput);
            socket.pingReady() += Pt::slot(*this, &WebSocketTest::onPingReady);
            socket.pong() += Pt::slot(*this, &WebSocketTest::onPong);
            socket.beginConnect("/ws");

            _loop->run();

            PT_UNIT_ASSERT(_pong);
            PT_UNIT_ASSERT(_pingReady);
            PT_UNIT_ASSERT(!_inputReady);
            PT_UNIT_ASSERT_EQUALS(_message, std::string("client\0ping", 11));
        }

        void onConnectedPong(Pt::Http::WebSocket& socket)
        {
            socket.endConnect();
            socket.beginPing("client\0ping", 11);
            socket.beginReceive();
        }

        void onPong(Pt::Http::WebSocket&, const std::string& payload)
        {
            _message = payload;
            _pong = true;
            if(_pingReady)
                _loop->exit();
        }

        void onPingReady(Pt::Http::WebSocket& socket)
        {
            socket.endPing();
            _pingReady = true;
            if(_pong)
                _loop->exit();
        }

        void onUnexpectedInput(Pt::Http::WebSocket&)
        {
            _inputReady = true;
            _loop->exit();
        }

    protected:
        void PingPayload()
        {
            Pt::Net::Endpoint ep("127.0.0.1", 8041);

            Pt::Http::Server server(*_loop, ep);
            IdleService service(*_loop, false);
            Pt::Http::WebSocketServlet sockets(service);

            Pt::Http::MapUrl mapUrl("/ws", service);
            server.addServlet(mapUrl);

            Pt::Http::Client http(*_loop, ep);
            Pt::Http::WebSocket socket(http);
            socket.connected() += Pt::slot(*this, &WebSocketTest::onConnectedPingPayload);
            socket.beginConnect("/ws");

            _loop->run();
        }

        void onConnectedPingPayload(Pt::Http::WebSocket& socket)
        {
            socket.endConnect();
            PT_UNIT_ASSERT_THROW(socket.beginPing(0, 1), std::invalid_argument);
            _loop->exit();
        }

    protected:
        void SessionPong()
        {
            Pt::Net::Endpoint ep("127.0.0.1", 8040);

            Pt::Http::Server server(*_loop, ep);
            PongService service(_pong, _pingReady, _inputReady, _message, *_loop);
            Pt::Http::WebSocketServlet sockets(service);

            Pt::Http::MapUrl mapUrl("/ws", service);
            server.addServlet(mapUrl);

            Pt::Http::Client http(*_loop, ep);
            Pt::Http::WebSocket socket(http);
            socket.connected() += Pt::slot(*this, &WebSocketTest::onConnectedSessionPong);
            socket.beginConnect("/ws");

            _loop->run();

            PT_UNIT_ASSERT(_pong);
            PT_UNIT_ASSERT(_pingReady);
            PT_UNIT_ASSERT(!_inputReady);
            PT_UNIT_ASSERT_EQUALS(_message, "session-ping");
        }

        void onConnectedSessionPong(Pt::Http::WebSocket& socket)
        {
            socket.endConnect();
            socket.beginReceive();
        }

    protected:
        void SessionShutdown()
        {
            Pt::Net::Endpoint ep("127.0.0.1", 8042);

            Pt::Http::Server server(*_loop, ep);
            ShutdownService service(_shutdownReady, _closed, *_loop);
            Pt::Http::WebSocketServlet sockets(service);

            Pt::Http::MapUrl mapUrl("/ws", service);
            server.addServlet(mapUrl);

            Pt::Http::Client http(*_loop, ep);
            Pt::Http::WebSocket socket(http);
            socket.connected() += Pt::slot(*this, &WebSocketTest::onConnectedSessionShutdown);
            socket.beginConnect("/ws");

            _loop->run();

            PT_UNIT_ASSERT(_shutdownReady);
            PT_UNIT_ASSERT(_closed);
            PT_UNIT_ASSERT_EQUALS(sockets.size(), static_cast<std::size_t>(0));
        }

        void onConnectedSessionShutdown(Pt::Http::WebSocket& socket)
        {
            socket.endConnect();
            socket.beginReceive();
        }

    protected:
        void Limit()
        {
            Pt::Net::Endpoint ep("127.0.0.1", 8018);

            Pt::Http::Server server(*_loop, ep);
            IdleService service(*_loop, false);
            service.setMaxSockets(1);
            Pt::Http::WebSocketServlet sockets(service);

            Pt::Http::MapUrl mapUrl("/ws", service);
            server.addServlet(mapUrl);

            Pt::Http::Client first(*_loop, ep);
            Pt::Http::WebSocket socket(first);

            Pt::Http::Client second(*_loop, ep);
            Pt::Http::WebSocket rejected(second);
            _limitSocket = &rejected;
            _limitClient = &second;

            socket.connected() += Pt::slot(*this, &WebSocketTest::onLimitConnected);
            rejected.connected() += Pt::slot(*this, &WebSocketTest::onLimitRejected);
            socket.beginConnect("/ws");

            _loop->run();

            PT_UNIT_ASSERT_EQUALS(_status, 503u);
            PT_UNIT_ASSERT_EQUALS(sockets.size(), static_cast<std::size_t>(1));
            _limitSocket = 0;
            _limitClient = 0;
        }

        void onLimitConnected(Pt::Http::WebSocket& socket)
        {
            socket.endConnect();
            _limitSocket->beginConnect("/ws");
        }

        void onLimitRejected(Pt::Http::WebSocket& socket)
        {
            PT_UNIT_ASSERT_THROW(socket.endConnect(), std::exception);

            if(_limitClient)
                _status = _limitClient->reply().statusCode();

            _loop->exit();
        }

    protected:
        void LimitNullSessionReleases()
        {
            checkLimitReleases<NullService>(8301);
        }

        void LimitConstructFailsReleases()
        {
            checkLimitReleases<ThrowService>(8302);
        }

        // A handshake whose session is declined or fails to construct
        // must give its place back, so the next handshake is accepted.
        template <typename ServiceT>
        void checkLimitReleases(unsigned short port)
        {
            Pt::Net::Endpoint ep("127.0.0.1", port);

            Pt::Http::Server server(*_loop, ep);
            ServiceT service;
            service.setMaxSockets(1);
            Pt::Http::WebSocketServlet sockets(service);

            Pt::Http::MapUrl mapUrl("/ws", service);
            server.addServlet(mapUrl);

            Pt::Http::Client first(*_loop, ep);
            Pt::Http::WebSocket socket(first);

            Pt::Http::Client second(*_loop, ep);
            Pt::Http::WebSocket again(second);
            _limitSocket = &again;
            _limitClient = &second;

            socket.connected() += Pt::slot(*this, &WebSocketTest::onSlotFirstConnected);
            again.connected() += Pt::slot(*this, &WebSocketTest::onSlotSecondConnected);
            socket.beginConnect("/ws");

            _loop->run();

            PT_UNIT_ASSERT_EQUALS(_status, 101u);
            _limitSocket = 0;
            _limitClient = 0;
        }

        void onSlotFirstConnected(Pt::Http::WebSocket& socket)
        {
            socket.endConnect();
            socket.closed() += Pt::slot(*this, &WebSocketTest::onSlotFirstClosed);
            socket.beginReceive();
        }

        void onSlotFirstClosed(Pt::Http::WebSocket& socket)
        {
            socket.close();
            _limitSocket->beginConnect("/ws");
        }

        void onSlotSecondConnected(Pt::Http::WebSocket& socket)
        {
            try
            {
                socket.endConnect();
            }
            catch(const std::exception&)
            {}

            _status = _limitClient->reply().statusCode();
            _loop->exit();
        }

        void HandshakeUnauthorized()
        {
            checkHandshakeDecision(8311, HandshakePolicyService::Unauthorized, 401u);
            PT_UNIT_ASSERT_EQUALS(_copied, "Bearer");
            PT_UNIT_ASSERT(_message.empty());
        }

        void HandshakeNotFound()
        {
            checkHandshakeDecision(8312, HandshakePolicyService::NotFound, 404u);
        }

        void HandshakeUnavailable()
        {
            Pt::Net::Endpoint ep("127.0.0.1", 8313);
            Pt::Http::Server server(*_loop, ep);
            HandshakePolicyService service(HandshakePolicyService::Unavailable);
            Pt::Http::WebSocketServlet sockets(service);
            Pt::Http::MapUrl mapUrl("/ws", service);
            server.addServlet(mapUrl);

            Pt::Http::Client http(*_loop, ep);
            Pt::Http::WebSocket socket(http);
            _limitClient = &http;
            socket.connected() += Pt::slot(*this, &WebSocketTest::onHandshakeDeclined);
            socket.beginConnect("/ws");
            _loop->run();

            PT_UNIT_ASSERT_EQUALS(_status, 503u);
            PT_UNIT_ASSERT_EQUALS(_copied, "120");
            PT_UNIT_ASSERT_EQUALS(_message, "later");
            PT_UNIT_ASSERT_EQUALS(service.asked(), static_cast<std::size_t>(1));
            PT_UNIT_ASSERT_EQUALS(service.socketCount(), static_cast<std::size_t>(0));
            _limitClient = 0;
        }

        void HandshakeUntouched()
        {
            checkHandshakeDecision(8314, HandshakePolicyService::Untouched, 500u);
            PT_UNIT_ASSERT(_copied.empty());
            PT_UNIT_ASSERT(_message.empty());
        }

        void HandshakeSwitching()
        {
            checkHandshakeDecision(8315, HandshakePolicyService::Switching, 500u);
            PT_UNIT_ASSERT(_copied.empty());
            PT_UNIT_ASSERT( ! _handshake );
        }

        void HandshakeCookie()
        {
            Pt::Net::Endpoint ep("127.0.0.1", 8316);
            Pt::Http::Server server(*_loop, ep);
            HandshakeHeaderService service(_message, _received, *_loop);
            Pt::Http::WebSocketServlet sockets(service);
            Pt::Http::MapUrl mapUrl("/ws", service);
            server.addServlet(mapUrl);

            Pt::Http::Client http(*_loop, ep);
            Pt::Http::WebSocket socket(http);
            _limitClient = &http;
            _limitSocket = &socket;
            socket.connected() += Pt::slot(*this, &WebSocketTest::onHandshakeCookie);
            socket.beginConnect("/ws");
            _loop->run();

            PT_UNIT_ASSERT_EQUALS(_status, 101u);
            PT_UNIT_ASSERT_EQUALS(_copied, "session=1");
            PT_UNIT_ASSERT(_received);
            PT_UNIT_ASSERT_EQUALS(_message, "frame");
            _limitClient = 0;
            _limitSocket = 0;
        }

        void HandshakeFraming()
        {
            Pt::Net::Endpoint ep("127.0.0.1", 8317);
            Pt::Http::Server server(*_loop, ep);
            HandshakePolicyService service(HandshakePolicyService::Framed);
            Pt::Http::WebSocketServlet sockets(service);
            Pt::Http::MapUrl mapUrl("/ws", service);
            server.addServlet(mapUrl);

            Pt::Net::TcpSocket raw(*_loop);
            raw.connected() += Pt::slot(*this, &WebSocketTest::onRawConnected);
            raw.outputReady() += Pt::slot(*this, &WebSocketTest::onRawWritten);
            raw.inputReady() += Pt::slot(*this, &WebSocketTest::onRawInput);
            raw.beginConnect(ep);
            _loop->run();

            PT_UNIT_ASSERT(_received);
            const std::string::size_type headerEnd = _message.find("\r\n\r\n");
            PT_UNIT_ASSERT(headerEnd != std::string::npos);
            const std::string header = _message.substr(0, headerEnd);
            PT_UNIT_ASSERT_EQUALS(countHeader(header, "Content-Length"), static_cast<std::size_t>(1));
            PT_UNIT_ASSERT_EQUALS(countHeader(header, "Transfer-Encoding"), static_cast<std::size_t>(0));
            PT_UNIT_ASSERT(header.find("Content-Length: 2\r\n") != std::string::npos);
            PT_UNIT_ASSERT_EQUALS(_message.substr(headerEnd + 4), "no");
        }

        void HandshakeLimitSkips()
        {
            Pt::Net::Endpoint ep("127.0.0.1", 8318);
            Pt::Http::Server server(*_loop, ep);
            HandshakePolicyService service(HandshakePolicyService::Accept);
            service.setMaxSockets(1);
            Pt::Http::WebSocketServlet sockets(service);
            Pt::Http::MapUrl mapUrl("/ws", service);
            server.addServlet(mapUrl);

            Pt::Http::Client first(*_loop, ep);
            Pt::Http::WebSocket held(first);
            Pt::Http::Client second(*_loop, ep);
            Pt::Http::WebSocket rejected(second);
            _limitClient = &second;
            service.follow(rejected);
            held.connected() += Pt::slot(*this, &WebSocketTest::onConnectedIdle);
            rejected.connected() += Pt::slot(*this, &WebSocketTest::onHandshakeDeclined);
            held.beginConnect("/ws");
            _loop->run();

            PT_UNIT_ASSERT_EQUALS(_status, 503u);
            PT_UNIT_ASSERT_EQUALS(service.asked(), static_cast<std::size_t>(1));
            PT_UNIT_ASSERT_EQUALS(service.socketCount(), static_cast<std::size_t>(1));
            _limitClient = 0;
        }

        void HandshakeThrows()
        {
            Pt::Net::Endpoint ep("127.0.0.1", 8319);
            Pt::Http::Server server(*_loop, ep);
            HandshakePolicyService service(HandshakePolicyService::Throw);
            Pt::Http::WebSocketServlet sockets(service);
            Pt::Http::MapUrl mapUrl("/ws", service);
            server.addServlet(mapUrl);

            Pt::Http::Client http(*_loop, ep);
            Pt::Http::WebSocket socket(http);
            socket.beginConnect("/ws");
            PT_UNIT_ASSERT_THROW(_loop->run(), std::runtime_error);

            PT_UNIT_ASSERT_EQUALS(service.asked(), static_cast<std::size_t>(1));
            PT_UNIT_ASSERT_EQUALS(service.socketCount(), static_cast<std::size_t>(0));
        }

        void checkHandshakeDecision(unsigned short port,
                                    HandshakePolicyService::Decision decision,
                                    unsigned status)
        {
            Pt::Net::Endpoint ep("127.0.0.1", port);
            Pt::Http::Server server(*_loop, ep);
            HandshakePolicyService service(decision);
            Pt::Http::WebSocketServlet sockets(service);
            Pt::Http::MapUrl mapUrl("/ws", service);
            server.addServlet(mapUrl);

            Pt::Http::Client http(*_loop, ep);
            Pt::Http::WebSocket socket(http);
            _limitClient = &http;
            socket.connected() += Pt::slot(*this, &WebSocketTest::onHandshakeDeclined);
            socket.beginConnect("/ws");
            _loop->run();

            PT_UNIT_ASSERT_EQUALS(_status, status);
            PT_UNIT_ASSERT_EQUALS(service.socketCount(), static_cast<std::size_t>(0));
            PT_UNIT_ASSERT_EQUALS(service.asked(), static_cast<std::size_t>(1));
            socket.outgoing().setType(Pt::Http::WebSocketMessage::Text);
            socket.outgoing().body() << "not sent";
            PT_UNIT_ASSERT_THROW(socket.beginSend(), std::logic_error);
            PT_UNIT_ASSERT_THROW(socket.beginReceive(), std::logic_error);
            _limitClient = 0;
        }

        void onHandshakeDeclined(Pt::Http::WebSocket& socket)
        {
            PT_UNIT_ASSERT_THROW(socket.endConnect(), Pt::Http::HttpError);
            if( ! _limitClient )
                return;

            Pt::Http::Reply& reply = _limitClient->reply();
            _status = reply.statusCode();
            const char* authenticate = reply.header().get("WWW-Authenticate");
            const char* retryAfter = reply.header().get("Retry-After");
            const char* accept = reply.header().get("Sec-WebSocket-Accept");
            if(authenticate)
                _copied = authenticate;
            else if(retryAfter)
                _copied = retryAfter;

            _handshake = accept != 0 || reply.header().isUpgrade();
            _message.assign( std::istreambuf_iterator<char>( reply.body() ),
                             std::istreambuf_iterator<char>() );
            _loop->exit();
        }

        void onHandshakeThrown(Pt::Http::WebSocket& socket)
        {
            PT_UNIT_ASSERT_THROW(socket.endConnect(), std::exception);
            if(_limitClient)
                PT_UNIT_ASSERT(_limitClient->reply().statusCode() != 404u);

            _declined = true;
            _loop->exit();
        }

        void onHandshakeCookie(Pt::Http::WebSocket& socket)
        {
            socket.endConnect();
            _status = _limitClient->reply().statusCode();
            const char* cookie = _limitClient->reply().header().get("Set-Cookie");
            if(cookie)
                _copied = cookie;

            _message.clear();
            socket.inputReady() += Pt::slot(*this, &WebSocketTest::onHandshakeFrame);
            socket.outputReady() += Pt::slot(*this, &WebSocketTest::onHandshakeFrameSent);
            socket.outgoing().setType(Pt::Http::WebSocketMessage::Text);
            socket.outgoing().body() << "frame";
            socket.beginSend();
        }

        void onHandshakeFrameSent(Pt::Http::WebSocket& socket)
        {
            socket.endSend();
        }

        void onHandshakeFrame(Pt::Http::WebSocket& socket)
        {
            socket.endReceive();
            _message.assign( std::istreambuf_iterator<char>( socket.incoming().body() ),
                             std::istreambuf_iterator<char>() );
            _received = true;
            _loop->exit();
        }

        void onHandshakeAccepted(Pt::Http::WebSocket& socket)
        {
            socket.endConnect();
            _received = true;
            _loop->exit();
        }

        void onRawConnected(Pt::Net::TcpSocket& socket)
        {
            socket.endConnect();
            const char request[] =
                "GET /ws HTTP/1.1\r\n"
                "Host: 127.0.0.1\r\n"
                "Connection: Upgrade\r\n"
                "Upgrade: websocket\r\n"
                "Sec-WebSocket-Version: 13\r\n"
                "Sec-WebSocket-Key: dGhlIHNhbXBsZSBub25jZQ==\r\n"
                "\r\n";
            socket.beginWrite(request, sizeof(request) - 1);
        }

        void onRawWritten(Pt::System::IODevice& device)
        {
            device.endWrite();
            device.beginRead(_raw, sizeof(_raw));
        }

        void onRawInput(Pt::System::IODevice& device)
        {
            const std::size_t n = device.endRead();
            _message.append(_raw, n);
            if( _message.find("\r\n\r\n") == std::string::npos
                || _message.size() < _message.find("\r\n\r\n") + 6 )
            {
                device.beginRead(_raw, sizeof(_raw));
                return;
            }

            _received = true;
            _loop->exit();
        }

        static std::size_t countHeader(const std::string& header, const char* name)
        {
            const std::string field = std::string(name) + ":";
            std::size_t count = 0;
            std::string::size_type at = 0;
            while( (at = header.find(field, at)) != std::string::npos )
            {
                ++count;
                at += field.size();
            }
            return count;
        }

        // An invalid handshake is declined before it takes a place from the limit.
        void LimitInvalidHandshake()
        {
            Pt::Net::Endpoint ep("127.0.0.1", 8303);

            Pt::Http::Server server(*_loop, ep);
            IdleService service(*_loop, false);
            service.setMaxSockets(1);
            Pt::Http::WebSocketServlet sockets(service);

            Pt::Http::MapUrl mapUrl("/ws", service);
            server.addServlet(mapUrl);

            Pt::Http::Client first(*_loop, ep);
            Pt::Http::WebSocket socket(first);

            Pt::Http::Client invalid(*_loop, ep);
            HandshakeObserver observer(*_loop);
            invalid.replyReceived() += Pt::slot(observer, &HandshakeObserver::onReply);
            invalid.request().setUrl("/ws");
            invalid.request().header().setUpgrade();
            invalid.request().header().set("Upgrade", "websocket");
            invalid.request().header().set("Sec-WebSocket-Version", "13");
            _limitClient = &invalid;

            socket.connected() += Pt::slot(*this, &WebSocketTest::onInvalidFirstConnected);
            socket.beginConnect("/ws");

            _loop->run();

            PT_UNIT_ASSERT_EQUALS(observer.count(), static_cast<std::size_t>(1));
            PT_UNIT_ASSERT_EQUALS(invalid.reply().statusCode(), 400u);
            PT_UNIT_ASSERT_EQUALS(sockets.size(), static_cast<std::size_t>(1));
            _limitClient = 0;
        }

        void onInvalidFirstConnected(Pt::Http::WebSocket& socket)
        {
            socket.endConnect();
            _limitClient->beginReceive();
        }

    protected:
        void Binary()
        {
            Pt::Net::Endpoint ep("127.0.0.1", 8019);

            Pt::Http::Server server(*_loop, ep);
            RecordService service(_message, _type, _received, _closed, *_loop);
            Pt::Http::WebSocketServlet sockets(service);

            Pt::Http::MapUrl mapUrl("/ws", service);
            server.addServlet(mapUrl);

            Pt::Http::Client http(*_loop, ep);
            Pt::Http::WebSocket socket(http);
            socket.connected() += Pt::slot(*this, &WebSocketTest::onConnectedBinary);
            socket.beginConnect("/ws");

            _loop->run();

            PT_UNIT_ASSERT(_received);
            PT_UNIT_ASSERT_EQUALS(_message, std::string("\x00\x01\x02", 3));
            PT_UNIT_ASSERT_EQUALS(_type, Pt::Http::WebSocketMessage::Binary);
        }

        void onConnectedBinary(Pt::Http::WebSocket& socket)
        {
            socket.endConnect();
            socket.outgoing().setType(Pt::Http::WebSocketMessage::Binary);
            socket.outgoing().body().write("\x00\x01\x02", 3);
            socket.beginSend();
        }

    protected:
        void Empty()
        {
            Pt::Net::Endpoint ep("127.0.0.1", 8020);

            Pt::Http::Server server(*_loop, ep);
            CollectService service(_message, _type, _received, *_loop);
            Pt::Http::WebSocketServlet sockets(service);

            Pt::Http::MapUrl mapUrl("/ws", service);
            server.addServlet(mapUrl);

            Pt::Http::Client http(*_loop, ep);
            Pt::Http::WebSocket socket(http);
            socket.connected() += Pt::slot(*this, &WebSocketTest::onConnectedEmpty);
            socket.beginConnect("/ws");

            _loop->run();

            PT_UNIT_ASSERT(_received);
            PT_UNIT_ASSERT(_message.empty());
            PT_UNIT_ASSERT_EQUALS(_type, Pt::Http::WebSocketMessage::Text);
        }

        void onConnectedEmpty(Pt::Http::WebSocket& socket)
        {
            socket.endConnect();
            socket.outgoing().setType(Pt::Http::WebSocketMessage::Text);
            socket.beginSend();
        }

    protected:
        void Large()
        {
            Pt::Net::Endpoint ep("127.0.0.1", 8021);

            Pt::Http::Server server(*_loop, ep);
            CollectService service(_message, _type, _received, *_loop);
            Pt::Http::WebSocketServlet sockets(service);

            Pt::Http::MapUrl mapUrl("/ws", service);
            server.addServlet(mapUrl);

            Pt::Http::Client http(*_loop, ep);
            Pt::Http::WebSocket socket(http);
            _limitSocket = &socket;
            socket.connected() += Pt::slot(*this, &WebSocketTest::onConnectedLarge);
            socket.outputReady() += Pt::slot(*this, &WebSocketTest::onLargeOutput);
            socket.beginConnect("/ws");

            _loop->run();

            PT_UNIT_ASSERT(_received);
            PT_UNIT_ASSERT_EQUALS(_message.size(), static_cast<std::size_t>(5000));
            PT_UNIT_ASSERT_EQUALS(_type, Pt::Http::WebSocketMessage::Text);
            _limitSocket = 0;
        }

        void onConnectedLarge(Pt::Http::WebSocket& socket)
        {
            socket.endConnect();
            socket.outgoing().setType(Pt::Http::WebSocketMessage::Text);
            socket.outgoing().body() << std::string(5000, 'a');
            socket.beginSend();
        }

        void onLargeOutput(Pt::Http::WebSocket& socket)
        {
            Pt::Http::WebSocketProgress progress = socket.endSend();
            if( ! _sendOpened )
            {
                PT_UNIT_ASSERT(progress.header());
                PT_UNIT_ASSERT( ! progress.continuation() );
                _sendOpened = true;
            }
            else
            {
                PT_UNIT_ASSERT(progress.continuation());
                PT_UNIT_ASSERT( ! progress.header() );
            }
            if( ! progress.finished() )
                socket.beginSend();
        }

    protected:
        void CloseHandshake()
        {
            Pt::Net::Endpoint ep("127.0.0.1", 8022);

            Pt::Http::Server server(*_loop, ep);
            RecordService service(_message, _type, _received, _closed, *_loop);
            Pt::Http::WebSocketServlet sockets(service);

            Pt::Http::MapUrl mapUrl("/ws", service);
            server.addServlet(mapUrl);

            Pt::Http::Client http(*_loop, ep);
            Pt::Http::WebSocket socket(http);
            socket.connected() += Pt::slot(*this, &WebSocketTest::onConnectedCloseCode);
            socket.beginConnect("/ws");

            _loop->run();

            PT_UNIT_ASSERT(_closed);
        }

        void onConnectedCloseCode(Pt::Http::WebSocket& socket)
        {
            socket.endConnect();
            socket.beginShutdown(1000, "bye");
        }

    protected:
        void CloseHandshakePeerCode()
        {
            Pt::Net::Endpoint ep("127.0.0.1", 8130);

            Pt::Http::Server server(*_loop, ep);
            PeerCloseService service(1001, "going away");
            Pt::Http::WebSocketServlet sockets(service);

            Pt::Http::MapUrl mapUrl("/ws", service);
            server.addServlet(mapUrl);

            Pt::Http::Client http(*_loop, ep);
            Pt::Http::WebSocket socket(http);
            socket.connected() += Pt::slot(*this, &WebSocketTest::onConnectedPeerCode);
            socket.shutdownReady() += Pt::slot(*this, &WebSocketTest::onPeerCodeShutdownReady);
            socket.beginConnect("/ws");

            _loop->run();

            PT_UNIT_ASSERT(_shutdownReady);
            PT_UNIT_ASSERT_EQUALS(_status, 1001u);
            PT_UNIT_ASSERT_EQUALS(_message, std::string("going away"));
        }

        void onConnectedPeerCode(Pt::Http::WebSocket& socket)
        {
            socket.endConnect();
            socket.beginShutdown(1000, "done");
            PT_UNIT_ASSERT_EQUALS(socket.closeCode(), 0u);
            PT_UNIT_ASSERT(socket.closeReason().empty());
        }

        void onPeerCodeShutdownReady(Pt::Http::WebSocket& socket)
        {
            socket.endShutdown();
            _status = socket.closeCode();
            _message = socket.closeReason();
            socket.close();
            _shutdownReady = true;
            _loop->exit();
        }

    protected:
        void Close()
        {
            Pt::Net::Endpoint ep("127.0.0.1", 8031);

            Pt::Http::Server server(*_loop, ep);
            IdleService service(*_loop, false);
            Pt::Http::WebSocketServlet sockets(service);

            Pt::Http::MapUrl mapUrl("/ws", service);
            server.addServlet(mapUrl);

            Pt::Http::Client http(*_loop, ep);
            Pt::Http::WebSocket socket(http);
            socket.connected() += Pt::slot(*this, &WebSocketTest::onConnectedAbort);
            socket.beginConnect("/ws");

            _loop->run();

            PT_UNIT_ASSERT(!_declined);
        }

        void onConnectedAbort(Pt::Http::WebSocket& socket)
        {
            socket.endConnect();
            socket.close();
            PT_UNIT_ASSERT_EQUALS(socket.closeCode(), 1006u);
            PT_UNIT_ASSERT(socket.closeReason().empty());
            PT_UNIT_ASSERT_THROW(socket.beginSend(), std::logic_error);
            PT_UNIT_ASSERT_THROW(socket.beginReceive(), std::logic_error);
            PT_UNIT_ASSERT_THROW(socket.beginPing(), std::logic_error);
            PT_UNIT_ASSERT_THROW(socket.beginShutdown(), std::logic_error);
            socket.close();
            _loop->exit();
        }

    protected:
        void CloseReceive()
        {
            Pt::Net::Endpoint ep("127.0.0.1", 8032);

            Pt::Http::Server server(*_loop, ep);
            IdleService service(*_loop, false);
            Pt::Http::WebSocketServlet sockets(service);

            Pt::Http::MapUrl mapUrl("/ws", service);
            server.addServlet(mapUrl);

            Pt::Http::Client http(*_loop, ep);
            Pt::Http::WebSocket socket(http);
            socket.connected() += Pt::slot(*this, &WebSocketTest::onConnectedCloseReceive);
            socket.beginConnect("/ws");

            _loop->run();
        }

        void onConnectedCloseReceive(Pt::Http::WebSocket& socket)
        {
            socket.endConnect();
            socket.beginReceive();
            socket.close();
            PT_UNIT_ASSERT_THROW(socket.beginReceive(), std::logic_error);
            PT_UNIT_ASSERT_EQUALS(socket.closeCode(), 1006u);
            _loop->exit();
        }

    protected:
        void CloseFromReceive()
        {
            Pt::Net::Endpoint ep("127.0.0.1", 8065);

            Pt::Http::Server server(*_loop, ep);
            CloseFromReceiveService service(*_loop);
            Pt::Http::WebSocketServlet sockets(service);

            Pt::Http::MapUrl mapUrl("/ws", service);
            server.addServlet(mapUrl);

            Pt::Http::Client http(*_loop, ep);
            Pt::Http::WebSocket socket(http);
            socket.connected() += Pt::slot(*this, &WebSocketTest::onConnected);
            socket.beginConnect("/ws");

            _loop->run();

            PT_UNIT_ASSERT_EQUALS(service.live(), static_cast<std::size_t>(0));
            PT_UNIT_ASSERT_EQUALS(sockets.size(), static_cast<std::size_t>(0));
        }

        void DestroyFromReceive()
        {
            Pt::Net::Endpoint ep("127.0.0.1", 8066);

            Pt::Http::Server server(*_loop, ep);
            PushService service;
            Pt::Http::WebSocketServlet sockets(service);

            Pt::Http::MapUrl mapUrl("/ws", service);
            server.addServlet(mapUrl);

            Pt::Http::Client http(*_loop, ep);
            Pt::Http::WebSocket* socket = new Pt::Http::WebSocket(http);
            DestroyOnReceive destroy(socket, *_loop, _received);
            socket->connected() += Pt::slot(destroy, &DestroyOnReceive::onConnected);
            socket->inputReady() += Pt::slot(destroy, &DestroyOnReceive::onInputReady);
            socket->beginConnect("/ws");

            _loop->run();

            PT_UNIT_ASSERT(_received);
            PT_UNIT_ASSERT(socket == 0);
        }

        void CloseFromPong()
        {
            Pt::Net::Endpoint ep("127.0.0.1", 8067);

            Pt::Http::Server server(*_loop, ep);
            CloseFromPongService service(*_loop);
            Pt::Http::WebSocketServlet sockets(service);

            Pt::Http::MapUrl mapUrl("/ws", service);
            server.addServlet(mapUrl);

            Pt::Http::Client http(*_loop, ep);
            Pt::Http::WebSocket socket(http);
            socket.connected() += Pt::slot(*this, &WebSocketTest::onConnectedSessionPong);
            socket.beginConnect("/ws");

            _loop->run();

            PT_UNIT_ASSERT_EQUALS(service.live(), static_cast<std::size_t>(0));
            PT_UNIT_ASSERT_EQUALS(sockets.size(), static_cast<std::size_t>(0));
        }

        void CloseFromShutdown()
        {
            Pt::Net::Endpoint ep("127.0.0.1", 8110);

            Pt::Http::Server server(*_loop, ep);
            CloseFromShutdownService service(*_loop);
            Pt::Http::WebSocketServlet sockets(service);

            Pt::Http::MapUrl mapUrl("/ws", service);
            server.addServlet(mapUrl);

            Pt::Http::Client http(*_loop, ep);
            Pt::Http::WebSocket socket(http);
            socket.connected() += Pt::slot(*this, &WebSocketTest::onConnectedSessionShutdown);
            socket.beginConnect("/ws");

            _loop->run();

            PT_UNIT_ASSERT_EQUALS(service.live(), static_cast<std::size_t>(0));
            PT_UNIT_ASSERT_EQUALS(sockets.size(), static_cast<std::size_t>(0));
        }

        void CloseFromClosed()
        {
            Pt::Net::Endpoint ep("127.0.0.1", 8111);

            Pt::Http::Server server(*_loop, ep);
            CloseFromClosedService service(*_loop);
            Pt::Http::WebSocketServlet sockets(service);

            Pt::Http::MapUrl mapUrl("/ws", service);
            server.addServlet(mapUrl);

            Pt::Http::Client http(*_loop, ep);
            Pt::Http::WebSocket socket(http);
            socket.connected() += Pt::slot(*this, &WebSocketTest::onConnectedClose);
            socket.shutdownReady() += Pt::slot(*this, &WebSocketTest::onShutdownReady);
            socket.beginConnect("/ws");

            _loop->run();

            PT_UNIT_ASSERT_EQUALS(service.live(), static_cast<std::size_t>(0));
            PT_UNIT_ASSERT_EQUALS(sockets.size(), static_cast<std::size_t>(0));
        }

        void CloseTwice()
        {
            Pt::Net::Endpoint ep("127.0.0.1", 8112);

            Pt::Http::Server server(*_loop, ep);
            CloseTwiceService service(*_loop);
            Pt::Http::WebSocketServlet sockets(service);

            Pt::Http::MapUrl mapUrl("/ws", service);
            server.addServlet(mapUrl);

            Pt::Http::Client http(*_loop, ep);
            Pt::Http::WebSocket socket(http);
            socket.connected() += Pt::slot(*this, &WebSocketTest::onConnected);
            socket.beginConnect("/ws");

            _loop->run();

            PT_UNIT_ASSERT_EQUALS(service.live(), static_cast<std::size_t>(0));
            PT_UNIT_ASSERT_EQUALS(sockets.size(), static_cast<std::size_t>(0));
        }

    protected:
        void CloseDestructor()
        {
            Pt::Net::Endpoint ep("127.0.0.1", 8033);

            Pt::Http::Server server(*_loop, ep);
            IdleService service(*_loop, false);
            Pt::Http::WebSocketServlet sockets(service);

            Pt::Http::MapUrl mapUrl("/ws", service);
            server.addServlet(mapUrl);

            Pt::Http::Client http(*_loop, ep);
            {
                Pt::Http::WebSocket socket(http);
                socket.connected() += Pt::slot(*this, &WebSocketTest::onConnectedCloseDtor);
                socket.beginConnect("/ws");
                _loop->run();
            }

            PT_UNIT_ASSERT(!_declined);
        }

        void onConnectedCloseDtor(Pt::Http::WebSocket& socket)
        {
            socket.endConnect();
            _loop->exit();
        }

    protected:
        void CloseNoFrame()
        {
            Pt::Net::Endpoint ep("127.0.0.1", 8034);

            Pt::Http::Server server(*_loop, ep);
            RecordService service(_message, _type, _received, _closed, *_loop, &_status);
            Pt::Http::WebSocketServlet sockets(service);

            Pt::Http::MapUrl mapUrl("/ws", service);
            server.addServlet(mapUrl);

            Pt::Http::Client http(*_loop, ep);
            Pt::Http::WebSocket socket(http);
            socket.connected() += Pt::slot(*this, &WebSocketTest::onConnectedCloseNoFrame);
            socket.beginConnect("/ws");

            _loop->run();

            PT_UNIT_ASSERT(_closed);
            PT_UNIT_ASSERT_EQUALS(_status, 1006u);
            PT_UNIT_ASSERT_EQUALS(socket.closeCode(), 1006u);
        }

        void onConnectedCloseNoFrame(Pt::Http::WebSocket& socket)
        {
            socket.endConnect();
            socket.close();
        }

    protected:
        void BeginTwice()
        {
            Pt::Net::Endpoint ep("127.0.0.1", 8023);

            Pt::Http::Server server(*_loop, ep);
            IdleService service(*_loop, false);
            Pt::Http::WebSocketServlet sockets(service);

            Pt::Http::MapUrl mapUrl("/ws", service);
            server.addServlet(mapUrl);

            Pt::Http::Client http(*_loop, ep);
            Pt::Http::WebSocket socket(http);
            socket.connected() += Pt::slot(*this, &WebSocketTest::onConnectedBeginTwice);
            socket.beginConnect("/ws");

            _loop->run();
        }

        void onConnectedBeginTwice(Pt::Http::WebSocket& socket)
        {
            socket.endConnect();
            socket.beginReceive();
            PT_UNIT_ASSERT_THROW(socket.beginReceive(), std::logic_error);
            socket.beginShutdown();
            PT_UNIT_ASSERT_THROW(socket.beginShutdown(), std::logic_error);
            _loop->exit();
        }

    protected:
        void EndBeforeReady()
        {
            Pt::Net::Endpoint ep("127.0.0.1", 8038);

            Pt::Http::Server server(*_loop, ep);
            IdleService service(*_loop, false);
            Pt::Http::WebSocketServlet sockets(service);

            Pt::Http::MapUrl mapUrl("/ws", service);
            server.addServlet(mapUrl);

            Pt::Http::Client http(*_loop, ep);
            Pt::Http::WebSocket socket(http);
            socket.connected() += Pt::slot(*this, &WebSocketTest::onConnectedEndBeforeReady);
            socket.beginConnect("/ws");

            _loop->run();
        }

        void onConnectedEndBeforeReady(Pt::Http::WebSocket& socket)
        {
            socket.endConnect();
            socket.outgoing().setType(Pt::Http::WebSocketMessage::Text);
            socket.outgoing().body() << "pending";
            socket.beginSend();
            socket.beginReceive();
            PT_UNIT_ASSERT_THROW(socket.endSend(), std::logic_error);
            PT_UNIT_ASSERT_THROW(socket.endReceive(), std::logic_error);
            _loop->exit();
        }

    protected:
        void CloseCodes()
        {
            Pt::Net::Endpoint ep("127.0.0.1", 8024);

            Pt::Http::Server server(*_loop, ep);
            IdleService service(*_loop, false);
            Pt::Http::WebSocketServlet sockets(service);

            Pt::Http::MapUrl mapUrl("/ws", service);
            server.addServlet(mapUrl);

            Pt::Http::Client http(*_loop, ep);
            Pt::Http::WebSocket socket(http);
            socket.connected() += Pt::slot(*this, &WebSocketTest::onConnectedCloseCodes);
            socket.beginConnect("/ws");

            _loop->run();
        }

        void onConnectedCloseCodes(Pt::Http::WebSocket& socket)
        {
            socket.endConnect();
            socket.outgoing().setType(Pt::Http::WebSocketMessage::Text);
            socket.outgoing().body() << "x";
            socket.beginSend();
            PT_UNIT_ASSERT_THROW(socket.beginSend(), std::logic_error);
            PT_UNIT_ASSERT_THROW(socket.beginShutdown(1005), std::invalid_argument);
            PT_UNIT_ASSERT_THROW(socket.beginShutdown(1006), std::invalid_argument);
            PT_UNIT_ASSERT_THROW(socket.beginShutdown(1015), std::invalid_argument);
            PT_UNIT_ASSERT_THROW(socket.beginShutdown(0), std::invalid_argument);
            PT_UNIT_ASSERT_THROW(socket.beginShutdown(999), std::invalid_argument);
            PT_UNIT_ASSERT_THROW(socket.beginShutdown(1004), std::invalid_argument);
            PT_UNIT_ASSERT_THROW(socket.beginShutdown(1016), std::invalid_argument);
            PT_UNIT_ASSERT_THROW(socket.beginShutdown(2999), std::invalid_argument);
            PT_UNIT_ASSERT_THROW(socket.beginShutdown(5000), std::invalid_argument);
            socket.beginShutdown(4999);
            PT_UNIT_ASSERT_EQUALS(socket.closeCode(), 0u);
            PT_UNIT_ASSERT_THROW(socket.beginShutdown(), std::logic_error);
            socket.beginReceive();
            _loop->exit();
        }

    protected:
        void Protocol()
        {
            Pt::Net::Endpoint ep("127.0.0.1", 8026);

            Pt::Http::Server server(*_loop, ep);
            ProtocolService service(_message, _offered, _copied, _handshake, _upgraded, *_loop);
            service.addProtocol("superchat");
            service.addProtocol("chat");
            Pt::Http::WebSocketServlet sockets(service);

            Pt::Http::MapUrl mapUrl("/ws", service);
            server.addServlet(mapUrl);

            Pt::Http::Client http(*_loop, ep);
            Pt::Http::WebSocket socket(http);
            socket.addProtocol("chat");
            socket.addProtocol("superchat");
            socket.connected() += Pt::slot(*this, &WebSocketTest::onConnectedProtocol);
            socket.beginConnect("/ws");

            _loop->run();

            PT_UNIT_ASSERT_EQUALS(socket.protocol(), "chat");
            PT_UNIT_ASSERT_EQUALS(_message, "chat");
            PT_UNIT_ASSERT_EQUALS(_offered, "chat, superchat");
            PT_UNIT_ASSERT_EQUALS(service.opened(), static_cast<std::size_t>(1));
        }

        void onConnectedProtocol(Pt::Http::WebSocket& socket)
        {
            socket.endConnect();
            _handshake = true;
            if(_upgraded)
                _loop->exit();
        }

    protected:
        void ProtocolNone()
        {
            Pt::Net::Endpoint ep("127.0.0.1", 8027);

            Pt::Http::Server server(*_loop, ep);
            ProtocolService service(_message, _offered, _copied, _handshake, _upgraded, *_loop);
            Pt::Http::WebSocketServlet sockets(service);

            Pt::Http::MapUrl mapUrl("/ws", service);
            server.addServlet(mapUrl);

            Pt::Http::Client http(*_loop, ep);
            Pt::Http::WebSocket socket(http);
            socket.addProtocol("chat");
            socket.connected() += Pt::slot(*this, &WebSocketTest::onConnectedProtocol);
            socket.beginConnect("/ws");

            _loop->run();

            PT_UNIT_ASSERT(socket.protocol().empty());
            PT_UNIT_ASSERT(_message.empty());
            PT_UNIT_ASSERT_EQUALS(service.opened(), static_cast<std::size_t>(1));
        }

    protected:
        void ProtocolRequired()
        {
            Pt::Net::Endpoint ep("127.0.0.1", 8028);

            Pt::Http::Server server(*_loop, ep);
            ProtocolService service(_message, _offered, _copied, _handshake, _upgraded, *_loop);
            service.addProtocol("chat");
            Pt::Http::WebSocketServlet sockets(service);

            Pt::Http::MapUrl mapUrl("/ws", service);
            server.addServlet(mapUrl);

            Pt::Http::Client http(*_loop, ep);
            Pt::Http::WebSocket socket(http);
            socket.connected() += Pt::slot(*this, &WebSocketTest::onConnectedProtocolFailed);
            socket.beginConnect("/ws");

            _loop->run();

            PT_UNIT_ASSERT(_declined);
            PT_UNIT_ASSERT_EQUALS(service.opened(), static_cast<std::size_t>(0));
        }

    protected:
        void ProtocolRejected()
        {
            Pt::Net::Endpoint ep("127.0.0.1", 8029);

            Pt::Http::Server server(*_loop, ep);
            ProtocolService service(_message, _offered, _copied, _handshake, _upgraded, *_loop);
            service.addProtocol("chat");
            Pt::Http::WebSocketServlet sockets(service);

            Pt::Http::MapUrl mapUrl("/ws", service);
            server.addServlet(mapUrl);

            Pt::Http::Client http(*_loop, ep);
            Pt::Http::WebSocket socket(http);
            socket.addProtocol("other");
            socket.connected() += Pt::slot(*this, &WebSocketTest::onConnectedProtocolFailed);
            socket.beginConnect("/ws");

            _loop->run();

            PT_UNIT_ASSERT(_declined);
            PT_UNIT_ASSERT_EQUALS(service.opened(), static_cast<std::size_t>(0));
        }

        void onConnectedProtocolFailed(Pt::Http::WebSocket& socket)
        {
            PT_UNIT_ASSERT_THROW(socket.endConnect(), Pt::Http::HttpError);
            _declined = true;
            _loop->exit();
        }

    protected:
        void AcceptMissing()
        {
            checkAcceptFailure(0, false);
        }

        void AcceptEmpty()
        {
            checkAcceptFailure("", false);
        }

        void AcceptWrong()
        {
            checkAcceptFailure("s3pPLMBiTxaQ9kYGzzhZRbK+xOo=", false);
        }

        void HandshakeStatus()
        {
            checkAcceptFailure("s3pPLMBiTxaQ9kYGzzhZRbK+xOo=", true);
        }

        void checkAcceptFailure(const char* accept, bool wrongStatus)
        {
            Pt::Net::Endpoint ep("127.0.0.1", 8181);
            Pt::Http::Server server(*_loop, ep);
            DeclineService service;
            Pt::Http::MapUrl mapUrl("/ws", service);
            server.addServlet(mapUrl);

            Pt::Http::Client http(*_loop, ep);
            Pt::Http::WebSocket socket(http);
            if(accept)
                socket.header().set("X-Test-Accept", accept);
            if(wrongStatus)
                socket.header().set("X-Test-Status", "200");

            HandshakeObserver observer(*_loop);
            socket.connected() += Pt::slot(observer, &HandshakeObserver::onConnected);
            socket.beginConnect("/ws");
            _loop->run();

            PT_UNIT_ASSERT_EQUALS(observer.count(), static_cast<std::size_t>(1));
            PT_UNIT_ASSERT_THROW(socket.endConnect(), Pt::Http::HttpError);
            PT_UNIT_ASSERT_THROW(socket.endConnect(), Pt::Http::HttpError);
            socket.outgoing().setType(Pt::Http::WebSocketMessage::Text);
            socket.outgoing().body() << "not sent";
            PT_UNIT_ASSERT_THROW(socket.beginSend(), std::logic_error);
            PT_UNIT_ASSERT_THROW(socket.beginReceive(), std::logic_error);
        }

        void HandshakeHeaders()
        {
            Pt::Net::Endpoint ep("127.0.0.1", 8180);
            Pt::Http::Server server(*_loop, ep);
            AcceptService service(_message, _offered, _copied, _handshake, _upgraded, *_loop);
            Pt::Http::WebSocketServlet sockets(service);
            Pt::Http::MapUrl mapUrl("/ws", service);
            server.addServlet(mapUrl);

            Pt::Http::Client http(*_loop, ep);
            Pt::Http::WebSocket socket(http);
            const Pt::Http::WebSocket& constSocket = socket;
            PT_UNIT_ASSERT(&socket.header() == &http.request().header());
            PT_UNIT_ASSERT(&constSocket.header() == &http.request().header());

            Pt::Http::Request& request = http.request();
            request.setMethod("POST");
            request.setQParams("old=query");
            request.body() << "old body";
            request.body().setstate(std::ios::failbit);
            socket.header().set("Authorization", "Bearer test");
            socket.header().set("Cookie", "session=test");
            socket.header().add("X-Custom", "first");
            socket.header().add("X-Custom", "second");
            socket.header().set("Origin", "https://header.test");
            socket.header().set("Sec-WebSocket-Version", "12");
            socket.header().set("Sec-WebSocket-Key", "old-key");
            socket.header().set("Sec-WebSocket-Protocol", "unoffered");
            socket.header().set("Content-Length", "8");
            socket.header().set("Transfer-Encoding", "chunked");
            socket.header().set("Trailer", "X-Trailer");
            socket.addProtocol("chat");
            socket.connected() += Pt::slot(*this, &WebSocketTest::onConnectedProtocol);
            socket.beginConnect("/ws");

            PT_UNIT_ASSERT_EQUALS(request.method(), "GET");
            PT_UNIT_ASSERT(request.qparams().empty());
            PT_UNIT_ASSERT_EQUALS(request.pending(), static_cast<std::size_t>(0));
            PT_UNIT_ASSERT(request.body().good());
            PT_UNIT_ASSERT(socket.header().isSet("Authorization", "Bearer test"));
            PT_UNIT_ASSERT(socket.header().isSet("Cookie", "session=test"));
            PT_UNIT_ASSERT(socket.header().isSet("Sec-WebSocket-Version", "13"));
            PT_UNIT_ASSERT( ! socket.header().isSet("Sec-WebSocket-Key", "old-key") );
            PT_UNIT_ASSERT(socket.header().isSet("Sec-WebSocket-Protocol", "chat"));
            PT_UNIT_ASSERT( ! socket.header().has("Transfer-Encoding") );
            PT_UNIT_ASSERT( ! socket.header().has("Trailer") );
            PT_UNIT_ASSERT_EQUALS(socket.header().contentLength(), static_cast<std::size_t>(0));

            std::size_t customFields = 0;
            for(Pt::Http::MessageHeader::FieldIterator field = socket.header().begin();
                field != socket.header().end(); ++field)
            {
                if(std::string(field->name()) == "X-Custom")
                {
                    const std::string expected = customFields == 0 ? "first" : "second";
                    PT_UNIT_ASSERT_EQUALS(std::string(field->value()), expected);
                    ++customFields;
                }
            }
            PT_UNIT_ASSERT_EQUALS(customFields, static_cast<std::size_t>(2));

            _loop->run();

            PT_UNIT_ASSERT(_handshake);
            PT_UNIT_ASSERT(_upgraded);
            socket.endConnect();
            PT_UNIT_ASSERT_EQUALS(_copied, "https://header.test");
            PT_UNIT_ASSERT_EQUALS(_offered, "chat");
        }

        void HandshakeKey()
        {
            Pt::Net::Endpoint ep("127.0.0.1", 8190);
            Pt::Http::Server server(*_loop, ep);
            ProtocolService service(_message, _offered, _copied, _handshake, _upgraded, *_loop);
            Pt::Http::WebSocketServlet sockets(service);
            Pt::Http::MapUrl mapUrl("/ws", service);
            server.addServlet(mapUrl);

            Pt::Http::Client http1(*_loop, ep);
            Pt::Http::WebSocket socket1(http1);
            socket1.beginConnect("/ws");

            const char* field1 = socket1.header().get("Sec-WebSocket-Key");
            PT_UNIT_ASSERT(field1 != 0);
            const std::string key1(field1);
            PT_UNIT_ASSERT(isHandshakeNonce(key1));

            Pt::Http::Client http2(*_loop, ep);
            Pt::Http::WebSocket socket2(http2);
            socket2.beginConnect("/ws");

            const char* field2 = socket2.header().get("Sec-WebSocket-Key");
            PT_UNIT_ASSERT(field2 != 0);
            const std::string key2(field2);
            PT_UNIT_ASSERT(isHandshakeNonce(key2));
            PT_UNIT_ASSERT(key1 != key2);
        }

        static bool isHandshakeNonce(const std::string& key)
        {
            if(key.size() != 24)
                return false;

            std::istringstream in(key);
            Pt::BasicTextIStream<char, char> decoder(in, new Pt::Base64Codec());
            char decoded[17];
            decoder.read(decoded, 17);
            return decoder.gcount() == 16;
        }

        void ProtocolHeader()
        {
            Pt::Net::Endpoint ep("127.0.0.1", 8030);

            Pt::Http::Server server(*_loop, ep);
            ProtocolService service(_message, _offered, _copied, _handshake, _upgraded, *_loop);
            Pt::Http::WebSocketServlet sockets(service);

            Pt::Http::MapUrl mapUrl("/ws", service);
            server.addServlet(mapUrl);

            Pt::Http::Client http(*_loop, ep);
            Pt::Http::WebSocket socket(http);
            socket.connected() += Pt::slot(*this, &WebSocketTest::onConnectedProtocol);
            socket.header().set("Origin", "https://header.test");
            socket.beginConnect("/ws", "https://example.test");

            _loop->run();

            PT_UNIT_ASSERT_EQUALS(_copied, "https://example.test");
            PT_UNIT_ASSERT_EQUALS(service.opened(), static_cast<std::size_t>(1));
        }

    protected:
        void EndAfterClosed()
        {
            Pt::Net::Endpoint ep("127.0.0.1", 8035);

            Pt::Http::Server server(*_loop, ep);
            CloseNowService service;
            Pt::Http::WebSocketServlet sockets(service);

            Pt::Http::MapUrl mapUrl("/ws", service);
            server.addServlet(mapUrl);

            Pt::Http::Client http(*_loop, ep);
            Pt::Http::WebSocket socket(http);
            socket.connected() += Pt::slot(*this, &WebSocketTest::onConnectedEndAfterClosed);
            socket.beginConnect("/ws");

            _loop->run();

            PT_UNIT_ASSERT(_closed);
            PT_UNIT_ASSERT_EQUALS(service.socketCount(), static_cast<std::size_t>(0));
            PT_UNIT_ASSERT_EQUALS(sockets.size(), static_cast<std::size_t>(0));
        }

        void onConnectedEndAfterClosed(Pt::Http::WebSocket& socket)
        {
            socket.endConnect();
            socket.closed() += Pt::slot(*this, &WebSocketTest::onClientEndAfterClosed);
            socket.beginReceive();
        }

        void onClientEndAfterClosed(Pt::Http::WebSocket& socket)
        {
            PT_UNIT_ASSERT_THROW(socket.endSend(), std::logic_error);
            PT_UNIT_ASSERT_THROW(socket.endReceive(), std::logic_error);
            PT_UNIT_ASSERT_THROW(socket.beginReceive(), std::logic_error);
            socket.close();
            _closed = true;
            _loop->exit();
        }

    protected:
        void AcceptCase()
        {
            checkAcceptVariant("case", false);
        }

        void AcceptWhitespace()
        {
            checkAcceptVariant("ows", true);
        }

        void AcceptList()
        {
            checkAcceptVariant("list", false);
        }

        void checkAcceptVariant(const char* mode, bool success)
        {
            Pt::Net::Endpoint ep("127.0.0.1", 8182);
            Pt::Http::Server server(*_loop, ep);
            AcceptService service(_message, _offered, _copied, _handshake, _upgraded, *_loop);
            Pt::Http::WebSocketServlet sockets(service);
            Pt::Http::MapUrl mapUrl("/ws", service);
            server.addServlet(mapUrl);

            Pt::Http::Client http(*_loop, ep);
            Pt::Http::WebSocket socket(http);
            socket.header().set("X-Test-Reply", mode);
            HandshakeObserver observer(*_loop);
            socket.connected() += Pt::slot(observer, &HandshakeObserver::onConnected);
            socket.beginConnect("/ws");
            _loop->run();

            PT_UNIT_ASSERT_EQUALS(observer.count(), static_cast<std::size_t>(1));
            if(success)
                socket.endConnect();
            else
            {
                PT_UNIT_ASSERT_THROW(socket.endConnect(), Pt::Http::HttpError);
                PT_UNIT_ASSERT_THROW(socket.beginReceive(), std::logic_error);
            }
        }

        void HandshakeUpgradeOther()
        {
            checkReplyRejected("upgrade-other", 8185, false);
        }

        void HandshakeUpgradeMissing()
        {
            checkReplyRejected("upgrade-missing", 8186, false);
        }

        void HandshakeConnectionMissing()
        {
            checkReplyRejected("connection-missing", 8187, false);
        }

        void HandshakeExtensions()
        {
            checkReplyRejected("extensions", 8188, false);
        }

        void HandshakeExtensionsEmpty()
        {
            checkReplyRejected("extensions-empty", 8189, false);
        }

        void HandshakeExtensionsRequested()
        {
            checkReplyRejected("", 8191, true);
        }

        void HandshakeExtensionsEchoed()
        {
            checkReplyRejected("extensions", 8192, true);
        }

        void checkReplyRejected(const char* mode, unsigned short port, bool requested)
        {
            Pt::Net::Endpoint ep("127.0.0.1", port);
            Pt::Http::Server server(*_loop, ep);
            AcceptService service(_message, _offered, _copied, _handshake, _upgraded, *_loop);
            Pt::Http::WebSocketServlet sockets(service);
            Pt::Http::MapUrl mapUrl("/ws", service);
            server.addServlet(mapUrl);

            Pt::Http::Client http(*_loop, ep);
            Pt::Http::WebSocket socket(http);
            if(*mode)
                socket.header().set("X-Test-Reply", mode);
            if(requested)
                socket.header().set("Sec-WebSocket-Extensions", "permessage-deflate");

            HandshakeObserver observer(*_loop);
            socket.connected() += Pt::slot(observer, &HandshakeObserver::onConnected);
            socket.beginConnect("/ws");
            _loop->run();

            PT_UNIT_ASSERT_EQUALS(observer.count(), static_cast<std::size_t>(1));
            if(*mode)
            {
                PT_UNIT_ASSERT_THROW(socket.endConnect(), Pt::Http::HttpError);
                PT_UNIT_ASSERT_THROW(socket.beginReceive(), std::logic_error);
            }
            else
            {
                // an extension in the request is not a reply extension
                socket.endConnect();
            }
        }

        void HandshakeReply()
        {
            Pt::Net::Endpoint ep("127.0.0.1", 8193);
            Pt::Http::Server server(*_loop, ep);
            AcceptService service(_message, _offered, _copied, _handshake, _upgraded, *_loop);
            Pt::Http::WebSocketServlet sockets(service);
            Pt::Http::MapUrl mapUrl("/ws", service);
            server.addServlet(mapUrl);

            Pt::Http::Client http(*_loop, ep);
            Pt::Http::WebSocket socket(http);
            socket.header().set("X-Test-Reply", "status");
            socket.connected() += Pt::slot(*this, &WebSocketTest::onConnectedReply);
            socket.beginConnect("/ws");
            _loop->run();

            PT_UNIT_ASSERT_EQUALS(_status, 503u);
            PT_UNIT_ASSERT_EQUALS(_message, "120");
            PT_UNIT_ASSERT_EQUALS(_offered, "retry=1");
            PT_UNIT_ASSERT_THROW(socket.endConnect(), Pt::Http::HttpError);
            PT_UNIT_ASSERT_THROW(socket.endConnect(), Pt::Http::HttpError);
            PT_UNIT_ASSERT_EQUALS(socket.reply().statusCode(), 503u);
            PT_UNIT_ASSERT(socket.reply().header().isSet("Retry-After", "120"));
            PT_UNIT_ASSERT(socket.reply().header().isSet("Set-Cookie", "retry=1"));
        }

        void onConnectedReply(Pt::Http::WebSocket& socket)
        {
            const Pt::Http::Reply& reply = socket.reply();
            const char* retry = reply.header().get("Retry-After");
            const char* cookie = reply.header().get("Set-Cookie");
            _status = reply.statusCode();
            _message = retry ? retry : "";
            _offered = cookie ? cookie : "";
            _loop->exit();
        }

        void HandshakeReplyUnfinished()
        {
            Pt::Net::Endpoint ep("127.0.0.1", 8194);
            Pt::Http::Server server(*_loop, ep);
            SilentService service;
            Pt::Http::MapUrl mapUrl("/ws", service);
            server.addServlet(mapUrl);

            Pt::Http::Client http(*_loop, ep);
            http.setTimeout(100);
            Pt::Http::WebSocket socket(http);
            HandshakeObserver observer(*_loop);
            socket.connected() += Pt::slot(observer, &HandshakeObserver::onConnected);
            socket.beginConnect("/ws");
            _loop->run();

            PT_UNIT_ASSERT_EQUALS(observer.count(), static_cast<std::size_t>(1));
            assertHandshakeTimeout(socket);

            // no reply was finished, so this is the default reply of the client
            PT_UNIT_ASSERT_EQUALS(socket.reply().statusCode(), 200u);
            PT_UNIT_ASSERT(socket.reply().header().begin() == socket.reply().header().end());
        }

        void AcceptRetry()
        {
            Pt::Net::Endpoint ep("127.0.0.1", 8183);
            Pt::Http::Server server(*_loop, ep);
            AcceptService service(_message, _offered, _copied, _handshake, _upgraded, *_loop);
            Pt::Http::WebSocketServlet sockets(service);
            Pt::Http::MapUrl mapUrl("/ws", service);
            server.addServlet(mapUrl);

            Pt::Http::Client http(*_loop, ep);
            Pt::Http::WebSocket socket(http);
            HandshakeObserver observer(*_loop);
            socket.connected() += Pt::slot(observer, &HandshakeObserver::onConnected);
            socket.header().set("X-Test-Reply", "missing");
            socket.beginConnect("/ws");
            _loop->run();

            PT_UNIT_ASSERT_EQUALS(observer.count(), static_cast<std::size_t>(1));
            PT_UNIT_ASSERT_THROW(socket.endConnect(), Pt::Http::HttpError);
            http.close();
            socket.header().remove("X-Test-Reply");
            socket.beginConnect("/ws");
            _loop->run();

            PT_UNIT_ASSERT_EQUALS(observer.count(), static_cast<std::size_t>(2));
            socket.endConnect();
        }

        void AcceptVector()
        {
            Pt::Net::Endpoint ep("127.0.0.1", 8184);
            Pt::Http::Server server(*_loop, ep);
            ProtocolService service(_message, _offered, _copied, _handshake, _upgraded, *_loop);
            Pt::Http::WebSocketServlet sockets(service);
            Pt::Http::MapUrl mapUrl("/ws", service);
            server.addServlet(mapUrl);

            Pt::Http::Client http(*_loop, ep);
            HandshakeObserver observer(*_loop);
            http.replyReceived() += Pt::slot(observer, &HandshakeObserver::onReply);
            http.request().setUrl("/ws");
            http.request().header().setUpgrade();
            http.request().header().set("Upgrade", "websocket");
            http.request().header().set("Sec-WebSocket-Version", "13");
            http.request().header().set("Sec-WebSocket-Key", "dGhlIHNhbXBsZSBub25jZQ==");
            http.beginReceive();
            _loop->run();

            PT_UNIT_ASSERT_EQUALS(observer.count(), static_cast<std::size_t>(1));
            PT_UNIT_ASSERT_EQUALS(http.reply().statusCode(), 101u);
            const char* accept = http.reply().header().get("Sec-WebSocket-Accept");
            PT_UNIT_ASSERT(accept != 0);
            PT_UNIT_ASSERT_EQUALS(std::string(accept), "s3pPLMBiTxaQ9kYGzzhZRbK+xOo=");
        }

        void HandshakeKeyMissing()
        {
            checkHandshakeDecline(8185, "13", 0, 400u, false);
        }

        void HandshakeKeyInvalid()
        {
            checkHandshakeDecline(8186, "13", "YQ==", 400u, false);
        }

        void HandshakeVersionMissing()
        {
            checkHandshakeDecline(8187, 0, "dGhlIHNhbXBsZSBub25jZQ==", 426u, true);
        }

        void HandshakeVersionOther()
        {
            checkHandshakeDecline(8188, "12", "dGhlIHNhbXBsZSBub25jZQ==", 426u, true);
        }

        void HandshakeMethodOther()
        {
            checkUpgradeDecline(8195, "POST", 1, "upgrade", "websocket", 405u);
        }

        void HandshakeVersionOld()
        {
            checkUpgradeDecline(8196, "GET", 0, "upgrade", "websocket", 400u);
        }

        void HandshakeNoUpgrade()
        {
            checkUpgradeDecline(8197, "GET", 1, 0, 0, 426u);
        }

        void HandshakeNoUpgradeKeepAlive()
        {
            checkUpgradeDecline(8198, "GET", 1, "keep-alive", 0, 426u);
        }

        void HandshakeConnectionOnly()
        {
            checkUpgradeDecline(8199, "GET", 1, "upgrade", 0, 400u);
        }

        void HandshakeUpgradeOnly()
        {
            checkUpgradeDecline(8200, "GET", 1, "keep-alive", "websocket", 400u);
        }

        void HandshakeUpgradeProtocol()
        {
            checkUpgradeDecline(8201, "GET", 1, "upgrade", "h2c", 400u);
        }

        // Sends a request that is valid except for method, version, Connection and Upgrade.
        void checkUpgradeDecline(unsigned short port,
                                 const char* method,
                                 unsigned versionMinor,
                                 const char* connection,
                                 const char* upgrade,
                                 unsigned expectedStatus)
        {
            Pt::Net::Endpoint ep("127.0.0.1", port);
            Pt::Http::Server server(*_loop, ep);
            ProtocolService service(_message, _offered, _copied, _handshake, _upgraded, *_loop);
            Pt::Http::WebSocketServlet sockets(service);
            Pt::Http::MapUrl mapUrl("/ws", service);
            server.addServlet(mapUrl);

            Pt::Http::Client http(*_loop, ep);
            HandshakeObserver observer(*_loop);
            http.replyReceived() += Pt::slot(observer, &HandshakeObserver::onReply);
            http.request().setUrl("/ws");
            http.request().setMethod(method);
            http.request().header().setVersion(1, versionMinor);
            if(connection)
                http.request().header().set("Connection", connection);
            if(upgrade)
                http.request().header().set("Upgrade", upgrade);
            http.request().header().set("Sec-WebSocket-Version", "13");
            http.request().header().set("Sec-WebSocket-Key", "dGhlIHNhbXBsZSBub25jZQ==");
            http.beginReceive();
            _loop->run();

            PT_UNIT_ASSERT_EQUALS(observer.count(), static_cast<std::size_t>(1));
            PT_UNIT_ASSERT_EQUALS(http.reply().statusCode(), expectedStatus);
            PT_UNIT_ASSERT_EQUALS(service.opened(), static_cast<std::size_t>(0));

            const Pt::Http::MessageHeader& header = http.reply().header();
            PT_UNIT_ASSERT(header.contains("Connection", "close"));
            if(expectedStatus == Pt::Http::Reply::MethodNotAllowed)
                PT_UNIT_ASSERT(header.isSet("Allow", "GET"));

            if(expectedStatus == Pt::Http::Reply::UpgradeRequired)
            {
                PT_UNIT_ASSERT(header.isSet("Upgrade", "websocket"));
                PT_UNIT_ASSERT(header.contains("Connection", "upgrade"));
            }
        }

        void checkHandshakeDecline(unsigned short port,
                                   const char* version,
                                   const char* key,
                                   unsigned expectedStatus,
                                   bool expectVersionHeader)
        {
            Pt::Net::Endpoint ep("127.0.0.1", port);
            Pt::Http::Server server(*_loop, ep);
            ProtocolService service(_message, _offered, _copied, _handshake, _upgraded, *_loop);
            Pt::Http::WebSocketServlet sockets(service);
            Pt::Http::MapUrl mapUrl("/ws", service);
            server.addServlet(mapUrl);

            Pt::Http::Client http(*_loop, ep);
            HandshakeObserver observer(*_loop);
            http.replyReceived() += Pt::slot(observer, &HandshakeObserver::onReply);
            http.request().setUrl("/ws");
            http.request().header().setUpgrade();
            http.request().header().set("Upgrade", "websocket");
            if(version)
                http.request().header().set("Sec-WebSocket-Version", version);
            if(key)
                http.request().header().set("Sec-WebSocket-Key", key);
            http.beginReceive();
            _loop->run();

            PT_UNIT_ASSERT_EQUALS(observer.count(), static_cast<std::size_t>(1));
            PT_UNIT_ASSERT_EQUALS(http.reply().statusCode(), expectedStatus);
            PT_UNIT_ASSERT_EQUALS(service.opened(), static_cast<std::size_t>(0));
            PT_UNIT_ASSERT(http.reply().header().contains("Connection", "close"));
            if(expectVersionHeader)
                PT_UNIT_ASSERT(http.reply().header().isSet("Sec-WebSocket-Version", "13"));
        }

        void checkOrigin(unsigned short port,
                         const char* accepted,
                         const char* origin,
                         bool sendOrigin,
                         unsigned expectedStatus)
        {
            Pt::Net::Endpoint ep("127.0.0.1", port);
            Pt::Http::Server server(*_loop, ep);
            ProtocolService service(_message, _offered, _copied, _handshake, _upgraded, *_loop);
            if(accepted)
                service.addOrigin(accepted);
            Pt::Http::WebSocketServlet sockets(service);
            Pt::Http::MapUrl mapUrl("/ws", service);
            server.addServlet(mapUrl);

            Pt::Http::Client http(*_loop, ep);
            HandshakeObserver observer(*_loop);
            http.replyReceived() += Pt::slot(observer, &HandshakeObserver::onReply);
            http.request().setUrl("/ws");
            http.request().header().setUpgrade();
            http.request().header().set("Upgrade", "websocket");
            http.request().header().set("Sec-WebSocket-Version", "13");
            http.request().header().set("Sec-WebSocket-Key", "dGhlIHNhbXBsZSBub25jZQ==");
            if(sendOrigin)
                http.request().header().add("Origin", origin ? origin : "");
            http.beginReceive();
            _loop->run();

            PT_UNIT_ASSERT_EQUALS(observer.count(), static_cast<std::size_t>(1));
            PT_UNIT_ASSERT_EQUALS(http.reply().statusCode(), expectedStatus);
            if(expectedStatus != 101u)
            {
                PT_UNIT_ASSERT_EQUALS(service.opened(), static_cast<std::size_t>(0));
                PT_UNIT_ASSERT(http.reply().header().contains("Connection", "close"));
            }
        }

        void OriginAccepted()
        {
            checkOrigin(8210, "https://example.test", "HTTPS://example.test", true, 101u);
        }

        void OriginRejected()
        {
            checkOrigin(8211, "https://example.test", "https://other.test", true, 403u);
        }

        void OriginMissing()
        {
            checkOrigin(8212, "https://example.test", 0, false, 403u);
        }

        void OriginUnchecked()
        {
            checkOrigin(8213, 0, "https://any.test", true, 101u);
        }

        void OriginNullRejected()
        {
            checkOrigin(8215, "https://example.test", "null", true, 403u);
        }

        void OriginNullAccepted()
        {
            checkOrigin(8216, "null", "null", true, 101u);
        }

        void HandshakeControl()
        {
            Pt::Net::Endpoint ep("127.0.0.1", 8025);
            Pt::Http::Client http(*_loop, ep);
            Pt::Http::WebSocket socket(http);
            PT_UNIT_ASSERT_THROW(socket.beginPing(), std::logic_error);
            PT_UNIT_ASSERT_THROW(socket.beginShutdown(), std::logic_error);
        }

    protected:
        void HandshakeTimeout()
        {
            Pt::Net::Endpoint ep("127.0.0.1", 8101);
            Pt::Http::Server server(*_loop, ep);
            SilentService service;
            Pt::Http::MapUrl mapUrl("/ws", service);
            server.addServlet(mapUrl);

            Pt::Http::Client http(*_loop, ep);
            http.setTimeout(100);
            Pt::Http::WebSocket socket(http);
            HandshakeObserver observer(*_loop);
            socket.connected() += Pt::slot(observer, &HandshakeObserver::onConnected);
            socket.beginConnect("/ws");

            _loop->run();

            PT_UNIT_ASSERT_EQUALS(observer.count(), static_cast<std::size_t>(1));
            assertHandshakeTimeout(socket);
            assertHandshakeTimeout(socket);
        }

        void assertHandshakeTimeout(Pt::Http::WebSocket& socket)
        {
            bool timedOut = false;
            try
            {
                socket.endConnect();
            }
            catch(const Pt::Http::HttpError&)
            {
                throw;
            }
            catch(const Pt::System::IOError& error)
            {
                const Pt::System::IOError expected("timeout");
                PT_UNIT_ASSERT_EQUALS(std::string(error.what()), std::string(expected.what()));
                timedOut = true;
            }

            PT_UNIT_ASSERT(timedOut);
        }

    protected:
        void HandshakePending()
        {
            Pt::Net::Endpoint ep("127.0.0.1", 8102);
            Pt::Http::Server server(*_loop, ep);
            IdleService service(*_loop, false);
            Pt::Http::WebSocketServlet sockets(service);
            Pt::Http::MapUrl mapUrl("/ws", service);
            server.addServlet(mapUrl);

            Pt::Http::Client http(*_loop, ep);
            Pt::Http::WebSocket socket(http);
            HandshakeObserver observer(*_loop);
            socket.connected() += Pt::slot(observer, &HandshakeObserver::onConnected);
            socket.beginConnect("/ws");
            PT_UNIT_ASSERT_THROW(socket.beginConnect("/ws"), std::logic_error);
            PT_UNIT_ASSERT_THROW(socket.endConnect(), std::logic_error);
            PT_UNIT_ASSERT_EQUALS(observer.count(), static_cast<std::size_t>(0));

            _loop->run();

            PT_UNIT_ASSERT_EQUALS(observer.count(), static_cast<std::size_t>(1));
            socket.endConnect();
        }

    protected:
        void ConnectedThrows()
        {
            Pt::Net::Endpoint ep("127.0.0.1", 8103);
            Pt::Http::Server server(*_loop, ep);
            IdleService service(*_loop, false);
            Pt::Http::WebSocketServlet sockets(service);
            Pt::Http::MapUrl mapUrl("/ws", service);
            server.addServlet(mapUrl);

            Pt::Http::Client http(*_loop, ep);
            Pt::Http::WebSocket socket(http);
            HandshakeObserver observer(*_loop, true);
            socket.connected() += Pt::slot(observer, &HandshakeObserver::onConnected);
            socket.beginConnect("/ws");

            assertConnectedThrows();

            PT_UNIT_ASSERT_EQUALS(observer.count(), static_cast<std::size_t>(1));
            socket.endConnect();
        }

        void assertConnectedThrows()
        {
            bool thrown = false;
            try
            {
                _loop->run();
            }
            catch(const std::runtime_error& error)
            {
                PT_UNIT_ASSERT_EQUALS(std::string(error.what()), "connected slot failed");
                thrown = true;
            }

            PT_UNIT_ASSERT(thrown);
        }

    protected:
        void FailedConnectedThrows()
        {
            Pt::Net::Endpoint ep("127.0.0.1", 8104);
            Pt::Http::Server server(*_loop, ep);
            SilentService service;
            Pt::Http::MapUrl mapUrl("/ws", service);
            server.addServlet(mapUrl);

            Pt::Http::Client http(*_loop, ep);
            http.setTimeout(100);
            Pt::Http::WebSocket socket(http);
            HandshakeObserver observer(*_loop, true);
            socket.connected() += Pt::slot(observer, &HandshakeObserver::onConnected);
            socket.beginConnect("/ws");

            assertConnectedThrows();

            PT_UNIT_ASSERT_EQUALS(observer.count(), static_cast<std::size_t>(1));
            assertHandshakeTimeout(socket);
        }

    protected:
        void HandshakeRetry()
        {
            Pt::Net::Endpoint ep("127.0.0.1", 8105);
            Pt::Http::Server server(*_loop, ep);
            ProtocolService service(_message, _offered, _copied, _handshake, _upgraded, *_loop);
            service.addProtocol("chat");
            Pt::Http::WebSocketServlet sockets(service);
            Pt::Http::MapUrl mapUrl("/ws", service);
            server.addServlet(mapUrl);

            Pt::Http::Client http(*_loop, ep);
            Pt::Http::WebSocket socket(http);
            socket.addProtocol("other");
            HandshakeObserver observer(*_loop);
            socket.connected() += Pt::slot(observer, &HandshakeObserver::onConnected);
            socket.beginConnect("/ws");

            _loop->run();

            PT_UNIT_ASSERT_EQUALS(observer.count(), static_cast<std::size_t>(1));
            PT_UNIT_ASSERT_THROW(socket.endConnect(), Pt::Http::HttpError);

            http.close();
            socket.addProtocol("chat");
            socket.beginConnect("/ws");

            _loop->run();

            PT_UNIT_ASSERT_EQUALS(observer.count(), static_cast<std::size_t>(2));
            socket.endConnect();
            PT_UNIT_ASSERT_EQUALS(socket.protocol(), "chat");
        }

    protected:
        void HandshakeReconnect()
        {
            Pt::Net::Endpoint ep("127.0.0.1", 8118);
            Pt::Http::Server server(*_loop, ep);
            PushService service;
            Pt::Http::WebSocketServlet sockets(service);
            Pt::Http::MapUrl mapUrl("/ws", service);
            server.addServlet(mapUrl);

            Pt::Http::Client http(*_loop, ep);
            Pt::Http::WebSocket socket(http);
            ReconnectObserver observer(*_loop);
            socket.connected() += Pt::slot(observer, &ReconnectObserver::onConnected);
            socket.inputReady() += Pt::slot(observer, &ReconnectObserver::onInputReady);
            socket.beginConnect("/ws");

            _loop->run();

            PT_UNIT_ASSERT_EQUALS(observer.connects(), static_cast<std::size_t>(2));
            PT_UNIT_ASSERT_EQUALS(observer.inputs(), static_cast<std::size_t>(2));
        }

    protected:
        void ConnectWhileOpen()
        {
            Pt::Net::Endpoint ep("127.0.0.1", 8119);
            Pt::Http::Server server(*_loop, ep);
            IdleService service(*_loop, false);
            Pt::Http::WebSocketServlet sockets(service);
            Pt::Http::MapUrl mapUrl("/ws", service);
            server.addServlet(mapUrl);

            Pt::Http::Client http(*_loop, ep);
            Pt::Http::WebSocket socket(http);
            HandshakeObserver observer(*_loop);
            socket.connected() += Pt::slot(observer, &HandshakeObserver::onConnected);
            socket.beginConnect("/ws");

            _loop->run();

            socket.endConnect();
            PT_UNIT_ASSERT_THROW(socket.beginConnect("/ws"), std::logic_error);
        }

    protected:
        void ProtocolClear()
        {
            Pt::Net::Endpoint ep("127.0.0.1", 8120);
            Pt::Http::Server server(*_loop, ep);
            ProtocolService service(_message, _offered, _copied, _handshake, _upgraded, *_loop);
            service.addProtocol("chat");
            Pt::Http::WebSocketServlet sockets(service);
            Pt::Http::MapUrl mapUrl("/ws", service);
            server.addServlet(mapUrl);

            Pt::Http::Client http(*_loop, ep);
            Pt::Http::WebSocket socket(http);
            socket.addProtocol("other");
            HandshakeObserver observer(*_loop);
            socket.connected() += Pt::slot(observer, &HandshakeObserver::onConnected);
            socket.beginConnect("/ws");

            _loop->run();

            PT_UNIT_ASSERT_EQUALS(observer.count(), static_cast<std::size_t>(1));
            PT_UNIT_ASSERT_THROW(socket.endConnect(), Pt::Http::HttpError);

            http.close();
            socket.clearProtocols();
            socket.addProtocol("chat");
            socket.beginConnect("/ws");

            _loop->run();

            PT_UNIT_ASSERT_EQUALS(observer.count(), static_cast<std::size_t>(2));
            socket.endConnect();
            PT_UNIT_ASSERT_EQUALS(socket.protocol(), "chat");
        }

    protected:
        void PingAfterShutdown()
        {
            Pt::Net::Endpoint ep("127.0.0.1", 8043);

            Pt::Http::Server server(*_loop, ep);
            IdleService service(*_loop, false);
            Pt::Http::WebSocketServlet sockets(service);

            Pt::Http::MapUrl mapUrl("/ws", service);
            server.addServlet(mapUrl);

            Pt::Http::Client http(*_loop, ep);
            Pt::Http::WebSocket socket(http);
            socket.connected() += Pt::slot(*this, &WebSocketTest::onConnectedPingAfterShutdown);
            socket.beginConnect("/ws");

            _loop->run();

            PT_UNIT_ASSERT(_rejected);
        }

        void onConnectedPingAfterShutdown(Pt::Http::WebSocket& socket)
        {
            socket.endConnect();
            socket.beginShutdown();

            try
            {
                socket.beginPing();
            }
            catch(const std::logic_error&)
            {
                _rejected = true;
            }

            _loop->exit();
        }

    protected:
        void IdleBeforeConnect()
        {
            Pt::Net::Endpoint ep("127.0.0.1", 8044);

            Pt::Http::Server server(*_loop, ep);
            IdleService service(*_loop, false);
            Pt::Http::WebSocketServlet sockets(service);

            Pt::Http::MapUrl mapUrl("/ws", service);
            server.addServlet(mapUrl);

            Pt::Http::Client http(*_loop, ep);
            Pt::Http::WebSocket socket(http);
            socket.setIdleTimeout(200);
            socket.connected() += Pt::slot(*this, &WebSocketTest::onConnectedIdleReceive);
            socket.closed() += Pt::slot(*this, &WebSocketTest::onIdleClosed);
            socket.beginConnect("/ws");

            _loop->run();

            PT_UNIT_ASSERT(_closed);
        }

        void onConnectedIdleReceive(Pt::Http::WebSocket& socket)
        {
            socket.endConnect();
            socket.beginReceive();
        }

        void onIdleClosed(Pt::Http::WebSocket& socket)
        {
            PT_UNIT_ASSERT_THROW(socket.beginReceive(), std::logic_error);
            socket.close();
            _closed = true;
            _loop->exit();
        }

    protected:
        void HttpTimeoutAfterUpgrade()
        {
            Pt::Net::Endpoint ep("127.0.0.1", 8048);

            Pt::Http::Server server(*_loop, ep);
            IdleService service(*_loop, false);
            Pt::Http::WebSocketServlet sockets(service);

            Pt::Http::MapUrl mapUrl("/ws", service);
            server.addServlet(mapUrl);

            Pt::Http::Client http(*_loop, ep);
            http.setTimeout(50);
            Pt::Http::WebSocket socket(http);
            _limitSocket = &socket;
            socket.connected() += Pt::slot(*this, &WebSocketTest::onConnectedHttpTimeout);
            socket.closed() += Pt::slot(*this, &WebSocketTest::onHttpTimeoutClosed);
            socket.beginConnect("/ws");

            _loop->run();

            PT_UNIT_ASSERT(!_closed);
            PT_UNIT_ASSERT(_finished);
            _limitSocket = 0;
        }

        void onConnectedHttpTimeout(Pt::Http::WebSocket& socket)
        {
            socket.endConnect();
            socket.beginReceive();
            _waitTimer.setActive(*_loop);
            _waitTimer.start(200);
        }

        void onHttpTimeoutWait()
        {
            if(_limitSocket)
                _limitSocket->close();

            _finished = true;
            _loop->exit();
        }

        void onHttpTimeoutClosed(Pt::Http::WebSocket& socket)
        {
            _closed = true;
            socket.close();
            _loop->exit();
        }

    protected:
        void IdleNotResetBySend()
        {
            Pt::Net::Endpoint ep("127.0.0.1", 8049);

            Pt::Http::Server server(*_loop, ep);
            IdleService service(*_loop, false);
            Pt::Http::WebSocketServlet sockets(service);

            Pt::Http::MapUrl mapUrl("/ws", service);
            server.addServlet(mapUrl);

            Pt::Http::Client http(*_loop, ep);
            Pt::Http::WebSocket socket(http);
            socket.setIdleTimeout(200);
            socket.connected() += Pt::slot(*this, &WebSocketTest::onConnectedIdleSend);
            socket.outputReady() += Pt::slot(*this, &WebSocketTest::onIdleSendOutput);
            socket.closed() += Pt::slot(*this, &WebSocketTest::onIdleClosed);
            socket.beginConnect("/ws");

            _loop->run();

            PT_UNIT_ASSERT(_closed);
        }

        void onConnectedIdleSend(Pt::Http::WebSocket& socket)
        {
            socket.endConnect();
            socket.outgoing().setType(Pt::Http::WebSocketMessage::Text);
            socket.outgoing().body() << "x";
            socket.beginSend();
        }

        void onIdleSendOutput(Pt::Http::WebSocket& socket)
        {
            socket.endSend();
        }

    protected:
        void PingTimeoutNoReply()
        {
            Pt::Net::Endpoint ep("127.0.0.1", 8050);

            Pt::Http::Server server(*_loop, ep);
            IdleService service(*_loop, false);
            Pt::Http::WebSocketServlet sockets(service);

            Pt::Http::MapUrl mapUrl("/ws", service);
            server.addServlet(mapUrl);

            Pt::Http::Client http(*_loop, ep);
            Pt::Http::WebSocket socket(http);
            socket.setPingTimeout(200);
            socket.connected() += Pt::slot(*this, &WebSocketTest::onConnectedPingTimeout);
            socket.pingReady() += Pt::slot(*this, &WebSocketTest::onPingTimeoutReady);
            socket.closed() += Pt::slot(*this, &WebSocketTest::onIdleClosed);
            socket.beginConnect("/ws");

            _loop->run();

            PT_UNIT_ASSERT(_closed);
        }

        void onConnectedPingTimeout(Pt::Http::WebSocket& socket)
        {
            socket.endConnect();
            socket.beginPing();
        }

        void onPingTimeoutReady(Pt::Http::WebSocket& socket)
        {
            socket.endPing();
        }

    protected:
        void PingTimeoutClearedByPong()
        {
            Pt::Net::Endpoint ep("127.0.0.1", 8051);

            Pt::Http::Server server(*_loop, ep);
            bool serverClosed = false;
            RecordService service(_message, _type, _received, serverClosed, *_loop);
            Pt::Http::WebSocketServlet sockets(service);

            Pt::Http::MapUrl mapUrl("/ws", service);
            server.addServlet(mapUrl);

            Pt::Http::Client http(*_loop, ep);
            Pt::Http::WebSocket socket(http);
            _limitSocket = &socket;
            socket.setPingTimeout(50);
            socket.connected() += Pt::slot(*this, &WebSocketTest::onConnectedPingPongWait);
            socket.pingReady() += Pt::slot(*this, &WebSocketTest::onPingTimeoutReady);
            socket.closed() += Pt::slot(*this, &WebSocketTest::onHttpTimeoutClosed);
            socket.beginConnect("/ws");

            _loop->run();

            PT_UNIT_ASSERT(!_closed);
            PT_UNIT_ASSERT(_finished);
            _limitSocket = 0;
        }

        void onConnectedPingPongWait(Pt::Http::WebSocket& socket)
        {
            socket.endConnect();
            socket.beginReceive();
            socket.beginPing();
            _waitTimer.setActive(*_loop);
            _waitTimer.start(200);
        }

    protected:
        void CloseTimeoutNoReply()
        {
            Pt::Net::Endpoint ep("127.0.0.1", 8052);

            Pt::Http::Server server(*_loop, ep);
            IdleService service(*_loop, false);
            Pt::Http::WebSocketServlet sockets(service);

            Pt::Http::MapUrl mapUrl("/ws", service);
            server.addServlet(mapUrl);

            Pt::Http::Client http(*_loop, ep);
            Pt::Http::WebSocket socket(http);
            socket.setCloseTimeout(200);
            socket.connected() += Pt::slot(*this, &WebSocketTest::onConnectedCloseTimeout);
            socket.shutdownReady() += Pt::slot(*this, &WebSocketTest::onCloseTimeoutFailed);
            socket.beginConnect("/ws");

            _loop->run();

            PT_UNIT_ASSERT(_rejected);
            PT_UNIT_ASSERT_EQUALS(socket.closeCode(), 1006u);
            PT_UNIT_ASSERT(socket.closeReason().empty());
        }

        void onConnectedCloseTimeout(Pt::Http::WebSocket& socket)
        {
            socket.endConnect();
            socket.beginShutdown();
        }

        void onCloseTimeoutFailed(Pt::Http::WebSocket& socket)
        {
            try
            {
                socket.endShutdown();
            }
            catch(const std::exception&)
            {
                _rejected = true;
            }

            socket.close();
            _loop->exit();
        }

    protected:
        void CloseTimeoutHandshake()
        {
            Pt::Net::Endpoint ep("127.0.0.1", 8053);

            Pt::Http::Server server(*_loop, ep);
            RecordService service(_message, _type, _received, _closed, *_loop);
            Pt::Http::WebSocketServlet sockets(service);

            Pt::Http::MapUrl mapUrl("/ws", service);
            server.addServlet(mapUrl);

            Pt::Http::Client http(*_loop, ep);
            Pt::Http::WebSocket socket(http);
            socket.setCloseTimeout(200);
            socket.connected() += Pt::slot(*this, &WebSocketTest::onConnectedCloseTimeout);
            socket.shutdownReady() += Pt::slot(*this, &WebSocketTest::onShutdownReady);
            socket.beginConnect("/ws");

            _loop->run();

            PT_UNIT_ASSERT(_shutdownReady);
        }

    protected:
        void ReceiveDuringShutdown()
        {
            Pt::Net::Endpoint ep("127.0.0.1", 8046);

            Pt::Http::Server server(*_loop, ep);
            FeedService service;
            Pt::Http::WebSocketServlet sockets(service);

            Pt::Http::MapUrl mapUrl("/ws", service);
            server.addServlet(mapUrl);

            Pt::Http::Client http(*_loop, ep);
            Pt::Http::WebSocket socket(http);
            socket.connected() += Pt::slot(*this, &WebSocketTest::onConnectedReceiveDuringShutdown);
            socket.inputReady() += Pt::slot(*this, &WebSocketTest::onReceiveDuringShutdownInput);
            socket.shutdownReady() += Pt::slot(*this, &WebSocketTest::onReceiveDuringShutdownReady);
            socket.beginConnect("/ws");

            _loop->run();

            PT_UNIT_ASSERT(_received);
            PT_UNIT_ASSERT_EQUALS(_message, "feed");
            PT_UNIT_ASSERT(_shutdownReady);
        }

        void onConnectedReceiveDuringShutdown(Pt::Http::WebSocket& socket)
        {
            socket.endConnect();
            socket.beginReceive();
            socket.beginShutdown();
        }

        void onReceiveDuringShutdownInput(Pt::Http::WebSocket& socket)
        {
            Pt::Http::WebSocketProgress progress = socket.endReceive();
            _message.append( std::istreambuf_iterator<char>( socket.incoming().body() ),
                             std::istreambuf_iterator<char>() );

            if( ! progress.finished() )
            {
                socket.beginReceive();
                return;
            }

            _received = true;
        }

        void onReceiveDuringShutdownReady(Pt::Http::WebSocket& socket)
        {
            socket.endShutdown();
            socket.close();
            _shutdownReady = true;
            _loop->exit();
        }

    protected:
        void ProtocolFailure()
        {
            Pt::Net::Endpoint ep("127.0.0.1", 8047);

            Pt::Http::Server server(*_loop, ep);
            FailService service(_rejected, _closed, _status, *_loop);
            service.setMaxMessageSize(16);
            Pt::Http::WebSocketServlet sockets(service);

            Pt::Http::MapUrl mapUrl("/ws", service);
            server.addServlet(mapUrl);

            Pt::Http::Client http(*_loop, ep);
            Pt::Http::WebSocket socket(http);
            socket.connected() += Pt::slot(*this, &WebSocketTest::onConnectedProtocolFailure);
            socket.beginConnect("/ws");

            _loop->run();

            PT_UNIT_ASSERT(_rejected);
            PT_UNIT_ASSERT(_closed);
            PT_UNIT_ASSERT_EQUALS(_status, 1009u);
        }

        void onConnectedProtocolFailure(Pt::Http::WebSocket& socket)
        {
            socket.endConnect();
            socket.outgoing().setType(Pt::Http::WebSocketMessage::Text);
            socket.outgoing().body() << std::string(64, 'x');
            socket.beginSend();
        }

    protected:
        void ProtocolFailureDuringShutdown()
        {
            Pt::Net::Endpoint ep("127.0.0.1", 8115);

            Pt::Http::Server server(*_loop, ep);
            OversizedSendService service(*_loop);
            Pt::Http::WebSocketServlet sockets(service);

            Pt::Http::MapUrl mapUrl("/ws", service);
            server.addServlet(mapUrl);

            Pt::Http::Client http(*_loop, ep);
            Pt::Http::WebSocket socket(http);
            socket.setMaxMessageSize(16);
            socket.setCloseTimeout(0);
            socket.connected() += Pt::slot(*this, &WebSocketTest::onConnectedProtocolFailureDuringShutdown);
            socket.shutdownReady() += Pt::slot(*this, &WebSocketTest::onProtocolFailureDuringShutdownReady);
            socket.beginConnect("/ws");

            _loop->run();

            PT_UNIT_ASSERT(_rejected);
            PT_UNIT_ASSERT(_shutdownReady);
            PT_UNIT_ASSERT_EQUALS(_status, 1009u);
        }

        void onConnectedProtocolFailureDuringShutdown(Pt::Http::WebSocket& socket)
        {
            socket.endConnect();
            socket.beginShutdown();
        }

        void onProtocolFailureDuringShutdownReady(Pt::Http::WebSocket& socket)
        {
            try
            {
                socket.endShutdown();
            }
            catch(const Pt::Http::WebSocketError& e)
            {
                _rejected = true;
                _status = e.code();
            }

            PT_UNIT_ASSERT_EQUALS(socket.closeCode(), 1006u);
            socket.close();
            _shutdownReady = true;
            _loop->exit();
        }

    protected:
        void ProtocolFailureDuringShutdownUtf8()
        {
            Pt::Net::Endpoint ep("127.0.0.1", 8117);

            Pt::Http::Server server(*_loop, ep);
            InvalidUtf8SendService service(*_loop);
            Pt::Http::WebSocketServlet sockets(service);

            Pt::Http::MapUrl mapUrl("/ws", service);
            server.addServlet(mapUrl);

            Pt::Http::Client http(*_loop, ep);
            Pt::Http::WebSocket socket(http);
            socket.setCloseTimeout(0);
            socket.connected() += Pt::slot(*this, &WebSocketTest::onConnectedProtocolFailureDuringShutdown);
            socket.shutdownReady() += Pt::slot(*this, &WebSocketTest::onProtocolFailureDuringShutdownReady);
            socket.beginConnect("/ws");

            _loop->run();

            PT_UNIT_ASSERT(_rejected);
            PT_UNIT_ASSERT(_shutdownReady);
            PT_UNIT_ASSERT_EQUALS(_status, 1007u);
        }

    protected:
        void ProtocolFailureReceiveDuringShutdown()
        {
            Pt::Net::Endpoint ep("127.0.0.1", 8116);

            Pt::Http::Server server(*_loop, ep);
            OversizedSendService service(*_loop);
            Pt::Http::WebSocketServlet sockets(service);

            Pt::Http::MapUrl mapUrl("/ws", service);
            server.addServlet(mapUrl);

            Pt::Http::Client http(*_loop, ep);
            Pt::Http::WebSocket socket(http);
            socket.setMaxMessageSize(16);
            socket.setCloseTimeout(200);
            socket.connected() += Pt::slot(*this, &WebSocketTest::onConnectedProtocolFailureReceiveDuringShutdown);
            socket.inputReady() += Pt::slot(*this, &WebSocketTest::onProtocolFailureReceiveDuringShutdownInput);
            socket.shutdownReady() += Pt::slot(*this, &WebSocketTest::onProtocolFailureDuringShutdownReady);
            socket.beginConnect("/ws");

            _loop->run();

            PT_UNIT_ASSERT(_rejected);
            PT_UNIT_ASSERT(_shutdownReady);
            PT_UNIT_ASSERT( ! _received );
            PT_UNIT_ASSERT_EQUALS(_status, 1009u);
        }

        void onConnectedProtocolFailureReceiveDuringShutdown(Pt::Http::WebSocket& socket)
        {
            socket.endConnect();
            socket.beginReceive();
            socket.beginShutdown();
        }

        void onProtocolFailureReceiveDuringShutdownInput(Pt::Http::WebSocket& socket)
        {
            socket.endReceive();
            _received = true;
        }

    protected:
        void ReadyExceptionReleases()
        {
            Pt::Net::Endpoint ep("127.0.0.1", 8060);

            Pt::Http::Server server(*_loop, ep);
            ReadyExceptionService service(*_loop);
            service.setMaxMessageSize(16);
            Pt::Http::WebSocketServlet sockets(service);

            Pt::Http::MapUrl mapUrl("/ws", service);
            server.addServlet(mapUrl);

            Pt::Http::Client http(*_loop, ep);
            Pt::Http::WebSocket socket(http);
            socket.connected() += Pt::slot(*this, &WebSocketTest::onConnectedProtocolFailure);
            socket.beginConnect("/ws");

            _loop->run();

            PT_UNIT_ASSERT_EQUALS(service.live(), static_cast<std::size_t>(0));
            PT_UNIT_ASSERT_EQUALS(sockets.size(), static_cast<std::size_t>(0));
        }

    protected:
        void ServerCancelReleases()
        {
            Pt::Net::Endpoint ep("127.0.0.1", 8046);

            Pt::Http::Server server(*_loop, ep);
            _server = &server;
            IdleService service(*_loop, false);
            Pt::Http::WebSocketServlet sockets(service);

            Pt::Http::MapUrl mapUrl("/ws", service);
            server.addServlet(mapUrl);

            Pt::Http::Client http(*_loop, ep);
            Pt::Http::WebSocket socket(http);
            socket.connected() += Pt::slot(*this, &WebSocketTest::onConnectedServerCancel);

            socket.beginConnect("/ws");

            _loop->run();

            PT_UNIT_ASSERT_EQUALS(service.live(), static_cast<std::size_t>(0));
            PT_UNIT_ASSERT_EQUALS(sockets.size(), static_cast<std::size_t>(0));
            PT_UNIT_ASSERT_EQUALS(service.socketCount(), static_cast<std::size_t>(0));
        }

        void onConnectedServerCancel(Pt::Http::WebSocket& socket)
        {
            socket.endConnect();
            _server->cancel();
            _loop->exit();
        }

    protected:
        void IdleTimeoutTwice()
        {
            Pt::Net::Endpoint ep("127.0.0.1", 8045);

            Pt::Http::Server server(*_loop, ep);
            IdleService service(*_loop, false);
            Pt::Http::WebSocketServlet sockets(service);

            Pt::Http::MapUrl mapUrl("/ws", service);
            server.addServlet(mapUrl);

            Pt::Http::Client http(*_loop, ep);
            Pt::Http::WebSocket socket(http);
            socket.connected() += Pt::slot(*this, &WebSocketTest::onConnectedIdleTwice);
            socket.beginConnect("/ws");

            _loop->run();

            PT_UNIT_ASSERT(_finished);
        }

        void onConnectedIdleTwice(Pt::Http::WebSocket& socket)
        {
            socket.endConnect();

            try
            {
                socket.setIdleTimeout(1000);
                socket.setIdleTimeout(2000);
                _finished = true;
            }
            catch(const std::exception&)
            {
            }

            _loop->exit();
        }

    protected:
        void PingIntervalPong()
        {
            Pt::Net::Endpoint ep("127.0.0.1", 8054);

            Pt::Http::Server server(*_loop, ep);
            RecordService service(_message, _type, _received, _closed, *_loop);
            Pt::Http::WebSocketServlet sockets(service);

            Pt::Http::MapUrl mapUrl("/ws", service);
            server.addServlet(mapUrl);

            Pt::Http::Client http(*_loop, ep);
            Pt::Http::WebSocket socket(http);
            socket.setPingInterval(50);
            socket.setPingTimeout(200);
            socket.connected() += Pt::slot(*this, &WebSocketTest::onConnectedIntervalReceive);
            socket.pong() += Pt::slot(*this, &WebSocketTest::onIntervalPong);
            socket.pingReady() += Pt::slot(*this, &WebSocketTest::onIntervalPingReady);
            socket.closed() += Pt::slot(*this, &WebSocketTest::onHttpTimeoutClosed);
            socket.beginConnect("/ws");

            _loop->run();

            PT_UNIT_ASSERT(_pong);
            PT_UNIT_ASSERT(!_pingReady);
            PT_UNIT_ASSERT(!_closed);
            PT_UNIT_ASSERT(_finished);
        }

        void onConnectedIntervalReceive(Pt::Http::WebSocket& socket)
        {
            socket.endConnect();
            socket.beginReceive();
        }

        void onIntervalPong(Pt::Http::WebSocket&, const std::string&)
        {
            if(_pong)
                return;

            _pong = true;
            _waitTimer.setActive(*_loop);
            _waitTimer.start(100);
        }

        void onIntervalPingReady(Pt::Http::WebSocket& socket)
        {
            socket.endPing();
            _pingReady = true;
        }

    protected:
        void PingIntervalTimeoutNoReply()
        {
            Pt::Net::Endpoint ep("127.0.0.1", 8055);

            Pt::Http::Server server(*_loop, ep);
            IdleService service(*_loop, false);
            Pt::Http::WebSocketServlet sockets(service);

            Pt::Http::MapUrl mapUrl("/ws", service);
            server.addServlet(mapUrl);

            Pt::Http::Client http(*_loop, ep);
            Pt::Http::WebSocket socket(http);
            socket.setPingInterval(50);
            socket.setPingTimeout(200);
            socket.connected() += Pt::slot(*this, &WebSocketTest::onConnectedIntervalReceive);
            socket.closed() += Pt::slot(*this, &WebSocketTest::onIdleClosed);
            socket.beginConnect("/ws");

            _loop->run();

            PT_UNIT_ASSERT(_closed);
            PT_UNIT_ASSERT_EQUALS(socket.closeCode(), 1006u);
        }

    protected:
        void PingIntervalWithoutReceive()
        {
            Pt::Net::Endpoint ep("127.0.0.1", 8056);

            Pt::Http::Server server(*_loop, ep);
            IdleService service(*_loop, false);
            Pt::Http::WebSocketServlet sockets(service);

            Pt::Http::MapUrl mapUrl("/ws", service);
            server.addServlet(mapUrl);

            Pt::Http::Client http(*_loop, ep);
            Pt::Http::WebSocket socket(http);
            socket.setPingInterval(50);
            socket.setPingTimeout(50);
            socket.connected() += Pt::slot(*this, &WebSocketTest::onConnectedIntervalNoReceive);
            socket.closed() += Pt::slot(*this, &WebSocketTest::onHttpTimeoutClosed);
            socket.beginConnect("/ws");

            _loop->run();

            PT_UNIT_ASSERT(!_closed);
            PT_UNIT_ASSERT(_finished);
        }

        void onConnectedIntervalNoReceive(Pt::Http::WebSocket& socket)
        {
            socket.endConnect();
            _waitTimer.setActive(*_loop);
            _waitTimer.start(200);
        }

    protected:
        void PingIntervalResetByInbound()
        {
            Pt::Net::Endpoint ep("127.0.0.1", 8057);

            Pt::Http::Server server(*_loop, ep);
            PulseService service(*_loop);
            Pt::Http::WebSocketServlet sockets(service);

            Pt::Http::MapUrl mapUrl("/ws", service);
            server.addServlet(mapUrl);

            Pt::Http::Client http(*_loop, ep);
            Pt::Http::WebSocket socket(http);
            socket.setPingInterval(80);
            socket.setPingTimeout(200);
            socket.connected() += Pt::slot(*this, &WebSocketTest::onConnectedIntervalPulse);
            socket.inputReady() += Pt::slot(*this, &WebSocketTest::onIntervalPulseInput);
            socket.pong() += Pt::slot(*this, &WebSocketTest::onIntervalPulsePong);
            socket.closed() += Pt::slot(*this, &WebSocketTest::onHttpTimeoutClosed);
            socket.beginConnect("/ws");

            _loop->run();

            PT_UNIT_ASSERT(!_pong);
            PT_UNIT_ASSERT(!_closed);
            PT_UNIT_ASSERT(_finished);
        }

        void onConnectedIntervalPulse(Pt::Http::WebSocket& socket)
        {
            socket.endConnect();
            socket.beginReceive();
            _waitTimer.setActive(*_loop);
            _waitTimer.start(250);
        }

        void onIntervalPulseInput(Pt::Http::WebSocket& socket)
        {
            socket.endReceive();
            socket.incoming().clear();
            socket.beginReceive();
        }

        void onIntervalPulsePong(Pt::Http::WebSocket&, const std::string&)
        {
            _pong = true;
            _loop->exit();
        }

    protected:
        void PingIntervalYieldsToBeginPing()
        {
            Pt::Net::Endpoint ep("127.0.0.1", 8058);

            Pt::Http::Server server(*_loop, ep);
            RecordService service(_message, _type, _received, _closed, *_loop);
            Pt::Http::WebSocketServlet sockets(service);

            Pt::Http::MapUrl mapUrl("/ws", service);
            server.addServlet(mapUrl);

            Pt::Http::Client http(*_loop, ep);
            Pt::Http::WebSocket socket(http);
            socket.setPingInterval(50);
            socket.setPingTimeout(200);
            socket.connected() += Pt::slot(*this, &WebSocketTest::onConnectedIntervalYield);
            socket.pingReady() += Pt::slot(*this, &WebSocketTest::onPingReady);
            socket.pong() += Pt::slot(*this, &WebSocketTest::onIntervalYieldPong);
            socket.closed() += Pt::slot(*this, &WebSocketTest::onHttpTimeoutClosed);
            socket.beginConnect("/ws");

            _loop->run();

            PT_UNIT_ASSERT(_pong);
            PT_UNIT_ASSERT(_pingReady);
            PT_UNIT_ASSERT_EQUALS(_message, std::string("user"));
            PT_UNIT_ASSERT(!_closed);
        }

        void onConnectedIntervalYield(Pt::Http::WebSocket& socket)
        {
            socket.endConnect();
            socket.beginReceive();
            socket.beginPing("user", 4);
        }

        void onIntervalYieldPong(Pt::Http::WebSocket&, const std::string& payload)
        {
            if(payload != "user")
                return;

            _message = payload;
            _pong = true;
            if(_pingReady)
                _loop->exit();
        }

    protected:
        void InputReadyThrows()
        {
            Pt::Net::Endpoint ep("127.0.0.1", 8106);

            Pt::Http::Server server(*_loop, ep);
            PushService service;
            Pt::Http::WebSocketServlet sockets(service);
            Pt::Http::MapUrl mapUrl("/ws", service);
            server.addServlet(mapUrl);

            Pt::Http::Client http(*_loop, ep);
            Pt::Http::WebSocket socket(http);
            socket.connected() += Pt::slot(*this, &WebSocketTest::onPushConnected);
            socket.inputReady() += Pt::slot(*this, &WebSocketTest::onInputReadyThrows);
            socket.beginConnect("/ws");

            bool thrown = false;
            try
            {
                _loop->run();
            }
            catch(const std::runtime_error& error)
            {
                PT_UNIT_ASSERT_EQUALS(std::string(error.what()), "input ready slot failed");
                thrown = true;
            }

            PT_UNIT_ASSERT(thrown);
            PT_UNIT_ASSERT_EQUALS(socket.closeCode(), 0u);
            socket.beginReceive();
        }

        void onInputReadyThrows(Pt::Http::WebSocket& socket)
        {
            socket.endReceive();
            throw std::runtime_error("input ready slot failed");
        }

    protected:
        void OutputReadyThrows()
        {
            Pt::Net::Endpoint ep("127.0.0.1", 8107);

            Pt::Http::Server server(*_loop, ep);
            IdleService service(*_loop, false);
            Pt::Http::WebSocketServlet sockets(service);
            Pt::Http::MapUrl mapUrl("/ws", service);
            server.addServlet(mapUrl);

            Pt::Http::Client http(*_loop, ep);
            Pt::Http::WebSocket socket(http);
            socket.connected() += Pt::slot(*this, &WebSocketTest::onConnected);
            socket.outputReady() += Pt::slot(*this, &WebSocketTest::onOutputReadyThrows);
            socket.beginConnect("/ws");

            bool thrown = false;
            try
            {
                _loop->run();
            }
            catch(const std::runtime_error& error)
            {
                PT_UNIT_ASSERT_EQUALS(std::string(error.what()), "output ready slot failed");
                thrown = true;
            }

            PT_UNIT_ASSERT(thrown);
            PT_UNIT_ASSERT_EQUALS(socket.closeCode(), 0u);
            socket.beginPing();
        }

        void onOutputReadyThrows(Pt::Http::WebSocket& socket)
        {
            socket.endSend();
            throw std::runtime_error("output ready slot failed");
        }

    protected:
        void ReceiveErrorLeavesLoop()
        {
            Pt::Net::Endpoint ep("127.0.0.1", 8108);

            Pt::Http::Server server(*_loop, ep);
            LongPushService service;
            Pt::Http::WebSocketServlet sockets(service);
            Pt::Http::MapUrl mapUrl("/ws", service);
            server.addServlet(mapUrl);

            Pt::Http::Client http(*_loop, ep);
            Pt::Http::WebSocket socket(http);
            socket.setMaxMessageSize(16);
            socket.connected() += Pt::slot(*this, &WebSocketTest::onPushConnected);
            socket.inputReady() += Pt::slot(*this, &WebSocketTest::onReceiveErrorInput);
            socket.beginConnect("/ws");

            bool thrown = false;
            try
            {
                _loop->run();
            }
            catch(const Pt::Http::WebSocketError&)
            {
                thrown = true;
            }
            catch(const Pt::System::IOError&)
            {
                thrown = true;
            }

            PT_UNIT_ASSERT(thrown);
        }

        void onReceiveErrorInput(Pt::Http::WebSocket& socket)
        {
            socket.endReceive();
        }

    protected:
        void ShutdownWriteError()
        {
            Pt::Net::Endpoint ep("127.0.0.1", 8112);

            Pt::Http::Server server(*_loop, ep);
            DropTcpService service(*_loop);
            Pt::Http::WebSocketServlet sockets(service);
            Pt::Http::MapUrl mapUrl("/ws", service);
            server.addServlet(mapUrl);

            Pt::Http::Client http(*_loop, ep);
            Pt::Http::WebSocket socket(http);
            Pt::System::Timer wait;
            wait.timeout() += Pt::slot(*this, &WebSocketTest::onShutdownWriteWait);
            _ioWait = &wait;
            _limitSocket = &socket;
            socket.connected() += Pt::slot(*this, &WebSocketTest::onDeadPeerConnected);
            socket.shutdownReady() += Pt::slot(*this, &WebSocketTest::onShutdownWriteReady);
            socket.closed() += Pt::slot(*this, &WebSocketTest::onDeadPeerClosed);
            socket.beginConnect("/ws");

            _loop->run();

            PT_UNIT_ASSERT(_rejected);
            _limitSocket = 0;
            _ioWait = 0;
        }

        void onDeadPeerConnected(Pt::Http::WebSocket& socket)
        {
            socket.endConnect();
            _ioWait->setActive(*_loop);
            _ioWait->start(200);
        }

        void onShutdownWriteWait()
        {
            _ioWait->stop();
            Pt::Http::WebSocket& socket = *_limitSocket;
            try
            {
                socket.beginShutdown();
            }
            catch(const Pt::System::IOError&)
            {
                _rejected = true;
                PT_UNIT_ASSERT_THROW(socket.beginSend(), Pt::System::IOError);
                _loop->exit();
            }
        }

        void onShutdownWriteReady(Pt::Http::WebSocket& socket)
        {
            try
            {
                socket.endShutdown();
            }
            catch(const Pt::System::IOError&)
            {
                _rejected = true;
            }

            _loop->exit();
        }

        void onDeadPeerClosed(Pt::Http::WebSocket&)
        {
            _closed = true;
            _loop->exit();
        }

    protected:
        void DeferredReceiveOneSignal()
        {
            Pt::Net::Endpoint ep("127.0.0.1", 8113);

            Pt::Http::Server server(*_loop, ep);
            PushService service;
            Pt::Http::WebSocketServlet sockets(service);
            Pt::Http::MapUrl mapUrl("/ws", service);
            server.addServlet(mapUrl);

            Pt::Http::Client http(*_loop, ep);
            Pt::Http::WebSocket socket(http);
            Pt::System::Timer wait;
            wait.timeout() += Pt::slot(*this, &WebSocketTest::onDeferredOneWait);
            _ioWait = &wait;
            socket.connected() += Pt::slot(*this, &WebSocketTest::onDeferredOneConnected);
            socket.inputReady() += Pt::slot(*this, &WebSocketTest::onDeferredOneInput);
            socket.beginConnect("/ws");

            _loop->run();

            PT_UNIT_ASSERT_EQUALS(_status, 1u);
            _ioWait = 0;
        }

        void onDeferredOneConnected(Pt::Http::WebSocket& socket)
        {
            socket.endConnect();
            socket.beginReceive();
            socket.beginShutdown();
        }

        void onDeferredOneInput(Pt::Http::WebSocket&)
        {
            ++_status;
            if(_status == 1 && _ioWait)
            {
                _ioWait->setActive(*_loop);
                _ioWait->start(150);
            }
        }

        void onDeferredOneWait()
        {
            _ioWait->stop();
            _loop->exit();
        }

    protected:
        void PeerCloseClosedOnce()
        {
            Pt::Net::Endpoint ep("127.0.0.1", 8114);

            Pt::Http::Server server(*_loop, ep);
            ShutdownService service(_shutdownReady, _closed, *_loop);
            Pt::Http::WebSocketServlet sockets(service);
            Pt::Http::MapUrl mapUrl("/ws", service);
            server.addServlet(mapUrl);

            Pt::Http::Client http(*_loop, ep);
            Pt::Http::WebSocket socket(http);
            socket.connected() += Pt::slot(*this, &WebSocketTest::onPushConnected);
            socket.closed() += Pt::slot(*this, &WebSocketTest::onPeerCloseOnce);
            socket.beginConnect("/ws");

            _loop->run();

            PT_UNIT_ASSERT_EQUALS(_status, 1u);
            PT_UNIT_ASSERT(_closed);
        }

        void onPeerCloseOnce(Pt::Http::WebSocket&)
        {
            ++_status;
            _closed = true;
            _loop->exit();
        }

    private:
        Pt::System::MainLoop* _loop;
        Pt::System::Timer _exitTimer;
        Pt::System::Timer _waitTimer;
        Pt::System::Timer _deferTimer;
        std::string _message;
        std::string _offered;
        std::string _copied;
        bool _received;
        bool _declined;
        bool _closed;
        bool _partial;
        bool _finished;
        bool _sendOpened;
        bool _pong;
        bool _inputReady;
        bool _pingReady;
        bool _shutdownReady;
        bool _handshake;
        bool _upgraded;
        bool _rejected;
        unsigned _status;
        Pt::Http::WebSocketMessage::Type _type;
        Pt::Http::WebSocket* _limitSocket;
        Pt::Http::Client* _limitClient;
        char _raw[512];
        DeferredCloseService* _deferredCloseService;
        Pt::System::Timer* _ioWait;
        Pt::Http::Server* _server;
};

Pt::Unit::RegisterTest<WebSocketTest> register_HttpWebSocketTest;
