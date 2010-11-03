#ifndef SSL_MEMORY_SERVER_H
#define SSL_MEMORY_SERVER_H

#include "SSLMemoryConnector.h"

class SSLMemoryServer : public SSLMemoryConnection {
    public:
        SSLMemoryServer(SSLContext& sslContext);
        virtual ~SSLMemoryServer();

        virtual void write(const char* buff, int len);

        friend class SSLMemoryClient;

    private:
        SSLMemoryConnection* _client;
};

#endif
