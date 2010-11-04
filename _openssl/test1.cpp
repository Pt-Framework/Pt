#include <iostream>
using namespace std;

#include "SSLMemoryServer.h"
#include "SSLMemoryClient.h"

// Our test server
class MySSLMemoryServer : public SSLMemoryServer {
    public:
        MySSLMemoryServer(SSLContext& sslContext, const char* sessionID)
        : SSLMemoryServer(sslContext, sessionID)
        {}

        virtual ~MySSLMemoryServer()
        {}

        virtual void onRecvData(const char* buff, int len) const
        { cerr << "[SERVER] " + string(buff, len) << endl; }
};

// Our test client
class MySSLMemoryClient : public SSLMemoryClient {
    public:
        MySSLMemoryClient(SSLContext& sslContext, const char* sessionID)
        : SSLMemoryClient(sslContext, sessionID)
        {}

        virtual ~MySSLMemoryClient()
        {}

        virtual void onRecvData(const char* buff, int len) const
        { cerr << "[CLIENT] " + string(buff, len) << endl; }
};

int main()
{
    try {
        cerr << "----- Contexts initialized -----" << endl;
        SSLContext serverContext("root.pem", "server.pem", "password", "session123");
        SSLContext clientContext("root.pem", "client.pem", "password", "session123");

        cerr << "----- Server & client initialized -----" << endl;
        MySSLMemoryServer server(serverContext, "session123");
        MySSLMemoryClient client(clientContext, "session123");
        cerr << "[MAIN()] Server: " << server.getStatusString() << endl;
        cerr << "[MAIN()] Client: " << client.getStatusString() << endl;

        cerr << "----- Connecting client & server -----" << endl;
        client.connect(server);
        cerr << "[MAIN()] Server: " << server.getStatusString() << endl;
        cerr << "[MAIN()] Client: " << client.getStatusString() << endl;

        cerr << "----- Sending messages -----" << endl;
        client.write("Hello world!", 12);
        server.write("Hello world!", 12);
        client.write("Hello world!", 12);
        server.write("Hello world!", 12);

        cerr << "----- Disconnecting client & server -----" << endl;
        client.disconnect();
        cerr << "[MAIN()] Server: " << server.getStatusString() << endl;
        cerr << "[MAIN()] Client: " << client.getStatusString() << endl;

        cerr << "----- Resetting client & server -----" << endl;
        server.reset();
        client.reset();

        cerr << "----- Connecting client & server -----" << endl;
        client.connect(server);
        cerr << "[MAIN()] Server: " << server.getStatusString() << endl;
        cerr << "[MAIN()] Client: " << client.getStatusString() << endl;

        cerr << "----- Sending messages -----" << endl;
        client.write("Hello world!", 12);
        server.write("Hello world!", 12);
    }
    catch(const char* msg) {
        cerr << msg << endl;
    }

    return 0;
}
