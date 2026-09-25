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
#include "Pt/Net/Endpoint.h"
#include "Pt/System/MainLoop.h"
#include "Pt/System/Timer.h"
#include <sstream>
#include <string>

class WebSocketTest : public Pt::Unit::TestSuite
                    , public Pt::Connectable
{
    public:
        WebSocketTest()
        : Pt::Unit::TestSuite("Pt::Http::WebSocketTest")
        , _loop(0)
        , _received(false)
        , _frame(Pt::Http::WebSocket::Unknow)
        {
            registerMethod("Text", *this, &WebSocketTest::Text);
        }

        void setUp()
        {
            _loop = new Pt::System::MainLoop();
            _received = false;
            _message.clear();
            _frame = Pt::Http::WebSocket::Unknow;

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

            std::ostringstream url;
            url << "ws://127.0.0.1:8011/ws";

            _client.setActive(*_loop);
            _client.connected() += Pt::slot(*this, &WebSocketTest::onConnected);
            _client.beginConnect(url.str());

            _loop->run();

            PT_UNIT_ASSERT(_received);
            PT_UNIT_ASSERT_EQUALS(_message, "hello");
            PT_UNIT_ASSERT_EQUALS(_frame, Pt::Http::WebSocket::Text);
        }

        void onUpgrade(Pt::Http::IOStream* stream, const std::string& /*protocol*/)
        {
            _serverSocket.accept(stream);
            _serverSocket.inputReady() += Pt::slot(*this, &WebSocketTest::onInput);
            _serverSocket.beginRead(_buffer, sizeof(_buffer));
        }

        void onConnected(Pt::Http::WebSocket& socket)
        {
            socket.endConnect();
            socket.setSendFrame(Pt::Http::WebSocket::Text);
            socket.beginWrite("hello", 5);
        }

        void onInput(Pt::System::IODevice& /*device*/)
        {
            std::size_t n = _serverSocket.endRead();
            _message.assign(_buffer, n);
            _frame = _serverSocket.receiveFrame();
            _received = true;
            _loop->exit();
        }

    private:
        Pt::System::MainLoop* _loop;
        Pt::System::Timer _exitTimer;
        Pt::Http::WebSocket _client;
        Pt::Http::WebSocket _serverSocket;
        char _buffer[64];
        std::string _message;
        bool _received;
        Pt::Http::WebSocket::Frame _frame;
};

Pt::Unit::RegisterTest<WebSocketTest> register_HttpWebSocketTest;
