/*
 * Copyright (C) 2010-2010 by Marc Boris Duerner
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

#include <Pt/Ssl/SSLStreamBuf.h>

namespace Pt {
namespace Ssl {

///// Logger for Pt-SSL ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
log_define("Pt.SSL.Logger");
#define PT_SSL_LOG(CODE) log_info(SSLContext::pt_ssl_gen_call_info("SSLStreamBuf", PT_FUNCTION) << CODE)
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

SSLStreamBuf::SSLStreamBuf(std::iostream& ios, SSLContext& ctx, const char* sessionID, size_t bufferSize)
: _in (0),
  _out(0),
  _ssl(0),
  _ios(&ios),
  _ibufferSize(bufferSize+4),
  _ibuffer(0),
  _obufferSize(bufferSize),
  _obuffer(0),
  _pbmax(4),
  _handshakeError(false)
{
    // Create the SSL objects
    _in  = BIO_new( BIO_s_mem() );
    _out = BIO_new (BIO_s_mem() );
    _ssl = SSL_new( ctx._ctx );

    // Connect the BIO
    BIO_set_nbio(_in, 1);
    BIO_set_nbio(_out, 1);
    SSL_set_bio(_ssl, _in, _out);

    // By default we do not care about the other peer's certificate
    SSL_set_verify(_ssl, SSL_VERIFY_NONE, NULL);

    // Set session ID
    if(sessionID)
        SSL_set_session_id_context(_ssl, reinterpret_cast<const unsigned char*>(sessionID), strlen(sessionID));
}

SSLStreamBuf::~SSLStreamBuf()
{ SSL_free(_ssl); }

bool SSLStreamBuf::connected() const
{ return SSL_get_state(_ssl) == SSL_ST_OK; }

const char* SSLStreamBuf::getStatusString() const
{ return SSL_state_string_long(_ssl); }

const std::string SSLStreamBuf::getPeerCN() const
{
    if(SSL_get_verify_result(_ssl) != X509_V_OK) return "";

    X509* peer = SSL_get_peer_certificate(_ssl);
    if(!peer) return "";

    char peerCN[256];
    int  ret = X509_NAME_get_text_by_NID(X509_get_subject_name(peer), NID_commonName, peerCN, sizeof(peerCN));
    return (ret > 0) ? peerCN : "";
}

void SSLStreamBuf::beginServerHandshake(bool verifyClientCert, bool requireCertBasedAuth)
{
    if(verifyClientCert) {
        if(requireCertBasedAuth) SSL_set_verify(_ssl, SSL_VERIFY_PEER | SSL_VERIFY_FAIL_IF_NO_PEER_CERT, NULL);
        else                     SSL_set_verify(_ssl, SSL_VERIFY_PEER, NULL);
    }

    SSL_set_accept_state(_ssl);
}

void SSLStreamBuf::beginClientHandshake(bool verifyServerCert)
{
    if(verifyServerCert) SSL_set_verify(_ssl, SSL_VERIFY_PEER, NULL);

    SSL_set_connect_state(_ssl);
    this->writeHandshake();
}

bool SSLStreamBuf::writeHandshake()
{
    PT_SSL_LOG("getStatusString() = " << getStatusString());

    if(!SSL_want_read(_ssl))
    {
        const int ret = SSL_do_handshake(_ssl);
        PT_SSL_LOG("SSL_do_handshake() = " << ret);

        if(ret <= 0)
        {
            const int sslerr = SSL_get_error(_ssl, ret);
            if(sslerr != SSL_ERROR_WANT_READ && sslerr != SSL_ERROR_WANT_WRITE) {
                _handshakeError = true;
                return false;
            }
        }
    }

    if(BIO_pending(_out))
    {
        char buff[1000];
        const int n = BIO_read(_out, buff, sizeof(buff));

        if(n <= 0)
            throw SSLRuntimeError("Failed reading from OpenSSL output BIO!", PT_SOURCEINFO);

        PT_SSL_LOG("Wrote " << n << " bytes from _out BIO to _ios");
        _ios->write(buff, n);

        return true;
    }

    return false;
}

bool SSLStreamBuf::readHandshake()
{
    PT_SSL_LOG("getStatusString() = " << getStatusString());

    char buf[1000];

    // Block until data can be read from the stream
    _ios->rdbuf()->sgetc();

    while(true)
    {
        unsigned n = _ios->readsome(buf, sizeof(buf));
        if(n == 0) break;

        while(n)
        {
            const int written = BIO_write(_in, buf, n);
            if(written <= 0)
            throw SSLRuntimeError("Failed writing to OpenSSL input BIO!", PT_SOURCEINFO);
            
            n -= written;
            PT_SSL_LOG("Wrote " << written << " bytes from _ios to _in BIO; leftover = " << n << " bytes");

            if(n > 0)
                std::memcpy(buf, buf + written, n);

            int ret = SSL_do_handshake(_ssl);
            if( ret <= 0 )
            {
                int sslerr = SSL_get_error(_ssl, ret);
                if( sslerr != SSL_ERROR_WANT_READ && sslerr != SSL_ERROR_WANT_WRITE) {
                    _handshakeError = true;
                    return false;
                }
            }
        }
    }

    if( BIO_pending(_out) > 0 || SSL_get_state(_ssl) == SSL_ST_OK )
        return false;

    return true;
}

std::streamsize SSLStreamBuf::import()
{
    if(_ios)
    {
        const std::streamsize avail = _ios->rdbuf()->in_avail();
        PT_SSL_LOG("_ios->rdbuf()->in_avail() = " << avail << " bytes");

        if(avail >= 0 )
        {
            const int n = do_underflow(avail);
            PT_SSL_LOG("do_underflow() = " << n << " bytes");
            PT_SSL_LOG("_ios->rdbuf()->in_avail() = " << _ios->rdbuf()->in_avail() << " bytes");
            
            // Shutdown?
            const int shutdownState = SSL_get_shutdown(_ssl);
            if(shutdownState & SSL_RECEIVED_SHUTDOWN) {
                PT_SSL_LOG("Received shutdown notification");
                this->shutdown();
                return -1;
            }
        }
    }

    return this->in_avail();
}

void SSLStreamBuf::shutdown()
{
    const int res = SSL_shutdown(_ssl);
    PT_SSL_LOG("SSL_shutdown() = " << res);

    // Send shutdown message to the other peer
    char buff[1000];
    const int n = BIO_read(_out, buff, sizeof(buff));
    if(n <= 0)
        throw SSLRuntimeError("Failed reading from OpenSSL ouput BIO!", PT_SOURCEINFO);
    
    _ios->write(buff, n);
    _ios->flush();
    PT_SSL_LOG("Wrote " << n << " bytes from _out BIO to _ios");

    // Reset all
    (void) BIO_reset(_in);
    (void) BIO_reset(_out);
    SSL_clear(_ssl);

    delete [] _ibuffer; _ibuffer = 0;
    delete [] _obuffer; _obuffer = 0;

    _handshakeError = false;
}

int SSLStreamBuf::sync()
{
    if( ! _ios ) return 0;

    if( this->pptr() )
    {
        while( this->pptr() > this->pbase() )
        {
            const int_type ch = this->overflow( traits_type::eof() );
            if( ch == traits_type::eof() )
            {
                return -1;
            }
        }
    }

    return 0;
}

SSLStreamBuf::int_type SSLStreamBuf::underflow()
{
    if( ! _ios )
        return traits_type::eof();

    if( this->gptr() < this->egptr() )
        return traits_type::to_int_type( *this->gptr() );

    this->do_underflow(_ibufferSize);

    if( this->gptr() < this->egptr() )
        return traits_type::to_int_type( *this->gptr() );

    return traits_type::eof();
}

std::streamsize SSLStreamBuf::do_underflow(std::streamsize isize)
{
    if(! _ios) return 0;

    if(!_ibuffer) {
        PT_SSL_LOG("Allocating _ibuffer of size " << _ibufferSize);
        _ibuffer = new char[_ibufferSize];
    }

    // Return 0 if full
    if(_ibufferSize == (size_t)(this->egptr() - this->gptr()))
    {
        PT_SSL_LOG("_ibuffer is full");
        return 0;
    }

    // Move unread bytes and putback to front
    size_t putback  = _pbmax;
    size_t leftover = 0;
    if( this->gptr() ) {
        putback = std::min<size_t>( this->gptr() - this->eback(), _pbmax);
        char* to = _ibuffer + _pbmax - putback;
        char* from = this->gptr() - putback;

        leftover = this->egptr() - this->gptr();
        std::memmove( to, from, putback + leftover );

        this->setg( _ibuffer + (_pbmax - putback),  // start of get area
                    _ibuffer + _pbmax,              // gptr position
                    _ibuffer + _pbmax + leftover ); // end of get area
    }

    BUF_MEM* bm = 0;
    while(true) {
        // Refill the BIO with encoded bytes for decoding
        BIO_get_mem_ptr(_in, &bm);
        if(bm->max > bm->length && isize > 0) {
            const size_t avail  = std::min<size_t>(_ios->rdbuf()->in_avail(), isize);
            const size_t refill = std::min<size_t>(bm->max - bm->length, avail);
            isize -= refill;

            _ios->read(bm->data + bm->length, refill);
            if(_ios->gcount() > 0) bm->length += _ios->gcount();

            PT_SSL_LOG("Wrote " << _ios->gcount() << " bytes from _ios to _in BUF_MEM");
        }
        if(bm->length == 0) return 0;

        // We do not need to read all bytes from _ssl, but only make some progress
        size_t used = _pbmax + leftover;
        size_t avail = _ibufferSize - used;

        if( ! avail )
            break;

        const int readSize = SSL_read(_ssl, _ibuffer + used, avail);
        PT_SSL_LOG("Read " << readSize << " bytes from _ssl");
        PT_SSL_LOG("SSL_get_shutdown() = " << SSL_get_shutdown(_ssl));

        long sslerr = SSL_get_error(_ssl, readSize);
        switch(sslerr) {
            // No error - good :)
            case SSL_ERROR_NONE:

                this->setg( _ibuffer + (_pbmax - putback), // start of get area
                            _ibuffer + _pbmax,             // gptr position
                            _ibuffer + used + readSize );  // end of get area
                return readSize;

            // This error may indicate that the other peer wants re-handshaking, or, there is just not enough raw bytes to be decoded
            case SSL_ERROR_WANT_READ:
                if(readSize < 0 && isize > 0) continue;
                return 0;

            // This error may indicate that the other peer has send shutdown message
            case SSL_ERROR_ZERO_RETURN:
                return 0;

            // Opps - we got a big problem here :(
            default:
                while( ( sslerr = ERR_get_error() ) ) {
                    PT_SSL_LOG("ERR_error_string = " << ERR_error_string(sslerr, 0));
                }
                throw SSLRuntimeError("Failed reading decrypted data from OpenSSL!", PT_SOURCEINFO);
        }
    }

    return 0;
}

SSLStreamBuf::int_type SSLStreamBuf::overflow(int_type ch)
{
    // We are being called when _obuffer, the output buffer area of the
    // i/o stream is full, or needs to be flushed. In case of a flush,
    // the eof character is passed to overflow(). When SSLStreamBuf is
    // constructed no output buffer area exists, therefore when overflow
    // is called for the first time, we need to set it up.
    if( ! _ios )
        return traits_type::eof();

    // No buffer area etablished yet
    if( ! _obuffer ) {
        PT_SSL_LOG("Allocating _obuffer of size " << _obufferSize);

        _obuffer = new char[_obufferSize];
        this->setp(_obuffer, _obuffer + _obufferSize);

    }

    // Write buffer to underlying stream
    else
    {
        // Normal blocking overflow case
        const size_t avail = this->pptr() - _obuffer;

        // Feed _obuffer to openssl
        int written = SSL_write(_ssl, _obuffer, avail);
        PT_SSL_LOG("Wrote " << written << " bytes to _ssl");

        // Move leftover in _obuffer to the front
        const size_t leftover = avail - written;
        if(leftover > 0)  traits_type::move(_obuffer, _obuffer + written, leftover);
        this->setp(_obuffer, _obuffer + _obufferSize);
        this->pbump( leftover );

        // Write encoded bytes to _ios

        BUF_MEM* bm = 0;
        BIO_get_mem_ptr(_out, &bm);
        if(bm->length > 0)
        {
            _ios->write(bm->data, bm->length);
            PT_SSL_LOG("Wrote " << bm->length << " bytes from _ios to _out BUF_MEM");
            bm->length = 0;
        }
    }

    // If the overflow char is not EOF, put it in the buffer area
    if( ! traits_type::eq_int_type( ch, traits_type::eof() ) )
    {
        *(this->pptr()) = traits_type::to_char_type(ch);
        this->pbump(1);
    }

    return traits_type::not_eof(ch);
}

} // namespace Pt
} // namespace Ssl
