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

    static int passwordCallback(char* buff, int num, int /*rwflag*/, void* userdata)
{
    // Get the password
    const std::string& password = *((std::string*) userdata);

    // If the wanted length is not the same with the given password length, just return 0
    if((unsigned) num < password.length() + 1) return 0;

    // Copy the password to the buffer and return the length
    strcpy(buff, &password[0]);
    return password.length();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

SSLPrivateKey::SSLPrivateKey(const std::string& password)
: _pswd(password), _pkey(0)
{}

SSLPrivateKey::SSLPrivateKey(const std::string& keyData, const std::string& password)
: _pswd(password), _pkey(0)
{ loadFromString(keyData); }

SSLPrivateKey::~SSLPrivateKey()
{ clear(); }

void SSLPrivateKey::loadFromString(const std::string& keyData)
{
    // Clear previous key (if any)
    clear();

    // Create a read-only memory BIO from the given string
    BioAutoPtr in( BIO_new_mem_buf( (void*) keyData.c_str(), keyData.length() ) );

    // Try to read/parse the private key
    _pkey = PEM_read_bio_PrivateKey(in.get(), 0, passwordCallback, (void*) &_pswd);
    if(!_pkey)
        throw SSLRuntimeError("Could not read/parse/decode private-key data!", PT_SOURCEINFO);
}

void SSLPrivateKey::loadFromFile(const std::string& fileName)
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

void SSLPrivateKey::clear()
{
    if(_pkey) {
        EVP_PKEY_free(_pkey);
        _pkey = 0;
    }
}

void SSLPrivateKey::apply(SSL_CTX* ctx)
{
    if(!_pkey)
        throw SSLRuntimeError("Trying to use an empty private key!", PT_SOURCEINFO);

    // Try to use the private key
    if( ! SSL_CTX_use_PrivateKey( ctx, _pkey ) )
        throw SSLRuntimeError("Invalid private-key!", PT_SOURCEINFO);
}

} // namespace Pt
} // namespace Ssl
