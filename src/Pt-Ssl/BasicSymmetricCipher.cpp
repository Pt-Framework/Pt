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
: BasicCipher(ios), _bioEnc(0)
{
    _ioBuf.resize(1024);
}

BasicSymmetricCipher::~BasicSymmetricCipher()
{
    if(_bioEnc) BIO_free(_bioEnc);
}

void BasicSymmetricCipher::startEncrypt(const std::string& password)
{
    if( _bioEnc )
        throw SSLRuntimeError("An encryption process is already active!", PT_SOURCEINFO);

    // Generate salt
    unsigned char salt[PKCS5_SALT_LEN];
    if(RAND_pseudo_bytes(salt, sizeof salt) < 0)
        throw SSLRuntimeError("Could not generate the encryption salt!", PT_SOURCEINFO);

    // Get the cipher
    // NOTE: Later we can use enum or ask the derivative class to set it
    const EVP_CIPHER* cipher = EVP_get_cipherbyname("aes-256-cbc");
    if(!cipher)
        throw SSLRuntimeError("Could not acquire cipher!", PT_SOURCEINFO);

    // Derives a key and an IV from teh user password and salt
    unsigned char key[EVP_MAX_KEY_LENGTH], iv[EVP_MAX_IV_LENGTH];
    EVP_BytesToKey( cipher, EVP_get_digestbyname("SHA1"), salt, (unsigned char *) password.c_str(), password.length(), 1, key, iv );

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

    // Copy the BIO
    _bioEnc = benc.get();
    benc.release();

    // Set the put pointers and reset the get pointers
    this->setp(&_ioBuf[0], &_ioBuf[0] + _ioBuf.size());
    this->setg(0, 0, 0);
}

void BasicSymmetricCipher::startDecrypt(const std::string& password)
{
}

void BasicSymmetricCipher::finish()
{
    // Encrypt the remaining data
    //if(_maxChunkSize) overflow(traits_type::eof());

    // Free the RSA
    //RSA_free(_rsa);
    //_rsa = 0;
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
}

BasicSymmetricCipher::int_type BasicSymmetricCipher::overflow(int_type ch)
{
}

} // namespace Pt
} // namespace Ssl
