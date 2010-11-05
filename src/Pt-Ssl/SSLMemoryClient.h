#ifndef SSL_MEMORY_CLIENT_H
#define SSL_MEMORY_CLIENT_H

#include "SSLMemoryServer.h"

// Just for testing: memory-based SSL client
class SSLMemoryClient : public SSLMemoryConnector {
    public:
        // Construct a memory-based SSL client that uses the given context
        SSLMemoryClient(SSLContext& sslContext, const char* sessionID);

        // Standard dtor
        virtual ~SSLMemoryClient();

        // Override the connect() method
        virtual void connect(SSLMemoryServer& server);

        // Override the disconnect() method
        virtual void disconnect();

        // Override the write() method
        virtual int write(const char* buff, int len);

    private:
        SSLMemoryServer* _server;
};

#endif
