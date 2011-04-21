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

#include <Pt/Ssl/SSLPrivateKey.h>
#include <fstream>

#include "Utils.h"

namespace Pt {
namespace Ssl {

SSLPrivateKey::Impl::Impl()
: _pswd(), _pkey(0)
{}

SSLPrivateKey::Impl::Impl(const std::string& password)
: _pswd(password), _pkey(0)
{}

SSLPrivateKey::Impl::~Impl()
{ clear(); }

void SSLPrivateKey::Impl::loadFromString(const std::string& keyData)
{
    // Clear previous key (if any)
    clear();

    // Create a read-only memory BIO from the given string
    BioAutoPtr in( BIO_new_mem_buf( (void*) keyData.c_str(), keyData.length() ) );

    // Try to read/parse the private key
    _pkey = PEM_read_bio_PrivateKey(in.get(), 0, SSLPrivateKey::Impl::passwordCallback, (void*) &_pswd);
    if(!_pkey)
        throw SSLRuntimeError("Could not read/parse/decode private-key data!", PT_SOURCEINFO);
}

void SSLPrivateKey::Impl::loadFromFile(const std::string& fileName)
{
    std::string   data;
    std::ifstream ifs;
    char          rbuf[4096];

    ifs.open(fileName.c_str(), std::ios::binary);
    while(ifs) {
        ifs.read( rbuf, sizeof(rbuf) );
        data += std::string( rbuf, ifs.gcount() );
    }

    loadFromString(data);
}

void SSLPrivateKey::Impl::clear()
{
    if(_pkey) {
        EVP_PKEY_free(_pkey);
        _pkey = 0;
    }
}

const std::string SSLPrivateKey::Impl::signString(const std::string& str, const char* digest) const
{
    if( ! _pkey )
        throw SSLRuntimeError("Attempting to sign string using an empty private key!", PT_SOURCEINFO);

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

    // Initialize a signing sb-context
    // (there is no need to free this sub-context because it is owned by the message-digest context)
    EVP_PKEY_CTX* pctx = 0;
    if(!EVP_DigestSignInit(mctx.get(), &pctx, EVP_get_digestbyname(digest), 0, _pkey)) {
        throw SSLRuntimeError("Could not initialize the signing context!", PT_SOURCEINFO);
    }

    // Add data to the message-digest context
    if(!EVP_DigestSignUpdate(mctx.get(), (void*) str.c_str(), str.length())) {
        throw SSLRuntimeError("Could not add data to the signing context!", PT_SOURCEINFO);
    }

    // Get the maximum length of the signature binary data
    size_t siglen = 0;
    EVP_DigestSignFinal(mctx.get(), 0, &siglen);

    // Allocate buffer for the signature binary data
    std::vector<unsigned char> sig;
    sig.resize(siglen);

    // Finalize the signing process
    if(!EVP_DigestSignFinal(mctx.get(), &sig[0], &siglen)) {
        throw SSLRuntimeError("Could not finalize the signing process!", PT_SOURCEINFO);
    }

    // Return the signature string
    return ssldata2string(&sig[0], siglen);
}

const std::string SSLPrivateKey::Impl::decryptString(const std::string& str) const
{
    if( ! _pkey )
        throw SSLRuntimeError("Attempting to decrypt string using an empty private key!", PT_SOURCEINFO);

    // Get the RSA key from the private key
    RSA* prsa = EVP_PKEY_get1_RSA(_pkey);
    if(!prsa)
        throw SSLRuntimeError("Could not extract the RSA key from the private key!", PT_SOURCEINFO);
    RsaAutoPtr rsa(prsa);

    // Tokenize the source by the '\n' character
    std::vector<std::string> tokens;
    SSLPrivateKey::Impl::tokenize(tokens, str, "\n");

    // Prepare the buffers
    const int                  rsaSize = RSA_size(rsa.get());
    std::string                dstBuff;
    std::vector<unsigned char> srcBuff;
    std::vector<unsigned char> tmpBuff;
    srcBuff.resize(rsaSize);
    tmpBuff.resize(rsaSize);

    // Walk torugh the tokens
    for(std::vector<std::string>::const_iterator it = tokens.begin(); it != tokens.end(); ++it) {
        const int slen = string2ssldata(*it, &srcBuff[0], rsaSize);
        const int dlen = RSA_private_decrypt(slen, &srcBuff[0], &tmpBuff[0], rsa.get(), RSA_PKCS1_PADDING);
        dstBuff += std::string((const char*) &tmpBuff[0], dlen);
    }

    // Return the decrypted string
    return dstBuff;
}

int SSLPrivateKey::Impl::passwordCallback(char* buff, int num, int /*rwflag*/, void* userdata)
{
    // Get the password
    const std::string& password = *((std::string*) userdata);

    // If the wanted length is not the same with the given password length, just return 0
    if((unsigned) num < password.length() + 1) return 0;

    // Copy the password to the buffer and return the length
    strcpy(buff, &password[0]);
    return password.length();
}

void SSLPrivateKey::Impl::tokenize(std::vector<std::string> &tokens, const std::string &source, const std::string &delimiters)
{
    // Clear the destination vector
    tokens.clear();

    // Check if the given string is empty
    if(source.empty()) return;

    // Skip delimiters at beginning
    std::string::size_type lastPos = source.find_first_not_of(delimiters, 0);

    // Find first "non-delimiter"
    std::string::size_type pos = source.find_first_of(delimiters, lastPos);

    // Tokenize it
    while((pos != std::string::npos) || (lastPos != std::string::npos)) {
        // Found a token, add it to the vector
        tokens.push_back(source.substr(lastPos, pos - lastPos));
        // Skip delimiters
        lastPos = source.find_first_not_of(delimiters, pos);
        // Find next "non-delimiter"
        pos = source.find_first_of(delimiters, lastPos);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

SSLPrivateKey::SSLPrivateKey()
: _impl( new Impl() )
{}

SSLPrivateKey::SSLPrivateKey(const SSLPrivateKey& pkey)
: _impl( pkey._impl )
{}

SSLPrivateKey::SSLPrivateKey(const std::string& password)
: _impl( new Impl(password) )
{}

SSLPrivateKey::SSLPrivateKey(const std::string& keyData, const std::string& password)
: _impl( new Impl(password) )
{ _impl->loadFromString(keyData); }

SSLPrivateKey::~SSLPrivateKey()
{}

void SSLPrivateKey::loadFromString(const std::string& keyData)
{
    _impl = new Impl(_impl->_pswd);
    _impl->loadFromString(keyData);
}

void SSLPrivateKey::loadFromFile(const std::string& fileName)
{
    _impl = new Impl(_impl->_pswd);
    _impl->loadFromFile(fileName);
}

void SSLPrivateKey::clear()
{ _impl = new Impl(); }

const std::string SSLPrivateKey::signString(const std::string& str, const char* digest) const
{ return _impl->signString(str, digest); }

const std::string SSLPrivateKey::decryptString(const std::string& str) const
{ return _impl->decryptString(str); }

evp_pkey_st* SSLPrivateKey::impl() const
{ return _impl->_pkey; }


} // namespace Pt
} // namespace Ssl
