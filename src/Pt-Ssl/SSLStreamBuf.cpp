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

#include <Pt/SourceInfo.h>
#include <iostream>

#include "SSLStreamBuf.h"

namespace Pt {
namespace Ssl {

///// JUST FOR TESTING /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define SSL_CALL_INFO SSLStreamBuf::_call_info("SSLStreamBuf", PT_FUNCTION)
const std::string SSLStreamBuf::_call_info(const char* className, const std::string& funcName)
{
    static int count = 0;

    size_t      a = funcName.find_first_of("(");
    std::string f = (a == std::string::npos) ? funcName : funcName.substr(0, a);
    a = f.find_last_of("::");
    if(a != std::string::npos) f = f.substr(a + 1);
    a = f.find_last_of(" ");
    if(a != std::string::npos) f = f.substr(a + 1);

    char buff[1024];
    sprintf(buff, "[%s] %06d [%22s] ", className, count++, f.c_str());

    return buff;
}
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
  _pbmax(4)
{
    // Create the SSL objects
    _in  = BIO_new( BIO_s_mem() );
    _out = BIO_new (BIO_s_mem() );
    _ssl = SSL_new( ctx._ctx );

    // Connect the BIO
    BIO_set_nbio(_in, 1);
    BIO_set_nbio(_out, 1);
    SSL_set_bio(_ssl, _in, _out);
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

void SSLStreamBuf::disconnect()
{
    SSL_shutdown(_ssl);

    while(BIO_pending(_out) > 0)
    {
        char buff[100];
        const int n = BIO_read(_out, buff, sizeof(buff) );
        std::cerr << SSL_CALL_INFO << "BIO_read = " << n << std::endl;

        if(n <= 0)
            throw std::runtime_error("BIO_read failed");

        _ios->write(buff, n);

        const int ret = SSL_do_handshake(_ssl);
        std::cerr << SSL_CALL_INFO << "SSL_do_handshake = " << ret << std::endl;

        if(ret <= 0)
        {
            const int sslerr = SSL_get_error(_ssl, ret);
            if(sslerr != SSL_ERROR_WANT_READ && sslerr != SSL_ERROR_WANT_WRITE)
                throw std::runtime_error("SSL_do_handshake failed");
        }
    }
}

void SSLStreamBuf::reset()
{
    BIO_reset(_in);
    BIO_reset(_out);
    SSL_clear(_ssl);
}

void SSLStreamBuf::beginServerHandshake()
{
    SSL_set_accept_state(_ssl);
}

void SSLStreamBuf::beginClientHandshake()
{
    SSL_set_connect_state(_ssl);
    this->writeHandshake();
}

bool SSLStreamBuf::writeHandshake()
{
    std::cerr << SSL_CALL_INFO << "getStatusString = " << getStatusString() << std::endl;

    if(!SSL_want_read(_ssl))
    {
        int ret = SSL_do_handshake(_ssl);
        std::cerr << SSL_CALL_INFO << "SSL_do_handshake = " << ret << std::endl;

        if(ret <= 0)
        {
            int sslerr = SSL_get_error(_ssl, ret);
            if( sslerr == SSL_ERROR_WANT_READ )
                std::cerr << SSL_CALL_INFO << "SSL_ERROR_WANT_READ" << std::endl;
            else if ( sslerr == SSL_ERROR_WANT_WRITE)
                std::cerr << SSL_CALL_INFO << "SSL_ERROR_WANT_WRITE" << std::endl;
            else
                throw std::runtime_error("SSL_do_handshake failed");
        }
    }

    if(BIO_pending(_out))
    {
        char buff[1000]; // Will be the steambufs buffer area later
        const int n = BIO_read(_out, buff, sizeof(buff));
        std::cerr << SSL_CALL_INFO << "BIO_read = " << n << std::endl;

        if(n <= 0)
            throw std::runtime_error("BIO_read failed");

        _ios->write(buff, n);

        return true;
    }

    return false;
}

bool SSLStreamBuf::readHandshake()
{
    std::cerr << SSL_CALL_INFO << "getStatusString = " << getStatusString() << std::endl;

    char buf[1000]; // Will be the steambufs buffer area later

    // Block until data can be read from the stream
    _ios->rdbuf()->sgetc();

    while(true)
    {
        unsigned n = _ios->readsome(buf, sizeof(buf));
        std::cerr << SSL_CALL_INFO << "readsome = " << n << std::endl;

        if(n == 0)
            break;

        while(n)
        {
            const int written = BIO_write(_in, buf, n);
            std::cerr << SSL_CALL_INFO << "BIO_write = " << written << std::endl;

            if(written <= 0)
                throw std::runtime_error("BIO_write failed");

            n -= written;
            if(n > 0)
                std::memcpy(buf, buf + written, n);

            int ret = SSL_do_handshake(_ssl);
            if( ret <= 0 )
            {
                int sslerr = SSL_get_error(_ssl, ret);
                if( sslerr != SSL_ERROR_WANT_READ && sslerr != SSL_ERROR_WANT_WRITE)
                    throw std::runtime_error("SSL_do_handshake failed");
            }
        }
    }

    if( BIO_pending(_out) > 0 || SSL_get_state(_ssl) == SSL_ST_OK )
        return false;

    return true;
}

std::streamsize SSLStreamBuf::import()
{
    std::cerr << SSL_CALL_INFO << "in_avail = " << this->in_avail() << std::endl;

    if( _ios )
    {
        std::streamsize n = _ios->rdbuf()->in_avail();
        return do_underflow(n);
    }

    return this->in_avail();
}


int SSLStreamBuf::sync()
{
    if( ! _ios )
        return 0;

    std::cerr << SSL_CALL_INFO << "sync; pptr = " << this->pptr() << std::endl;

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

    std::cerr << SSL_CALL_INFO << "underflow" << std::endl;

    if( this->gptr() < this->egptr() )
        return traits_type::to_int_type( *this->gptr() );

    this->do_underflow(_ibufferSize);

    if( this->gptr() < this->egptr() )
        return traits_type::to_int_type( *this->gptr() );

    return traits_type::eof();
}


std::streamsize SSLStreamBuf::do_underflow(std::streamsize size)
{
    if( ! _ios )
        return 0;

    std::cerr << SSL_CALL_INFO << "size = " << size << std::endl;

    if( ! _ibuffer )
    {
        std::cerr << SSL_CALL_INFO << "Allocating ibuffer " << std::endl;
        _ibuffer = new char[_ibufferSize];
    }

    // return 0 if full
    if( _ibuffer + _ibufferSize == this->egptr() )
    {
        return 0;
    }

    size_t putback = _pbmax;
    size_t leftover = 0;

    // Move unread bytes and putback to front
    if( this->gptr() )
    {
        putback = std::min<size_t>( this->gptr() - this->eback(), _pbmax);
        char* to = _ibuffer + _pbmax - putback;
        char* from = this->gptr() - putback;

        leftover = this->egptr() - this->gptr();
        std::memmove( to, from, putback + leftover );
    }

    // refill the BIO with encoded bytes for decoding
    BUF_MEM* bm = 0;
    BIO_get_mem_ptr(_in, &bm);
    std::cerr << SSL_CALL_INFO << "BUF_MEM, used " << bm->length << " of " << bm->max << std::endl;

    if(bm->max > bm->length)
    {
        const size_t refill = std::min<size_t>(bm->max - bm->length, size);
        _ios->read(bm->data + bm->length, refill);
        bm->length += _ios->gcount();
    }

    // We do not need to read all bytes from _ssl, but only make some progress
    size_t used = _pbmax + leftover;
    size_t avail = _ibufferSize - used;
    int readSize = SSL_read(_ssl, _ibuffer + used, _ibufferSize - used);
    std::cerr << SSL_CALL_INFO << "SSL_read " << readSize << " of " << avail << std::endl;

    this->setg( _ibuffer + (_pbmax - putback), // start of get area
                _ibuffer + _pbmax, // gptr position
                _ibuffer + used + readSize ); // end of get area

    return readSize;
}


SSLStreamBuf::int_type SSLStreamBuf::overflow(int_type ch)
{
    //
    // We are being called when _obuffer, the output buffer area of the
    // i/o stream is full, or needs to be flushed. In case of a flush,
    // the eof character is passed to overflow(). When SSLStreamBuf is
    // constructed no output buffer area exists, therefore when overflow
    // is called for the first time, we need to set it up.
    //
    if( ! _ios )
        return traits_type::eof();

    std::cerr << SSL_CALL_INFO << "ch = " << ch << std::endl;

    if( ! _obuffer )
    {
        std::cerr << SSL_CALL_INFO << "Allocating buffer " << ch << std::endl;

        _obuffer = new char[_obufferSize];
        this->setp(_obuffer, _obuffer + _obufferSize);

    }
    else if (traits_type::eq_int_type( ch, traits_type::eof() ) )
    {
        // Normal blocking overflow case
        const size_t avail = this->pptr() - _obuffer;

        // Feed _obuffer to openssl
        std::cerr << SSL_CALL_INFO << "Feeding data to be encrypted to OpenSSL" << std::endl;
        int written = SSL_write(_ssl, _obuffer, avail);

        // Move leftover in _obuffer to the front
        const size_t leftover = avail - written;
        std::cerr << SSL_CALL_INFO << "Shifting buffer; leftover = " << leftover << std::endl;
        if(leftover > 0)
        {
            traits_type::move(_obuffer, _obuffer + written, leftover);
        }
        this->setp(_obuffer, _obuffer + _obufferSize);
        this->pbump( leftover );

        // Write encoded bytes to _ios
        std::cerr << SSL_CALL_INFO << "Writing encrypted data to _ios" << std::endl;
        while(true)
        {
            BUF_MEM* bm = 0;
            BIO_get_mem_ptr(_out, &bm);
            std::cerr << SSL_CALL_INFO << "BUF_MEM, used " << bm->length << " of " << bm->max << std::endl;
            if(bm->length <= 0) break;

            _ios->write(bm->data, bm->length);
            bm->length = 0;

            /*
            char buf[255];
            const int n = BIO_read( _out, buf, sizeof(buf) );
            if(n < 0) break;
            */
        }
    }

    // If the overflow char is not EOF, so put it in buffer
    if( traits_type::eq_int_type(ch, traits_type::eof()) ==  false )
    {
        std::cerr << SSL_CALL_INFO << "ch is not EOF, putting it in buffer" << std::endl;
        *(this->pptr()) = traits_type::to_char_type(ch);
        this->pbump(1);
    }

    std::cerr << SSL_CALL_INFO << "Done" << std::endl;

    return traits_type::not_eof(ch);
}

} // namespace Pt
} // namespace Ssl
