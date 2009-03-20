/*
 * Copyright (C) 2006 - 2009 by Marc Boris Duerner
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
#include "Pt/Net/HttpServer.h"
#include "Pt/Net/HttpClient.h"
#include "Pt/Net/TcpSocket.h"
#include "Pt/System/EventLoop.h"
#include <string>

class HttpServerTest : public Pt::Unit::TestSuite
{
    public:
        HttpServerTest()
        : Pt::Unit::TestSuite("HttpServerTest")
        {
            this->registerMethod( "NotFoundRequest", *this, &HttpServerTest::NotFoundRequest);
        }

        void setUp()
        {
            loop = new Pt::System::EventLoop();
            loop->setIdleTimeout(2000);
        }

        void tearDown()
        {
            delete loop;
        }

        void NotFoundRequest()
        {
            connect(loop->timeout, *loop, &Pt::System::EventLoop::exit);

            Pt::Net::HttpServer server(*loop, "127.0.0.1", 8001);

            Pt::Net::HttpClient client("127.0.0.1", 8001);
            client.setSelector(*loop);
            connect(client.headerReceived, *this, &HttpServerTest::onReplyHeader);
            connect(client.bodyAvailable, *this, &HttpServerTest::onReply);
            connect(client.replyFinished, *this, &HttpServerTest::onReplyFinished);

            Pt::Net::HttpRequest request("/index.html");
            request.setHeader("foo", "bar");
            client.beginExecute(request);

            loop->run();
        }

    private:
        Pt::System::EventLoop* loop;

        void onReplyHeader(Pt::Net::HttpClient& client)
        {
            std::cout << "Server=" << client.header().getHeader("server") << std::endl;
            std::cout << "Connection=" << client.header().getHeader("connection") << std::endl;
        }

        std::size_t onReply(Pt::Net::HttpClient& client)
        {
            std::size_t ret = 0;
            while ( client.in().rdbuf()->in_avail() )
            {
                char ch;
                client.in().get(ch);
                ++ret;
                std::cout << ch;
            }

            return ret;
        }

        void onReplyFinished(Pt::Net::HttpClient& client)
        {
            PT_UNIT_ASSERT_EQUALS(client.header().httpReturnCode(), 404);
            std::cout << "THE END" << std::endl;
            loop->exit();
        }
};

Pt::Unit::RegisterTest<HttpServerTest> register_HttpServerTest;
