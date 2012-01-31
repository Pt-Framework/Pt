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
#include "Pt/System/Logger.h"
#include <string>

class TcpSocketTest : public Pt::Unit::TestSuite
{
    public:
        TcpSocketTest()
        : Pt::Unit::TestSuite("TcpSocketTest")
        , _acceptor(0)
        , _client(0)
        , _loop(0)
        {
          Pt::System::Logger::getTarget("").setLogLevel(Pt::System::Warn);

          this->registerMethod( "NonBlockingWithLoop", *this,
                                &TcpSocketTest::NonBlockingWithLoop);

          this->registerMethod( "CloseOnAccept", *this,
                                &TcpSocketTest::CloseOnAccept);

          this->registerMethod( "CloseOnConnect", *this,
                                &TcpSocketTest::CloseOnConnect);

          this->registerMethod( "ConnectFailed", *this,
                                &TcpSocketTest::ConnectFailed);
        }

        void setUp()
        {
            std::memset(input, 0, 20);

            _loop = new Pt::System::MainLoop();
            _loop->timeout() += Pt::slot(*_loop, &Pt::System::MainLoop::exit);
            _loop->setIdleTimeout(3000);

            _acceptor = new Pt::Net::TcpSocket();
        }

        void tearDown()
        {
            if(_acceptor)
            {
                _acceptor->close();
                delete _acceptor;
                _acceptor = 0;
            }

            if(_client)
            {
                _client->close();
                delete _client;
                _client = 0;
            }

            if(_loop)
            {
                delete _loop;
                _loop = 0;
            }
        }

        void ConnectFailed()
        {
            Pt::Net::TcpSocket client;
            client.setActive(*_loop);
            client.connected() += Pt::slot(*this, &TcpSocketTest::onConnectFailed);

            try
            {
                client.beginConnect("127.0.0.2", 9000);
            }
            catch(const Pt::System::IOError&)
            {
                // early success
                this->reportMessage("handled beginConnect directly");
                return;
            }

            _loop->run();
            PT_UNIT_ASSERT( false == client.isConnected() );
        }

        void onConnectFailed(Pt::Net::TcpSocket& socket)
        {
            this->reportMessage("reached connect callback");
            _loop->exit();
            PT_UNIT_ASSERT_THROW(socket.endConnect(), Pt::System::IOError);
        }


        void CloseOnAccept()
        {
            Pt::Net::TcpServer server("127.0.0.1", 9000);
            server.connectionPending() += Pt::slot(*this, &TcpSocketTest::onAcceptAndClose);
            server.setActive(*_loop);
            server.beginAccept();

            _acceptor->setActive(*_loop);

            _client = new Pt::Net::TcpSocket;
            _client->connected() += Pt::slot(*this, &TcpSocketTest::onConnectAndBeginRead);
            _client->inputReady() += Pt::slot(*this, &TcpSocketTest::onInputExpectEof);
            _client->setActive(*_loop);
            _client->beginConnect("127.0.0.1", 9000);

            PT_UNIT_ASSERT( ! _client->eof() );
            _loop->run();
            PT_UNIT_ASSERT( _client->eof() );
        }

        void CloseOnConnect()
        {
            Pt::Net::TcpServer server("127.0.0.1", 9000);
            server.connectionPending() += Pt::slot(*this, &TcpSocketTest::onAcceptAndBeginRead);
            server.setActive(*_loop);
            server.beginAccept();

            _acceptor->inputReady() += Pt::slot(*this, &TcpSocketTest::onInputExpectEof);
            _acceptor->setActive(*_loop);

            _client = new Pt::Net::TcpSocket;
            _client->connected() += Pt::slot(*this, &TcpSocketTest::onConnectAndClose);
            _client->inputReady() += Pt::slot(*this, &TcpSocketTest::onInputExpectEof);
            _client->setActive(*_loop);
            _client->beginConnect("127.0.0.1", 9000);

            PT_UNIT_ASSERT( ! _acceptor->eof() );
            _loop->run();
            PT_UNIT_ASSERT( _acceptor->eof() );
        }

        void onAcceptAndClose(Pt::Net::TcpServer& server)
        {
            _acceptor->accept(server);
            //this->reportMessage( "ACCEPTED IP: " + _acceptor->socketAddress() +
            //                     " PEER: " + _acceptor->peerAddress() );

            _acceptor->close();
            delete _acceptor;
            _acceptor = 0;
        }

        void onConnectAndBeginRead(Pt::Net::TcpSocket& socket)
        {
            socket.endConnect();
            //this->reportMessage( "CONNECTED IP: " + socket.socketAddress() +
            //                     " PEER: " + socket.peerAddress() );

            _client->beginRead(input, sizeof(input));
        }

        void onInputExpectEof(Pt::System::IODevice& device)
        {
            std::size_t n = device.endRead();
            PT_UNIT_ASSERT(n == 0);
            PT_UNIT_ASSERT(device.eof());
            _loop->exit();
        }

        void onConnectAndClose(Pt::Net::TcpSocket& socket)
        {
            socket.close();
        }

        void NonBlockingWithLoop()
        {
            Pt::Net::TcpServer server("127.0.0.1", 9000);
            {
                //this->reportMessage("\nSTART");

                server.connectionPending() += Pt::slot(*this, &TcpSocketTest::onAcceptAndBeginRead);
                server.setActive(*_loop);
                server.beginAccept();

                _acceptor->inputReady() += Pt::slot(*this, &TcpSocketTest::onInput);
                _acceptor->setActive(*_loop);

                Pt::Net::TcpSocket client;
                client.connected() += Pt::slot(*this, &TcpSocketTest::onConnectAndBeginWrite);
                client.outputReady() += Pt::slot(*this, &TcpSocketTest::onOutput);
                client.setActive(*_loop);
                client.beginConnect("127.0.0.1", 9000);

                _loop->run();

                server.close();

                delete _loop;
                _loop  = 0;
            }

            server.listen("127.0.0.1", 9000);

            PT_UNIT_ASSERT( 0 == std::strncmp(input, "Hello World !!!", 15) );
            //this->reportMessage("FINISHED");
        }

        void onAcceptAndBeginRead(Pt::Net::TcpServer& server)
        {
            _acceptor->accept(server);
            //this->reportMessage( "ACCEPTED IP: " + _acceptor->socketAddress() +
            //                     " PEER: " + _acceptor->peerAddress() );

            _acceptor->beginRead(input, 200);
        }

        void onConnectAndBeginWrite(Pt::Net::TcpSocket& socket)
        {
            socket.endConnect();
            //this->reportMessage( "CONNECTED IP: " + socket.socketAddress() +
            //                     " PEER: " + socket.peerAddress() );

            static const char buffer[] = "Hello World !!!";
            socket.beginWrite(buffer, sizeof(buffer));
        }

        void onInput(Pt::System::IODevice& device)
        {
            _loop->exit();
            std::size_t n = device.endRead();
            //std::string msg("INPUT RECEIVED: ");
            //msg.append(input, n);
            //this->reportMessage(msg);
            PT_UNIT_ASSERT(n > 5);
        }

        void onOutput(Pt::System::IODevice& device)
        {
            std::size_t n = device.endWrite();
            //this->reportMessage("### ### ### OUTPUT SENT");
            PT_UNIT_ASSERT(n > 5);
        }

    private:
        Pt::Net::TcpSocket* _acceptor;
        Pt::Net::TcpSocket* _client;
        Pt::System::MainLoop* _loop;
        char input[200];
};

Pt::Unit::RegisterTest<TcpSocketTest> register_TcpSocketTest;
