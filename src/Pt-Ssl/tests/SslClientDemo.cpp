/*
 * Copyright (C) 2013 by Marc Boris Duerner
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

#include "Pkcs12Data.h"
#include <Pt/Ssl/Certificate.h>
#include <Pt/Ssl/Context.h>
#include <Pt/Ssl/CertificateStore.h>
#include <Pt/Ssl/IOStream.h>
#include <Pt/Net/TcpSocket.h>
#include <Pt/System/IOStream.h>
#include <Pt/System/MainLoop.h>
#include <Pt/System/Logger.h>

log_define("Pt.Ssl.SslClient")

class SslClient : public Pt::Connectable
{
    public:
        SslClient(Pt::System::EventLoop& loop, Pt::Ssl::Context& ctx, const std::string& host, unsigned short port)
        : _ctx(&ctx)
        , _host(host)
        , _port(port)
        , _beginExecute(&SslClient::beginConnect)
        , _endExecute(&SslClient::endConnect)
        {
            _tcpSocket.setActive(loop);
            _tcpSocket.connected() += Pt::slot(*this, &SslClient::onConnect);

            _ios.attach(_tcpSocket);
            _ios.ioBuffer().inputReady() += Pt::slot(*this, &SslClient::onInput);
            _ios.ioBuffer().outputReady() += Pt::slot(*this, &SslClient::onOutput);
        }

        Pt::System::EventLoop& loop()
        { return *_tcpSocket.parent(); }

        std::string& request()
        { return _request; }
        
        const std::string& reply()
        { return _reply; }

        void beginExecute()
        { (this->*_beginExecute)(); }

        bool endExecute()
        { return (this->*_endExecute)(); }

        void close()
        {
            _ssl.close();
            _ios.clear();
            _tcpSocket.close();
            
            _beginExecute = &SslClient::beginConnect;
            _endExecute = &SslClient::endConnect;
        }

        Pt::Signal<SslClient&>& requestProgressed()
        { return _requestProgressed; }

    private:
        void beginConnect()
        {
            log_info("connecting to " << _host << ':' << _port);
            _tcpSocket.beginConnect(_host, _port);
            
            _endExecute = &SslClient::endConnect;
        }

        bool endConnect()
        {
            _tcpSocket.endConnect();
            _ssl.open(*_ctx, _ios, Pt::Ssl::Connect);

            log_info("connected to " << _host << ':' << _port);
            _beginExecute = &SslClient::beginHandshake;
            _endExecute = &SslClient::endWrite;
            return false;
        }

        void beginHandshake()
        {
            bool wantRead = _ssl.readHandshake();
            if(wantRead)
            {
                _ios.ioBuffer().beginRead();
                _endExecute = &SslClient::endRead;
                return;
            }
                
            if( ! _ssl.isConnected() )
            {
                _ssl.writeHandshake();
                _ios.ioBuffer().beginWrite();
                _endExecute = &SslClient::endWrite;
                return;
            }

            // write any pending handshake data
            if( _ios.ioBuffer().out_avail() > 0 )
            {
                _ios.ioBuffer().beginWrite();
                _endExecute = &SslClient::endWrite;
                return;
            }
                
            // start sending the request
            log_info("sending " << _request.size() << " bytes" );
            _ssl.write( _request.c_str(), _request.size() );
            _ssl.flush();

            _beginExecute = &SslClient::beginRequest;
            _endExecute = &SslClient::endWrite;
            beginRequest();
        }

        void beginRequest()
        {
            // write pending request data
            if( _ios.ioBuffer().out_avail() > 0 )
            {
                _ios.ioBuffer().beginWrite();
                _endExecute = &SslClient::endWrite;
                return;
            }

            // start receiving the reply
            _ios.ioBuffer().beginRead();
            _beginExecute = &SslClient::beginRead;
            _endExecute = &SslClient::endReply;
        }

        bool endReply()
        {
            _ios.ioBuffer().endRead();

            for(;;)
            {
                _ssl.import();

                if(_ssl.sslBuffer().in_avail() <= 0)
                    break;

                char buf[255];
                do
                {
                    std::streamsize n = _ssl.readsome( buf, sizeof(buf) );
                    _reply.append(buf, static_cast<size_t>(n));
                }
                while(_ssl.sslBuffer().in_avail() > 0);

                log_info( "received " << _reply.size() << " bytes");
            }

            if( _ssl.isShutdown() )
            {
                log_info("received shutdown alert");

                // start sending the shutdown acknowledge
                _ssl.shutdown();
                _beginExecute = &SslClient::beginWrite;
                _endExecute = &SslClient::endShutdown;
                
                std::clog << "SHUTDOWN: " << _ssl.isShutdown() << std::endl;
                std::clog << "CLOSED: " << _ssl.isClosed() << std::endl;
                std::clog << "EOF: " << _ios.ioBuffer().device()->eof() << std::endl;
                std::clog << "CONNCTED: " << _tcpSocket.isConnected() << std::endl;
            }

            return false;
        }

        bool endShutdown()
        {
            log_trace("endShutdown");

            _ios.ioBuffer().endWrite();
            std::clog << "OUT AVAIL: " << _ios.ioBuffer().out_avail() << std::endl;

            if(_ios.ioBuffer().out_avail() <= 0)
            {
                log_info("sent shutdown alert acknowledge");
                _beginExecute = &SslClient::beginReadAck;
                _endExecute = &SslClient::endReadAck;
            }
            
            return false;
            
            // shutdown was completed
            // if(_ios.ioBuffer().out_avail() <= 0)
            // {
            //     close();
            //     return true;
            // }
        }
        
        //21:42:46.098 [Pt.Ssl.SslClient] Info - received shutdown alert
        //SHUTDOWN: 0
        //CLOSED: 1
        //EOF: 0
        //CONNCTED: 1
        //21:42:46.099 [Pt.Ssl.SslClient] Trace - endShutdown
        //OUT AVAIL: 0
        //21:42:46.099 [Pt.Ssl.SslClient] Info - sent shutdown alert acknowledge
        //beginReadAck EOF: 0
        //beginReadAck EOF: 1
        //endReadAck EOF: 1
        //CONNCTED: 1
        //after close CONNCTED: 0
        
        void beginReadAck()
        {
            std::clog << "beginReadAck EOF: " << _ios.ioBuffer().device()->eof() << std::endl;
            _ios.ioBuffer().beginRead();

            std::clog << "beginReadAck EOF: " << _ios.ioBuffer().device()->eof() << std::endl;
        }

        bool endReadAck()
        {
            _ios.ioBuffer().endRead();

            std::clog << "endReadAck EOF: " << _ios.ioBuffer().device()->eof() << std::endl;
            std::clog << "CONNCTED: " << _tcpSocket.isConnected() << std::endl;

            if( _ios.device()->eof() )
            {
                _tcpSocket.close();
                std::clog << "after close CONNCTED: " << _tcpSocket.isConnected() << std::endl;
                return true;
            }

            throw Pt::System::IOError("connection lost");
            return false;
        }

        void beginRead()
        {
            _ios.ioBuffer().beginRead();

            if( _ios.device()->eof() )
                throw Pt::System::IOError("connection lost");
        }

        bool endRead()
        {
            _ios.ioBuffer().endRead();
            return false;
        }

        void beginWrite()
        {
            _ios.ioBuffer().beginWrite();
        }

        bool endWrite()
        {
            _ios.ioBuffer().endWrite();
            return false;
        }

        void onConnect(Pt::Net::TcpSocket&)
        { _requestProgressed.send(*this); }

        void onInput(Pt::System::IOBuffer&)
        { _requestProgressed.send(*this); }

        void onOutput(Pt::System::IOBuffer&)
        { _requestProgressed.send(*this); }

    private:
        Pt::Net::TcpSocket _tcpSocket;
        Pt::System::IOStream _ios;
        Pt::Ssl::Context* _ctx;
        Pt::Ssl::IOStream _ssl;
        std::string _host;
        unsigned short _port;
        std::string _request;
        std::string _reply;
        Pt::Signal<SslClient&> _requestProgressed;

        typedef void (SslClient::*BeginFunc)();
        BeginFunc _beginExecute;

        typedef bool (SslClient::*EndFunc)();
        EndFunc _endExecute;
};


void onProgress(SslClient& client)
{
    bool finished = client.endExecute();
    if(finished)
    {
        client.loop().exit();
        return;
    }
        
    client.beginExecute();
}


int main(const char* argc, int argv)
{
    try
    {
        Pt::System::Logger::setLogLevel("Pt.Ssl.SslClient", Pt::System::Trace);

        const char request[] = "GET / HTTP/1.1\r\n"
                               "Host: www.pt-framework.org\r\n"
                               "Connection: close\r\n"
                               "\r\n";

        Pt::System::MainLoop loop;
        
        Pt::Ssl::Context ctx;
        ctx.setVerifyMode(Pt::Ssl::NoVerify);
        
        SslClient client(loop, ctx, "www.pt-framework.org", 443);
        client.request() = request;
        client.requestProgressed() += Pt::slot(onProgress);
        client.beginExecute();

        loop.run();
        return 0;
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }

    return 1;
}
