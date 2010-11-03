#include "SSLMemoryServer.h"

SSLMemoryServer::SSLMemoryServer(SSLContext& sslContext)
: SSLMemoryConnection(sslContext), _client(0)
{ SSL_set_accept_state(_ssl); }

SSLMemoryServer::~SSLMemoryServer()
{ }

void SSLMemoryServer::write(const char* buff, int len)
{
    SSL_write(_ssl, buff, len);
    SSLMemoryConnection::processMessage(*this, *_client);
}
