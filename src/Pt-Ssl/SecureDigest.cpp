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
#include <Pt/Ssl/RSACipher.h>

#include <Pt/Ssl/SecureDigest.h>
#include <fstream>

#include "Utils.h"

namespace Pt {
namespace Ssl {

SecureDigest::SecureDigest()
: _sig(""), _rawSigSize(0), _sbio(0), _mctx(0)
{}

SecureDigest::SecureDigest(const SSLPrivateKey& pkey, DigestType digestType)
: _sig(""), _rawSigSize(0), _sbio(0), _mctx(0)
{ start(pkey, digestType); }

SecureDigest::SecureDigest(const SSLPublicKey& pkey, const std::string& sig, DigestType digestType)
: _sig(""), _rawSigSize(0), _sbio(0), _mctx(0)
{ start(pkey, sig, digestType); }

SecureDigest::~SecureDigest()
{ if(_sbio) BIO_free(_sbio); }

void SecureDigest::start(const SSLPrivateKey& pkey, DigestType digestType)
{
    if( _sbio )
        throw SSLRuntimeError("A data signing/verification process is already active!", PT_SOURCEINFO);

    // Initialize a message-digest BIO
    BioAutoPtr bmd( BIO_new(BIO_f_md()) );
    if(!bmd)
        throw SSLRuntimeError("Could not initialize a message-digest BIO!", PT_SOURCEINFO);

    // Initialize a message-digest context
    // (there is no need to free this context because it is owned by the message-digest BIO)
    if(!BIO_get_md_ctx(bmd.get(), &_mctx)) {
        throw SSLRuntimeError("Could not initialize a message-digest context!", PT_SOURCEINFO);
    }

    // Initialize a signing sub-context
    // (there is no need to free this sub-context because it is owned by the message-digest context)
    EVP_PKEY_CTX* pctx = 0;
    if(!EVP_DigestSignInit(_mctx, &pctx, EVP_get_digestbyname(digestEnumToString(digestType)), 0, pkey.impl()))
        throw SSLRuntimeError("Could not initialize the signing context!", PT_SOURCEINFO);

    // Clear the signature
    _sig.clear();

    // Copy the BIO
    _sbio = bmd.get();
    bmd.release();
}

void SecureDigest::start(const SSLPublicKey& pkey, const std::string& sig, DigestType digestType)
{
    if( _sbio )
        throw SSLRuntimeError("A data signing/verification process is already active!", PT_SOURCEINFO);

    // Initialize a message-digest BIO
    BioAutoPtr bmd( BIO_new(BIO_f_md()) );
    if(!bmd)
        throw SSLRuntimeError("Could not initialize a message-digest BIO!", PT_SOURCEINFO);

    // Initialize a message-digest context
    // (there is no need to free this context because it is owned by the message-digest BIO)
    if(!BIO_get_md_ctx(bmd.get(), &_mctx))
        throw SSLRuntimeError("Could not initialize a message-digest context!", PT_SOURCEINFO);

    // Initialize a verification sub-context
    // (there is no need to free this sub-context because it is owned by the message-digest context)
    EVP_PKEY_CTX* pctx = 0;
    if(!EVP_DigestVerifyInit(_mctx, &pctx, EVP_get_digestbyname(digestEnumToString(digestType)), 0, pkey.impl()))
        throw SSLRuntimeError("Could not initialize the verification context!", PT_SOURCEINFO);

    // Copy the signature and get the raw signature size
    _sig = sig;
    _rawSigSize = EVP_PKEY_size(pkey.impl());
    
    // Copy the BIO
    _sbio = bmd.get();
    bmd.release();
}

void SecureDigest::update(const char* str, int len)
{
    if( !_sbio )
        throw SSLRuntimeError("No active data signing/verification process!", PT_SOURCEINFO);

    if(!EVP_DigestSignUpdate(_mctx, str, len))
        throw SSLRuntimeError("Could update the state of the the signing/verification context!", PT_SOURCEINFO);
}

void SecureDigest::update(const std::string& str)
{
    if( !_sbio )
        throw SSLRuntimeError("No active data signing/verification process!", PT_SOURCEINFO);

    if(!EVP_DigestSignUpdate(_mctx, str.c_str(), str.length()))
        throw SSLRuntimeError("Could update the state of the the signing/verification context!", PT_SOURCEINFO);
}

void SecureDigest::update(std::istream& is)
{
    if( !_sbio )
        throw SSLRuntimeError("No active data signing/verification process!", PT_SOURCEINFO);

    char buff[1024];

    do {

        is.read(buff, sizeof(buff));
        const std::streamsize got = is.gcount();

        if(got > 0 && !EVP_DigestSignUpdate(_mctx, buff, got))
            throw SSLRuntimeError("Could update the state of the the signing/verification context!", PT_SOURCEINFO);

    } while(!is.eof());
}

bool SecureDigest::finish()
{
    if( !_sbio )
        throw SSLRuntimeError("No active data signing/verification process!", PT_SOURCEINFO);

    // Data
    std::vector<unsigned char> bin;
    bool                       ok = true;

    // Finish a signing process
    if(_sig.empty()) {
        // Get the maximum length of the signature binary data
        // and allocate buffer to read the signature binary data
        size_t binlen = 0;
        EVP_DigestSignFinal(_mctx, 0, &binlen);
        bin.resize(binlen);
        // Finalize the signing process
        if(!EVP_DigestSignFinal(_mctx, &bin[0], &binlen))
            throw SSLRuntimeError("Could not finalize the signing process!", PT_SOURCEINFO);
        // Convert the signature binary data to hex string
        _sig = ssldata2string(&bin[0], binlen);
    }

    // Finish a verification process
    else {
        // Allocate buffer for the signature binary data
        // and convert the signature string to binary data
        bin.resize(_rawSigSize);
        string2ssldata(_sig, &bin[0], _rawSigSize);
        // Finalize the verification process
        const int ret = EVP_DigestVerifyFinal(_mctx, &bin[0], _rawSigSize);
        if(ret < 0) 
            throw SSLRuntimeError("Could not finalize the verification process!", PT_SOURCEINFO);
        // Check if the signature is OK
        ok = (ret > 0);
    }
    
    // Free the BIO (and all the contexts)
    BIO_free(_sbio);
    _sbio = 0;

    // Done
    return ok;
}

const char* SecureDigest::digestEnumToString(DigestType digestType)
{
    if(digestType == SHA1_Digest) return "SHA1";
    return "MD5";
}

} // namespace Pt
} // namespace Ssl
