#ifndef SSL_MEMORY_SERVER_H
#define SSL_MEMORY_SERVER_H

#include "SSLMemoryConnector.h"

// Just for testing: memory-based SSL server
class SSLMemoryServer : public SSLMemoryConnector {
    public:
        // Construct a memory-based SSL server that uses the given context
        SSLMemoryServer(SSLContext& sslContext, const char* sessionID);

        // Standard dtor
        virtual ~SSLMemoryServer();

        // Override the write() method
        virtual int write(const char* buff, int len);

        friend class SSLMemoryClient;

    private:
        SSLMemoryConnector* _client;
};

#endif
