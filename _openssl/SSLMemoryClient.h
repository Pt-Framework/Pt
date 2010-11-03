#ifndef SSL_MEMORY_CLIENT_H
#define SSL_MEMORY_CLIENT_H

#include "SSLMemoryServer.h"

class SSLMemoryClient : public SSLMemoryConnection {
    public:
        SSLMemoryClient(SSLContext& sslContext);
        virtual ~SSLMemoryClient();

        void connect(SSLMemoryServer& server);

        virtual void write(const char* buff, int len);

    private:
        SSLMemoryConnection* _server;
};

#endif
