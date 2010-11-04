#include "SSLMemoryClient.h"

SSLMemoryClient::SSLMemoryClient(SSLContext& sslContext)
: SSLMemoryConnector(sslContext), _server(0)
{ }

SSLMemoryClient::~SSLMemoryClient()
{ }

void SSLMemoryClient::connect(SSLMemoryServer& server)
{
    // Connect
    SSLConnector::connect();

    // Ensure that the client and server reference each other
    server._client = this;
    _server = &server;

    // Perform message passing until the connection is established
    while(!connectionEstablished()) {
        SSLMemoryConnector::processMessage(*this,    *_server);
        SSLMemoryConnector::processMessage(*_server, *this   );
    }
}

int SSLMemoryClient::write(const char* buff, int len)
{
    const int bytesWritten = SSLConnector::write(buff, len);

    SSLMemoryConnector::processMessage(*this, *_server);

    return bytesWritten;
}

