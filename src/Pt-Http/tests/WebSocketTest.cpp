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
            reply.header().set("Connection", "Upgrade");
            setReady(true);
        }
};

typedef Pt::Http::BasicService<DeclineResponder> DeclineService;

class RecordSession : public Pt::Http::WebSocketSession
{
    public:
        RecordSession(Pt::Http::WebSocketServlet& servlet,
                      Pt::System::EventLoop& loop,
                      Pt::Http::Stream& stream,
                      std::string& message,
                      Pt::Http::WebSocket::Frame& frame,
                      bool& received,
                      bool& closed,
                      Pt::System::EventLoop& exitLoop)
        : Pt::Http::WebSocketSession(servlet, loop, stream)
        , _message(&message)
        , _frame(&frame)
        , _received(&received)
        , _closed(&closed)
        , _exitLoop(&exitLoop)
        {
            beginReceive();
        }

    protected:
        virtual void onInput()
        {
            endReceive();

            _message->assign( std::istreambuf_iterator<char>( body() ),
                              std::istreambuf_iterator<char>() );
            *_frame = frame();
            *_received = true;
            _exitLoop->exit();
        }

        virtual void onOutput()
        {
            endSend();
        }

        virtual void onClose()
        {
            *_closed = true;
            _exitLoop->exit();
        }

    private:
        std::string* _message;
        Pt::Http::WebSocket::Frame* _frame;
        bool* _received;
        bool* _closed;
        Pt::System::EventLoop* _exitLoop;
};

class RecordService : public Pt::Http::WebSocketService
{
    public:
        RecordService(std::string& message,
                      Pt::Http::WebSocket::Frame& frame,
                      bool& received,
                      bool& closed,
                      Pt::System::EventLoop& loop)
        : _message(&message)
        , _frame(&frame)
        , _received(&received)
        , _closed(&closed)
        , _loop(&loop)
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
        virtual Pt::Http::WebSocketSession* onGetSession(Pt::Http::WebSocketServlet& servlet,
                                                         Pt::System::EventLoop& loop,
                                                         Pt::Http::Stream& stream)
        {
            ++_opened;
            return new RecordSession(servlet, loop, stream,
                                     *_message, *_frame, *_received, *_closed, *_loop);
        }

        virtual void onReleaseSession(Pt::Http::WebSocketSession* session)
        {
            ++_released;
            delete session;
        }

    private:
        std::string* _message;
        Pt::Http::WebSocket::Frame* _frame;
        bool* _received;
        bool* _closed;
        Pt::System::EventLoop* _loop;
        std::size_t _opened;
        std::size_t _released;
};

class IdleSession : public Pt::Http::WebSocketSession
{
    public:
        IdleSession(Pt::Http::WebSocketServlet& servlet,
                    Pt::System::EventLoop& loop,
                    Pt::Http::Stream& stream,
                    Pt::System::EventLoop& exitLoop,
                    bool exitOnAccept)
        : Pt::Http::WebSocketSession(servlet, loop, stream)
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

        virtual void onClose()
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

