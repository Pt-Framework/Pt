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

#include <Pt/System/MainLoop.h>

#include "SSLConnector2.h"

/*
class Server : public Pt::Ssl::SSLSocketServer {
    public:
        Server(Pt::System::EventLoop& loop, const std::string& addr, unsigned short port, Pt::Ssl::SSLContext& sslServerContext)
        : SSLSocketServer(loop, addr, port, sslServerContext, 0)
        {
            this->decryptedDataAvailable += Pt::slot(*this, &Server::onDecryptedDataAvailable);
        }

        void onDecryptedDataAvailable(Pt::Ssl::SSLConnector& ssl)
        {
            std::string cum;
            char        buff[128];

            int len = 0;
            do {
                len = readDecryptedData(buff, sizeof(buff));
                cum += std::string(buff, len);
            } while(len > 0);

            std::cout << "[SERVER-SSL] " + cum << std::endl;
            write("Hello world from server!", 25);
        }
};

class Client : public Pt::Ssl::SSLSocketClient {
    public:
        Client(Pt::System::EventLoop& loop, const std::string& addr, unsigned short port, Pt::Ssl::SSLContext& sslClientContext)
        : SSLSocketClient(loop, addr, port, sslClientContext, 0)
        {
            this->connected              += Pt::slot(*this, &Client::onSSLConnect            );
            this->decryptedDataAvailable += Pt::slot(*this, &Client::onDecryptedDataAvailable);
        }

        void onSSLConnect(Pt::Ssl::SSLConnector& ssl)
        {
            std::cout << "[CLIENT-SSL] Peer CN = " + getPeerCN() << std::endl;
            write("Hello world from client!", 25);
        }

        void onDecryptedDataAvailable(Pt::Ssl::SSLConnector& ssl)
        {
            std::string cum;
            char        buff[128];

            int len = 0;
            do {
                len = readDecryptedData(buff, sizeof(buff));
                cum += std::string(buff, len);
            } while(len > 0);

            std::cout << "[CLIENT-SSL] " + cum << std::endl;
            write("Hello world from server!", 25);

            disconnect();
        }
};
*/
int main(int argc, char** argv)
{
    try
    {
        std::cout << "[## MAIN ##] OpenSSL test progam started..." << std::endl;

        Pt::System::MainLoop loop;
        std::string          addr("127.0.0.1");
        unsigned short       port = 8000;

        Pt::Ssl::SSLContext serverContext("root.pem", "server.pem", "password", 0);
        Pt::Ssl::SSLContext clientContext("root.pem", "client.pem", "password", 0);

        //Server server(loop, addr, port, serverContext);
        //Client client(loop, addr, port, clientContext);

        loop.setIdleTimeout(2000);
        loop.run();

        std::cout << "[## MAIN ##] OpenSSL test progam finished..." << std::endl;
        return 0;
    }
    catch(const std::exception& ex)
    {
        std::cerr << "[## MAIN ##] Error: " << ex.what() << std::endl;
    }
    catch(const char* ex)
    {
        std::cerr << "[## MAIN ##] Error: " << ex << std::endl;
    }
    return 1;
}
