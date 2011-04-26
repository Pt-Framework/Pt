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

#include <Pt/Ssl/SSLPublicKey.h>
#include <iostream>

#include "Utils.h"

namespace Pt {
namespace Ssl {

SSLPublicKey::Impl::Impl(EVP_PKEY* pkey)
: _pkey(pkey)
{}

SSLPublicKey::Impl::~Impl()
{ if(_pkey) EVP_PKEY_free(_pkey); }

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

SSLPublicKey::SSLPublicKey(EVP_PKEY* pkey)
: _impl( new Impl(pkey) ), _sbio(0), _mctx(0), _rsa ( 0 )
{}

SSLPublicKey::SSLPublicKey(const SSLPublicKey& pkey)
: _impl( pkey._impl ), _sbio(0), _mctx(0), _rsa ( 0 )
{}

SSLPublicKey::~SSLPublicKey()
{
    if(_rsa) {
        RSA_free(_rsa);
        _rsa = 0;
    }

    if(_sbio) {
        BIO_free(_sbio);
        _sbio = 0;
        _mctx = 0;
    }    
}

void SSLPublicKey::beginVerifyString(const char* digest)
{
    if( !_impl || !_impl->_pkey )
        throw SSLRuntimeError("Attempting to verify string using an empty public key!", PT_SOURCEINFO);
    if( _sbio )
        throw SSLRuntimeError("A verification process is already active!", PT_SOURCEINFO);

    // Initialize a message-digest BIO
    BioAutoPtr bmd( BIO_new(BIO_f_md()) );
    if(!bmd) {
        throw SSLRuntimeError("Could not initialize message-digest BIO!", PT_SOURCEINFO);
    }

    // Initialize a message-digest context
    // (there is no need to free this context because it is owned by the message-digest BIO)
    if(!BIO_get_md_ctx(bmd.get(), &_mctx)) {
        throw SSLRuntimeError("Could not initialize message-digest context!", PT_SOURCEINFO);
    }

    // Initialize a verification sub-context
    // (there is no need to free this sub-context because it is owned by the message-digest context)
    EVP_PKEY_CTX* pctx = 0;
    if(!EVP_DigestVerifyInit(_mctx, &pctx, EVP_get_digestbyname(digest), 0, _impl->_pkey)) {
        throw SSLRuntimeError("Could not initialize the signing context!", PT_SOURCEINFO);
    }

    // Copy the BIO
    _sbio = bmd.get();
    bmd .release();
}

void SSLPublicKey::addStringToVerify(const std::string& str)
{
    if( !_sbio )
        throw SSLRuntimeError("No active verification process!", PT_SOURCEINFO);

    if(!EVP_DigestSignUpdate(_mctx, (void*) str.c_str(), str.length())) {
        throw SSLRuntimeError("Could not add data to the verification context!", PT_SOURCEINFO);
    }
}

const bool SSLPublicKey::endVerifyString(const std::string& sig)
{
    if( !_sbio )
        throw SSLRuntimeError("No active verification process!", PT_SOURCEINFO);

    // Allocate buffer for the signature binary data
    const size_t               binlen = EVP_PKEY_size(_impl->_pkey);
    std::vector<unsigned char> bin;
    bin.resize(binlen);

    // Convert the signature string to binary data
    string2ssldata(sig, &bin[0], binlen);

    // Finalize the verification process
    const int ret = EVP_DigestVerifyFinal(_mctx, &bin[0], binlen);
    if(ret < 0) {
        throw SSLRuntimeError("Could not finalize the verification process!", PT_SOURCEINFO);
    }

    // Free the BIO (and all the contexts)
    BIO_free(_sbio);
    _sbio = 0;

    // Return the result
    return ret > 0;
}

void SSLPublicKey::beginEncryptString(PaddingMode pmode)
{
    if( !_impl || !_impl->_pkey )
        throw SSLRuntimeError("Attempting to encrypt string using an empty public key!", PT_SOURCEINFO);
    if( _rsa )
        throw SSLRuntimeError("An encryption process is already active!", PT_SOURCEINFO);

    // Get the RSA key from the public key
    _rsa = EVP_PKEY_get1_RSA(_impl->_pkey);
    if(!_rsa)
        throw SSLRuntimeError("Could not extract the RSA key from the public key!", PT_SOURCEINFO);

    // Determine the padding mode
    _epmode = (pmode == RSA_PKCS1_OAEP) ? RSA_PKCS1_OAEP_PADDING : RSA_PKCS1_PADDING;
    
    // Get the RSA and maximum chunk size
    _rsaSize      = RSA_size(_rsa);
    _maxChunkSize = (pmode == RSA_PKCS1_OAEP) ? (_rsaSize - 41) : (_rsaSize - 11);
    --_maxChunkSize;

    // Clear the input buffer and resize the output buffer
    _eibuf.clear();
    _eobuf.resize(_rsaSize);
}

const std::string SSLPublicKey::tryEncryptString(const std::string& str)
{
    if( !_rsa )
        throw SSLRuntimeError("No active encryption process!", PT_SOURCEINFO);

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
}

const std::string SSLPublicKey::endEncryptString()
{
    if( !_rsa )
        throw SSLRuntimeError("No active encryption process!", PT_SOURCEINFO);

    // Encrypt the remaining data
    std::string dstBuff;
    if(!_eibuf.empty()) {
        const int dlen = RSA_public_encrypt(_eibuf.length(), (const unsigned char*) _eibuf.c_str(), &_eobuf[0], _rsa, _epmode);
        if(dlen < 0) throw SSLRuntimeError("Failed encrypting a string chunk!", PT_SOURCEINFO);
        if(dlen > 0) dstBuff = ssldata2string(&_eobuf[0], dlen);
    }

    // Free the RSA
    RSA_free(_rsa);
    _rsa = 0;

    // Return the encrypted string
    return dstBuff;
}

evp_pkey_st* SSLPublicKey::impl() const
{ return _impl->_pkey; }
    
} // namespace Pt
} // namespace Ssl

/*
    // Initialize a cipher BIO
    BioAutoPtr benc( BIO_new(BIO_f_cipher()) );
    if(!benc) {
        throw SSLRuntimeError("Could not initialize cipher BIO!", PT_SOURCEINFO);
    }

    // Initialize a cipher context
    EVP_CIPHER_CTX* pcctx = 0;
    if(!BIO_get_cipher_ctx(benc.get(), &pcctx)) {
        throw SSLRuntimeError("Could not initialize cipher context!", PT_SOURCEINFO);
    }
    EvpCipherCtxAutoPtr cctx(pcctx);

    const EVP_CIPHER* cipher = EVP_get_cipherbyname("aes-256-cbc");
    if(!cipher) {
        throw SSLRuntimeError("Could not acquire cipher!", PT_SOURCEINFO);
    }

    if(!EVP_CipherInit_ex(cctx.get(), cipher, 0, 0, 0, 1)) {
        throw SSLRuntimeError("Could not initialize cipher context!", PT_SOURCEINFO);
    }
*/

