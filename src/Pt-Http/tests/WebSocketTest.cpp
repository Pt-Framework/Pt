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
#include "Pt/Http/Message.h"
#include "Pt/Http/WebSocketService.h"
#include "Pt/Http/WebSocketServlet.h"
#include "Pt/Http/WebSocketSession.h"
#include "Pt/Http/Client.h"
#include "Pt/Http/Request.h"
#include "Pt/Http/Reply.h"
#include "Pt/Net/Endpoint.h"
#include "Pt/System/MainLoop.h"
#include "Pt/System/Timer.h"

#include <iterator>
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

        virtual void onWriteReply(const Pt::Http::Request&, Pt::Http::Reply& reply, Pt::System::EventLoop&)
        {
            reply.setStatus(101, "Switching Protocols");
            reply.header().setUpgrade();
            reply.header().set("Upgrade", "websocket");
            setReady(true);
        }
};

typedef Pt::Http::BasicService<DeclineResponder> DeclineService;

class RecordSession : public Pt::Http::WebSocketSession
{
    public:
        RecordSession(Pt::Http::WebSocketService& service,
                      Pt::Http::Stream& stream,
                      const Pt::Http::Reply& reply,
                      std::string& message,
                      Pt::Http::WebSocketMessage::Type& type,
                      bool& received,
                      bool& closed,
                      Pt::System::EventLoop& exitLoop,
                      unsigned* closeCode = 0)
        : Pt::Http::WebSocketSession(service, stream, reply)
        , _message(&message)
        , _type(&type)
        , _received(&received)
        , _closed(&closed)
        , _exitLoop(&exitLoop)
        , _closeCode(closeCode)
        {
            beginReceive();
        }

    protected:
        virtual void onInput()
        {
            try
            {
                endReceive();

                _message->assign( std::istreambuf_iterator<char>( incoming().body() ),
                                  std::istreambuf_iterator<char>() );
                *_type = incoming().type();
                *_received = true;
                _exitLoop->exit();
            }
            catch(const std::exception&)
            {
                if(_closeCode)
                    *_closeCode = closeCode();

                *_closed = true;
                _exitLoop->exit();
            }
        }

        virtual void onOutput()
        {
            endSend();
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
            beginReceive();
        }

    protected:
        virtual void onInput()
        {
            Pt::Http::MessageProgress progress = endReceive();
            std::string chunk;
            chunk.assign( std::istreambuf_iterator<char>( incoming().body() ),
                          std::istreambuf_iterator<char>() );
            *_message += chunk;
            *_type = incoming().type();

            if( ! progress.finished() )
            {
                incoming().discard();
                beginReceive();
                return;
            }

            *_received = true;
            _exitLoop->exit();
        }

        virtual void onOutput()
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

    protected:
        virtual void onInput()
        {}

