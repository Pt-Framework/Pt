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
#include "Pt/System/MainLoop.h"
#include <string>

class TcpSocketTest : public Pt::Unit::TestSuite
{
    public:
        TcpSocketTest()
        : Pt::Unit::TestSuite("TcpSocketTest")
        , _loop(0)
        {
          this->registerMethod( "NonBlockingWithLoop", *this,
                                &TcpSocketTest::NonBlockingWithLoop);
          //this->registerMethod( "NonBlockingWithWait", *this,
          //                      &TcpSocketTest::NonBlockingWithWait);
          //this->registerMethod( "ConnectFailed", *this,
          //                      &TcpSocketTest::ConnectFailed);
        }

        void setUp()
        {
            std::memset(input, 0, 20);

            _loop = new Pt::System::MainLoop();
            _loop->timeout() += Pt::slot(*_loop, &Pt::System::MainLoop::exit);
            _loop->setIdleTimeout(2000);

            _acceptor = new Pt::Net::TcpSocket();
        }

        void tearDown()
        {
            _acceptor->close();
            delete _acceptor;

            delete _loop;
            _loop = 0;
        }

        void ConnectFailed()
        {
            Pt::Net::TcpSocket client;
            client.setActive(*_loop);
            connect(client.connected, *this, &TcpSocketTest::onConnectFailed);

            try
            {
                client.beginConnect("127.0.0.2", 9000);
            }
            catch(const Pt::System::IOError&)
            {
                // success
                this->reportMessage("handled beginConnect directly");
                return;
            }

            //_loop->add(client);
            _loop->run();
            PT_UNIT_ASSERT( false == client.isConnected() );
        }

        void onConnectFailed(Pt::Net::TcpSocket& socket)
        {
            this->reportMessage("reached connect callback");
            _loop->exit();
            PT_UNIT_ASSERT_THROW(socket.endConnect(), Pt::System::IOError);
        }

        /*void NonBlockingWithWait()
        {
            //this->reportMessage("\nSTART");

            Pt::Net::TcpServer server("127.0.0.1", 8000);
            connect(server.connectionPending, *this, &TcpSocketTest::onAccept);

            connect(_acceptor->inputReady, *this, &TcpSocketTest::onInput);

            Pt::Net::TcpSocket client;
            connect(client.connected, *this, &TcpSocketTest::onConnect);
            connect(client.outputReady, *this, &TcpSocketTest::onOutput);
            client.beginConnect("127.0.0.1", 8000);

            server.wait(1000); //on accept
            client.wait(1000); //on connect
            client.wait(1000); //on write
            _acceptor->wait(1000);//on read

            PT_UNIT_ASSERT( 0 == std::strncmp(input, "Hello World !!!", 15) );
            //this->reportMessage("FINISHED");
        }*/

        void NonBlockingWithLoop()
        {
            Pt::Net::TcpServer server("127.0.0.1", 9000);
            {
                //this->reportMessage("\nSTART");

                connect(server.connectionPending, *this, &TcpSocketTest::onAccept);
                server.setActive(*_loop);

                _acceptor->inputReady() += Pt::slot(*this, &TcpSocketTest::onInput);
                _acceptor->setActive(*_loop);

                Pt::Net::TcpSocket client;
                connect(client.connected, *this, &TcpSocketTest::onConnect);
                client.outputReady() += Pt::slot(*this, &TcpSocketTest::onOutput);
                client.setActive(*_loop);
                client.beginConnect("127.0.0.1", 9000);
                //_loop->add(client);

                _loop->run();

                server.close();

                delete _loop;
                _loop  = 0;
            }
            server.listen("127.0.0.1", 9000);

            PT_UNIT_ASSERT( 0 == std::strncmp(input, "Hello World !!!", 15) );
            this->reportMessage("FINISHED");
        }

        void onAccept(Pt::Net::TcpServer& server)
        {
            _acceptor->accept(server);
            this->reportMessage( "ACCEPTED IP: " + _acceptor->getSockAddr() +
                                 " PEER: " + _acceptor->getPeerAddr() );

            _acceptor->beginRead(input, 200);
        }

        void onConnect(Pt::Net::TcpSocket& socket)
        {
            socket.endConnect();
            //this->reportMessage( "CONNECTED IP: " + socket.getSockAddr() +
            //                     " PEER: " + socket.getPeerAddr() );

            static const char buffer[] = "Hello World !!!";
            socket.beginWrite(buffer, sizeof(buffer));
        }

        void onInput(Pt::System::IODevice& device)
        {
            _loop->exit();
            std::size_t n = device.endRead();
            std::string msg("INPUT RECEIVED: ");
            msg.append(input, n);
            this->reportMessage(msg);
            PT_UNIT_ASSERT(n > 5);
        }

        void onOutput(Pt::System::IODevice& device)
        {
            std::size_t n = device.endWrite();
            std::cerr << "out: " << n << std::endl;
            this->reportMessage("### ### ### OUTPUT SENT");
            PT_UNIT_ASSERT(n > 5);
        }

    private:
        Pt::Net::TcpSocket* _acceptor;
        Pt::System::MainLoop* _loop;
        char input[200];
};

Pt::Unit::RegisterTest<TcpSocketTest> register_TcpSocketTest;
