#ifndef SSL_CONTEXT_H
#define SSL_CONTEXT_H

#include <string>
#include <openssl/ssl.h>

//! \brief SSL context.
class SSLContext {
    public:
        //! \brief Construct an SSL context that uses the given certificate-key file and password.
        SSLContext(const char* caFile, const char* keyFile, const char* password);

        //! \brief Standard dtor.
        ~SSLContext();

        friend class SSLConnector;

    private:
        SSL_CTX*    _ctx;    // OpenSSL's SSL context
        std::string _pswd;   // The password
        static BIO* _bioErr; // Error BIO for OpenSSL

        // Password callback to feed the password to OpenSSL
        static int _passwordCallback(char* buf, int num, int rwflag, void* userdata);
};

#endif
