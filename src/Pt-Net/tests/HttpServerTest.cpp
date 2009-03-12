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
#include "Pt/Net/TcpSocket.h"
#include "Pt/System/EventLoop.h"
#include <string>

class HttpServerTest : public Pt::Unit::TestSuite
{
    public:
        HttpServerTest()
        : Pt::Unit::TestSuite("HttpServerTest")
        {
            this->registerMethod( "serverInstance", *this,
                                  &HttpServerTest::serverInstance);
        }

        void setUp()
        {
            clientStream = new Pt::System::IOStream();
        }

        void tearDown()
        {
            delete clientStream;
        }

        void serverInstance()
        {
            Pt::System::EventLoop loop;
            loop.setIdleTimeout(5000);

            Pt::Net::HttpServer server(loop, "127.0.0.1", 8001);

            Pt::Net::TcpSocket client;
            clientStream->attachDevice(client);
            client.beginConnect("127.0.0.1", 8001);
            connect(client.connected, *this, &HttpServerTest::onConnect);
            connect(clientStream->buffer().outputReady, *this, &HttpServerTest::onOutput);
            connect(clientStream->buffer().inputReady, *this, &HttpServerTest::onInput);
            loop.add(client);

            loop.run();
        }

    private:

        void onConnect(Pt::Net::TcpSocket& socket)
        {
            *clientStream << "GET /foo HTTP/1.0\r\n\r\n";
            clientStream->buffer().beginWrite();
        }

        void onOutput(Pt::System::StreamBuffer& device)
        {
            if (device.out_avail() != 0)
                device.beginWrite();
            else
                device.beginRead();
        }

        void onInput(Pt::System::StreamBuffer& device)
        {
            while (device.in_avail())
            {
                char ch;
                clientStream->get(ch);
                std::cout << ch;
            }
        }

        Pt::System::IOStream* clientStream;
};

Pt::Unit::RegisterTest<HttpServerTest> register_HttpServerTest;
