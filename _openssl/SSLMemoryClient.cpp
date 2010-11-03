#include "SSLMemoryClient.h"

SSLMemoryClient::SSLMemoryClient(SSLContext& sslContext)
: SSLMemoryConnection(sslContext), _server(0)
{ SSL_set_connect_state(_ssl); }

SSLMemoryClient::~SSLMemoryClient()
{ }

void SSLMemoryClient::connect(SSLMemoryServer& server)
{
    server._client = this;
    _server = &server;

    SSL_do_handshake(_ssl);

    while(SSL_get_state(_ssl) != SSL_ST_OK) {
        SSLMemoryConnection::processMessage(*this,    *_server);
        SSLMemoryConnection::processMessage(*_server, *this   );
    }
}

void SSLMemoryClient::write(const char* buff, int len)
{
    SSL_write(_ssl, buff, len);
    SSLMemoryConnection::processMessage(*this, *_server);
}

