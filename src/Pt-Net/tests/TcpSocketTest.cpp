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
#include "Pt/Net/TcpServer.h"
#include "Pt/Net/TcpSocket.h"
#include <string>

class TcpSocketTest : public Pt::Unit::TestSuite
{
    public:
        TcpSocketTest()
        : Pt::Unit::TestSuite("TcpSocketTest")
        {
            this->registerMethod( "NonBlockingWithSelector", *this,
                                  &TcpSocketTest::NonBlockingWithSelector);
            this->registerMethod( "NonBlockingWithWait", *this,
                                  &TcpSocketTest::NonBlockingWithWait);
        }

        void setUp()
        {
            _acceptor = new Pt::Net::TcpSocket();
        }

        void tearDown()
        {
            delete _acceptor;
        }

        void NonBlockingWithWait()
        {
            this->reportMessage("\nSTART");

            Pt::Net::TcpServer server("127.0.0.1", 8000);
            connect(server.connectionPending, *this, &TcpSocketTest::onAccept);

            connect(_acceptor->inputReady, *this, &TcpSocketTest::onInput);

            Pt::Net::TcpSocket client;
            client.beginConnect("127.0.0.1", 8000);
            connect(client.connected, *this, &TcpSocketTest::onConnect);
            connect(client.outputReady, *this, &TcpSocketTest::onOutput);

            server.wait(1000);
            client.wait(1000);
            _acceptor->wait(1000);
            this->reportMessage("FINISHED");
        }

        void NonBlockingWithSelector()
        {
            this->reportMessage("\nSTART");
            Pt::System::Selector selector;

            Pt::Net::TcpServer server("127.0.0.1", 8000);
            connect(server.connectionPending, *this, &TcpSocketTest::onAccept);
            selector.add(server);

            connect(_acceptor->inputReady, *this, &TcpSocketTest::onInput);
            selector.add(*_acceptor);

            Pt::Net::TcpSocket client;
            client.beginConnect("127.0.0.1", 8000);
            connect(client.connected, *this, &TcpSocketTest::onConnect);
            connect(client.outputReady, *this, &TcpSocketTest::onOutput);
            selector.add(client);

            selector.wait(1000);
            selector.wait(1000);
            selector.wait(1000);
            selector.wait(1000);

            this->reportMessage("FINISHED");
        }

        void onAccept(Pt::Net::TcpServer& server)
        {
            this->reportMessage("CLIENT CONNECTION ACCEPTED");
            _acceptor->accept(server);
            _acceptor->beginRead(input, 200);
        }

        void onConnect(Pt::Net::TcpSocket& socket)
        {
            this->reportMessage("CONNECTED TO SERVER");
            socket.endConnect();

            static const char buffer[] = "Hello World !!!";
            socket.beginWrite(buffer, sizeof(buffer));
        }

        void onInput(Pt::System::IODevice& device)
        {
            std::size_t n = device.endRead();
            std::string msg("INPUT RECEIVED: ");
            msg.append(input, n);
            this->reportMessage(msg);
            PT_UNIT_ASSERT(n > 5);
        }

        void onOutput(Pt::System::IODevice& device)
        {
            std::size_t n = device.endWrite();
            this->reportMessage("OUTPUT SENT");
            PT_UNIT_ASSERT(n > 5);
        }

    private:
        Pt::Net::TcpSocket* _acceptor;
        char input[200];

};

Pt::Unit::RegisterTest<TcpSocketTest> register_TcpSocketTest;
