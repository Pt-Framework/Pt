#include "SSLMemoryServer.h"

SSLMemoryServer::SSLMemoryServer(SSLContext& sslContext)
: SSLMemoryConnector(sslContext), _client(0)
{ SSL_set_accept_state(_ssl); }

SSLMemoryServer::~SSLMemoryServer()
{ }

void SSLMemoryServer::write(const char* buff, int len)
{
    SSL_write(_ssl, buff, len);
    SSLMemoryConnector::processMessage(*this, *_client);
}
