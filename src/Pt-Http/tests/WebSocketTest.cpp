/*
 * Copyright (C) 2015 by Laurentiu-Gheorghe Crisan  
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

#include "Pt/Unit/Assertion.h"
#include "Pt/Unit/TestSuite.h"
#include "Pt/Unit/RegisterTest.h"
#include "Pt/Http/Server.h"
#include "Pt/Http/Client.h"
#include "Pt/Http/Authorizer.h"
#include "Pt/Http/Authenticator.h"
#include "Pt/Http/Request.h"
#include "Pt/Http/Reply.h"
#include "Pt/Http/Service.h"
#include "Pt/Http/Servlet.h"
#include "Pt/Http/Responder.h"
#include "Pt/Http/WebSocketResponder.h"
#include "Pt/Http/WebSocketService.h"
#include "Pt/Http/WebSocket.h"
#include "Pt/Net/TcpSocket.h"
#include "Pt/System/MainLoop.h"
#include "Pt/System/Timer.h"
#include "Pt/System/Logger.h"
#include <string>
#include <fstream>
#include <sstream>

class HtmlResponder : public Pt::Http::Responder
{
    public:
        HtmlResponder(Pt::Http::Service& s)
        : Pt::Http::Responder(s)
        {}

        virtual void onBeginRequest(Pt::Http::Request& request, Pt::Http::Reply& reply,
                                    Pt::System::EventLoop& loop)
        {
            setReady(false);
        }

        virtual void onReadRequest(Pt::Http::Request& request, Pt::Http::Reply& reply,
                                   Pt::System::EventLoop& loop)
        {
            setReady(false);
        }

        virtual void onBeginReply(const Pt::Http::Request& request, Pt::Http::Reply& reply,
                                  Pt::System::EventLoop& loop)
        {
            return onWriteReply(request, reply, loop);
        }

        virtual void onWriteReply(const Pt::Http::Request& request, Pt::Http::Reply& reply,
                                  Pt::System::EventLoop& loop)
        {
            #ifdef _WIN32
                std::ifstream ifs("WebSocketTest.html");
            #else
                std::ifstream ifs("WebSocketTest.html");
            #endif

            if( ifs )
            {
                std::ostringstream oss;
                oss << ifs.rdbuf();
                std::string html = oss.str();

                reply.setStatus(200, "OK");
                reply.header().set("Content-Type", "text/html; charset=utf-8");
                reply.body() << html;
            }
            else
            {
                reply.setStatus(404, "Not found");
            }

            setReady(true);
        }
};

typedef Pt::Http::BasicService<HtmlResponder> FileService;

class WebSocketTest : public Pt::Unit::TestSuite, public Pt::Connectable
{
    public:
        WebSocketTest()
        : Pt::Unit::TestSuite("WebSocketTest")        
        {

            _webSocket.inputReady() += Pt::slot(*this, &WebSocketTest::onInputReady);   
            registerMethod("WebServer", *this, &WebSocketTest::webSocketServer);
        }

        void setUp()
        {
            _loop = new Pt::System::MainLoop();
        }

        void tearDown()
        {
            delete _loop;
        }

        void webSocketServer()
        {
#if 1
            Pt::Net::Endpoint ep("127.0.0.1", 8011);
            Pt::Http::Server server(*_loop, ep);

            Pt::Http::WebSocketService webSocketService;
            Pt::Http::MapUrl mapurl("/ws", webSocketService);

            server.addServlet(mapurl);

            FileService fileService;
            Pt::Http::MapUrl htmlMap("/fs", fileService);
            server.addServlet(htmlMap);

            webSocketService.upgradeRequested() += Pt::slot(*this, &WebSocketTest::onUpgrade);

            _loop->run();
#endif
        }

        void onUpgrade(Pt::Http::IOStream* stream, const std::string& protocol)
        {            
            std::cout << "Protocol: " << protocol << std::endl; 
            _webSocket.accept(stream);
            _webSocket.beginRead(_buffer, 1024);         
        }

        void onInputReady(Pt::System::IODevice& dev)
        {
            size_t n = _webSocket.endRead();
            
            std::string msg(_buffer, n);
            std::cout << "Received: " << msg << std::endl;  
            
        }

    private:
        Pt::System::MainLoop* _loop;
        Pt::Http::WebSocket   _webSocket;
        char _buffer[1024];
};

Pt::Unit::RegisterTest<WebSocketTest> register_HttpWebSocketTest  ;
