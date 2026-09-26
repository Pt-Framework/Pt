/* Copyright (C) 2015-2026 by Laurentiu-Gheorghe Crisan
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
*/

#include "Pt/Unit/Assertion.h"
#include "Pt/Unit/TestSuite.h"
#include "Pt/Unit/RegisterTest.h"
#include "Pt/Http/Server.h"
#include "Pt/Http/Servlet.h"
#include "Pt/Http/WebSocket.h"
#include "Pt/Http/WebSocketService.h"
#include "Pt/Http/Client.h"
#include "Pt/Http/Request.h"
#include "Pt/Net/Endpoint.h"
#include "Pt/System/MainLoop.h"
#include "Pt/System/Timer.h"
#include <memory>
#include <streambuf>
#include <string>

class WebSocketTest : public Pt::Unit::TestSuite
                    , public Pt::Connectable
{
    public:
        WebSocketTest()
        : Pt::Unit::TestSuite("Pt::Http::WebSocketTest")
        , _loop(0)
        , _received(false)
        , _declined(false)
        , _frame(Pt::Http::WebSocket::Unknown)
        {
            registerMethod("Text", *this, &WebSocketTest::Text);
            registerMethod("Decline", *this, &WebSocketTest::Decline);
        }

        void setUp()
        {
            _loop = new Pt::System::MainLoop();
            _received = false;
            _declined = false;
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

        void Text()
        {
            Pt::Net::Endpoint ep("127.0.0.1", 8011);

            Pt::Http::Server server(*_loop, ep);
            Pt::Http::WebSocketService service;
            service.upgradeRequested() += Pt::slot(*this, &WebSocketTest::onUpgrade);

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

        void Decline()
        {
            Pt::Net::Endpoint ep("127.0.0.1", 8012);

            Pt::Http::Server server(*_loop, ep);
            Pt::Http::WebSocketService service;
            service.upgradeRequested() += Pt::slot(*this, &WebSocketTest::onDecline);

            Pt::Http::MapUrl mapUrl("/ws", service);
            server.addServlet(mapUrl);

            Pt::Http::Client client(*_loop);
            client.setHost(ep);
            client.request().setUrl("/ws");
            client.request().header().set("Connection", "Upgrade");
            client.request().header().set("Upgrade", "websocket");
            client.request().header().set("Sec-WebSocket-Key", "dGhlIHNhbXBsZSBub25jZQ==");
            client.request().header().set("Sec-WebSocket-Version", "13");
            client.replyReceived() += Pt::slot(*this, &WebSocketTest::onDeclinedReply);
            client.beginReceive();

            _loop->run();

            PT_UNIT_ASSERT(_declined);
        }

        void onDecline(Pt::Http::Stream& /*stream*/)
        {
        }

        void onDeclinedReply(Pt::Http::Client& client)
        {
            try
            {
                Pt::Http::MessageProgress progress = client.endReceive();
                if( ! progress.finished() )
                {
                    client.beginReceive();
                    return;
                }
            }
            catch(const std::exception&)
            {
            }

            _declined = true;
            _loop->exit();
        }

        void onUpgrade(Pt::Http::Stream& stream)
        {
            _server.reset(new Pt::Http::WebSocket(stream));
            _server->inputReady() += Pt::slot(*this, &WebSocketTest::onInput);
            _server->beginReceive();
        }

        void onConnected(Pt::Http::WebSocket& socket)
        {
            socket.endConnect();
            socket.buffer().sputn("hello", 5);
            socket.beginSend(Pt::Http::WebSocket::Text);
        }

        void onInput(Pt::Http::WebSocket& socket)
        {
            socket.endReceive();
            std::streambuf& buf = socket.buffer();
            while( buf.in_avail() > 0 )
                _message.push_back( static_cast<char>(buf.sbumpc()) );

            _frame = socket.frame();
            _received = true;
            _loop->exit();
        }

    private:
        Pt::System::MainLoop* _loop;
        Pt::System::Timer _exitTimer;
        std::unique_ptr<Pt::Http::WebSocket> _server;
        std::string _message;
        bool _received;
        bool _declined;
        Pt::Http::WebSocket::Frame _frame;
};

Pt::Unit::RegisterTest<WebSocketTest> register_HttpWebSocketTest;