        virtual void onOutput()
        {}

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
                             bool& closed,
                             Pt::System::EventLoop& exitLoop)
        : Pt::Http::WebSocketSession(service, stream, reply)
        , _message(&message)
        , _type(&type)
        , _received(&received)
        , _closed(&closed)
        , _exitLoop(&exitLoop)
        {}

        void beginDeferredReceive()
        {
            beginReceive();
        }

    protected:
        virtual void onInput()
        {
            try
            {
                Pt::Http::MessageProgress progress = endReceive();
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
            catch(const std::exception&)
            {
                *_closed = true;
                _exitLoop->exit();
            }
        }

        virtual void onOutput()
        {
            endSend();
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
                            bool& closed,
                            Pt::System::EventLoop& exitLoop)
        : Pt::Http::WebSocketSession(service, stream, reply)
        , _partial(&partial)
        , _finished(&finished)
        , _closed(&closed)
        , _exitLoop(&exitLoop)
        {
            beginReceive();
        }

    protected:
        virtual void onInput()
        {
            try
            {
                Pt::Http::MessageProgress progress = endReceive();
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
            catch(const std::exception&)
            {
                *_closed = true;
                _exitLoop->exit();
            }
        }

        virtual void onOutput()
        {
            endSend();
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

    protected:
        virtual void onInput()
        {}

        virtual void onOutput()
        {}
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
            outgoing().setType(Pt::Http::WebSocketMessage::Text);
            outgoing().body() << "feed";
            beginSend();
        }

    protected:
        virtual void onInput()
        {}

        virtual void onOutput()
        {
            endSend();
        }
};

typedef Pt::Http::BasicWebSocketService<PushSession> PushService;


class FeedSession : public Pt::Http::WebSocketSession
{
    public:
        FeedSession(Pt::Http::WebSocketService& service,
                    Pt::Http::Stream& stream,
                    const Pt::Http::Reply& reply)
        : Pt::Http::WebSocketSession(service, stream, reply)
        {
            outgoing().setType(Pt::Http::WebSocketMessage::Text);
            outgoing().body() << "feed";
            beginSend();
            beginReceive();
        }

    protected:
        virtual void onInput()
        {
            endReceive();
        }

        virtual void onOutput()
        {
            endSend();
        }
};

typedef Pt::Http::BasicWebSocketService<FeedSession> FeedService;


class PongSession : public Pt::Http::WebSocketSession
{
    public:
        PongSession(Pt::Http::WebSocketService& service,
                    Pt::Http::Stream& stream,
                    const Pt::Http::Reply& reply,
                    bool& pong,
                    bool& pingReady,
                    bool& inputReady,
                    std::string& payload,
                    Pt::System::EventLoop& exitLoop)
        : Pt::Http::WebSocketSession(service, stream, reply)
        , _pong(&pong)
        , _pingReady(&pingReady)
        , _inputReady(&inputReady)
        , _payload(&payload)
        , _exitLoop(&exitLoop)
        {
            beginPing("session-ping", 12);
            beginReceive();
        }

    protected:
        virtual void onInput()
        {
            *_inputReady = true;
            _exitLoop->exit();
        }

        virtual void onOutput()
        {}

        virtual void onPingReady()
        {
            endPing();
            *_pingReady = true;
            finish();
        }

        virtual void onPong(const std::string& payload)
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
                        bool& shutdownReady,
                        bool& closed,
                        Pt::System::EventLoop& exitLoop)
        : Pt::Http::WebSocketSession(service, stream, reply)
        , _shutdownReady(&shutdownReady)
        , _closed(&closed)
        , _exitLoop(&exitLoop)
        {
            beginShutdown();
        }

    protected:
        virtual void onInput()
        {}

        virtual void onOutput()
        {}

        virtual void onShutdownReady()
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

    protected:
        virtual void onInput()
        {}

        virtual void onOutput()
        {}

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

class FailSession : public Pt::Http::WebSocketSession
{
    public:
        FailSession(Pt::Http::WebSocketService& service,
                    Pt::Http::Stream& stream,
                    const Pt::Http::Reply& reply,
                    bool& failed,
                    bool& closed,
                    unsigned& code,
                    Pt::System::EventLoop& exitLoop)
        : Pt::Http::WebSocketSession(service, stream, reply)
        , _failed(&failed)
        , _closed(&closed)
        , _code(&code)
        , _exitLoop(&exitLoop)
        {
            beginReceive();
        }

    protected:
        virtual void onInput()
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
            }
        }

        virtual void onOutput()
        {}

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

    protected:
        virtual void onInput()
        {}

        virtual void onOutput()
        {}
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
        {
            registerMethod("Text", *this, &WebSocketTest::Text);
            registerMethod("Accepted", *this, &WebSocketTest::Accepted);
            registerMethod("NoScope", *this, &WebSocketTest::NoScope);
            registerMethod("Scope", *this, &WebSocketTest::Scope);
            registerMethod("SecondScope", *this, &WebSocketTest::SecondScope);
            registerMethod("PeerClose", *this, &WebSocketTest::PeerClose);
            registerMethod("DataBeforeClose", *this, &WebSocketTest::DataBeforeClose);
            registerMethod("ClosePartialReceive", *this, &WebSocketTest::ClosePartialReceive);
            registerMethod("Decline", *this, &WebSocketTest::Decline);
            registerMethod("NullSession", *this, &WebSocketTest::NullSession);
            registerMethod("ConstructFails", *this, &WebSocketTest::ConstructFails);
            registerMethod("Push", *this, &WebSocketTest::Push);
            registerMethod("Pong", *this, &WebSocketTest::Pong);
            registerMethod("SessionPong", *this, &WebSocketTest::SessionPong);
            registerMethod("SessionShutdown", *this, &WebSocketTest::SessionShutdown);
            registerMethod("PingPayload", *this, &WebSocketTest::PingPayload);
            registerMethod("Limit", *this, &WebSocketTest::Limit);
            registerMethod("Binary", *this, &WebSocketTest::Binary);
            registerMethod("Empty", *this, &WebSocketTest::Empty);
            registerMethod("Large", *this, &WebSocketTest::Large);
            registerMethod("CloseHandshake", *this, &WebSocketTest::CloseHandshake);
            registerMethod("Close", *this, &WebSocketTest::Close);
            registerMethod("CloseReceive", *this, &WebSocketTest::CloseReceive);
            registerMethod("CloseDestructor", *this, &WebSocketTest::CloseDestructor);
            registerMethod("CloseNoFrame", *this, &WebSocketTest::CloseNoFrame);
            registerMethod("BeginTwice", *this, &WebSocketTest::BeginTwice);
            registerMethod("EndBeforeReady", *this, &WebSocketTest::EndBeforeReady);
            registerMethod("CloseCodes", *this, &WebSocketTest::CloseCodes);
            registerMethod("HandshakeControl", *this, &WebSocketTest::HandshakeControl);
            registerMethod("Protocol", *this, &WebSocketTest::Protocol);
            registerMethod("ProtocolNone", *this, &WebSocketTest::ProtocolNone);
            registerMethod("ProtocolRequired", *this, &WebSocketTest::ProtocolRequired);
            registerMethod("ProtocolRejected", *this, &WebSocketTest::ProtocolRejected);
            registerMethod("ProtocolHeader", *this, &WebSocketTest::ProtocolHeader);
            registerMethod("EndAfterClosed", *this, &WebSocketTest::EndAfterClosed);
            registerMethod("PingAfterShutdown", *this, &WebSocketTest::PingAfterShutdown);
            registerMethod("IdleBeforeConnect", *this, &WebSocketTest::IdleBeforeConnect);
            registerMethod("ReceiveDuringShutdown", *this, &WebSocketTest::ReceiveDuringShutdown);
            registerMethod("ProtocolFailure", *this, &WebSocketTest::ProtocolFailure);
            registerMethod("IdleTimeoutTwice", *this, &WebSocketTest::IdleTimeoutTwice);
        }

        void setUp()
        {
            _loop = new Pt::System::MainLoop();
            _received = false;
            _declined = false;
            _closed = false;
            _partial = false;
            _finished = false;
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
        }

        void onConnectedClose(Pt::Http::WebSocket& socket)
        {
            socket.endConnect();
            socket.beginShutdown();
        }

        void onShutdownReady(Pt::Http::WebSocket& socket)
        {
            socket.endShutdown();
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
            Pt::Http::MessageProgress progress = socket.endSend();
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
            Pt::Http::MessageProgress progress = socket.endSend();
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
            socket.connected() += Pt::slot(*this, &WebSocketTest::onDeclineConnected);
            socket.beginConnect("/ws");

            _loop->run();

            PT_UNIT_ASSERT(_declined);
        }

        void onDeclineConnected(Pt::Http::WebSocket& socket)
        {
            socket.endConnect();
            socket.inputReady() += Pt::slot(*this, &WebSocketTest::onDeclineInput);
            socket.beginReceive();
        }

        void onDeclineInput(Pt::Http::WebSocket& socket)
        {
            PT_UNIT_ASSERT_THROW(socket.endReceive(), std::exception);
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
            RecordService service(_message, _type, _received, _closed, *_loop);
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
            Pt::Http::MessageProgress progress = socket.endSend();
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
            socket.beginShutdown();
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
            PT_UNIT_ASSERT_THROW(socket.endConnect(), std::exception);
            _declined = true;
            _loop->exit();
        }

    protected:
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
        }

        void onConnectedEndAfterClosed(Pt::Http::WebSocket& socket)
        {
            socket.endConnect();
            socket.inputReady() += Pt::slot(*this, &WebSocketTest::onClientEndAfterClosed);
            socket.beginReceive();
        }

        void onClientEndAfterClosed(Pt::Http::WebSocket& socket)
        {
            PT_UNIT_ASSERT_THROW(socket.endSend(), std::logic_error);
            PT_UNIT_ASSERT_THROW(socket.endReceive(), std::exception);
            _closed = true;
            _loop->exit();
        }

    protected:
        void HandshakeControl()
        {
            Pt::Net::Endpoint ep("127.0.0.1", 8025);
            Pt::Http::Client http(*_loop, ep);
            Pt::Http::WebSocket socket(http);
            PT_UNIT_ASSERT_THROW(socket.beginPing(), std::logic_error);
            PT_UNIT_ASSERT_THROW(socket.beginShutdown(), std::logic_error);
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
            socket.inputReady() += Pt::slot(*this, &WebSocketTest::onIdleInput);
            socket.beginConnect("/ws");

            _loop->run();

            PT_UNIT_ASSERT(_closed);
        }

        void onConnectedIdleReceive(Pt::Http::WebSocket& socket)
        {
            socket.endConnect();
            socket.beginReceive();
        }

        void onIdleInput(Pt::Http::WebSocket& socket)
        {
            PT_UNIT_ASSERT_THROW(socket.endReceive(), std::exception);
            _closed = true;
            _loop->exit();
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
            Pt::Http::MessageProgress progress = socket.endReceive();
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

    private:
        Pt::System::MainLoop* _loop;
        Pt::System::Timer _exitTimer;
        std::string _message;
        std::string _offered;
        std::string _copied;
        bool _received;
        bool _declined;
        bool _closed;
        bool _partial;
        bool _finished;
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
        DeferredCloseService* _deferredCloseService;
};

Pt::Unit::RegisterTest<WebSocketTest> register_HttpWebSocketTest;
