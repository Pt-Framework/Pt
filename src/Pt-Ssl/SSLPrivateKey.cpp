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
    std::string data;
    readFileToString(fileName, data);
    loadFromString(data);
}

void SSLPrivateKey::Impl::clear()
{
    if(_pkey) {
        EVP_PKEY_free(_pkey);
        _pkey = 0;
    }
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

////////////////////////////////////////////////////////////////////////////////////////////////////

evp_pkey_st* SSLPrivateKey::impl() const
{ return _impl->_pkey; }


} // namespace Ssl
} // namespace Pt
