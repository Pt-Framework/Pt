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
            this->registerMethod( "serverInstance", *this, &HttpServerTest::serverInstance);
        }

        void setUp()
        { }

        void tearDown()
        { }

        void serverInstance()
        {
            Pt::System::EventLoop loop;
            loop.setIdleTimeout(2000);
            connect(loop.timeout, loop, &Pt::System::EventLoop::exit);

            Pt::Net::HttpServer server(loop, "127.0.0.1", 8001);

            Pt::Net::HttpReply reply;
            reply.setSelector(loop);
            connect(reply.headerReceived, *this, &HttpServerTest::onReplyHeader);
            connect(reply.replyReceived, *this, &HttpServerTest::onReply);

            Pt::Net::HttpRequest request("127.0.0.1", 8001, "/index.html");
            request.setHeader("foo", "bar");
            reply.beginExecute(request);

            loop.run();
        }

    private:
        void onReplyHeader(Pt::Net::HttpReply& reply)
        {
            std::cout << "Content-Size=" << reply.getHeader("Content-Size") << std::endl;
        }

        std::size_t onReply(Pt::Net::HttpReply& reply)
        {
            std::size_t ret = 0;
            while ( reply.in().rdbuf()->in_avail() )
            {
                char ch;
                reply.in().get(ch);
                ++ret;
                std::cout << ch;
            }

            if( reply.isReady() )
                std::cout << "THE END" << std::endl;

            return ret;
        }
};

Pt::Unit::RegisterTest<HttpServerTest> register_HttpServerTest;
