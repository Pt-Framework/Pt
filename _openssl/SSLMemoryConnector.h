#ifndef SSL_MEMORY_CONNECTOR_H
#define SSL_MEMORY_CONNECTOR_H

#include "SSLConnector.h"

class SSLMemoryConnection {
    public:
        SSLMemoryConnection(SSLContext& sslContext);
        virtual ~SSLMemoryConnection();

        const char* getStatusString();

        virtual void write(const char* buff, int len) = 0;
        virtual void onRecvData(const char* buff, int len) = 0;

    protected:
        SSL* _ssl;
        BIO* _in;
        BIO* _out;

        static void processMessage(SSLMemoryConnection& src, SSLMemoryConnection& dst);
};

#endif
