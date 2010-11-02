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
        friend class SSLMemoryServer;
        friend class SSLMemoryClient;

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
{ SSL_CTX_free(_ctx); }

////////////////////////////////////////////////////////////////////////////////

class SSLMemoryConnection {
    public:
        SSLMemoryConnection(SSLContext& sslContext);
        virtual ~SSLMemoryConnection();

    protected:
        SSL* _ssl;
        BIO* _in;
        BIO* _out;

        static void processMessage(SSLMemoryConnection& src, SSLMemoryConnection& dst);
};

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
{
    SSL_free(_ssl);
    //BIO_free(_in);
    //BIO_free(_out);
}

void SSLMemoryConnection::processMessage(SSLMemoryConnection& src, SSLMemoryConnection& dst)
{
    char buffer[1024];
    int  read    = BIO_read(src._out, buffer, sizeof(buffer));
    int  written = (read > 0) ? BIO_write(dst._in, buffer, read) : -1;

    if(written > 0) {
        if(!SSL_is_init_finished(dst._ssl)) {
            SSL_do_handshake(dst._ssl);
        }
        else {
            read = SSL_read(dst._ssl, buffer, sizeof(buffer));
            cerr << string(buffer, read) << endl;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////

class SSLMemoryServer : public SSLMemoryConnection {
    public:
        SSLMemoryServer(SSLContext& sslContext);
        virtual ~SSLMemoryServer();
};

SSLMemoryServer::SSLMemoryServer(SSLContext& sslContext)
: SSLMemoryConnection(sslContext)
{
    SSL_set_accept_state(_ssl);
}

SSLMemoryServer::~SSLMemoryServer()
{
}

////////////////////////////////////////////////////////////////////////////////

class SSLMemoryClient : public SSLMemoryConnection {
    public:
        SSLMemoryClient(SSLContext& sslContext);
        virtual ~SSLMemoryClient();

        void connect(SSLMemoryServer& server);

    private:
        SSLMemoryServer* _server;
};

SSLMemoryClient::SSLMemoryClient(SSLContext& sslContext)
: SSLMemoryConnection(sslContext), _server(0)
{
    SSL_set_connect_state(_ssl);
}

SSLMemoryClient::~SSLMemoryClient()
{
}

void SSLMemoryClient::connect(SSLMemoryServer& server)
{
    _server = &server;

    SSL_do_handshake(_ssl);
    SSLMemoryConnection::processMessage(*this,    *_server);
    SSLMemoryConnection::processMessage(*_server, *this   );
    SSLMemoryConnection::processMessage(*this,    *_server);
    SSLMemoryConnection::processMessage(*_server, *this   );
}

////////////////////////////////////////////////////////////////////////////////

int main()
{
    SSLContext serverContext("server.pem", "password");
    SSLContext clientContext("client.pem", "password");

    try {
        SSLMemoryServer server(serverContext);
        SSLMemoryClient client(clientContext);

        client.connect(server);
    }
    catch(const char* msg) {
        cerr << msg << endl;
    }

    return 0;
}
