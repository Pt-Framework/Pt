/*
 * Copyright (C) 2006 - 2007 by Marc Boris Duerner
 * Copyright (C) 2006 - 2007 by Tommi Maekitalo
 * Copyright (C) 2006 - 2007 by Sebastian Pieck
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
            this->registerMethod( "NonBlockingWithSelector", *this, &TcpSocketTest::NonBlockingWithSelector);
            this->registerMethod( "NonBlockingWithWait", *this, &TcpSocketTest::NonBlockingWithWait);
        }

        void setUp()
        {

        }

        void tearDown()
        {

        }

        void NonBlockingWithWait()
        {
            this->reportMessage("\nSTART");
            
            Pt::Net::TcpServer server("127.0.0.1", 8000);
            connect(server.connectionPending, *this, &TcpSocketTest::reply);        

            Pt::Net::TcpSocket client;
            client.beginConnect("127.0.0.1", 8000);
            connect(client.connected, *this, &TcpSocketTest::request);

            server.wait(3000);
            client.wait(3000);
            this->reportMessage("FINISHED");
        }

        void NonBlockingWithSelector()
        {
            this->reportMessage("\nSTART");
            Pt::System::Selector selector;

            Pt::Net::TcpServer server("127.0.0.1", 8000);
            connect(server.connectionPending, *this, &TcpSocketTest::reply);
            selector.add(server);            

            Pt::Net::TcpSocket client;
            client.beginConnect("127.0.0.1", 8000);
            connect(client.connected, *this, &TcpSocketTest::request);
            selector.add(client);

            selector.wait(2000);
            selector.wait(2000);

            this->reportMessage("FINISHED");
        }

        void reply(Pt::Net::TcpServer& server)
        {
            this->reportMessage("CLIENT CONNECTION ACCEPTED");
            Pt::Net::TcpSocket socket(server);
        }

        void request(Pt::Net::TcpSocket& socket)
        {
            this->reportMessage("CONNECTED TO SERVER");
            socket.endConnect();
        }

    private:

};

Pt::Unit::RegisterTest<TcpSocketTest> register_TcpSocketTest;
