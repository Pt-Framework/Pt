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
#include <fstream>

#include "Utils.h"

namespace Pt {
namespace Ssl {

SSLPublicKey::Impl::Impl()
: _pkey(0)
{}

SSLPublicKey::Impl::Impl(EVP_PKEY* pkey)
: _pkey(pkey)
{}

SSLPublicKey::Impl::~Impl()
{ clear(); }

void SSLPublicKey::Impl::loadFromString(const std::string& keyData)
{
    // Clear previous key (if any)
    clear();

    // Create a read-only memory BIO from the given string
    BioAutoPtr in( BIO_new_mem_buf( (void*) keyData.c_str(), keyData.length() ) );

    // Try to read/parse the public key
    _pkey = PEM_read_bio_PUBKEY(in.get(), 0, 0, 0);//SSLPublicKey::Impl::passwordCallback, (void*) &_pswd);
    if(!_pkey)
        throw SSLRuntimeError("Could not read/parse/decode public-key data!", PT_SOURCEINFO);
}

void SSLPublicKey::Impl::loadFromFile(const std::string& fileName)
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

void SSLPublicKey::Impl::clear()
{
    if(_pkey) {
        EVP_PKEY_free(_pkey);
        _pkey = 0;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

SSLPublicKey::SSLPublicKey()
: _impl( new Impl() )
{}

SSLPublicKey::SSLPublicKey(const SSLPublicKey& pkey)
: _impl( pkey._impl )
{}

SSLPublicKey::SSLPublicKey(const std::string& keyData)
: _impl( new Impl() )
{ _impl->loadFromString(keyData); }

SSLPublicKey::~SSLPublicKey()
{}

void SSLPublicKey::loadFromString(const std::string& keyData)
{
    _impl = new Impl();
    _impl->loadFromString(keyData);
}

void SSLPublicKey::loadFromFile(const std::string& fileName)
{
    _impl = new Impl();
    _impl->loadFromFile(fileName);
}

void SSLPublicKey::clear()
{ _impl = new Impl(); }

////////////////////////////////////////////////////////////////////////////////////////////////////

SSLPublicKey::SSLPublicKey(EVP_PKEY* pkey)
: _impl( new Impl(pkey) )
{}

evp_pkey_st* SSLPublicKey::impl() const
{ return _impl->_pkey; }
    
} // namespace Ssl
} // namespace Pt

