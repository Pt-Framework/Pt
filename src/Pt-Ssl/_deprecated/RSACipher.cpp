/*
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

#include <Pt/Ssl/RSACipher.h>

#include "Utils.h"

namespace Pt {
namespace Ssl {

RSACipher::RSACipher(std::iostream& ios)
: BasicCipher(ios)
, _rsa(0)
, _rsaPriv(0)
, _maxChunkSize(0)
{
    setPadding(RSA_PKCS1);

    this->setg(0, 0, 0);
    this->setp(0, 0);
}

RSACipher::RSACipher(std::iostream& ios, const SSLPublicKey& pkey)
: BasicCipher(ios)
, _rsa(0)
, _rsaPriv(0)
, _maxChunkSize(0)
{
    setPadding(RSA_PKCS1);
    setPublicKey(pkey);

    this->setg(0, 0, 0);
    this->setp(0, 0);
}

RSACipher::RSACipher(std::iostream& ios, const SSLPrivateKey& pkey)
: BasicCipher(ios)
, _rsa(0)
, _rsaPriv(0)
, _maxChunkSize(0)
{
    setPadding(RSA_PKCS1);
    setPrivateKey(pkey);

    this->setg(0, 0, 0);
    this->setp(0, 0);
}

RSACipher::~RSACipher()
{
    if(_rsa) {
        RSA_free(_rsa);
        _rsa = 0;
    }

    if(_rsaPriv) {
        RSA_free(_rsaPriv);
        _rsaPriv = 0;
    }
}


void RSACipher::setPadding(PaddingMode pmode)
{
    _pmode = (pmode == RSA_PKCS1_OAEP) ? RSA_PKCS1_OAEP_PADDING : RSA_PKCS1_PADDING;

    if(_rsa)
    {
        _rsaSize      = RSA_size(_rsa);
        _maxChunkSize = (_pmode == RSA_PKCS1_OAEP_PADDING) ? (_rsaSize - 42) : (_rsaSize - 12);
    }
}


void RSACipher::setPublicKey(const SSLPublicKey& pkey)
{
    // Get the RSA key from the public key
    _rsa = EVP_PKEY_get1_RSA(pkey.impl());
    if(!_rsa)
        throw SSLRuntimeError("Could not extract the RSA key from the public key!", PT_SOURCEINFO);

    // Get the RSA and maximum chunk size
    _rsaSize      = RSA_size(_rsa);
    _maxChunkSize = (_pmode == RSA_PKCS1_OAEP_PADDING) ? (_rsaSize - 42) : (_rsaSize - 12);
}


void RSACipher::setPrivateKey(const SSLPrivateKey& pkey)
{
    // Get the RSA key from the private key
    _rsaPriv = EVP_PKEY_get1_RSA(pkey.impl());
    if(!_rsaPriv)
        throw SSLRuntimeError("Could not extract the RSA key from the private key!", PT_SOURCEINFO);

    // Get the RSA size and set the maximum chunk size to zero
    _rsaSizePriv      = RSA_size(_rsaPriv);
}


void RSACipher::finish()
{
    overflow( traits_type::eof() );
    this->setp(0, 0);
}


int RSACipher::sync()
{
    if( this->pptr() ) {
        while( this->pptr() > this->pbase() ) {
            const int_type ch = this->overflow( traits_type::eof() );
            if( ch == traits_type::eof() ) return -1;
        }
    }

    return 0;
}


RSACipher::int_type RSACipher::underflow()
{
    // No need to continue if not in data encryption mode
    if( ! _rsaPriv || this->gptr() )
        return traits_type::eof();

    if( ! this->gptr() )
    {
        // Prepare the output buffer
        _ioBuf.resize(_rsaSizePriv);

        // Reset the put and set pointers
        this->setg(&_ioBuf[0], &_ioBuf[0] + _rsaSizePriv, &_ioBuf[0] + _rsaSizePriv);

        // Resize the conversion buffer
        _cnvBuf.resize(_rsaSizePriv);
    }

    // Check if we still have anything left if the get buffer
    if( this->gptr() && this->gptr() < this->egptr() )
        return traits_type::to_int_type( *this->gptr() );

    // RSA only can decrypt a chunk with a fixed size,
    // assume EOF if we do not have it
    if(_ios->rdbuf()->in_avail() < _rsaSizePriv) return traits_type::eof();

    // Read from the attached iostream
    _ios->read(&_cnvBuf[0], _rsaSizePriv);

    // Decrypt the data
    const int dlen = RSA_private_decrypt( _rsaSizePriv,
                                          (const unsigned char*) &_cnvBuf[0],
                                          (unsigned char*) &_ioBuf[0], _rsaPriv, _pmode );
    if(dlen < 0) {
        long i = ERR_get_error();
        while(i) {
            std::cerr << ERR_error_string(i, 0) << std::endl;
            i = ERR_get_error();
        }
        throw SSLRuntimeError("Failed decrypting a string chunk!", PT_SOURCEINFO);
    }

    // Set the get pointers
    if(dlen > 0) {
        this->setg(&_ioBuf[0], &_ioBuf[0], &_ioBuf[0] + dlen);
        return traits_type::to_int_type( *this->gptr() );
    }

    // EOF
    return traits_type::eof();
}


RSACipher::int_type RSACipher::overflow(int_type ch)
{
    // No need to continue if not in data encryption mode
    if( ! _rsa || this->gptr() )
        return traits_type::eof();

    if( ! this->pptr() )
    {
        // Prepare the input buffer
        _ioBuf.resize(_maxChunkSize);

        // Set the put pointers and reset the get pointers
        this->setp(&_ioBuf[0], &_ioBuf[0] + _maxChunkSize);

        // Resize the conversion buffer
        _cnvBuf.resize(_rsaSize);
    }

    // Is there any data to be encrypted?
    const size_t avail = this->pptr() - this->pbase();

    if(avail) {
        const char* from_next = 0;
        char*       to_next   = 0;
        const int   ret       = traits_type::eq_int_type(ch, traits_type::eof())
                              ? finish(this->pbase(), this->pptr(), from_next, &_cnvBuf[0], &_cnvBuf[0] + _cnvBuf.size(), to_next)
                              : encode(this->pbase(), this->pptr(), from_next, &_cnvBuf[0], &_cnvBuf[0] + _cnvBuf.size(), to_next);
        // Check for EOF
        if(ret < 0) return traits_type::eof();
        // Write the data to the output stream
        if(ret > 0) _ios->write((const char*) &_cnvBuf[0], ret);
        // Reset the put pointers
        setp(&_ioBuf[0], &_ioBuf[0] + _maxChunkSize);
    }

    // If the overflow char is not EOF, put it in the buffer area
    if( ! traits_type::eq_int_type( ch, traits_type::eof() ) )
    {
        *(this->pptr()) = traits_type::to_char_type(ch);
        this->pbump(1);
    }

    return traits_type::not_eof(ch);
}


size_t RSACipher::blockSize() const
{ return _rsaSize; }


int RSACipher::encode(const char* from, const char* from_end, const char*& from_next, char* to, char* to_end, char*& to_next)
{
    if(_maxChunkSize)
        return do_encrypt(from, from_end, from_next, to, to_end, to_next, false);
    else
        return do_decrypt(from, from_end, from_next, to, to_end, to_next, false);
}


int RSACipher::finish(const char* from, const char* from_end, const char*& from_next, char* to, char* to_end, char*& to_next)
{
    if(_maxChunkSize)
        return do_encrypt(from, from_end, from_next, to, to_end, to_next, true);
    else
        return do_decrypt(from, from_end, from_next, to, to_end, to_next, true);
}


int RSACipher::do_encrypt(const char* from, const char* from_end, const char*& from_next, char* to, char* to_end, char*& to_next, bool finish)
{
    if( ! _rsa )
        throw SSLRuntimeError("No active encryption process!", PT_SOURCEINFO);
    if( this->gptr() )
        throw SSLRuntimeError("The cipher is currently in data decryption mode!", PT_SOURCEINFO);

    // Is there any data to be encrypted?
    const size_t inAvail = from_end - from;
    if(!inAvail) return -1; // EOF

    // Is the output area large enough?
    const size_t outAvail = to_end - to;
    if(outAvail < (size_t) _rsaSize) return 0;

    // Encrypt data
    if(finish || inAvail >= (size_t) _maxChunkSize) {
        // Encrypt the data
        const size_t readMax = std::min(inAvail, (size_t) _maxChunkSize);
        const int    dlen    = RSA_public_encrypt( readMax,
                                                   (const unsigned char*) from,
                                                   (unsigned char*) to, _rsa, _pmode );
        if(dlen < 0) {
            long i = ERR_get_error();
            while(i) {
                std::cerr << ERR_error_string(i, 0) << std::endl;
                i = ERR_get_error();
            }
            throw SSLRuntimeError("Failed encrypting a string chunk!", PT_SOURCEINFO);
        }
        // Adjust the pointers
        from_next = from + readMax;
        to_next   = to   + _rsaSize;
        // Free the RSA (only if this is the final call)
        if(finish) {
            RSA_free(_rsa);
            _rsa = 0;
        }
        // Return the number of written bytes
        return _rsaSize;
    }

    // Not enough input data
    return 0;
}

int RSACipher::do_decrypt(const char* from, const char* from_end, const char*& from_next, char* to, char* to_end, char*& to_next, bool finish)
{
    throw SSLRuntimeError("Not implemented yet!", PT_SOURCEINFO);
    return 0;
}

} // namespace Pt
} // namespace Ssl
