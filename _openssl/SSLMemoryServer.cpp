#include "SSLMemoryServer.h"

SSLMemoryServer::SSLMemoryServer(SSLContext& sslContext)
: SSLMemoryConnector(sslContext), _client(0)
{ }

SSLMemoryServer::~SSLMemoryServer()
{ }

int SSLMemoryServer::write(const char* buff, int len)
{
    const int bytesWritten = SSLConnector::write(buff, len);

    SSLMemoryConnector::processMessage(*this, *_client);

    return bytesWritten;
}
