#ifndef SSL_MEMORY_CONNECTOR_H
#define SSL_MEMORY_CONNECTOR_H

#include "SSLConnector.h"

// Just for testing: memory-based SSL connector
class SSLMemoryConnector : public SSLConnector {
    public:
        // Construct a memory-based SSL connector that uses the given context
        SSLMemoryConnector(SSLContext& sslContext, const char* sessionID);

        // Standard dtor
        virtual ~SSLMemoryConnector();

    protected:
        // Used for passing messages between memory-based SSL connectors
        static void processMessage(SSLConnector& src, SSLConnector& dst);
};

#endif
