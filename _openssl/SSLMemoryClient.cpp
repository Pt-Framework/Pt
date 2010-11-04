#include "SSLMemoryClient.h"

SSLMemoryClient::SSLMemoryClient(SSLContext& sslContext)
: SSLMemoryConnector(sslContext), _server(0)
{ }

SSLMemoryClient::~SSLMemoryClient()
{ }

void SSLMemoryClient::connect(SSLMemoryServer& server)
{
    SSLConnector::connect();

    server._client = this;
    _server = &server;

    while(SSL_get_state(_ssl) != SSL_ST_OK) {
        SSLMemoryConnector::processMessage(*this,    *_server);
        SSLMemoryConnector::processMessage(*_server, *this   );
    }
}

int SSLMemoryClient::write(const char* buff, int len)
{
    SSLConnector::write(buff, len);
    SSLMemoryConnector::processMessage(*this, *_server);
}

