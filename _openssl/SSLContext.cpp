#include "SSLContext.h"

BIO* SSLContext::_bioErr = 0;

int SSLContext::_passwordCallback(char* buf, int num, int rwflag, void* userdata)
{
    SSLContext& sslCtx = *reinterpret_cast<SSLContext*>(userdata);

    if(num < sslCtx._pswd.length() + 1) return 0;

    strcpy(buf, &sslCtx._pswd[0]);
    return sslCtx._pswd.length();
}

SSLContext::SSLContext(const char* keyfile, const char* password)
: _pswd(password)
{
    if(!SSLContext::_bioErr) {
        SSL_library_init();
        SSL_load_error_strings();
        SSLContext::_bioErr = BIO_new_fp(stderr, BIO_NOCLOSE);
    }

    _ctx = SSL_CTX_new(SSLv23_method());

    if(!SSL_CTX_use_certificate_chain_file(_ctx, keyfile)) throw "Could not read certificate file!";

    SSL_CTX_set_default_passwd_cb(_ctx, _passwordCallback);
    SSL_CTX_set_default_passwd_cb_userdata(_ctx, this);
    if(!SSL_CTX_use_PrivateKey_file(_ctx, keyfile, SSL_FILETYPE_PEM)) throw "Could not read key file!";

    if(!SSL_CTX_load_verify_locations(_ctx, CA_LIST, 0)) throw "Could not read CA list!";

#if (OPENSSL_VERSION_NUMBER < 0x00905100L)
    SSL_CTX_set_verify_depth(_ctx,1);
#endif
}

SSLContext::~SSLContext()
{ SSL_CTX_free(_ctx); }
