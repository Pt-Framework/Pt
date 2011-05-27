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

RSACipher::RSACipher()
: _pmode     (RSA_PKCS1),
  _rsaPub    (0),
  _rsaPubSize(0),
  _encCSize  (0),
  _rsaPrv    (0),
  _rsaPrvSize(0)
{}

RSACipher::RSACipher(const SSLPublicKey& pubKey, PaddingMode pmode)
: _pmode     (pmode),
  _rsaPub    (0),
  _rsaPubSize(0),
  _encCSize  (0),
  _rsaPrv    (0),
  _rsaPrvSize(0)
{ setPublicKey(pubKey); }

RSACipher::RSACipher(const SSLPrivateKey& prvKey, PaddingMode pmode)
: _pmode     (pmode),
  _rsaPub    (0),
  _rsaPubSize(0),
  _encCSize  (0),
  _rsaPrv    (0),
  _rsaPrvSize(0)
{ setPrivateKey(prvKey); }

RSACipher::RSACipher(const SSLPublicKey& pubKey, const SSLPrivateKey& prvKey, PaddingMode pmode)
: _pmode     (pmode),
  _rsaPub    (0),
  _rsaPubSize(0),
  _encCSize  (0),
  _rsaPrv    (0),
  _rsaPrvSize(0)
{
    setPublicKey(pubKey);
    setPrivateKey(prvKey);
}

RSACipher::~RSACipher()
{
    if(_rsaPub) {
        RSA_free(_rsaPub);
        _rsaPub = 0;
    }

    // Free the old key (if any)
    if(_rsaPrv) {
        RSA_free(_rsaPrv);
        _rsaPrv = 0;
    }    
}

void RSACipher::setPublicKey(const SSLPublicKey& pubKey)
{
    // Free the old key (if any)
    if(_rsaPub) {
        RSA_free(_rsaPub);
        _rsaPub     = 0;
        _rsaPubSize = 0;
    }
        
    // Get the RSA key from the public key
    _rsaPub = EVP_PKEY_get1_RSA(pubKey.impl());
    if(!_rsaPub)
        throw SSLRuntimeError("Could not extract the RSA key from the public key!", PT_SOURCEINFO);

    // Get the RSA size and maximum chunk size
    _rsaPubSize = RSA_size(_rsaPub);
    _encCSize   = (_pmode == RSA_PKCS1_OAEP_PADDING) ? (_rsaPubSize - 42) : (_rsaPubSize - 12);
}

void RSACipher::setPrivateKey(const SSLPrivateKey& prvKey)
{
    // Free the old key (if any)
    if(_rsaPrv) {
        RSA_free(_rsaPrv);
        _rsaPrv     = 0;
        _rsaPrvSize = 0;
    }

    // Get the RSA key from the private key
    _rsaPrv = EVP_PKEY_get1_RSA(prvKey.impl());
    if(!_rsaPrv)
        throw SSLRuntimeError("Could not extract the RSA key from the private key!", PT_SOURCEINFO);

    // Get the RSA size
    _rsaPrvSize = RSA_size(_rsaPrv);
}

void RSACipher::setPadding(PaddingMode pmode)
{
    _pmode = (pmode == RSA_PKCS1_OAEP) ? RSA_PKCS1_OAEP_PADDING : RSA_PKCS1_PADDING;

    // Update the maximum chunk size (if needed)
    if(_rsaPub) {
        _encCSize   = (_pmode == RSA_PKCS1_OAEP_PADDING) ? (_rsaPubSize - 42) : (_rsaPubSize - 12);
    }
}

size_t RSACipher::blockSize() const
{ return std::max(_rsaPubSize, _rsaPrvSize); }

int RSACipher::encode(const char* from, const char* from_end, const char*& from_next, char* to, char* to_end, char*& to_next, bool finish)
{
    if(!_rsaPub)
        throw SSLRuntimeError("No public key specified!", PT_SOURCEINFO);

    // Is there any data to be encrypted?
    const size_t inAvail = from_end - from;
    if(!inAvail) return 0;

    // Is the output area large enough?
    const size_t outAvail = to_end - to;
    if(outAvail < _rsaPubSize) return -1;

    // Encrypt data
    if(finish || inAvail >= _encCSize) {
        // Encrypt the data
        const size_t readMax = std::min(inAvail, _encCSize);
        const int    dlen    = RSA_public_encrypt( readMax, (const unsigned char*) from,
                                                   (unsigned char*) to, _rsaPub, _pmode );
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
        to_next   = to   + _rsaPubSize;
        // Finish happily :D
        return 1;
    }

    // Not enough input data
    return 0;
    
}

int RSACipher::decode(const char* from, const char* from_end, const char*& from_next, char* to, char* to_end, char*& to_next, bool finish)
{
    return 0;
}

} // namespace Pt
} // namespace Ssl
