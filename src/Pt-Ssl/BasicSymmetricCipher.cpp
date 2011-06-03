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
#include <Pt/Ssl/Exception.h>

#include "Utils.h"

#include <openssl/rand.h>

namespace Pt {
namespace Ssl {

BasicSymmetricCipher::BasicSymmetricCipher(const std::string& password, OperationMode operMode)
: _operMode (operMode),
  _password (password),
  _salt     (""),
  _bioEncIn (0),
  _bioEncOut(0),
  _bioDecIn (0),
  _bioDecOut(0)
{}


BasicSymmetricCipher::~BasicSymmetricCipher()
{
    if(_bioEncIn ) BIO_free(_bioEncIn );
    if(_bioEncOut) BIO_free(_bioEncOut);

    if(_bioDecIn ) BIO_free(_bioDecIn );
    if(_bioDecOut) BIO_free(_bioDecOut);
}

void BasicSymmetricCipher::setMode(OperationMode operMode)
{ _operMode = operMode; }

void BasicSymmetricCipher::setPassword(const std::string& password)
{ _password = password; }

size_t BasicSymmetricCipher::saltLength() const
{ return PKCS5_SALT_LEN; }

void BasicSymmetricCipher::setSalt(const std::string& salt)
{
    if( !salt.empty() && salt.length() != saltLength() )
        throw SSLRuntimeError("Invalid salt length!", PT_SOURCEINFO);
}

const std::string& BasicSymmetricCipher::getSalt() const
{ return _salt; }

void BasicSymmetricCipher::genSalt(SaltType saltType)
{
    unsigned char saltBuff[PKCS5_SALT_LEN];

    switch(saltType) {
        case StrongSalt:
            if(RAND_bytes(saltBuff, sizeof(saltBuff)) < 0)
                throw SSLRuntimeError("Could not generate a new random salt!", PT_SOURCEINFO);
            break;

        case NormalSalt:
            if(RAND_pseudo_bytes(saltBuff, sizeof(saltBuff)) < 0)
                throw SSLRuntimeError("Could not generate a new random salt!", PT_SOURCEINFO);
            break;

        default:
            throw SSLRuntimeError("Invalid salt type!", PT_SOURCEINFO);
    }

    _salt = std::string((char*) saltBuff, PKCS5_SALT_LEN);
}

int BasicSymmetricCipher::encode(const char* from, const char* from_end, const char*& from_next, char* to, char* to_end, char*& to_next)
{
    // Use salt?
    unsigned char* salt = 0;
    if( !_salt.empty() ) {
        assert( _salt.length() == PKCS5_SALT_LEN );
        salt = (unsigned char*) _salt.c_str();
    }

    // Initialize the cipher (if not yet)
    if(!_bioEncIn) {
        // Get the cipher
        const EVP_CIPHER* cipher = EVP_get_cipherbyname(getOpenSSLCipherName());
        if(!cipher)
            throw SSLRuntimeError("Could not acquire cipher!", PT_SOURCEINFO);
        // Derives a key and an IV from the user password and salt
        unsigned char key[EVP_MAX_KEY_LENGTH], iv[EVP_MAX_IV_LENGTH];
        EVP_BytesToKey( cipher, EVP_get_digestbyname("SHA1"), salt, (unsigned char *) _password.c_str(), _password.length(), 1, key, iv );
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
        _bioEncIn = benc.get();
        benc.release();
        // Create the output BIO
        _bioEncOut = BIO_new(BIO_s_mem());
        BIO_push(_bioEncIn, _bioEncOut);
    }

    // Is the output area large enough?
    const size_t outAvail = to_end - to;
    if(!outAvail) return -1;

    // Is there any data to be encrypted?
    const size_t inAvail = from_end - from;
    if(!inAvail) return -1;

    // Write the data to the encryption BIO
    const int written = BIO_write(_bioEncIn, from, inAvail);
    if(written < 0)
        throw SSLRuntimeError("Failed encrypting a string chunk!", PT_SOURCEINFO);

    // Read the ecnrypted data
    const int read = BIO_read(_bioEncOut, to, outAvail);
    
    // Adjust the pointers
    from_next = from + written;
    if(read < 0) to_next = to + read;

    // Done happily :D
    return 1;
}

int BasicSymmetricCipher::decode(const char* from, const char* from_end, const char*& from_next, char* to, char* to_end, char*& to_next)
{
    // Use salt?
    unsigned char* salt = 0;
    if( !_salt.empty() ) {
        assert( _salt.length() == PKCS5_SALT_LEN );
        salt = (unsigned char*) _salt.c_str();
    }
    
    // Initialize the cipher (if not yet)
    if(!_bioDecIn) {
        // Get the cipher
        const EVP_CIPHER* cipher = EVP_get_cipherbyname(getOpenSSLCipherName());
        if(!cipher)
            throw SSLRuntimeError("Could not acquire cipher!", PT_SOURCEINFO);
        // Derives a key and an IV from the user password and salt
        unsigned char key[EVP_MAX_KEY_LENGTH], iv[EVP_MAX_IV_LENGTH];
        EVP_BytesToKey( cipher, EVP_get_digestbyname("SHA1"), salt, (unsigned char *) _password.c_str(), _password.length(), 1, key, iv );
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
        _bioDecIn = bdec.get();
        bdec.release();
        // Create the output BIO
        _bioDecIn = BIO_new(BIO_s_mem());
        BIO_push(_bioDecIn, _bioDecIn);
    }

    return -1;
}

} // namespace Pt
} // namespace Ssl
