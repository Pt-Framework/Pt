#include "SSLConnector.h"

SSLConnector::SSLConnector(SSLContext& sslContext)
: _in ( BIO_new(BIO_s_mem()) ),
  _out( BIO_new(BIO_s_mem()) ),
  _ssl( SSL_new(sslContext._ctx) )
{
    BIO_set_nbio(_in, 1);
    BIO_set_nbio(_out, 1);
    SSL_set_bio(_ssl, _in, _out);
    SSL_set_verify(_ssl, SSL_VERIFY_NONE, NULL);
    SSL_set_accept_state(_ssl);
}

SSLConnector::~SSLConnector()
{ SSL_free(_ssl); }

const char* SSLConnector::getStatusString() const
{ return SSL_state_string_long(_ssl); }

bool SSLConnector::connectionEstablished() const
{ return SSL_get_state(_ssl) == SSL_ST_OK; }

void SSLConnector::connect()
{
    SSL_set_connect_state(_ssl);
    SSL_do_handshake(_ssl);
}

int SSLConnector::write(const char* buff, int len)
{
    int bytesWritten = SSL_write(_ssl, buff, len);

    if(bytesWritten < 0) {
        if(!SSL_want_read(_ssl)) throw "Connection error!";
        return 0;
    }

    return bytesWritten;
}

int SSLConnector::pullData(char* buff, int buffSize) const
{
    const int bytesRead = BIO_read(_out, buff, buffSize);

    if(bytesRead < 0) {
        if(!BIO_should_retry(_out)) throw "Output buffer error!";
        return 0;
    }

    return bytesRead;
}

int SSLConnector::pushData(const char* buff, int len)
{
    const int bytesWritten = BIO_write(_in, buff, len);

    if(!SSL_is_init_finished(_ssl)) {
        SSL_do_handshake(_ssl);
    }
    else {
        char rbuff[8192];
        const int bytesRead = SSL_read(_ssl, rbuff, sizeof(rbuff));
        onRecvData(rbuff, bytesRead);
    }

    return bytesWritten;
}

