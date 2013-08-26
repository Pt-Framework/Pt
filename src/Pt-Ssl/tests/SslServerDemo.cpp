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

#include <Pt/Ssl/Certificate.h>
#include <Pt/Ssl/Context.h>
#include <Pt/Ssl/CertificateStore.h>
#include <Pt/Ssl/IOStream.h>
#include <Pt/Net/TcpServer.h>
#include <Pt/Net/TcpSocket.h>
#include <Pt/System/IOStream.h>
#include <Pt/System/MainLoop.h>
#include "Pkcs12Data.h"

struct Error
{};

class SslServer : public Pt::Connectable
{
    public:
        SslServer(Pt::System::EventLoop& loop)
        {
            _store.loadPkcs12((const char*)serverPkcs12, sizeof(serverPkcs12), "123");
            const Pt::Ssl::Certificate* servCert = _store.findCertificate("SGC Mainframe");
            if( ! servCert)
                throw Error();

            _ctx.setIdentity( *servCert );
            _ctx.setVerifyMode(Pt::Ssl::NoVerify);

            _tcpServer.setActive(loop);
            _tcpServer.connectionPending() += Pt::slot(*this, &SslServer::onAccept);

            _tcpSocket.setActive(loop);

            _ios.attach(_tcpSocket);
            _ios.ioBuffer().inputReady() += Pt::slot(*this, &SslServer::onInput);
            _ios.ioBuffer().outputReady() += Pt::slot(*this, &SslServer::onOutput);
        }

        void listen(const std::string host, unsigned short port)
        {
            _tcpServer.listen(host, port);
            _tcpServer.beginAccept();
        }

        void onAccept(Pt::Net::TcpServer& server)
        {
            _tcpSocket.accept(server);
            _ssl.open(_ctx, _ios, Pt::Ssl::Accept);
            _ios.ioBuffer().beginRead();
        }

        void onInput(Pt::System::IOBuffer& sb)
        {
            sb.endRead();

            if( ! _ssl.isConnected() && ! handshake(sb) )
            {
                return;
            }
        }

        void onOutput(Pt::System::IOBuffer& sb)
        {
            sb.endWrite();

            if( sb.out_avail() > 0 )
            {
                sb.beginWrite();
                return;
            }

            if( ! _ssl.isConnected() && ! handshake(sb) )
            {
                return;
            }
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

            return true;
        }

    private:
        Pt::Net::TcpServer _tcpServer;
        Pt::Net::TcpSocket _tcpSocket;
        Pt::System::IOStream _ios;
        Pt::Ssl::CertificateStore _store;
        Pt::Ssl::Context _ctx;
        Pt::Ssl::IOStream _ssl;
};


int main(const char* argc, int argv)
{
    try
    {
        Pt::System::MainLoop loop;
        SslServer server(loop);
        server.listen("127.0.0.1", 443);

        loop.run();
        return 0;
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }

    return 1;
}
