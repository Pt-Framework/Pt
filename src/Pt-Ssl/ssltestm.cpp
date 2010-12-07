/*
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
using namespace std;

#include "SSLMemoryServer.h"
#include "SSLMemoryClient.h"
using namespace Pt;
using namespace Pt::Ssl;

// Our test server
class MySSLMemoryServer : public SSLMemoryServer {
    public:
        MySSLMemoryServer(SSLContext& sslContext, const char* sessionID)
        : SSLMemoryServer(sslContext, sessionID)
        {
            this->decryptedDataAvailable += Pt::slot(*this, &MySSLMemoryServer::onDecryptedDataAvailable);
        }

        virtual ~MySSLMemoryServer()
        {}

        void onDecryptedDataAvailable(SSLConnector& ssl)
        {
            std::string cum;
            char        buff[128];

            int len = 0;
            do {
                len = readDecryptedData(buff, sizeof(buff));
                cum += std::string(buff, len);
            } while(len > 0);

            cerr << "[SERVER      ]                           " + cum << endl;
        }
};

// Our test client
class MySSLMemoryClient : public SSLMemoryClient {
    public:
        MySSLMemoryClient(SSLContext& sslContext, const char* sessionID)
        : SSLMemoryClient(sslContext, sessionID)
        {
            this->decryptedDataAvailable += Pt::slot(*this, &MySSLMemoryClient::onDecryptedDataAvailable);
        }

        virtual ~MySSLMemoryClient()
        {}

        void onDecryptedDataAvailable(SSLConnector& ssl)
        {
            std::string cum;
            char        buff[128];

            int len = 0;
            do {
                len = readDecryptedData(buff, sizeof(buff));
                cum += std::string(buff, len);
            } while(len > 0);

            cerr << "[CLIENT      ]                           " + cum << endl;
        }
};

int main()
{
    try {
        cerr << "----- Initializing SSL contexts -----" << endl;
        SSLContext serverContext("root.pem", "server.pem", "password", "session123");
        SSLContext clientContext("root.pem", "client.pem", "password", "session123");

        cerr << "----- Initializing SSL server and client -----" << endl;
        MySSLMemoryServer server(serverContext, "session123");
        MySSLMemoryClient client(clientContext, "session123");
        cerr << "[MAIN()      ]                           Server: " << server.getStatusString() << endl;
        cerr << "[MAIN()      ]                           Client: " << client.getStatusString() << endl;

        cerr << "----- Connecting client & server -----" << endl;
        client.connect(server);
        cerr << "[MAIN()      ]                           Server: " << server.getStatusString() << endl;
        cerr << "[MAIN()      ]                           Client: " << client.getStatusString() << endl;

        cerr << "----- Sending messages -----" << endl;
        client.write("Hello world from client!", 25);
        server.write("Hello world from server!", 25);
        client.write("Hello world from client!", 25);
        server.write("Hello world from server!", 25);

        cerr << "----- Disconnecting client & server -----" << endl;
        client.disconnect();
        cerr << "[MAIN()      ]                           Server: " << server.getStatusString() << endl;
        cerr << "[MAIN()      ]                           Client: " << client.getStatusString() << endl;

        cerr << "----- Resetting client & server -----" << endl;
        server.reset();
        client.reset();

        cerr << "----- Connecting client & server -----" << endl;
        client.connect(server);
        cerr << "[MAIN()      ]                           Server: " << server.getStatusString() << endl;
        cerr << "[MAIN()      ]                           Client: " << client.getStatusString() << endl;

        cerr << "----- Sending messages -----" << endl;
        client.write("Hello world from client!", 25);
        server.write("Hello world from server!", 25);
        client.write("Hello world from client!", 25);
        server.write("Hello world from server!", 25);
    }
    catch(const char* msg) {
        cerr << msg << endl;
    }

    return 0;
}
