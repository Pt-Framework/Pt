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
#include <Pt/System/Application.h>
#include <Pt/System/Logger.h>
#include <cassert>

log_define("Pt.Ssl.SslClient")

class SslClient : public Pt::Connectable
{
    public:
        SslClient(Pt::System::EventLoop& loop, Pt::Ssl::Context& ctx)
        : _ctx(&ctx)
        , _host()
        , _port(0)
        , _beginExecute(&SslClient::beginConnect)
        , _endExecute(&SslClient::endConnect)
        {
            _tcpSocket.setActive(loop);
            _tcpSocket.connected() += Pt::slot(*this, &SslClient::onConnect);

            _ios.attach(_tcpSocket);
            _ios.ioBuffer().inputReady() += Pt::slot(*this, &SslClient::onInput);
            _ios.ioBuffer().outputReady() += Pt::slot(*this, &SslClient::onOutput);
        }

        void setTarget(const std::string& url, const std::string& host, unsigned short port)
        {
            _host = host;
            _port = port;
            _request = "GET " + url + " HTTP/1.1\r\n"
                       "Host: " + host + "\r\n"
                       "Connection: close\r\n"
                       "\r\n";
        }
        
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
            _endExecute = &SslClient::endHandshake;
            return false;
        }

        void beginHandshake()
        {
            bool wantRead = _ssl.readHandshake();
            if(wantRead)
            {
                _ios.ioBuffer().beginRead();
                return;
            }
                
            if( ! _ssl.isConnected() )
            {
                _ssl.writeHandshake();
                _ios.ioBuffer().beginWrite();
                return;
            }
                
            // start sending the request
            log_info("sending " << _request.size() << " bytes" );
            _ssl.write( _request.c_str(), _request.size() );
            _ssl.flush();

            _beginExecute = &SslClient::beginRequest;
            _endExecute = &SslClient::endRequest;
            _ios.ioBuffer().beginWrite();
        }
        
        bool endHandshake()
        {
            if( _ios.ioBuffer().isReading() )
            {
                _ios.ioBuffer().endRead();
            
                if( _ios.device()->eof() )
                    throw Pt::System::IOError("connection lost");
            }

            if( _ios.ioBuffer().isWriting() )
            {
                _ios.ioBuffer().endWrite();
            }
            
            return false;
        }
        
        void beginRequest()
        {
            _ios.ioBuffer().beginWrite();
        }

        bool endRequest()
        {
            _ios.ioBuffer().endWrite();

            // write pending request data
            if( _ios.ioBuffer().out_avail() > 0 )
            {
                return false;
            }

            // start receiving the reply
            _beginExecute = &SslClient::beginReply;
            _endExecute = &SslClient::endReply;
            return false;
        }

        void beginReply()
        {
            _ios.ioBuffer().beginRead();
        }

        bool endReply()
        {
            _ios.ioBuffer().endRead();

            if( _ios.device()->eof() )
                throw Pt::System::IOError("connection lost");

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
                _beginExecute = &SslClient::beginShutdown;
                _endExecute = &SslClient::endShutdown;
            }

            return false;
        }

        void beginShutdown()
        {
            _ios.ioBuffer().beginWrite();
        }

        bool endShutdown()
        {
            _ios.ioBuffer().endWrite();

            // write any pending shutdown data
            if(_ios.ioBuffer().out_avail() > 0)
            {
                return false;
            }

            log_info("shutdown complete");
            close();
            return true;
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
        log_info("reply finished, got " << client.reply().size() << " bytes");
        Pt::System::Application::instance().exit();
        return;
    }
        
    client.beginExecute();
}


int main(int argv, const char** argc)
{
    try
    {
        Pt::System::Logger::setLogLevel("Pt.Ssl.SslClient", Pt::System::Info);

        Pt::System::Application app;
        
        Pt::Ssl::Context ctx;
        ctx.setVerifyMode(Pt::Ssl::NoVerify);
        
        SslClient client(app.loop(), ctx);
        client.setTarget("/index.html", "www.pt-framework.org", 443);
        client.requestProgressed() += Pt::slot(onProgress);
        client.beginExecute();

        app.run();
        return 0;
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }

    return 1;
}
