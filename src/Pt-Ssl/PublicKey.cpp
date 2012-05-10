/*
 * Copyright (C) 2010-2010 by Aloysius Indrayanto
 * Copyright (C) 2010-2012 by Marc Duerner
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

#include "OpenSsl.h"
#include <Pt/Ssl/PublicKey.h>
#include <iostream>
#include <fstream>

namespace Pt {

namespace Ssl {

PublicKey::PublicKey()
: _impl( new PublicKeyImpl() )
{
}


PublicKey::PublicKey(const PublicKey& pkey)
: _impl( pkey._impl )
{
}


PublicKey::~PublicKey()
{
}


void PublicKey::fromPem(const char* data, size_t len)
{
    _impl = new PublicKeyImpl();
    _impl->fromPem(data, len);
}


void PublicKey::fromPem(std::istream& is)
{
    _impl = new PublicKeyImpl();
    _impl->fromPem(is);
}


void PublicKey::fromPemFile(const char* fileName)
{
    _impl = new PublicKeyImpl();
    _impl->fromPemFile(fileName);
}


void PublicKey::toPem(std::ostream& os) const
{
    _impl->toPem(os);
}


void PublicKey::clear()
{ 
    _impl = new PublicKeyImpl(); 
}


PublicKey::PublicKey(EVP_PKEY* pkey)
: _impl( new PublicKeyImpl(pkey) )
{
}


evp_pkey_st* PublicKey::impl() const
{ 
    return _impl->_pkey; 
}


PublicKeyImpl::PublicKeyImpl()
: _pkey(0)
{
}


PublicKeyImpl::PublicKeyImpl(EVP_PKEY* pkey)
: _pkey(pkey)
{
}


PublicKeyImpl::~PublicKeyImpl()
{ 
    clear();
}


void PublicKeyImpl::fromPem(const char* data, size_t len)
{
    // Clear previous key (if any)
    clear();

    // Create a read-only memory BIO from the given string
    BioAutoPtr in( BIO_new_mem_buf( (void*) data, len ) );

    // Try to read/parse the public key
    _pkey = PEM_read_bio_PUBKEY(in.get(), 0, 0, 0);//SSLPublicKey::Impl::passwordCallback, (void*) &_pswd);
    if(!_pkey)
        throw InvalidKey("Could not read/parse/decode public-key data!");
}


void PublicKeyImpl::fromPem(std::istream& is)
{
    char rbuf[4096];
    const std::streamsize rbufSize = sizeof(rbuf);
    std::string data;
    while( is ) 
    {
        is.read(rbuf, rbufSize);
        size_t count = size_t( is.gcount() );
        data.append(rbuf, count);
    }

    fromPem( data.c_str(), data.size() );
}


void PublicKeyImpl::fromPemFile(const char* path)
{
    std::ifstream ifs(path, std::ios::binary);
    fromPem(ifs);
}


void PublicKeyImpl::toPem(std::ostream& os) const
{
    BioAutoPtr out( BIO_new(BIO_s_mem()) );

    if( ! _pkey)
        return;

    int ret = PEM_write_bio_PUBKEY(out.get(), _pkey);
    if( ! ret)
        throw InvalidKey("Could not write key in pem format");

    char* data = 0;
    long len = BIO_get_mem_data(out.get(), &data);
    os.write(data, len);
}


void PublicKeyImpl::clear()
{
    if(_pkey) {
        EVP_PKEY_free(_pkey);
        _pkey = 0;
    }
}
    
} // namespace Ssl

} // namespace Pt
