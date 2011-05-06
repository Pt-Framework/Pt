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

#include <Pt/Ssl/BasicSymmetricCipher.h>

#include "Utils.h"

#include <openssl/rand.h>

namespace Pt {
namespace Ssl {

BasicSymmetricCipher::BasicSymmetricCipher(std::iostream& ios)
: BasicCipher(ios), _bioEnc(0), _bioDec(0), _bioIO(0)
{
    _ioBuf.resize(1024);
    _cnvBuf.resize(1024);
}

BasicSymmetricCipher::~BasicSymmetricCipher()
{
    if(_bioEnc) BIO_free(_bioEnc);
    if(_bioDec) BIO_free(_bioDec);
    if(_bioIO ) BIO_free(_bioIO );
}

void BasicSymmetricCipher::startEncrypt(const std::string& password)
{
    if( _bioEnc )
        throw SSLRuntimeError("An encryption process is already active!", PT_SOURCEINFO);
    if( _bioDec )
        throw SSLRuntimeError("An decryption process is already active!", PT_SOURCEINFO);

    // Generate salt
    /*
    unsigned char salt[PKCS5_SALT_LEN];
    if(RAND_pseudo_bytes(salt, sizeof salt) < 0)
        throw SSLRuntimeError("Could not generate the encryption salt!", PT_SOURCEINFO);
    */

    // Get the cipher
    // NOTE: Later we can use enum or ask the derivative class to set it
    const EVP_CIPHER* cipher = EVP_get_cipherbyname("aes-256-cbc");
    if(!cipher)
        throw SSLRuntimeError("Could not acquire cipher!", PT_SOURCEINFO);

    // Derives a key and an IV from teh user password and salt
    unsigned char key[EVP_MAX_KEY_LENGTH], iv[EVP_MAX_IV_LENGTH];
    EVP_BytesToKey( cipher, EVP_get_digestbyname("SHA1"), 0 /*salt*/, (unsigned char *) password.c_str(), password.length(), 1, key, iv );

    // Initialize a cipher BIO
    BioAutoPtr benc( BIO_new(BIO_f_cipher()) );
    if(!benc)
        throw SSLRuntimeError("Could not initialize cipher BIO!", PT_SOURCEINFO);

    // Initialize a cipher context
    // (there is no need to free this context because it is owned by the cihper BIO)
    EVP_CIPHER_CTX* pcctx = 0;
    if(!BIO_get_cipher_ctx(benc.get(), &pcctx))
        throw SSLRuntimeError("Could not initialize cipher context!", PT_SOURCEINFO);

    // Initialize the cipher
    if(!EVP_CipherInit_ex(pcctx, cipher, 0, 0, 0, 1))
        throw SSLRuntimeError("Could not initialize cipher context!", PT_SOURCEINFO);
    
    if(!EVP_CipherInit_ex(pcctx, NULL, NULL, key, iv, 1))
        throw SSLRuntimeError("Could not initialize cipher context!", PT_SOURCEINFO);

    // Copy the cipher BIO
    _bioEnc = benc.get();
    benc.release();

    // Create an input/output BIO
    _bioIO = BIO_push(_bioEnc, BIO_new(BIO_s_mem()));
        
    // Set the put pointers and reset the get pointers
    this->setp(&_ioBuf[0], &_ioBuf[0] + _ioBuf.size());
    this->setg(0, 0, 0);
}

void BasicSymmetricCipher::startDecrypt(const std::string& password)
{
    if( _bioEnc )
        throw SSLRuntimeError("An encryption process is already active!", PT_SOURCEINFO);
    if( _bioDec )
        throw SSLRuntimeError("An decryption process is already active!", PT_SOURCEINFO);

    // Generate salt
    /*
    unsigned char salt[PKCS5_SALT_LEN];
    if(RAND_pseudo_bytes(salt, sizeof salt) < 0)
        throw SSLRuntimeError("Could not generate the encryption salt!", PT_SOURCEINFO);
    */

    // Get the cipher
    // NOTE: Later we can use enum or ask the derivative class to set it
    const EVP_CIPHER* cipher = EVP_get_cipherbyname("aes-256-cbc");
    if(!cipher)
        throw SSLRuntimeError("Could not acquire cipher!", PT_SOURCEINFO);

    // Derives a key and an IV from teh user password and salt
    unsigned char key[EVP_MAX_KEY_LENGTH], iv[EVP_MAX_IV_LENGTH];
    EVP_BytesToKey( cipher, EVP_get_digestbyname("SHA1"), 0 /*salt*/, (unsigned char *) password.c_str(), password.length(), 1, key, iv );

    // Initialize a cipher BIO
    BioAutoPtr bdec( BIO_new(BIO_f_cipher()) );
    if(!bdec)
        throw SSLRuntimeError("Could not initialize cipher BIO!", PT_SOURCEINFO);

    // Initialize a cipher context
    // (there is no need to free this context because it is owned by the cihper BIO)
    EVP_CIPHER_CTX* pcctx = 0;
    if(!BIO_get_cipher_ctx(bdec.get(), &pcctx))
        throw SSLRuntimeError("Could not initialize cipher context!", PT_SOURCEINFO);

    // Initialize the cipher
    if(!EVP_CipherInit_ex(pcctx, cipher, 0, 0, 0, 0))
        throw SSLRuntimeError("Could not initialize cipher context!", PT_SOURCEINFO);

    if(!EVP_CipherInit_ex(pcctx, NULL, NULL, key, iv, 0))
        throw SSLRuntimeError("Could not initialize cipher context!", PT_SOURCEINFO);

    // Copy the cipher BIO
    _bioDec = bdec.get();
    bdec.release();

    // Create an input/output BIO
    _bioIO = BIO_push(_bioDec, BIO_new(BIO_s_mem()));

    // Reset the put and set pointers
    this->setp(0, 0);
    this->setg(&_ioBuf[0], &_ioBuf[0] + _ioBuf.size(), &_ioBuf[0] + _ioBuf.size());
}

void BasicSymmetricCipher::finish()
{
    // Data encryption mode?
    if(_bioEnc) {
        // Encrypt the remaining data
        overflow(traits_type::eof());
        // Free the BIO
        BIO_free(_bioEnc);
        _bioEnc = 0;
    }

    // Data decrypyion mode?
    if(_bioDec) {
        BIO_free(_bioDec);
        _bioDec = 0;
    }

    BIO_free(_bioIO);
    _bioIO = 0;
}

int BasicSymmetricCipher::sync()
{
    if( this->pptr() ) {
        while( this->pptr() > this->pbase() ) {
            const int_type ch = this->overflow( traits_type::eof() );
            if( ch == traits_type::eof() ) return -1;
        }
    }

    return 0;
}

BasicSymmetricCipher::int_type BasicSymmetricCipher::underflow()
{
    if( ! _bioDec )
        throw SSLRuntimeError("No active decryption process!", PT_SOURCEINFO);
    if( this->pptr() )
        throw SSLRuntimeError("The cipher is currently in data encryption mode!", PT_SOURCEINFO);

    // Check if we still have anything left if the get buffer
    if( this->gptr() && this->gptr() < this->egptr() )
        return traits_type::to_int_type( *this->gptr() );
    
/*
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
*/
    // EOF
    return traits_type::eof();
    
}

BasicSymmetricCipher::int_type BasicSymmetricCipher::overflow(int_type ch)
{
    if( ! _bioEnc )
        throw SSLRuntimeError("No active encryption process!", PT_SOURCEINFO);
    if( this->gptr() )
        throw SSLRuntimeError("The cipher is currently in data decryption mode!", PT_SOURCEINFO);

    // Is there any data to be encrypted?
    size_t avail = this->pptr() - this->pbase();

    if(avail) {
        while(avail > 0) {
            // Encrypt the data
            const int written = BIO_write(_bioIO, &_ioBuf[0], avail);
            if(written < 0)
                throw SSLRuntimeError("Failed encrypting a string chunk!", PT_SOURCEINFO);
            avail -= written;

            std::cerr << "$$$$$ overflow() : written = " << written << std::endl;

            // Read the encrypted data and write it to the output stream
            while(true) {
                const int read = BIO_read(_bioIO, &_cnvBuf[0], _cnvBuf.size());
                std::cerr << "$$$$$ overflow() : read = " << read << std::endl;
                if(read <= 0) break;
                _ios->write((const char*) &_cnvBuf[0], read);
            }
        }
        // Reset the put pointers
        setp(&_ioBuf[0], &_ioBuf[0] + _ioBuf.size());
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
