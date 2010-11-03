#include "SSLMemoryConnector.h"

SSLMemoryConnection::SSLMemoryConnection(SSLContext& sslContext)
: _in ( BIO_new(BIO_s_mem()) ),
  _out( BIO_new(BIO_s_mem()) ),
  _ssl( SSL_new(sslContext._ctx) )
{
    BIO_set_nbio(_in, 1);
    BIO_set_nbio(_out, 1);
    SSL_set_bio(_ssl, _in, _out);
    SSL_set_verify(_ssl, SSL_VERIFY_NONE, NULL);
}

SSLMemoryConnection::~SSLMemoryConnection()
{ SSL_free(_ssl); }

const char* SSLMemoryConnection::getStatusString()
{ return SSL_state_string_long(_ssl); }

void SSLMemoryConnection::processMessage(SSLMemoryConnection& src, SSLMemoryConnection& dst)
{
    char buff[4096];
    int  read    = BIO_read(src._out, buff, sizeof(buff));
    int  written = (read > 0) ? BIO_write(dst._in, buff, read) : -1;

    if(written > 0) {
        if(!SSL_is_init_finished(dst._ssl)) {
            SSL_do_handshake(dst._ssl);
        }
        else {
            read = SSL_read(dst._ssl, buff, sizeof(buff));
            dst.onRecvData(buff, read);
        }
    }
}
