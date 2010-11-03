#ifndef SSL_MEMORY_CONNECTOR_H
#define SSL_MEMORY_CONNECTOR_H

#include "SSLConnector.h"

class SSLMemoryConnector : public SSLConnector {
    public:
        SSLMemoryConnector(SSLContext& sslContext);
        virtual ~SSLMemoryConnector();

    protected:
};

#endif
