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

RSACipher::RSACipher(std::ostream& out)
: BasicCipher(out), _rsa(0), _inpBuf(0)
{}

RSACipher::RSACipher(std::ostream& out, const SSLPublicKey& pkey, PaddingMode pmode)
: BasicCipher(out), _rsa(0), _inpBuf(0)
{ startEncrypt(pkey, pmode); }

RSACipher::RSACipher(std::ostream& out, const SSLPrivateKey& pkey, PaddingMode pmode)
: BasicCipher(out), _rsa(0), _inpBuf(0)
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
            throw SSLRuntimeError("An decryption process is already active!", PT_SOURCEINFO);
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

    // Prepare the buffers
    _cnvBuf.resize(_rsaSize);
    _inpBuf.resize(_maxChunkSize);
    setp(&_inpBuf[0], &_inpBuf[0] + _maxChunkSize);
}

void RSACipher::startDecrypt(const SSLPrivateKey& pkey, PaddingMode pmode)
{
    if( _rsa ) {
        if(_maxChunkSize)
            throw SSLRuntimeError("An decryption process is already active!", PT_SOURCEINFO);
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

    // Prepare the buffers
    _cnvBuf.resize(_rsaSize);
    _inpBuf.resize(_rsaSize);
    setp(&_inpBuf[0], &_inpBuf[0] + _rsaSize);
}

void RSACipher::update(const char* str, int len)
{
    if( !_rsa )
        throw SSLRuntimeError("No active data encryption/decryption process!", PT_SOURCEINFO);

    sputn(str, len);
}

void RSACipher::update(const std::string& str)
{ update(str.c_str(), str.length()); }

void RSACipher::update(std::istream& is)
{
    if( !_rsa )
        throw SSLRuntimeError("No active data encryption/decryption process!", PT_SOURCEINFO);

    char buff[1024];
    do {
        is.read(buff, sizeof(buff));
        const std::streamsize got = is.gcount();
        if(got > 0) sputn(buff, got);

    } while(!is.eof());
}

void RSACipher::finish()
{
}

int RSACipher::sync()
{
    return 0;
}

int RSACipher::underflow()
{
    return 0;
}

int RSACipher::overflow(int c)
{
    return 0;
}


/*
    // Append the string to the input buffer
    _eibuf += str;
    if(_eibuf.length() < size_t(_maxChunkSize)) return "";

    // Get some information about the source string
    size_t               leftOver = _eibuf.length();
    const unsigned char* srcBuf   = (const unsigned char*) _eibuf.c_str();

    // Encrypt the string
    std::string dstBuff;
    while(leftOver >= size_t(_maxChunkSize)) {
        // Perform encryption
        const int dlen = RSA_public_encrypt(_maxChunkSize, srcBuf, &_eobuf[0], _rsa, _epmode);
        if(dlen < 0) {
            long i = ERR_get_error();
            while(i) {
                std::cerr << ERR_error_string(i, 0) << std::endl;
                i = ERR_get_error();
            }
            throw SSLRuntimeError("Failed encrypting a string chunk!", PT_SOURCEINFO);
        }
        // Append the result to the output buffer
        if(dlen > 0) {
            dstBuff += ssldata2string(&_eobuf[0], dlen);
            dstBuff += '\n';
        }
        // Adjust the state of the source string
        leftOver -= _maxChunkSize;
        srcBuf   += _maxChunkSize;
    }

    // Remove the encrypted string from the input buffer
    if(leftOver) _eibuf.erase(0, _eibuf.length() - leftOver);
    else         _eibuf.clear();

    // Return the encrypted string
    return dstBuff;
*/

} // namespace Pt
} // namespace Ssl
