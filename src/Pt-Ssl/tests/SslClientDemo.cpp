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

class Request
{
    public:
        Request()
        {}

        ~Request()
        {}

        std::string& data()
        { return _data; }

        const std::string& data() const
        { return _data; }

    private:
        std::string _data;
};

class Reply
{
    public:
        Reply()
        {}

        ~Reply()
        {}

        std::string& data()
        { return _data; }

        const std::string& data() const
        { return _data; }

    private:
        std::string _data;
};

class SslClient : public Pt::Connectable
{
    public:
        SslClient(Pt::System::EventLoop& loop, const std::string& host, unsigned short port)
        : _host(host)
        , _port(port)
        , _state(Idle)
        {
            _ctx.setVerifyMode(Pt::Ssl::NoVerify);

            _tcpSocket.setActive(loop);
            _tcpSocket.connected() += Pt::slot(*this, &SslClient::onConnect);

            _ios.attach(_tcpSocket);
            _ios.ioBuffer().inputReady() += Pt::slot(*this, &SslClient::onInput);
            _ios.ioBuffer().outputReady() += Pt::slot(*this, &SslClient::onOutput);
        }

        Pt::System::EventLoop& loop()
        { return *_tcpSocket.parent(); }

        Request& request()
        { return _request; }
        
        const Reply& reply()
        { return _reply; }

        void beginExecute()
        {
            if(_state == Idle)
            {
                log_info("connecting to " << _host << ':' << _port);
                _tcpSocket.beginConnect(_host, _port);
                return;
            }

            if(_state == Connected || _state == ReadHandshake || _state == WriteHandshake)
            {
                bool wantRead = _ssl.readHandshake();
                if(wantRead)
                {
                    _ios.ioBuffer().beginRead();
                    _state = ReadHandshake;
                    return;
                }
                
                if( ! _ssl.isConnected() )
                {
                    _ssl.writeHandshake();
                    _ios.ioBuffer().beginWrite();
                    _state = WriteHandshake;
                    return;
                }

                _state = SendRequest;
            }

            if(_state == SendRequest)
            {
                log_info("sending:\n" << _request.data() );
                _ssl.write( _request.data().c_str(), _request.data().size() );
                _ssl.flush();

                _ios.ioBuffer().beginWrite();
            }

            if(_state == ReceiveReply)
            {
                _ios.ioBuffer().beginRead();
            }
        }

        bool endExecute()
        {
            if(_state == Idle)
            {
                _tcpSocket.endConnect();
                _ssl.open(_ctx, _ios, Pt::Ssl::Connect);
                _state = Connected;
                return false;
            }

            if(_state == ReadHandshake)
            {
                _ios.ioBuffer().endRead();
                return false;
            }

            if(_state == WriteHandshake)
            {
                _ios.ioBuffer().endWrite();
                return false;
            }

            if(_state == SendRequest)
            {
                _ios.ioBuffer().endWrite();
                
                if(  _ios.ioBuffer().out_avail() <= 0 )
                {
                    log_info("request sent");
                    _state = ReceiveReply;
                }
                
                return false;
            }

            if(_state == ReceiveReply)
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
                        std::cout.write(buf, n);
                    }
                    while(_ssl.sslBuffer().in_avail() > 0);
                }

                if( _ssl.isShutdown() )
                {
                    log_info("received shutdown alert");
                }
                
                //if( ! _tcpSocket.isConnected() )
                //{
                //    log_info("connection closed");
                //    return true;
                //}

                if( _ssl.isShutdown() )
                {
                    std::clog << "SHUTDOWN: " << _ssl.isShutdown() << std::endl;
                    std::clog << "CLOSED: " << _ssl.isClosed() << std::endl;
                    std::clog << "EOF: " << _ios.ioBuffer().device()->eof() << std::endl;
                    std::clog << "CONNCTED: " << _tcpSocket.isConnected() << std::endl;

                    _ssl.shutdown();
                    _ios.ioBuffer().beginWrite();
                    std::clog << "EOF2: " << _ios.ioBuffer().device()->eof() << std::endl;
                    return true;
                }
            }

            return false;
        }

        void onConnect(Pt::Net::TcpSocket&)
        {
            _requestProgressed.send(*this);
        }

        void onInput(Pt::System::IOBuffer&)
        {
            _requestProgressed.send(*this);
        }

        void onOutput(Pt::System::IOBuffer&)
        {
            _requestProgressed.send(*this);
        }

        Pt::Signal<SslClient&>& requestProgressed()
        { return _requestProgressed; }

    private:
        Pt::Net::TcpSocket _tcpSocket;
        Pt::System::IOStream _ios;
        Pt::Ssl::CertificateStore _store;
        Pt::Ssl::Context _ctx;
        Pt::Ssl::IOStream _ssl;
        std::string _host;
        unsigned short _port;
        Request _request;
        Reply _reply;
        Pt::Signal<SslClient&> _requestProgressed;

        enum State {
            Idle,
            Connected,
            Handshake,
            ReadHandshake,
            WriteHandshake,
            SendRequest,
            ReceiveReply
        } _state;
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
        SslClient client(loop, "www.pt-framework.org", 443);
        client.request().data() = request;
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
