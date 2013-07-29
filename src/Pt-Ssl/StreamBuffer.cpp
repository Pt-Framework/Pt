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

#include <Pt/Ssl/StreamBuffer.h>
#include <Pt/Ssl/SslError.h>
#include <Pt/System/Logger.h>
#include <Pt/System/IOError.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <cassert>
#include <cstring>

log_define("Pt.Ssl.StreamBuffer")

namespace Pt {

namespace Ssl {

StreamBuffer::StreamBuffer(std::streambuf& sb, size_t bufferSize)
: _in(0)
, _out(0)
, _ssl(0)
, _ios(&sb)
, _ibufferSize(bufferSize + 4)
, _ibuffer(0)
, _obufferSize(bufferSize)
, _obuffer(0)
, _pbmax(4)
{
}


StreamBuffer::StreamBuffer(Context& ctx, std::streambuf& sb, size_t bufferSize)
: _in(0)
, _out(0)
, _ssl(0)
, _ios(&sb)
, _ibufferSize(bufferSize + 4)
, _ibuffer(0)
, _obufferSize(bufferSize)
, _obuffer(0)
, _pbmax(4)
{
    this->init(ctx);
}


StreamBuffer::StreamBuffer(Context& ctx, size_t bufferSize)
: _in(0)
, _out(0)
, _ssl(0)
, _ios(0)
, _ibufferSize(bufferSize + 4)
, _ibuffer(0)
, _obufferSize(bufferSize)
, _obuffer(0)
, _pbmax(4)
{
    this->init(ctx);
}


StreamBuffer::~StreamBuffer()
{ 
    if(_ssl)
        SSL_free(_ssl); 

    delete [] _ibuffer;
    delete [] _obuffer;
}


void StreamBuffer::init(Context& ctx)
{
    if(_ssl)
    {
        SSL_free(_ssl); 
        _ssl = 0;
    }

    // Create the SSL objects
    _in  = BIO_new( BIO_s_mem() );
    _out = BIO_new( BIO_s_mem() );
    _ssl = SSL_new( ctx.impl() );

    // Connect the BIO
    BIO_set_nbio(_in, 1);
    BIO_set_nbio(_out, 1);
    SSL_set_bio(_ssl, _in, _out);
}


void StreamBuffer::discard()
{
    if( ! _ssl)
        return;

    // Reset all
    (void) BIO_reset(_in);
    (void) BIO_reset(_out);
    SSL_clear(_ssl);

    delete [] _ibuffer; _ibuffer = 0;
    delete [] _obuffer; _obuffer = 0;
}


CipherList StreamBuffer::ciphers() const
{
    if( ! _ssl )
        return CipherList();

    // TODO: possibly cache the available ciphers in the context
    STACK_OF(SSL_CIPHER)* ciphers = SSL_get_ciphers(_ssl);
    return CipherList(ciphers);
}


Cipher StreamBuffer::currentCipher() const
{
    if( ! _ssl )
        return Cipher();

    // TODO: possibly return a Cipher that has internally a reference to a 
    //       CipherData in the context cache
    const SSL_CIPHER* c = SSL_get_current_cipher(_ssl);
    return Cipher(c);
}


bool StreamBuffer::connected() const
{ 
    if( ! _ssl )
        return false;

    return SSL_get_state(_ssl) == SSL_ST_OK; 
}


const char* StreamBuffer::getStatus() const
{ 
    if( ! _ssl )
        return "uninitialized";

    return SSL_state_string_long(_ssl); 
}


std::string StreamBuffer::peerName() const
{
    if( ! _ssl )
        return std::string();

    if(SSL_get_verify_result(_ssl) != X509_V_OK) 
        return std::string();

    X509* peer = SSL_get_peer_certificate(_ssl);
    if( ! peer) 
        return std::string();

    char peerCN[256];
    int  ret = X509_NAME_get_text_by_NID(X509_get_subject_name(peer), NID_commonName, peerCN, sizeof(peerCN));
    return (ret > 0) ? peerCN : "";
}


Session StreamBuffer::session() const
{
    if( ! _ssl ) 
        return Session();

    SSL_SESSION* sess = SSL_get1_session(_ssl);
    if( ! sess) 
        return Session(); // No session available

   return Session(sess);
}


void StreamBuffer::setSession(const Session& sess)
{
    if( ! _ssl)
        return;

    SSL_SESSION* rsess = sess.impl();
    if(!rsess)
        throw SessionFailed("Invalid session data!");

    if(SSL_set_session(_ssl, rsess) == 0)
        throw SessionFailed("Could not set session!");
}


void StreamBuffer::setAccepting()
{
    if( ! _ssl)
        return;

    SSL_set_accept_state(_ssl);
}


void StreamBuffer::setConnecting()
{
    if( ! _ssl)
        return;

    SSL_set_connect_state(_ssl);
}


bool StreamBuffer::writeHandshake()
{
    log_trace("StreamBuffer::writeHandshake");
    
    if( ! _ios || ! _ssl)
        throw System::IOError("SSL Buffer not initialized");

    log_debug("getStatus() = " << getStatus());

    if( ! SSL_want_read(_ssl) )
    {
        const int ret = SSL_do_handshake(_ssl);
        log_debug("SSL_do_handshake() = " << ret);

        if(ret <= 0)
        {
            const int sslerr = SSL_get_error(_ssl, ret);
            if(sslerr != SSL_ERROR_WANT_READ && sslerr != SSL_ERROR_WANT_WRITE) 
            {
                throw HandshakeFailed("SSL handshake failed");
            }
        }
    }

    if( BIO_pending(_out) )
    {
        char buff[1000];
        const int n = BIO_read(_out, buff, sizeof(buff));
        log_debug("Wrote " << n << " bytes from _out BIO to _ios");

        if(n <= 0)
            throw System::IOError("Failed reading from OpenSSL output BIO!");

        _ios->sputn(buff, n);
        return true;
    }

    return false;
}


bool StreamBuffer::readHandshake()
{
    log_trace("StreamBuffer::readHandshake");
    
    if( ! _ios || ! _ssl)
        throw System::IOError("SSL Buffer not initialized");

    log_debug("getStatus() = " << getStatus());

    const std::streamsize bufsize = 1000;
    char buf[bufsize];

    // Block until data can be read from the stream
    _ios->sgetc();

    while(true)
    {
        std::streamsize gsize = std::min(_ios->in_avail(), bufsize);
        std::streamsize n = gsize > 0 ? _ios->sgetn(buf, gsize) : 0;
        if(n == 0) 
          break;

        while(n)
        {
            const int written = BIO_write(_in, buf, static_cast<int>(n));
            if(written <= 0)
              throw System::IOError("Failed writing to OpenSSL input BIO!");

            n -= written;
            log_debug("Wrote " << written << " bytes from _ios to _in BIO; leftover = " << n << " bytes");

            if(n > 0)
                std::memcpy(buf, buf + written, static_cast<size_t>(n));

            int ret = SSL_do_handshake(_ssl);
            if( ret <= 0 )
            {
                
                int sslerr = SSL_get_error(_ssl, ret);
                if( sslerr != SSL_ERROR_WANT_READ && sslerr != SSL_ERROR_WANT_WRITE) 
                {
                    char buf[255];
                    std::string msg = "SSL handshake failed";
                    if(sslerr == SSL_ERROR_SSL)
                    {
                        msg += ' ';
                        msg += ERR_error_string(ERR_get_error(), buf);
                    }

                    log_warn("handshake failed: " << msg);
                    throw HandshakeFailed("SSL handshake failed");
                }
            }
        }
    }

    if( BIO_pending(_out) > 0 || SSL_get_state(_ssl) == SSL_ST_OK ) 
    {
        return false;
    }

    return true;
}


StreamBuffer::HandshakeProgress StreamBuffer::handshake()
{
    if( ! _ios || ! _ssl )
        throw System::IOError("SSL Buffer not initialized");
      
    int ret = SSL_do_handshake(_ssl);
    //Sleep(1000);
    std::clog << "HANDSHAKE " << ret << std::endl;

    if(ret <= 0)
    {
        const int sslerr = SSL_get_error(_ssl, ret);

        if(sslerr != SSL_ERROR_WANT_READ && sslerr != SSL_ERROR_WANT_WRITE) 
        {
            throw HandshakeFailed("SSL handshake failed");
        }
    }

    while( BIO_pending(_out) )
    {
        char buf[1000];
        int n = BIO_read( _out, buf, sizeof(buf) );
        log_debug("Wrote " << n << " bytes from _out BIO to _ios");
        std::clog << "WRITE HANDSHAKE: " << n << std::endl;

        if(n <= 0)
            throw System::IOError("BIO_read");

        _ios->sputn(buf, n);

        if( ! BIO_pending(_out) )
            return Output;
    }
        
    if( SSL_want_read(_ssl)  )
    {
        std::clog << "WANT READ" << std::endl;

        std::streamsize avail = _ios->in_avail();
        if(avail == 0)
        {
            std::clog << "READ HANDSHAKE: break" << std::endl;
            return Input;
        }

        const std::streamsize bufsize = 2000;
        char buf[bufsize];
        std::streamsize refill = std::min( bufsize, _ios->in_avail() );
        std::clog << "IN AVAIL: " << _ios->in_avail() << std::endl;

        std::streamsize gcount = _ios->sgetn(buf, refill);
        log_debug("read " << gcount << " bytes from input stream");
        std::clog << "READ HANDSHAKE: " << gcount << std::endl;

        int n = BIO_write(_in, buf, static_cast<int>(gcount));
        if(n <= 0)
            throw System::IOError("BIO_write");

        ret = SSL_do_handshake(_ssl);
        std::clog << "HANDSHAKE " << ret << std::endl;

        if(ret <= 0)
        {
            const int sslerr = SSL_get_error(_ssl, ret);
            if(sslerr != SSL_ERROR_WANT_READ && sslerr != SSL_ERROR_WANT_WRITE) 
            {
                throw HandshakeFailed("SSL handshake failed");
            }
        }

        return Input;
    }
    
    return None;
}


void StreamBuffer::writeShutdown()
{
    if( ! _ios || ! _ssl)
        throw System::IOError("SSL Buffer not initialized");

    const int res = SSL_shutdown(_ssl);
    log_debug("SSL_shutdown() = " << res);

    // Send shutdown message to the other peer
    char buff[1000];
    const int n = BIO_read(_out, buff, sizeof(buff));
    if(n <= 0)
        throw System::IOError("Failed reading from OpenSSL ouput BIO!");

    _ios->sputn(buff, n);
    //_ios->flush();
    log_debug("Wrote " << n << " bytes from _out BIO to _ios");

    // Reset all
    (void) BIO_reset(_in);
    (void) BIO_reset(_out);
    SSL_clear(_ssl);

    delete [] _ibuffer; _ibuffer = 0;
    delete [] _obuffer; _obuffer = 0;
}


bool StreamBuffer::isShutdown() const
{
    if(_ssl)
    {
        const int shutdownState = SSL_get_shutdown(_ssl);
        if(shutdownState & SSL_RECEIVED_SHUTDOWN) 
        {
            log_debug("Received shutdown notification");
            return true;
        }
    }
    return false;
}


void StreamBuffer::import(std::streamsize n)
{
    if(_ios && _ssl)
    {
        const std::streamsize avail = n == 0 ? _ios->in_avail() : n;
        log_debug("_ios->in_avail() = " << avail << " bytes");

        if(avail >= 0 )
        {
            const std::streamsize n = do_underflow(avail);
            log_debug("do_underflow() = " << n << " bytes");
            log_debug("_ios->in_avail() = " << _ios->in_avail() << " bytes");
        }
    }
}


int StreamBuffer::sync()
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


StreamBuffer::int_type StreamBuffer::underflow()
{
    log_trace("StreamBuffer::underflow");

    if( ! _ios || ! _ssl )
        return traits_type::eof();

    if( this->gptr() < this->egptr() )
        return traits_type::to_int_type( *this->gptr() );

    while( 0 == this->do_underflow(_ibufferSize) )
    {
        if(SSL_RECEIVED_SHUTDOWN & SSL_get_shutdown(_ssl)) 
        {
            log_debug("Received shutdown notification");
            return traits_type::eof();
        }

        if( traits_type::eof() == _ios->sgetc() )
        {
            log_debug("underlying streambuf is EOF");
            return traits_type::eof();
        }
    }

    if( this->gptr() < this->egptr() )
        return traits_type::to_int_type( *this->gptr() );

    return traits_type::eof();
}


std::streamsize StreamBuffer::do_underflow(std::streamsize isize)
{
    log_trace("StreamBuffer::do_underflow");

    if(! _ios || ! _ssl ) 
        return 0;

    if(!_ibuffer) {
        log_debug("Allocating _ibuffer of size " << _ibufferSize);
        _ibuffer = new char[_ibufferSize];
    }

    // Return 0 if full
    if(_ibufferSize == (size_t)(this->egptr() - this->gptr() + _pbmax))
    {
        log_debug("_ibuffer is full");
        return 0;
    }

    // Move unread bytes and putback to front
    size_t putback  = _pbmax;
    size_t leftover = 0;
    if( this->gptr() ) 
    {
        putback = std::min<size_t>( this->gptr() - this->eback(), _pbmax);
        char* to = _ibuffer + _pbmax - putback;
        char* from = this->gptr() - putback;

        leftover = this->egptr() - this->gptr();
        std::memmove( to, from, putback + leftover );

        this->setg( _ibuffer + (_pbmax - putback),  // start of get area
                    _ibuffer + _pbmax,              // gptr position
                    _ibuffer + _pbmax + leftover ); // end of get area
    }

    // We do not need to read all bytes from _ssl, but only make some progress
    size_t used = _pbmax + leftover;
    size_t unused = _ibufferSize - used;

    log_debug("available to fill: " << unused);
    assert(unused);

    std::streamsize readSize = sslRead(_ibuffer + used, unused, isize);

    if(readSize > 0)
    {
        log_debug("place decoded data in buffer");
        this->setg( _ibuffer + (_pbmax - putback), // start of get area
                    _ibuffer + _pbmax,             // gptr position
                    _ibuffer + used + readSize );  // end of get area
    }

    return readSize;
}


std::streamsize StreamBuffer::sslRead(char* buf, size_t n, std::streamsize isize)
{
    while(true) 
    {
        // even if we could not refill the BIO, we might still get data from the SSL
        const int readSize = SSL_read(_ssl, buf, n);
        log_debug("Read " << readSize << " bytes from _ssl");
        log_debug("SSL_get_shutdown() = " << SSL_get_shutdown(_ssl));

        if(readSize > 0)
        {           
            return readSize;
        }

        long sslerr = SSL_get_error(_ssl, readSize);
        if(sslerr != SSL_ERROR_WANT_READ)
        {
            // This error may indicate that the other peer has send shutdown message
            if(sslerr == SSL_ERROR_ZERO_RETURN)
            {
                log_debug("SSL_ERROR_ZERO_RETURN");
                return 0;
            }

            log_debug("ssl error occured");
            while( ( sslerr = ERR_get_error() ) ) 
            {
                log_debug("ERR_error_string = " << ERR_error_string(sslerr, 0));
            }
            
            throw System::IOError("Failed reading decrypted data from OpenSSL!");
        }

        if(isize == 0)
            return 0;

        // Refill the BIO with encoded bytes for decoding
        BUF_MEM* bm = 0;
        BIO_get_mem_ptr(_in, &bm);

        if(bm->max == bm->length)
            continue;

        const std::streamsize refill = std::min(static_cast<std::streamsize>(bm->max - bm->length), isize);
        log_debug("get " << refill << " bytes from _ios");
        
        std::streamsize gcount = _ios->sgetn(bm->data + bm->length, refill);
        bm->length += static_cast<int>( gcount );
        log_debug("Wrote " << gcount << " bytes from _ios to _in BUF_MEM");

        isize -= gcount;
    }

    return 0;
}

StreamBuffer::int_type StreamBuffer::overflow(int_type ch)
{
    // We are being called when _obuffer, the output buffer area of the
    // i/o stream is full, or needs to be flushed. In case of a flush,
    // the eof character is passed to overflow(). When StreamBuffer is
    // constructed no output buffer area exists, therefore when overflow
    // is called for the first time, we need to set it up.
    if( ! _ios || ! _ssl )
        return traits_type::eof();

    // No buffer area etablished yet
    if( ! _obuffer ) {
        log_debug("Allocating _obuffer of size " << _obufferSize);

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
        log_debug("Wrote " << written << " bytes to _ssl");

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
            _ios->sputn(bm->data, bm->length);
            log_debug("Wrote " << bm->length << " bytes from _ios to _out BUF_MEM");
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

/*
std::vector<CipherInfo> StreamBuffer::availableCiphers() const
{
    std::vector<CipherInfo> availCiphers;

    STACK_OF(SSL_CIPHER)* chp = SSL_get_ciphers(_ssl);
    for(int i = 0; i < sk_SSL_CIPHER_num(chp); ++i)
    {
        // Skip if not valid
        const SSL_CIPHER* c = sk_SSL_CIPHER_value(chp, i);
        if( ! c->valid )
            continue;

        // Get the ID and split it
        const unsigned long id  = c->id;
        const int           id0 = (int) (  id >> 24);
        const int           id1 = (int) ( (id >> 16) & 0xFFL );
        const int           id2 = (int) ( (id >>  8) & 0xFFL );
        const int           id3 = (int) (  id        & 0xFFL );

        // Convert the ID to a readable string
        char strid[64];
        if((id & 0xFF000000L) == 0x02000000L)
            sprintf(strid, "0x%02X,0x%02X,0x%02X", id1, id2, id3);
        else if((id & 0xFF000000L) == 0x03000000L)
            sprintf(strid, "0x%02X,0x%02X", id2, id3);
        else
            sprintf(strid, "0x%02X,0x%02X,0x%02X,0x%02X", id0, id1, id2, id3);

        // Get some information
        char desc[512];
        SSL_CIPHER_description(c, desc, sizeof(desc));
        const int dlen = strlen(desc);
        if(desc[dlen - 1] == '\n')
            desc[dlen - 1] = 0;

        // Store the chiper information
        int usedBits;
        int bits = SSL_CIPHER_get_bits(c, &usedBits);
        CipherInfo cipher(id, strid, SSL_CIPHER_get_name(c), bits, usedBits, 
                          SSL_CIPHER_get_version(c), desc);
        
        availCiphers.push_back(cipher);
    }

    return availCiphers;
}*/

/*
void StreamBuffer::setCiphers(const std::vector<SSLCipherInfo>& ciphers)
{
    std::string str;
    for(size_t i = 0; i < ciphers.size(); ++i) {
        if(!str.empty()) str += ":";
        str += ciphers[i].name;
    }

    if(!SSL_set_cipher_list(_ssl, str.c_str()))
        throw SSLRuntimeError("Failed selecting SSL ciphers!", PT_SOURCEINFO);

    _enabledCiphers = ciphers;
}
*/

} // namespace Ssl

} // namespace Pt
