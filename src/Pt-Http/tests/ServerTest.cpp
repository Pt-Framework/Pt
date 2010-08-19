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
#include "Pt/Http/Server.h"
#include "Pt/Http/Client.h"
#include "Pt/Http/ReplyHeader.h"
#include "Pt/Http/Request.h"
#include "Pt/Net/TcpSocket.h"
#include "Pt/System/MainLoop.h"
#include <string>

class ServerTest : public Pt::Unit::TestSuite
{
    public:
        ServerTest()
        : Pt::Unit::TestSuite("ServerTest")
        {
            this->registerMethod( "NotFoundRequest", *this, &ServerTest::NotFoundRequest);
        }

        void setUp()
        {
            loop = new Pt::System::MainLoop();
            loop->setIdleTimeout(2000);
        }

        void tearDown()
        {
            delete loop;
        }

        void NotFoundRequest()
        {
            connect(loop->timeout, *loop, &Pt::System::MainLoop::exit);

            Pt::Http::Server server(*loop, "127.0.0.1", 8001);

            Pt::Http::Client client("127.0.0.1", 8001);
            client.setSelector(*loop);
            connect(client.headerReceived, *this, &ServerTest::onReplyHeader);
            connect(client.bodyAvailable, *this, &ServerTest::onReply);
            connect(client.replyFinished, *this, &ServerTest::onReplyFinished);

            Pt::Http::Request request("/index.html");
            request.setHeader("foo", "bar");
            client.beginExecute(request);

            loop->run();
        }

    private:
        Pt::System::MainLoop* loop;

        void onReplyHeader(Pt::Http::Client& client)
        {
        }

        std::size_t onReply(Pt::Http::Client& client)
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

        void onReplyFinished(Pt::Http::Client& client)
        {
            PT_UNIT_ASSERT_EQUALS(client.header().httpReturnCode(), 404);
            loop->exit();
        }
};

Pt::Unit::RegisterTest<ServerTest> register_HttpServerTest;
