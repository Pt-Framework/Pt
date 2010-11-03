#include <iostream>
using namespace std;

#include "SSLMemoryServer.h"
#include "SSLMemoryClient.h"

class MySSLMemoryServer : public SSLMemoryServer {
    public:
        MySSLMemoryServer(SSLContext& sslContext)
        : SSLMemoryServer(sslContext)
        {}

        virtual ~MySSLMemoryServer()
        {}

        virtual void onRecvData(const char* buff, int len)
        { cerr << "[SERVER] " + string(buff, len) << endl; }
};

class MySSLMemoryClient : public SSLMemoryClient {
    public:
        MySSLMemoryClient(SSLContext& sslContext)
        : SSLMemoryClient(sslContext)
        {}

        virtual ~MySSLMemoryClient()
        {}

        virtual void onRecvData(const char* buff, int len)
        { cerr << "[CLIENT] " + string(buff, len) << endl; }
};

int main()
{
    SSLContext serverContext("server.pem", "password");
    SSLContext clientContext("client.pem", "password");

    try {
        MySSLMemoryServer server(serverContext);
        MySSLMemoryClient client(clientContext);

        cerr << "[MAIN()] " << server.getStatusString() << endl;
        cerr << "[MAIN()] " << client.getStatusString() << endl;

        client.connect(server);
        cerr << "[MAIN()] " << server.getStatusString() << endl;
        cerr << "[MAIN()] " << client.getStatusString() << endl;

        client.write("Hello world!", 12);
        server.write("Hello world!", 12);
    }
    catch(const char* msg) {
        cerr << msg << endl;
    }

    return 0;
}
