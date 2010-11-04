#include "SSLMemoryClient.h"

SSLMemoryClient::SSLMemoryClient(SSLContext& sslContext, const char* sessionID)
: SSLMemoryConnector(sslContext, sessionID), _server(0)
{}

SSLMemoryClient::~SSLMemoryClient()
{}

void SSLMemoryClient::connect(SSLMemoryServer& server)
{
    // Connect
    SSLConnector::connect();

    // Ensure that the client and server reference each other
    server._client = this;
    _server = &server;

    // Perform message passing until the connection is established and then pass possible remaining messages
    while(!connectionEstablished()) {
        SSLMemoryConnector::processMessage(*this,    *_server);
        SSLMemoryConnector::processMessage(*_server, *this   );
    }
    SSLMemoryConnector::processMessage(*this,    *_server);
    SSLMemoryConnector::processMessage(*_server, *this   );
}

void SSLMemoryClient::disconnect()
{
    // Disconnect
    SSLConnector::disconnect();

    // Perform message passing once
    SSLMemoryConnector::processMessage(*this,    *_server);
    SSLMemoryConnector::processMessage(*_server, *this   );

    // Ensure that the client and server no longer reference each other
    _server->_client = 0;
    _server = 0;
}

int SSLMemoryClient::write(const char* buff, int len)
{
    const int bytesWritten = SSLConnector::write(buff, len);

    SSLMemoryConnector::processMessage(*this, *_server);

    return bytesWritten;
}

