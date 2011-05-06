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
: BasicCipher(ios), _rsa(0)
{}

RSACipher::RSACipher(std::iostream& ios, const SSLPublicKey& pkey, PaddingMode pmode)
: BasicCipher(ios), _rsa(0)
{ startEncrypt(pkey, pmode); }

RSACipher::RSACipher(std::iostream& ios, const SSLPrivateKey& pkey, PaddingMode pmode)
: BasicCipher(ios), _rsa(0)
{ startDecrypt(pkey, pmode); }

RSACipher::~RSACipher()
{
    if(_rsa) {
        RSA_free(_rsa);
        _rsa = 0;
    }
}

void RSACipher::startEncrypt(const SSLPublicKey& pkey, PaddingMode pmode)
{
    if( _rsa ) {
        if(_maxChunkSize)
            throw SSLRuntimeError("An encryption process is already active!", PT_SOURCEINFO);
        else
            throw SSLRuntimeError("A decryption process is already active!", PT_SOURCEINFO);
    }

    // Get the RSA key from the public key
    _rsa = EVP_PKEY_get1_RSA(pkey.impl());
    if(!_rsa)
        throw SSLRuntimeError("Could not extract the RSA key from the public key!", PT_SOURCEINFO);

    // Determine the padding mode
    _pmode = (pmode == RSA_PKCS1_OAEP) ? RSA_PKCS1_OAEP_PADDING : RSA_PKCS1_PADDING;

    // Get the RSA and maximum chunk size
    _rsaSize      = RSA_size(_rsa);
    _maxChunkSize = (pmode == RSA_PKCS1_OAEP) ? (_rsaSize - 42) : (_rsaSize - 12);

    // Prepare the input buffer
    _ioBuf.resize(_maxChunkSize);

    // Set the put pointers and reset the get pointers
    this->setp(&_ioBuf[0], &_ioBuf[0] + _maxChunkSize);
    this->setg(0, 0, 0);

    // Resize the conversion buffer
    _cnvBuf.resize(_rsaSize);
}

void RSACipher::startDecrypt(const SSLPrivateKey& pkey, PaddingMode pmode)
{
    if( _rsa ) {
        if(_maxChunkSize)
            throw SSLRuntimeError("An encryption process is already active!", PT_SOURCEINFO);
        else
            throw SSLRuntimeError("A decryption process is already active!", PT_SOURCEINFO);
    }

    // Get the RSA key from the private key
    _rsa = EVP_PKEY_get1_RSA(pkey.impl());
    if(!_rsa)
        throw SSLRuntimeError("Could not extract the RSA key from the private key!", PT_SOURCEINFO);

    // Determine the padding mode
    _pmode = (pmode == RSA_PKCS1_OAEP) ? RSA_PKCS1_OAEP_PADDING : RSA_PKCS1_PADDING;

    // Get the RSA size and set the maximum chunk size to zero
    _rsaSize      = RSA_size(_rsa);
    _maxChunkSize = 0;

    // Prepare the output buffer
    _ioBuf.resize(_rsaSize);

    // Reset the put and set pointers
    this->setp(0, 0);
    this->setg(&_ioBuf[0], &_ioBuf[0] + _rsaSize, &_ioBuf[0] + _rsaSize);

    // Resize the conversion buffer
    _cnvBuf.resize(_rsaSize);
}

void RSACipher::finish()
{
    // Encrypt the remaining data
    if(_maxChunkSize) overflow(traits_type::eof());

    // Free the RSA
    RSA_free(_rsa);
    _rsa = 0;
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
    if( ! _rsa )
        throw SSLRuntimeError("No active decryption process!", PT_SOURCEINFO);
    if( this->pptr() )
        throw SSLRuntimeError("The cipher is currently in data encryption mode!", PT_SOURCEINFO);

    // Check if we still have anything left if the get buffer
    if( this->gptr() && this->gptr() < this->egptr() )
        return traits_type::to_int_type( *this->gptr() );

    // RSA only can decrypt a chunk with a fixed size,
    // assume EOF if we do not have it 
    if(_ios->rdbuf()->in_avail() < _rsaSize) return traits_type::eof();

    // Read from the attached iostream
    _ios->read(&_cnvBuf[0], _rsaSize);
    
    // Decrypt the data
    const int dlen = RSA_private_decrypt( _rsaSize,
                                          (const unsigned char*) &_cnvBuf[0],
                                          (unsigned char*) &_ioBuf[0], _rsa, _pmode );
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
    if( ! _rsa )
        throw SSLRuntimeError("No active encryption process!", PT_SOURCEINFO);
    if( this->gptr() )
        throw SSLRuntimeError("The cipher is currently in data decryption mode!", PT_SOURCEINFO);

    // Is there any data to be encrypted?
    const size_t avail = this->pptr() - this->pbase();

    if(avail) {
        // Encrypt the data
        const int dlen = RSA_public_encrypt( avail,
                                            (const unsigned char*) &_ioBuf[0],
                                            (unsigned char*) &_cnvBuf[0], _rsa, _pmode );
        if(dlen < 0) {
            long i = ERR_get_error();
            while(i) {
                std::cerr << ERR_error_string(i, 0) << std::endl;
                i = ERR_get_error();
            }
            throw SSLRuntimeError("Failed encrypting a string chunk!", PT_SOURCEINFO);
        }
        // Write the data to the output stream
        if(dlen > 0) _ios->write((const char*) &_cnvBuf[0], dlen);
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

} // namespace Pt
} // namespace Ssl
