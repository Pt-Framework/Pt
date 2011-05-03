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

namespace Pt {
namespace Ssl {

BasicSymmetricCipher::BasicSymmetricCipher(std::iostream& ios)
: BasicCipher(ios), _mode(Invalid)
{}

BasicSymmetricCipher::~BasicSymmetricCipher()
{}

void BasicSymmetricCipher::startEncrypt(const std::string& password)
{
    _mode = Encrypt;
    _pswd = password;
}

void BasicSymmetricCipher::startDecrypt(const std::string& password)
{
    _mode = Decrypt;
    _pswd = password;
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

