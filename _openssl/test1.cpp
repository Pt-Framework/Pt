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
        virtual ~SSLMemoryConnection();

        void connect();
        void feedData(const char* buff, int len);

        const char* getStateString();

        virtual void onSendData(const char* buff, int len) = 0;
        virtual void onRecvData(const char* buff, int len) = 0;

    protected:
        BIO* _in;
        BIO* _out;
        SSL* _ssl;

    private:
        void _recvBinaryDataFromInputBuffer();
        void _sendBinaryDataFromOutputBuffer();

};

SSLMemoryConnection::SSLMemoryConnection(SSLContext& sslContext)
: _in ( BIO_new(BIO_s_mem()) ),
  _out( BIO_new(BIO_s_mem()) ),
  _ssl( SSL_new(sslContext._ctx) )
{
   SSL_set_bio(_ssl, _in, _out);
}

SSLMemoryConnection::~SSLMemoryConnection()
{
    SSL_free(_ssl);
    BIO_free(_in);
    BIO_free(_out);
}

void SSLMemoryConnection::connect()
{
    SSL_connect(_ssl);
    _sendBinaryDataFromOutputBuffer();
}

void SSLMemoryConnection::feedData(const char* buff, int len)
{
    while(len) {
        size_t bytesWritten = BIO_write(_in, buff, len);
        if(bytesWritten > 0)
            len -= bytesWritten;
        else
            throw "Input buffer error!";
        _recvBinaryDataFromInputBuffer();
    }
}
const char* SSLMemoryConnection::getStateString()
{
    return SSL_state_string_long(_ssl);
}

void SSLMemoryConnection::_recvBinaryDataFromInputBuffer()
{
    while(BIO_ctrl_pending(_in) > 0) {
        char buff[4096];
        int  bytesToSend = BIO_read(_in, buff, sizeof(buff));
        if(bytesToSend > 0)
            onRecvData(buff, bytesToSend);
        else if(!BIO_should_retry(_in))
            throw "Output buffer error!";
    }
}

void SSLMemoryConnection::_sendBinaryDataFromOutputBuffer()
{
    while(BIO_ctrl_pending(_out) > 0) {
        char buff[4096];
        int  bytesToSend = BIO_read(_out, buff, sizeof(buff));
        if(bytesToSend > 0)
            onSendData(buff, bytesToSend);
        else if(!BIO_should_retry(_out))
            throw "Output buffer error!";
    }
}

////////////////////////////////////////////////////////////////////////////////

class TestServer : public SSLMemoryConnection {
    public:
        TestServer()
        : SSLMemoryConnection(serverContext), _client(0)
        {}

        void setClient(SSLMemoryConnection* client);

        virtual ~TestServer()
        {}

        virtual void onSendData(const char* buff, int len);
        virtual void onRecvData(const char* buff, int len);

    private:
        SSLMemoryConnection* _client;
};

class TestClient : public SSLMemoryConnection {
    public:
        TestClient(TestServer& server)
        : SSLMemoryConnection(clientContext), _server(server)
        { _server.setClient(this); }

        virtual ~TestClient()
        {}

        virtual void onSendData(const char* buff, int len);
        virtual void onRecvData(const char* buff, int len);

    private:
        TestServer& _server;
};

void TestServer::setClient(SSLMemoryConnection* client)
{ _client = client; }

void TestServer::onSendData(const char* buff, int len)
{
    cerr << "[SERVER] onSendData(): " << getStateString() << endl;
    _client->feedData(buff, len);
}

void TestServer::onRecvData(const char* buff, int len)
{
    cerr << "[SERVER] onRecvData(): " << getStateString() << endl;
}

void TestClient::onSendData(const char* buff, int len)
{
    cerr << "[CLIENT] onSendData(): " << getStateString() << endl;
    _server.feedData(buff, len);
}

void TestClient::onRecvData(const char* buff, int len)
{
    cerr << "[CLIENT] onRecvData(): " << getStateString() << endl;
    cerr << string(buff, len) << endl;
}

int main()
{
    try {
        TestServer memServer;

        TestClient memClient(memServer);
        memClient.connect();

        //memClient.write("Hello world!", 12);
    }
    catch(const char* msg) {
        cerr << msg << endl;
    }

    return 0;
}
