/*
 * Copyright (C) 2010-2010 by Marc Boris Duerner
 * Copyright (C) 2010-2010 by Aloysius Indrayanto
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

// Build using: ./jam.sh -q --with-openssl

#include <iostream>
#include <stdexcept>

#include <Pt/Net/TcpSocket.h>
#include <Pt/Net/TcpServer.h>
#include <Pt/System/Thread.h>
#include <Pt/System/MainLoop.h>

#include "SSLStreamBufferClient.h"

class Client : public Pt::Connectable {
    public:
        Client(Pt::System::EventLoop& loop, const std::string& addr, unsigned short port, Pt::Ssl::SSLContext& sslClientContext)
        : _sslContext(sslClientContext), _ssl(0), _loop(loop), _msgCnt(0)
        {
            std::cout << "[@@ Client-TCP  ] ################################### Connecting to server" << std::endl;
            _socket.connected += Pt::slot(*this, &Client::_onTCPConnect);
            _socket.beginConnect(addr, port);
            _loop.add(_socket);
        }

        ~Client()
        { delete _ssl; }

        void sendMessage(const char* msg)
        {
            std::cout << "[@@ Client-SSL  ] ################################### Sending message to server" << std::endl;

            std::ostream os(_ssl);
            os << msg;
            os.flush();

            ++_msgCnt;
        }

    private:
        void _onTCPConnect(Pt::Net::TcpSocket& socket)
        {
            _socket.endConnect();

            std::cout << "[@@ Client-SSL  ] ################################### Initializing SSL" << std::endl;
            _ssl = new Pt::Ssl::SSLStreamBufferClient(_socket, _sslContext, 0);
            _ssl->connected              += Pt::slot(*this, &Client::_onSSLConnect            );
            _ssl->decryptedDataAvailable += Pt::slot(*this, &Client::_onDecryptedDataAvailable);

            _ssl->connect();
            std::cout << "[@@ Client-SSL  ] ################################### Status = " << _ssl->getStatusString() << std::endl;
        }

        void _onSSLConnect(Pt::Ssl::SSLStreamBuffer& ssl)
        {
            std::cout << "[@@ Client-SSL  ] ################################### Peer CN = " + _ssl->getPeerCN() << std::endl;

            sendMessage(
                "GET https://localhost/not_exist.html HTTP/1.1\r\n"
                "Host: localhost\r\n"
                "User-Agent: Platinum\r\n"
                "Accept: text/html\r\n"
                "Accept-Language: en-us\r\n"
                "Accept-Charset: utf-8\r\n"
                "Connection: close\r\n\r\n"
            );
        }

        void _onDecryptedDataAvailable(Pt::Ssl::SSLStreamBuffer& ssl)
        {
            std::string cum;
            char        buff[128];

            std::istream is(_ssl);

            int len = 0;
            do {
                len = is.readsome(buff, sizeof(buff));
                cum += std::string(buff, len);
            } while(len > 0);
            std::cout << "[@@ Client-SSL  ] ################################### Receiving message from server:" << std::endl << cum << std::endl;

            _loop.exit();
        }

    private:
        Pt::Ssl::SSLContext&            _sslContext;
        Pt::Ssl::SSLStreamBufferClient* _ssl;
        Pt::System::EventLoop&          _loop;
        Pt::Net::TcpSocket              _socket;
        int                             _msgCnt;
};

int main(int argc, char** argv)
{
    try {
        std::cout << "[@@ main() @@@  ] ################################### OpenSSL test progam started" << std::endl;

        Pt::System::MainLoop loop;
        std::string          addr("127.0.0.1");
        unsigned short       port = 443;

        Pt::Ssl::SSLContext clientContext("root.pem", "client.pem", "password", 0);

        Client client(loop, addr, port, clientContext);

        loop.setIdleTimeout(2000);
        loop.timeout += Pt::slot(loop, &Pt::System::EventLoop::exit);
        loop.run();

        std::cout << "[@@ main() @@@  ] ################################### OpenSSL test progam ended" << std::endl;
        return 0;
    }
    catch(const std::exception& ex)
    {
        std::cerr << "[@@ main() @@@  ] ################################### Error: " << ex.what() << std::endl;
    }
    catch(const char* ex)
    {
        std::cerr << "[@@ main() @@@  ] ################################### Error: " << ex << std::endl;
    }
    return 1;
}

/*
rm -f build/debug/libPt-Ssl* && ./jam.sh --with-openssl -q
*/