    protected:
        virtual Pt::Http::WebSocketSession* onGetSession(Pt::Http::WebSocketServlet& servlet,
                                                         Pt::System::EventLoop& loop,
                                                         Pt::Http::Stream& stream)
        {
            ++_opened;
            return new IdleSession(servlet, loop, stream, *_loop, _exitOnAccept);
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

class NullService : public Pt::Http::WebSocketService
{
    protected:
        virtual Pt::Http::WebSocketSession* onGetSession(Pt::Http::WebSocketServlet&,
                                                         Pt::System::EventLoop&,
                                                         Pt::Http::Stream&)
        {
            return 0;
        }

        virtual void onReleaseSession(Pt::Http::WebSocketSession*)
        {}
};

class ThrowSession : public Pt::Http::WebSocketSession
{
    public:
        ThrowSession(Pt::Http::WebSocketServlet& servlet,
                     Pt::System::EventLoop& loop,
                     Pt::Http::Stream& stream)
        : Pt::Http::WebSocketSession(servlet, loop, stream)
        {
            throw std::runtime_error("session construction failed");
        }

    protected:
        virtual void onInput()
        {}

        virtual void onOutput()
        {}

        virtual void onClose()
        {}
};

class ThrowService : public Pt::Http::WebSocketService
{
    protected:
        virtual Pt::Http::WebSocketSession* onGetSession(Pt::Http::WebSocketServlet& servlet,
                                                         Pt::System::EventLoop& loop,
                                                         Pt::Http::Stream& stream)
        {
            return new ThrowSession(servlet, loop, stream);
        }

        virtual void onReleaseSession(Pt::Http::WebSocketSession* session)
        {
            delete session;
        }
};

class PushSession : public Pt::Http::WebSocketSession
{
    public:
        PushSession(Pt::Http::WebSocketServlet& servlet,
                    Pt::System::EventLoop& loop,
                    Pt::Http::Stream& stream)
        : Pt::Http::WebSocketSession(servlet, loop, stream)
        {
            body() << "feed";
            beginSend(Pt::Http::WebSocket::Text);
        }

    protected:
        virtual void onInput()
        {}

        virtual void onOutput()
        {
            endSend();
        }

        virtual void onClose()
        {}
};

typedef Pt::Http::BasicWebSocketService<PushSession> PushService;

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
        , _status(0)
        , _frame(Pt::Http::WebSocket::Unknown)
        , _limitSocket(0)
        , _limitClient(0)
        {
            registerMethod("Text", *this, &WebSocketTest::Text);
            registerMethod("Accepted", *this, &WebSocketTest::Accepted);
            registerMethod("PeerClose", *this, &WebSocketTest::PeerClose);
            registerMethod("Decline", *this, &WebSocketTest::Decline);
            registerMethod("NullSession", *this, &WebSocketTest::NullSession);
            registerMethod("ConstructFails", *this, &WebSocketTest::ConstructFails);
            registerMethod("Push", *this, &WebSocketTest::Push);
            registerMethod("Limit", *this, &WebSocketTest::Limit);
        }

        void setUp()
        {
            _loop = new Pt::System::MainLoop();
            _received = false;
            _declined = false;
            _closed = false;
            _status = 0;
            _limitSocket = 0;
            _limitClient = 0;
            _message.clear();
            _frame = Pt::Http::WebSocket::Unknown;

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
            RecordService service(_message, _frame, _received, _closed, *_loop);
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
            PT_UNIT_ASSERT_EQUALS(_frame, Pt::Http::WebSocket::Text);
        }

        void onConnected(Pt::Http::WebSocket& socket)
        {
            socket.endConnect();
            socket.body() << "hello";
            socket.beginSend(Pt::Http::WebSocket::Text);
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
        void PeerClose()
        {
            Pt::Net::Endpoint ep("127.0.0.1", 8013);

            Pt::Http::Server server(*_loop, ep);
            RecordService service(_message, _frame, _received, _closed, *_loop);
            Pt::Http::WebSocketServlet sockets(service);

            Pt::Http::MapUrl mapUrl("/ws", service);
            server.addServlet(mapUrl);

            Pt::Http::Client http(*_loop, ep);
            Pt::Http::WebSocket socket(http);
            socket.connected() += Pt::slot(*this, &WebSocketTest::onConnectedClose);
            socket.beginConnect("/ws");

            _loop->run();

            PT_UNIT_ASSERT(_closed);
            PT_UNIT_ASSERT_EQUALS(sockets.size(), static_cast<std::size_t>(0));
        }

        void onConnectedClose(Pt::Http::WebSocket& socket)
        {
            socket.endConnect();
            socket.close();
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
            socket.closed() += Pt::slot(*this, &WebSocketTest::onDeclineClosed);
            socket.beginReceive();
        }

        void onDeclineClosed(Pt::Http::WebSocket&)
        {
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
            PT_UNIT_ASSERT_EQUALS(_frame, Pt::Http::WebSocket::Text);
        }

        void onPushConnected(Pt::Http::WebSocket& socket)
        {
            socket.endConnect();
            socket.beginReceive();
        }

        void onPushInput(Pt::Http::WebSocket& socket)
        {
            socket.endReceive();
            _message.assign( std::istreambuf_iterator<char>( socket.body() ),
                             std::istreambuf_iterator<char>() );
            _frame = socket.frame();
            _received = true;
            _loop->exit();
        }

    protected:
        void Limit()
        {
            Pt::Net::Endpoint ep("127.0.0.1", 8018);

            Pt::Http::Server server(*_loop, ep);
            IdleService service(*_loop, false);
            Pt::Http::WebSocketServlet sockets(service);
            service.setMaxSockets(1);

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

    private:
        Pt::System::MainLoop* _loop;
        Pt::System::Timer _exitTimer;
        std::string _message;
        bool _received;
        bool _declined;
        bool _closed;
        unsigned _status;
        Pt::Http::WebSocket::Frame _frame;
        Pt::Http::WebSocket* _limitSocket;
        Pt::Http::Client* _limitClient;
};

Pt::Unit::RegisterTest<WebSocketTest> register_HttpWebSocketTest;
