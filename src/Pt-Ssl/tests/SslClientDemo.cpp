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
        SslClient(Pt::System::EventLoop& loop)
        {
            _ctx.setVerifyMode(Pt::Ssl::NoVerify);

            _tcpSocket.setActive(loop);
            _tcpSocket.connected() += Pt::slot(*this, &SslClient::onConnect);

            _ios.attach(_tcpSocket);
            _ios.ioBuffer().inputReady() += Pt::slot(*this, &SslClient::onInput);
            _ios.ioBuffer().outputReady() += Pt::slot(*this, &SslClient::onOutput);
        }

        void beginSend(const std::string& host, unsigned short port, const std::string& data)
        {
            _data = data;
            _tcpSocket.beginConnect(host, port);
        }

        void onConnect(Pt::Net::TcpSocket& socket)
        {
            socket.endConnect();
            _ssl.open(_ctx, _ios, Pt::Ssl::Connect);
            handshake( _ios.ioBuffer() );
        }

        void onInput(Pt::System::IOBuffer& sb)
        {
            sb.endRead();

            if( ! _ssl.isConnected()  )
            {
                if( handshake(sb) )
                {
                    beginSend(sb);
                }
                
                return;
            }

            //TODO: add signals for connected()
            //      add beginRequest() / endRequest()
            //      add requestFinished();

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
                std::clog << "SHUTDOWN: " << _ssl.isShutdown() << std::endl;
                std::clog << "CLOSED: " << _ssl.isClosed() << std::endl;
                std::clog << "EOF: " << sb.device()->eof() << std::endl;

                _ssl.shutdown();
                sb.beginWrite();
                std::clog << "EOF2: " << sb.device()->eof() << std::endl;
                return;
            }

            if( sb.device()->eof() )
            {
                std::clog << "EOF: " << sb.device()->eof() << std::endl;
                return;
            }

            sb.beginRead();
        }

        void onOutput(Pt::System::IOBuffer& sb)
        {
            sb.endWrite();

            if( sb.out_avail() > 0 )
            {
                sb.beginWrite();
                return;
            }

            if( ! _ssl.isConnected()  )
            {
                if( handshake(sb) )
                {
                    beginSend(sb);
                }
                
                return;
            }

            sb.beginRead();
        }

        bool handshake(Pt::System::IOBuffer& sb)
        {
            bool wantRead = _ssl.readHandshake();
            if(wantRead)
            {
                sb.beginRead();
                return false;
            }
                
            if( ! _ssl.isConnected() )
            {
                _ssl.writeHandshake();
                sb.beginWrite();
                return false;
            }

            log_info("connected");
            return true;
        }

        void beginSend(Pt::System::IOBuffer& sb)
        {
            log_info("sending " << _data);
            _ssl.write( _data.c_str(), _data.size() );
            _ssl.flush();

            sb.beginWrite();
        }

    private:
        Pt::Net::TcpSocket _tcpSocket;
        Pt::System::IOStream _ios;
        Pt::Ssl::CertificateStore _store;
        Pt::Ssl::Context _ctx;
        Pt::Ssl::IOStream _ssl;
        std::string _data;
};


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
        SslClient client(loop);
        client.beginSend("www.pt-framework.org", 443, request);

        loop.run();
        return 0;
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }

    return 1;
}
