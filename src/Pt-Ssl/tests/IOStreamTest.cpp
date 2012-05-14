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
#include <Pt/Ssl/IOStream.h>
#include "Pt/Ssl/CertificateList.h"
#include "Pt/Ssl/Context.h"
#include <Pt/Net/TcpSocket.h>
#include <Pt/Net/TcpServer.h>
#include <Pt/System/MainLoop.h>
#include "Pt/System/Logger.h"
#include <sstream>

log_define("Pt.Ssl.IOStreamTest")

class TcpAcceptor : public Pt::Connectable 
{
    public:
        TcpAcceptor(Pt::System::EventLoop& loop, Pt::Ssl::Context& ctx,
                    const std::string& addr, unsigned short port)
        : _loop(loop)
        , _ssl(0)
        , _ios(8192, true)
        , _socket(0)
        {
            log_debug("listening on " << addr << ':' << port);
            _server.listen(addr, port);
            _server.connectionPending() += Pt::slot(*this, &TcpAcceptor::onAccept);
            _server.setActive(_loop);
            _server.beginAccept();

            _ssl = new Pt::Ssl::IOStream(ctx, _ios, 0);
        }

        ~TcpAcceptor()
        {
            delete _socket;
            delete _ssl;
        }

        Pt::Ssl::IOStream& sslStream()
        { return *_ssl; }

   private:
        void onAccept(Pt::Net::TcpServer& server)
        {
            log_debug("accepting TCP connection");
            _socket = new Pt::Net::TcpSocket(server);
            _socket->setActive(_loop);
            _ios.attach(*_socket);

            log_debug("starting accept handshake");
            _ssl->beginAcceptHandshake(true, true);
        }

    private:
        Pt::System::EventLoop& _loop;
        Pt::Ssl::IOStream* _ssl;
        Pt::System::IOStream _ios;
        Pt::Net::TcpServer _server;
        Pt::Net::TcpSocket* _socket;
};

class TcpConnector : public Pt::Connectable 
{
    public:
        TcpConnector(Pt::System::EventLoop& loop, Pt::Ssl::Context& ctx, 
                     const std::string& addr, unsigned short port)
        : _loop(loop)
        , _ssl(0)
        , _ios(8192, true)
        {
            log_debug("connecting to " << addr << ':' << port);

            _socket.setActive(_loop);
            _socket.connected() += Pt::slot(*this, &TcpConnector::onConnect);
            _socket.beginConnect(addr, port);

            _ssl = new Pt::Ssl::IOStream(ctx, _ios, 0);
        }

        ~TcpConnector()
        { delete _ssl; }

        Pt::Ssl::IOStream& sslStream()
        { return *_ssl; }

    private:
        void onConnect(Pt::Net::TcpSocket& socket)
        {
            _socket.endConnect();
            _ios.attach(socket);

            log_debug("starting connect handshake");
            _ssl->beginConnectHandshake(true);
        }

    private:
        Pt::System::EventLoop& _loop;
        Pt::Ssl::IOStream* _ssl;
        Pt::System::IOStream _ios;
        Pt::Net::TcpSocket _socket;
};


class IOStreamTest : public Pt::Unit::TestSuite
{
    public:
        IOStreamTest()
        : Pt::Unit::TestSuite("IOStreamTest")
        , _serverPrivKey("abc123")
        , _clientPrivKey("")
        , _loop(0)
        , _msgCnt(0)
        {
            //Pt::System::Logger::getTarget("Pt.Ssl").setLogLevel(Pt::System::Trace);

            this->registerMethod("ReadWrite", *this, &IOStreamTest::ReadWrite);

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

        void onAcceptHandshake(Pt::Ssl::IOStream& ssl)
        {
            log_trace("IOStreamTest::onAcceptHandshake");
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

            log_debug("peer name = " << ssl.buffer().peerName());
            log_debug("current cipher = " << ssl.buffer().currentCipher().name());
            ssl.beginRead();
        }

        void onServerShutdown(Pt::Ssl::IOStream& ssl)
        {
            ssl.endShutdown();
            log_info("server finished shutdown");
            _loop->exit();
        }

        void onServerInput(Pt::Ssl::IOStream& ssl)
        {
            std::streamsize r = ssl.endRead();
            if(r < 0) 
            {                   
                log_info("received shutdown from client");
                ssl.beginShutdown();
                return;
            }
            
            log_debug("server ends read, shutdown: " << r);

            std::string msg;
            char buf[512];
            while( ssl.readsome(buf, 512) > 0 ) 
            {
                size_t n = static_cast<size_t>( ssl.gcount() );
                log_debug("server received: " << n);
                msg.append(buf, n);
            }

            log_debug("server received message: " << msg);

            // Send reply
            ssl << "Hello world from server!";
            for(int i = 0; i < 1024; ++i) 
                ssl << "_12345678X";
            ssl << "!!!" << std::flush;

            ssl.beginWrite();
        }

        void onServerOutput(Pt::Ssl::IOStream& ssl)
        {
            ssl.endWrite();
            ssl.beginRead();
        }

        void onConnectHandshake(Pt::Ssl::IOStream& ssl)
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

            log_debug("peer name = " << ssl.buffer().peerName());
            log_debug("current cipher = " << ssl.buffer().currentCipher().name());

            std::string lmsg = "Hello world from client!";
            log_debug("client sending message... size = " << lmsg.length());
            ssl << lmsg << std::flush;
            ssl.beginWrite();
        }

        void onClientShutdown(Pt::Ssl::IOStream& ssl)
        {
            ssl.endShutdown();
            log_info("client finished shutdown");
        }

        void onClientInput(Pt::Ssl::IOStream& ssl)
        {
            std::streamsize r = ssl.endRead();
            if(r < 0) 
            {                   
                log_info("received shutdown from server");
                ssl.beginShutdown();
                return;
            }
            
            log_debug("client ends read, shutdown: " << r);

            std::string msg;
            char buf[512];
            while( ssl.readsome(buf, 512) > 0 ) 
            {
                size_t n = static_cast<size_t>( ssl.gcount() );
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
                ssl << "Good morning from client!" << std::flush;
                ssl.beginWrite();
            }
            else 
            {
                log_debug("client shutting down the stream");
                ssl.beginShutdown();
            }
        }

        void onClientOutput(Pt::Ssl::IOStream& ssl)
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


Pt::Unit::RegisterTest<IOStreamTest> register_IOStreamTest;


void IOStreamTest::ReadWrite()
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
    acceptor.sslStream().handshakeFinished() += Pt::slot(*this, &IOStreamTest::onAcceptHandshake);
    acceptor.sslStream().shutdownFinished() += Pt::slot(*this, &IOStreamTest::onServerShutdown);
    acceptor.sslStream().inputReady() += Pt::slot(*this, &IOStreamTest::onServerInput);
    acceptor.sslStream().outputReady() += Pt::slot(*this, &IOStreamTest::onServerOutput);

    TcpConnector connector(*_loop, clientContext, addr, port);
    connector.sslStream().handshakeFinished() += Pt::slot(*this, &IOStreamTest::onConnectHandshake);
    connector.sslStream().shutdownFinished() += Pt::slot(*this, &IOStreamTest::onClientShutdown);
    connector.sslStream().inputReady() += Pt::slot(*this, &IOStreamTest::onClientInput);
    connector.sslStream().outputReady() += Pt::slot(*this, &IOStreamTest::onClientOutput);

    _loop->run();

    PT_UNIT_ASSERT_EQUALS(_msgCnt, 3);
}
