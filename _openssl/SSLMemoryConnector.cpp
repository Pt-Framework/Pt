#include "SSLMemoryConnector.h"

SSLMemoryConnector::SSLMemoryConnector(SSLContext& sslContext)
: SSLConnector(sslContext)
{ }

SSLMemoryConnector::~SSLMemoryConnector()
{ }

void SSLMemoryConnector::processMessage(SSLConnector& src, SSLConnector& dst)
{
    // Pull data from source
    char      buff[4096];
    const int bytesRead = src.pullData(buff, sizeof(buff));

    // Write data to destination
    if(bytesRead) dst.pushData(buff, bytesRead);
}
