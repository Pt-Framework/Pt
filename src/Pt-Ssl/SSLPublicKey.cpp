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

SSLPublicKey::SSLPublicKey(EVP_PKEY* pkey)
: _impl(new Impl(pkey))
{}

SSLPublicKey::~SSLPublicKey()
{}

bool SSLPublicKey::verifyStringSignature(const std::string& str, const std::string& sig, const char* digest) const
{ return _impl->verifyStringSignature(str, sig, digest); }

const std::string SSLPublicKey::encryptString(const std::string& str) const
{ return _impl->encryptString(str); }

evp_pkey_st* SSLPublicKey::impl() const
{ return _impl->_pkey; }
    
////////////////////////////////////////////////////////////////////////////////////////////////////

SSLPublicKey::Impl::Impl(EVP_PKEY* pkey)
: _pkey(pkey)
{}

SSLPublicKey::Impl::~Impl()
{ if(_pkey) EVP_PKEY_free(_pkey); }

bool SSLPublicKey::Impl::verifyStringSignature(const std::string& str, const std::string& sig, const char* digest) const
{
    if( ! _pkey )
        throw SSLRuntimeError("Attempting to verify signature using an empty public key!", PT_SOURCEINFO);

    // Initialize a message-digest BIO
    BioAutoPtr bmd( BIO_new(BIO_f_md()) );
    if(!bmd) {
        throw SSLRuntimeError("Could not initialize message-digest BIO!", PT_SOURCEINFO);
    }

    // Initialize a message-digest context
    EVP_MD_CTX* pmctx = 0;
    if(!BIO_get_md_ctx(bmd.get(), &pmctx)) {
        throw SSLRuntimeError("Could not initialize message-digest context!", PT_SOURCEINFO);
    }
    EvpMdCtxAutoPtr mctx(pmctx);

    // Initialize a verification sb-context
    // (there is no need to free this sub-context because it is owned by the message-digest context)
    EVP_PKEY_CTX* pctx = 0;
    if(!EVP_DigestVerifyInit(mctx.get(), &pctx, EVP_get_digestbyname(digest), 0, _pkey)) {
        throw SSLRuntimeError("Could not initialize the verification context!", PT_SOURCEINFO);
    }

    // Add data to the message-digest context
    if(!EVP_DigestSignUpdate(mctx.get(), (void*) str.c_str(), str.length())) {
        throw SSLRuntimeError("Could not add data to the signing context!", PT_SOURCEINFO);
    }

    // Allocate buffer for the signature binary data
    const size_t               binlen = EVP_PKEY_size(_pkey);
    std::vector<unsigned char> bin;
    bin.resize(binlen);

    // Convert the signature string to binary data
    string2ssldata(sig, &bin[0], binlen);

    // Finalize the verification process
    const int ret = EVP_DigestVerifyFinal(mctx.get(), &bin[0], binlen);
    if(ret < 0) {
        throw SSLRuntimeError("Could not finalize the verification process!", PT_SOURCEINFO);
    }

    // Return the result
    return ret > 0;
}

const std::string SSLPublicKey::Impl::encryptString(const std::string& str) const
{
    if( ! _pkey )
        throw SSLRuntimeError("Attempting to encrypt string using an empty public key!", PT_SOURCEINFO);

    // Get the RSA key from the public key
    RSA* prsa = EVP_PKEY_get1_RSA(_pkey);
    if(!prsa)
        throw SSLRuntimeError("Could not extract the RSA key from the public key!", PT_SOURCEINFO);
    RsaAutoPtr rsa(prsa);
    
    // Get some information about the source string
    size_t               leftOver = str.length();
    const unsigned char* src      = (const unsigned char*) str.c_str();

    // Prepare the destination buffer
    std::string                dstBuff;
    const int                  rsaSize = RSA_size(rsa.get());
    std::vector<unsigned char> tmpBuff;
    tmpBuff.resize(rsaSize);

    // Encrypt the string
    while(leftOver > 0) {
        const int slen = std::min<size_t>(leftOver, rsaSize - 11);
        const int dlen = RSA_public_encrypt(slen, src, &tmpBuff[0], rsa.get(), RSA_PKCS1_PADDING);
        if(dlen < 0) throw SSLRuntimeError("Failed encrypting a string block!", PT_SOURCEINFO);
        dstBuff += ssldata2string(&tmpBuff[0], dlen);
        dstBuff += '\n';
        leftOver -= slen;
    }

    // Return the encrypted string
    return dstBuff;
}

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

