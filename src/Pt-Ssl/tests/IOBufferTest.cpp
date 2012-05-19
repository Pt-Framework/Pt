/*
 * Copyright (C) 2010 - 2012 by Marc Boris Duerner
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

#include "PemData.h"
#include "Pt/Unit/Assertion.h"
#include "Pt/Unit/TestSuite.h"
#include "Pt/Unit/RegisterTest.h"
#include <Pt/Ssl/IOBuffer.h>
#include "Pt/Ssl/CertificateList.h"
#include "Pt/Ssl/Context.h"
#include <Pt/Net/TcpSocket.h>
#include <Pt/Net/TcpServer.h>
#include <Pt/System/MainLoop.h>
#include "Pt/System/Logger.h"
#include <sstream>

log_define("Pt.Ssl.IOBufferTest")

class TcpAcceptor : public Pt::Connectable 
{
    public:
        TcpAcceptor(Pt::System::EventLoop& loop, Pt::Ssl::Context& ctx,
                    const std::string& addr, unsigned short port)
        : _loop(loop)
        , _ios(8192, true)
        , _ssl(ctx, _ios)
        , _socket(0)
        {
            log_debug("listening on " << addr << ':' << port);
            _server.listen(addr, port);
            _server.connectionPending() += Pt::slot(*this, &TcpAcceptor::onAccept);
            _server.setActive(_loop);
            _server.beginAccept();
        }

        ~TcpAcceptor()
        {
            delete _socket;
        }

        Pt::Ssl::IOBuffer& buffer()
        { return _ssl; }

   private:
        void onAccept(Pt::Net::TcpServer& server)
        {
            log_debug("accepting TCP connection");
            _socket = new Pt::Net::TcpSocket(server);
            _socket->setActive(_loop);
            _ios.attach(*_socket);

            log_debug("starting accept handshake");
            //_ssl.attach(_ios);
            _ssl.beginAccept(true, true);
        }

    private:
        Pt::System::EventLoop& _loop;
        Pt::Net::TcpServer _server;
        Pt::System::IOStream _ios;
        Pt::Ssl::IOBuffer _ssl;
        Pt::Net::TcpSocket* _socket;
};

class TcpConnector : public Pt::Connectable 
{
    public:
        TcpConnector(Pt::System::EventLoop& loop, Pt::Ssl::Context& ctx, 
                     const std::string& addr, unsigned short port)
        : _loop(loop)
        , _ios(8192, true)
        , _ssl(ctx, _ios)
        {
            log_debug("connecting to " << addr << ':' << port);

            _socket.setActive(_loop);
            _socket.connected() += Pt::slot(*this, &TcpConnector::onConnect);
            _socket.beginConnect(addr, port);
        }

        ~TcpConnector()
        { }

        Pt::Ssl::IOBuffer& buffer()
        { return _ssl; }

    private:
        void onConnect(Pt::Net::TcpSocket& socket)
        {
            _socket.endConnect();
            _ios.attach(socket);

            log_debug("starting connect handshake");
            _ssl.beginConnect(true);
        }

    private:
        Pt::System::EventLoop& _loop;
        Pt::Net::TcpSocket _socket;
        Pt::System::IOStream _ios;
        Pt::Ssl::IOBuffer _ssl;
};


class IOBufferTest : public Pt::Unit::TestSuite
{
    public:
        IOBufferTest()
        : Pt::Unit::TestSuite("IOBufferTest")
        , _serverPrivKey("abc123")
        , _clientPrivKey("")
        , _loop(0)
        , _msgCnt(0)
        {
            //Pt::System::Logger::getTarget("Pt").setLogLevel(Pt::System::Trace);

            this->registerMethod("ReadWrite", *this, &IOBufferTest::ReadWrite);

            _cACert.fromPem(caPemData, sizeof(caPemData));
            _serverCertChain.fromPem(serverCertPemData, sizeof(serverCertPemData));
            _serverPrivKey.fromPem(serverKeyData, sizeof(serverKeyData));
            _clientCertChain.fromPem(clientCertPemData, sizeof(clientCertPemData));
            _clientPrivKey.fromPem(clientKeyData, sizeof(clientKeyData));
        }

        void setUp()
        {
            _loop = new Pt::System::MainLoop;
            _msgCnt = 0;
        }

        void tearDown()
        { 
            delete _loop;
            _loop = 0;
        }

        void onAcceptHandshake(Pt::Ssl::IOBuffer& ssl)
        {
            log_trace("IOBufferTest::onAcceptHandshake");
            try 
            {
                ssl.endHandshake();
            }
            catch(...) 
            {
                log_error("accept handshake failed");
                _loop->exit();
                return;
            }

            log_debug("peer name = " << ssl.peerName());
            log_debug("current cipher = " << ssl.currentCipher().name());
            ssl.beginRead();
        }

        void onServerShutdown(Pt::Ssl::IOBuffer& ssl)
        {
            ssl.endShutdown();
            log_info("server finished shutdown");
            _loop->exit();
        }

        void onServerInput(Pt::Ssl::IOBuffer& ssl)
        {
            std::streamsize r = ssl.endRead();
            if(r < 0) 
            {                   
                log_info("received shutdown from client");
                ssl.beginShutdown();
                return;
            }
            
            log_debug("server ends read, shutdown: " << r);
            std::iostream ios(&ssl);
            
            std::string msg;
            char buf[512];
            while( ios.readsome(buf, 512) > 0 ) 
            {
                size_t n = static_cast<size_t>( ios.gcount() );
                log_debug("server received: " << n);
                msg.append(buf, n);
            }

            log_debug("server received message: " << msg);

            // Send reply
            ios << "Hello world from server!";
            for(int i = 0; i < 1024; ++i) 
                ios << "_12345678X";
            ios << "!!!" << std::flush;

            ssl.beginWrite();
        }

        void onServerOutput(Pt::Ssl::IOBuffer& ssl)
        {
            ssl.endWrite();
            ssl.beginRead();
        }

        void onConnectHandshake(Pt::Ssl::IOBuffer& ssl)
        {
            log_trace("IOStreamTest::onConnectHandshake");
            
            try 
            {
                ssl.endHandshake();
            }
            catch(...) 
            {
                log_error("connect handshake failed");
                _loop->exit();
                return;
            }

            log_debug("peer name = " << ssl.peerName());
            log_debug("current cipher = " << ssl.currentCipher().name());

            std::string lmsg = "Hello world from client!";
            log_debug("client sending message... size = " << lmsg.length());

            std::ostream os(&ssl);
            os << lmsg << std::flush;

            ssl.beginWrite();
        }

        void onClientShutdown(Pt::Ssl::IOBuffer& ssl)
        {
            ssl.endShutdown();
            log_info("client finished shutdown");
        }

        void onClientInput(Pt::Ssl::IOBuffer& ssl)
        {
            std::streamsize r = ssl.endRead();
            if(r < 0) 
            {                   
                log_info("received shutdown from server");
                ssl.beginShutdown();
                return;
            }
            
            log_debug("client ends read, shutdown: " << r);
            std::iostream ios(&ssl);
            std::string msg;
            char buf[512];
            while( ios.readsome(buf, 512) > 0 ) 
            {
                size_t n = static_cast<size_t>( ios.gcount() );
                log_debug("client received: " << n);
                msg.append(buf, n);
            }

            log_debug("client received: " << msg);
            if( msg.find("!!!") == std::string::npos )
            {
                log_debug("client message not complete ");
                ssl.beginRead();
            }
            else if(++_msgCnt < 3) 
            {
                log_debug("client sending another message to the server ...");
                ios << "Good morning from client!" << std::flush;
                ssl.beginWrite();
            }
            else 
            {
                log_debug("client shutting down the stream");
                ssl.beginShutdown();
            }
        }

        void onClientOutput(Pt::Ssl::IOBuffer& ssl)
        {
            ssl.endWrite();
            ssl.beginRead();
        }

        void ReadWrite();

    private:
        Pt::Ssl::CertificateList _cACert;
        Pt::Ssl::CertificateList _serverCertChain;
        Pt::Ssl::PrivateKey _serverPrivKey;
        Pt::Ssl::CertificateList _clientCertChain;
        Pt::Ssl::PrivateKey _clientPrivKey;
        Pt::System::MainLoop* _loop;
        unsigned _msgCnt;
};


Pt::Unit::RegisterTest<IOBufferTest> register_IOBufferTest;


void IOBufferTest::ReadWrite()
{
    Pt::Ssl::Context serverContext(Pt::Ssl::Context::Default);
    serverContext.setCACertificates(_cACert);
    serverContext.setCertificateChain(_serverCertChain);
    serverContext.setPrivateKey(_serverPrivKey);

    Pt::Ssl::Context clientContext(Pt::Ssl::Context::Default);
    clientContext.setCACertificates(_cACert);
    clientContext.setCertificateChain(_clientCertChain);
    clientContext.setPrivateKey(_clientPrivKey);

    std::string addr("127.0.0.1");
    unsigned short port = 6000;

    TcpAcceptor acceptor(*_loop, serverContext, addr, port);
    acceptor.buffer().handshakeFinished() += Pt::slot(*this, &IOBufferTest::onAcceptHandshake);
    acceptor.buffer().shutdownFinished() += Pt::slot(*this, &IOBufferTest::onServerShutdown);
    acceptor.buffer().inputReady() += Pt::slot(*this, &IOBufferTest::onServerInput);
    acceptor.buffer().outputReady() += Pt::slot(*this, &IOBufferTest::onServerOutput);

    TcpConnector connector(*_loop, clientContext, addr, port);
    connector.buffer().handshakeFinished() += Pt::slot(*this, &IOBufferTest::onConnectHandshake);
    connector.buffer().shutdownFinished() += Pt::slot(*this, &IOBufferTest::onClientShutdown);
    connector.buffer().inputReady() += Pt::slot(*this, &IOBufferTest::onClientInput);
    connector.buffer().outputReady() += Pt::slot(*this, &IOBufferTest::onClientOutput);

    _loop->run();

    PT_UNIT_ASSERT_EQUALS(_msgCnt, 3);
}
