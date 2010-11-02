#include <string>
#include <iostream>
using namespace std;

#include <openssl/ssl.h>

#define CA_LIST "root.pem"

class SSLContext {
    public:
        SSLContext(const char* keyfile, const char* password);
        ~SSLContext();

        friend class SSLMemoryConnection;

    private:
        SSL_CTX* _ctx;
        string   _pswd;

        static BIO* _bioErr;
        static int  _passwordCallback(char* buf,int num, int rwflag, void* userdata);
};

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
{
    SSL_CTX_free(_ctx);
}

static SSLContext serverContext("server.pem", "password");
static SSLContext clientContext("client.pem", "password");

////////////////////////////////////////////////////////////////////////////////

class SSLMemoryConnection {
    public:
        SSLMemoryConnection(SSLContext& sslContext);
        ~SSLMemoryConnection();

        int write(const char* buff, int len);

    protected:
        BIO* _in;
        BIO* _out;
        SSL* _ssl;
};

SSLMemoryConnection::SSLMemoryConnection(SSLContext& sslContext)
: _in ( BIO_new(BIO_s_mem()) ),
  _out( BIO_new(BIO_s_mem()) ),
  _ssl( SSL_new(sslContext._ctx) )
{
   SSL_set_bio(_ssl, _in, _out);

      if (SSL_in_init(_ssl))
      {
         cerr << SSL_state_string_long(_ssl) << endl;;
      }
}

SSLMemoryConnection::~SSLMemoryConnection()
{
}

int SSLMemoryConnection::write(const char* sbuff, int len)
{
    while(len > 0) {
        //
        int srcBytesWritten = SSL_write(_ssl, sbuff, len);
        if(srcBytesWritten < 0)
            throw "Could not write!";
        else
            len -= srcBytesWritten;
        //
        while(BIO_ctrl_pending(_out) > 0) {
            char dbuff[4096];
            int  bytesToSend = BIO_read(_out, dbuff, sizeof(dbuff));
            if(bytesToSend <= 0) {
                if(!BIO_should_retry(_out)) throw "Output buffer error!";
            }
            else {
                cerr << string(dbuff, bytesToSend) << endl;
            }
        }
    }
}

////////////////////////////////////////////////////////////////////////////////

int main()
{
    try {
        SSLMemoryConnection memClient(clientContext);
      //  memClient.write("Hello world!", 12);
    }
    catch(const char* msg) {
        cerr << msg << endl;
    }

    return 0;
}
