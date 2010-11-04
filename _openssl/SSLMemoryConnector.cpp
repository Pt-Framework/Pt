#include "SSLMemoryConnector.h"

SSLMemoryConnector::SSLMemoryConnector(SSLContext& sslContext, const char* sessionID)
: SSLConnector(sslContext, sessionID)
{}

SSLMemoryConnector::~SSLMemoryConnector()
{}

void SSLMemoryConnector::processMessage(SSLConnector& src, SSLConnector& dst)
{
    // Pull data from source
    char      buff[8192];
    const int bytesRead = src.pullData(buff, sizeof(buff));

    // Write data to destination
    if(bytesRead > 0) {
        int bytesLeft = bytesRead;
        while(bytesLeft) {
            const int bytesWritten = dst.pushData(buff + bytesRead - bytesLeft, bytesLeft);
            if(bytesWritten > 0) bytesLeft -= bytesWritten;
        }
    }
}
