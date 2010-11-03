#ifndef SSL_CONNECTOR_H
#define SSL_CONNECTOR_H

#include "SSLContext.h"

class SSLConnector {
    public:
        SSLConnector(SSLContext& sslContext);
        virtual ~SSLConnector();

        const char* getStatusString();

        virtual void write(const char* buff, int len) = 0;
        virtual void onRecvData(const char* buff, int len) = 0;

    protected:
        SSL* _ssl;
        BIO* _in;
        BIO* _out;

        static void processMessage(SSLConnector& src, SSLConnector& dst);
};

#endif
