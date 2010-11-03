#include "SSLMemoryConnector.h"

SSLMemoryConnector::SSLMemoryConnector(SSLContext& sslContext)
: SSLConnector(sslContext)
{ }

SSLMemoryConnector::~SSLMemoryConnector()
{ }
