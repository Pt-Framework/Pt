/* Copyright (C) 2015-2026 by Laurentiu-Gheorghe Crisan
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
#include "Pt/Http/Client.h"
#include "Pt/Http/Request.h"
#include "Pt/Http/Reply.h"
#include "Pt/Net/Endpoint.h"
#include "Pt/System/MainLoop.h"
#include "Pt/System/Timer.h"


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
        , _service(0)
        , _frame(Pt::Http::WebSocket::Unknown)
        {
            registerMethod("Text", *this, &WebSocketTest::Text);
            registerMethod("Accepted", *this, &WebSocketTest::Accepted);
            registerMethod("PeerClose", *this, &WebSocketTest::PeerClose);
            registerMethod("Decline", *this, &WebSocketTest::Decline);
        }

        void setUp()
        {
            _loop = new Pt::System::MainLoop();
            _received = false;
            _declined = false;
            _closed = false;
            _service = 0;
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
            Pt::Http::WebSocketService service;
            service.accepted() += Pt::slot(*this, &WebSocketTest::onAccepted);

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

        void onAccepted(Pt::Http::WebSocket& socket)
        {
            socket.inputReady() += Pt::slot(*this, &WebSocketTest::onTextInput);
            socket.beginReceive();
        }

        void onTextInput(Pt::Http::WebSocket& socket)
        {
            socket.endReceive();

            _message.assign( std::istreambuf_iterator<char>( socket.body() ),
                             std::istreambuf_iterator<char>() );

            _frame = socket.frame();
            _received = true;
            _loop->exit();
        }

    protected:
        void Accepted()
        {
            Pt::Net::Endpoint ep("127.0.0.1", 8012);

            Pt::Http::Server server(*_loop, ep);
            Pt::Http::WebSocketService service;
            _service = &service;
            service.accepted() += Pt::slot(*this, &WebSocketTest::onAcceptedCount);

            Pt::Http::MapUrl mapUrl("/ws", service);
            server.addServlet(mapUrl);

            Pt::Http::Client http(*_loop, ep);
            Pt::Http::WebSocket socket(http);
            socket.connected() += Pt::slot(*this, &WebSocketTest::onConnectedIdle);
            socket.beginConnect("/ws");

            _loop->run();

            PT_UNIT_ASSERT(_service);
            PT_UNIT_ASSERT_EQUALS(_service->size(), static_cast<std::size_t>(1));
        }

        void onConnectedIdle(Pt::Http::WebSocket& socket)
        {
            socket.endConnect();
        }

        void onAcceptedCount(Pt::Http::WebSocket&)
        {
            _loop->exit();
        }

    protected:
        void PeerClose()
        {
            Pt::Net::Endpoint ep("127.0.0.1", 8013);

            Pt::Http::Server server(*_loop, ep);
            Pt::Http::WebSocketService service;
            _service = &service;
            service.accepted() += Pt::slot(*this, &WebSocketTest::onAcceptedClose);

            Pt::Http::MapUrl mapUrl("/ws", service);
            server.addServlet(mapUrl);

            Pt::Http::Client http(*_loop, ep);
            Pt::Http::WebSocket socket(http);
            socket.connected() += Pt::slot(*this, &WebSocketTest::onConnectedClose);
            socket.beginConnect("/ws");

            _loop->run();

            PT_UNIT_ASSERT(_closed);
            PT_UNIT_ASSERT(_service);
            PT_UNIT_ASSERT_EQUALS(_service->size(), static_cast<std::size_t>(0));
        }

        void onConnectedClose(Pt::Http::WebSocket& socket)
        {
            socket.endConnect();
            socket.close();
        }

        void onAcceptedClose(Pt::Http::WebSocket& socket)
        {
            socket.closed() += Pt::slot(*this, &WebSocketTest::onServerClosed);
            socket.beginReceive();
        }

        void onServerClosed(Pt::Http::WebSocket&)
        {
            _closed = true;
            _loop->exit();
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

    private:
        Pt::System::MainLoop* _loop;
        Pt::System::Timer _exitTimer;
        std::string _message;
        bool _received;
        bool _declined;
        bool _closed;
        Pt::Http::WebSocketService* _service;
        Pt::Http::WebSocket::Frame _frame;
};

Pt::Unit::RegisterTest<WebSocketTest> register_HttpWebSocketTest;
