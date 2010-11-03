#ifndef SSL_CONTEXT_H
#define SSL_CONTEXT_H

#include <string>
#include <openssl/ssl.h>

#define CA_LIST "root.pem"

class SSLContext {
    public:
        SSLContext(const char* keyfile, const char* password);
        ~SSLContext();

        friend class SSLMemoryConnection;
        friend class SSLMemoryServer;
        friend class SSLMemoryClient;

    private:
        SSL_CTX*    _ctx;
        std::string _pswd;

        static BIO* _bioErr;
        static int  _passwordCallback(char* buf, int num, int rwflag, void* userdata);
};

#endif
