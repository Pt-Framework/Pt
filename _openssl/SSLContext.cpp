#include "SSLContext.h"

BIO* SSLContext::_bioErr = 0;

int SSLContext::_passwordCallback(char* buff, int num, int rwflag, void* userdata)
{
    // Get the SSLContext instance
    SSLContext& sslCtx = *reinterpret_cast<SSLContext*>(userdata);

    // If the wanted length is not the same with the given password length, just return 0
    if(num < sslCtx._pswd.length() + 1) return 0;

    // Copy the password to the buffer and return the length
    strcpy(buff, &sslCtx._pswd[0]);
    return sslCtx._pswd.length();
}

SSLContext::SSLContext(const char* caFile, const char* keyFile, const char* password)
: _pswd(password ? password : "")
{
    // Only need to perform these once for every application
    if(!SSLContext::_bioErr) {
        SSL_library_init();
        SSL_load_error_strings();
        SSLContext::_bioErr = BIO_new_fp(stderr, BIO_NOCLOSE);
    }

    // Create a new SSL context that supports SSL version 2  and 3
    _ctx = SSL_CTX_new(SSLv23_method());

    // If a certificate-key file is available, load it
    if(keyFile) {
        // Load the certificate chain
        if(!SSL_CTX_use_certificate_chain_file(_ctx, keyFile)) throw "Could not read certificate file!";
        // Load the private key
        SSL_CTX_set_default_passwd_cb(_ctx, _passwordCallback);
        SSL_CTX_set_default_passwd_cb_userdata(_ctx, this);
        if(!SSL_CTX_use_PrivateKey_file(_ctx, keyFile, SSL_FILETYPE_PEM)) throw "Could not read key file!";
    }

    // Load and verify CA list (if given)
    if(caFile && !SSL_CTX_load_verify_locations(_ctx, caFile, 0)) throw "Could not read CA list!";
#if (OPENSSL_VERSION_NUMBER < 0x00905100L)
    SSL_CTX_set_verify_depth(_ctx,1);
#endif
}

SSLContext::~SSLContext()
{ SSL_CTX_free(_ctx); }
