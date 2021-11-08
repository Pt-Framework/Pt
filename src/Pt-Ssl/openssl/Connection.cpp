/*
 * Copyright (C) 2010-2012 by Marc Boris Duerner
 * Copyright (C) 2010-2010 by Aloysius Indrayanto
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * As a special exception, you may use this file as part of a free
 * software library without restriction. Specifically, if other files
 * instantiate templates or use macros or inline functions from this
 * file, or you compile this file and link it with other files to
 * produce an executable, this file does not by itself cause the
 * resulting executable to be covered by the GNU General Public
 * License. This exception does not however invalidate any other
 * reasons why the executable file might be covered by the GNU Library
 * General Public License.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "Connection.h"
#include "ContextImpl.h"
#include "OpenSsl.h"
#include <Pt/Ssl/StreamBuffer.h>
#include <Pt/Ssl/SslError.h>
#include <Pt/System/Logger.h>
#include <algorithm>
#include <cassert>

PT_LOG_DEFINE("Pt.Ssl.StreamBuffer")

namespace {

#if OPENSSL_VERSION_NUMBER < 0x10100000L

int BIO_get_new_index()
{
    return 0;
}


BIO_METHOD* BIO_meth_new(int type, const char* name)
{
    BIO_METHOD* m = new BIO_METHOD; 
    *m = {
	      type, name,
	      NULL,
	      NULL,
	      NULL,
	      NULL,
	      NULL,
	      NULL,
	      NULL,
	      NULL
    };

    return m;
}


void BIO_meth_free(BIO_METHOD* m)
{
    delete m;
}


void BIO_meth_set_create(BIO_METHOD* method, int (*create)(BIO *))
{
  method->create = create;
}


void BIO_meth_set_destroy(BIO_METHOD* method, int (*destroy)(BIO *))
{
  method->destroy = destroy;
}


void BIO_meth_set_ctrl(BIO_METHOD* method, long (*ctrl)(BIO *, int, long, void *))
{
  method->ctrl = ctrl;
}


void BIO_meth_set_write(BIO_METHOD* method, int (*bwrite)(BIO *, const char *, int))
{
  method->bwrite = bwrite;
}


void BIO_meth_set_read(BIO_METHOD* method, int (*bread)(BIO *, char *, int))
{
  method->bread = bread;
}


void BIO_set_data(BIO* bio, void *ptr)
{
    bio->ptr = ptr;
}


void* BIO_get_data(BIO* bio)
{
    return bio->ptr;
}


void BIO_set_init(BIO* bio, int init)
{
    bio->init = init;
}


int BIO_get_init(BIO* bio)
{
    return bio->init;
}


void BIO_set_shutdown(BIO* bio, int shut)
{
    bio->shutdown = shut;
}


int BIO_get_shutdown(BIO* bio)
{
    return bio->shutdown;
}

#endif

} // namespace

namespace Pt {

namespace Ssl {


int Connection::bio_create(BIO* bio)
{
    BIO_set_init(bio, 1);
    return 1;
}


int Connection::bio_destroy(BIO* bio)
{
    if(bio == NULL) 
        return 0;

    return 1;
}


long Connection::bio_ctrl(BIO *bio, int cmd, long argn, void* argv)
{
    void* ptr = BIO_get_data(bio);
    return static_cast<Connection*>(ptr)->bioCtrl(bio, cmd, argn, argv);
}


int Connection::bio_read(BIO* bio, char *buf, int len)
{
    void* ptr = BIO_get_data(bio);
    return static_cast<Connection*>(ptr)->bioRead(buf, len);
}


int Connection::bio_write(BIO* bio, const char *buf, int len)
{
    void* ptr = BIO_get_data(bio);
    return static_cast<Connection*>(ptr)->bioWrite(buf, len);
}


Connection::ReleaseMethod::~ReleaseMethod()
{
    if(Connection::_method)
    {
        BIO_meth_free(Connection::_method);
        Connection::_method = 0;
    }
}


Connection::ReleaseMethod Connection::_releaseMethod;


BIO_METHOD* Connection::createMethod()
{
    int index = BIO_get_new_index();
    BIO_METHOD* method = BIO_meth_new(index|BIO_TYPE_SOURCE_SINK, "pt-ssl");

    BIO_meth_set_create(method, &Connection::bio_create);
    BIO_meth_set_destroy(method, &Connection::bio_destroy);
    BIO_meth_set_ctrl(method, &Connection::bio_ctrl);
    BIO_meth_set_write(method, &Connection::bio_write);
    BIO_meth_set_read(method, &Connection::bio_read);

    return method;
}


BIO_METHOD* Connection::_method = createMethod();


Connection::Connection(Context& ctx, std::ios& ios, OpenMode omode)
: _ios(&ios)
, _in(0)
, _out(0)
, _ssl(0)
, _connected(false)
, _isReading(false)
, _maxImport(0)
, _isWriting(false)
{
    _in  = BIO_new(_method);
    BIO_set_data(_in, this);
    BIO_set_nbio(_in, 1);

    _out = BIO_new(_method);
    BIO_set_data(_out, this);
    BIO_set_nbio(_out, 1);

    _ssl = SSL_new( ctx.impl()->ctx() );
    SSL_set_bio(_ssl, _in, _out);

    if(omode == Accept)
        SSL_set_accept_state(_ssl);
    else
        SSL_set_connect_state(_ssl);

    assert(_ssl);
}


Connection::~Connection()
{
    if(_ssl)
        SSL_free(_ssl);
}


void Connection::setPeerName(const std::string& peerName)
{
    _peerName = peerName;
}


void Connection::verifyPeerName()
{
    if( _peerName.empty() )
        return;

    if( ! _ssl )
        throw HandshakeFailed("SSL handshake failed");

    X509AutoPtr peer( SSL_get_peer_certificate(_ssl) );
    if( ! peer) 
        throw HandshakeFailed("Invalid peer name");

    char peerCN[256];
    int ret = X509_NAME_get_text_by_NID(X509_get_subject_name( peer.get() ), 
                                        NID_commonName, 
                                        peerCN, sizeof(peerCN));
    if(ret <= 0) 
        throw HandshakeFailed("Invalid peer name");

    if(_peerName != peerCN)
        throw HandshakeFailed("Invalid peer name");
}


//std::string Connection::peerName() const
//{
//    if( ! _ssl )
//        return std::string();
//
//    if(SSL_get_verify_result(_ssl) != X509_V_OK) 
//        return std::string();
//
//    X509* peer = SSL_get_peer_certificate(_ssl);
//    if( ! peer) 
//        return std::string();
//
//    char peerCN[256];
//    int  ret = X509_NAME_get_text_by_NID(X509_get_subject_name(peer), NID_commonName, peerCN, sizeof(peerCN));
//    return (ret > 0) ? peerCN : "";
//}

//CipherList Connection::ciphers() const
//{
//    if( ! _ssl )
//        return CipherList();
//
//    // TODO: possibly cache the available ciphers in the context
//    STACK_OF(SSL_CIPHER)* ciphers = SSL_get_ciphers(_ssl);
//    return CipherList(ciphers);
//}


//void Connection::setCiphers(const std::vector<SSLCipherInfo>& ciphers)
//{
//    std::string str;
//    for(std::size_t i = 0; i < ciphers.size(); ++i) {
//        if(!str.empty()) str += ":";
//        str += ciphers[i].name;
//    }
//
//    if( ! SSL_set_cipher_list(_ssl, str.c_str()))
//        throw SSLError("invalid cipher");
//
//    _enabledCiphers = ciphers;
//}

const char* Connection::currentCipher() const
{
    //char desc[512];
    //SSL_CIPHER_description(c, desc, sizeof(desc));
    //bits = SSL_CIPHER_get_bits(c, &usedBits);
    //name = SSL_CIPHER_get_name(c);
    //version = SSL_CIPHER_get_version(c);

    const SSL_CIPHER* c = SSL_get_current_cipher(_ssl);
    const char* name = SSL_CIPHER_get_name(c);
    return name;
}


bool Connection::writeHandshake()
{
    PT_LOG_TRACE("Connection::writeHandshake");

    if( SSL_want_read(_ssl) || _connected )      
        return false;

    _isWriting = true;
    _isReading = false;
    _maxImport = 0;

    int ret = SSL_do_handshake(_ssl);
    PT_LOG_DEBUG("SSL_do_handshake: " << ret);
    
    _isWriting = false;

    if(ret <= 0)
    {
        const int sslerr = SSL_get_error(_ssl, ret);
        if(sslerr != SSL_ERROR_WANT_READ && sslerr != SSL_ERROR_WANT_WRITE) 
        {
            if(sslerr == SSL_ERROR_SSL)
            {
                char buf[255];
                ERR_error_string_n(ERR_get_error(), buf, sizeof(buf));
                PT_LOG_WARN("handshake failed: " << buf);
            }
            
            throw HandshakeFailed("SSL handshake failed");
        }
    }

    if(ret == 1)
    {
        verifyPeerName();
        _connected = true;
    }

    return SSL_want_write(_ssl);   
}


bool Connection::readHandshake()
{
    PT_LOG_TRACE("Connection::readHandshake");

    if( SSL_want_write(_ssl) || _connected )
        return false;

    _isWriting = false;
    _isReading = true;
    _maxImport = 0;
    
    int ret = SSL_do_handshake(_ssl);
    PT_LOG_DEBUG("SSL_do_handshake returns " << ret);

    _isReading = false;
    _maxImport = 0;

    if( ret <= 0 )
    {
        int sslerr = SSL_get_error(_ssl, ret);
        if( sslerr != SSL_ERROR_WANT_READ && sslerr != SSL_ERROR_WANT_WRITE) 
        {
            if(sslerr == SSL_ERROR_SSL)
            {
                char buf[255];
                ERR_error_string_n(ERR_get_error(), buf, sizeof(buf));
                PT_LOG_WARN("handshake failed: " << buf);
            }

            throw HandshakeFailed("SSL handshake failed");
        }
    }

    if(ret == 1)
    {
        verifyPeerName();
        _connected = true;
    }  

    return SSL_want_read(_ssl); 
}


bool Connection::shutdown()
{
    PT_LOG_DEBUG("Connection::shutdown");

    if( ! _connected )
        return true;

    int state = SSL_get_shutdown(_ssl);
    PT_LOG_DEBUG("SSL_get_shutdown() = " << state);

    bool shutdownSent = (SSL_SENT_SHUTDOWN & state) == SSL_SENT_SHUTDOWN;
    if( ! shutdownSent )
    {
        // write shutdown notify
        PT_LOG_DEBUG("write shutdown notify");

        _isWriting = true;
        _isReading = false;
        _maxImport = 0;

        int r = SSL_shutdown(_ssl);
        PT_LOG_DEBUG("SSL_shutdown() = " << r);

        _isWriting = false;

        if(r == 1)
        {
            PT_LOG_DEBUG("shutdown complete");
            SSL_clear(_ssl);
            _connected = false;
            return true;
        }
    }

    // read shutdown notify
    PT_LOG_DEBUG("read shutdown notify");

    _isWriting = false;
    _isReading = true;
    _maxImport = 0;

    int r = SSL_shutdown(_ssl);
    PT_LOG_DEBUG("SSL_shutdown() = " << r);

    _isReading = false;
    _maxImport = 0;

    if(r == 1)
    {
        PT_LOG_DEBUG("shutdown complete");
        SSL_clear(_ssl);
        _connected = false;
        return true;
    }

    return false;
}


bool Connection::isShutdown() const
{
    int state = SSL_get_shutdown(_ssl);
    PT_LOG_DEBUG("SSL_get_shutdown() = " << state);
    
    return state != 0;
}


bool Connection::isClosed() const
{
    return ! _connected;
}


std::streamsize Connection::write(const char* buf, std::size_t n)
{
    _isWriting = true;
    _isReading = false;
    _maxImport = 0;
    
    std::streamsize written = SSL_write(_ssl, buf, n);
    PT_LOG_DEBUG("encrypted " << written << " bytes");

    _isWriting = false;

    return written;
}


std::streamsize Connection::read(char* buf, std::size_t n, std::streamsize maxImport)
{
    _isWriting = false;
    _isReading = true;
    _maxImport = maxImport;
    
    const int readSize = SSL_read(_ssl, buf, n);
    PT_LOG_DEBUG("Read " << readSize << " bytes from _ssl");
    
    _isReading = false;
    _maxImport = 0;
    
    PT_LOG_DEBUG("SSL_get_shutdown() = " << SSL_get_shutdown(_ssl));

    if(readSize >= 0)
        return readSize;

    long sslerr = SSL_get_error(_ssl, readSize);

    // happens when the peer has send the shutdown alert
    if(sslerr == SSL_ERROR_ZERO_RETURN)
    {
        PT_LOG_DEBUG("SSL_ERROR_ZERO_RETURN");
        return 0;
    }

    if(sslerr == SSL_ERROR_WANT_READ)
    {
        PT_LOG_DEBUG("SSL_ERROR_WANT_READ");
        return 0;
    }

    PT_LOG_DEBUG("ssl error occured");
    while( (sslerr = ERR_get_error()) ) 
    {
        PT_LOG_DEBUG("ERR_error_string = " << ERR_error_string(sslerr, 0));
    }
            
    throw SslError("SSL input failed");
    return 0;
}


int Connection::bioRead(char *buf, int len)
{
    std::streambuf* sb = _ios->rdbuf();

    if(_isWriting || ! sb )
    {
        BIO_set_retry_read(_in);
        return -1;
    }
    
    std::streamsize avail = sb->in_avail();
    if(avail == 0 && _maxImport == 0)
    {
        BIO_set_retry_read(_in);
        return -1;
    }

    std::streamsize n = static_cast<std::streamsize>(len);
    if(_maxImport != 0)
        n = std::min(n, _maxImport);
    else
        n = std::min(n, avail);
        
    std::streamsize r = sb->sgetn(buf, n);
    return r;
}


int Connection::bioWrite(const char *buf, int len)
{
    std::streambuf* sb = _ios->rdbuf();
    if( ! sb )
        return -1;

    if(_isReading)
    {
        BIO_set_retry_write(_out);
        return -1;
    }

    std::streamsize n = sb->sputn(buf, len);
    return n;
}


long Connection::bioCtrl(BIO *bio, int cmd, long argn, void* argv)
{
    long ret = 0;

    std::streambuf* sb = _ios->rdbuf();

    switch(cmd)
    {
        case BIO_CTRL_RESET:
            ret = 1;
            break;
        
	      case BIO_CTRL_EOF:
		        ret = _ios->eof() ? 1 : 0;
		        break;

	      case BIO_CTRL_INFO:
		        ret =  sb ? sb->in_avail() : 0;
		      break;

        case BIO_CTRL_WPENDING:
            ret = 0;
            break;

        case BIO_CTRL_PENDING:
            ret = sb ? sb->in_avail() : 0;
            break;

        case BIO_CTRL_GET_CLOSE:
            ret = BIO_get_shutdown(bio);
            break;
        
        case BIO_CTRL_SET_CLOSE:
            BIO_set_shutdown(bio, argn);
            ret = 1;
            break;
      
	      case BIO_CTRL_DUP:
	      case BIO_CTRL_FLUSH:
		        ret = 1;
		        break;

	      case BIO_CTRL_PUSH:
	      case BIO_CTRL_POP:
	      default:
		        ret = 0;
		        break;
    }

    return ret;
}

} // namespace Ssl

} // namespace Pt
