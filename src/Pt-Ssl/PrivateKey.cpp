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
#include "PrivateKeyImpl.h"
#include <Pt/Ssl/PrivateKey.h>

namespace Pt {

namespace Ssl {

PrivateKey::PrivateKey(PrivateKeyImpl* impl)
: _pswd()
, _impl(impl)
{
}


PrivateKey::PrivateKey()
: _pswd()
, _impl( 0 )
{
}


PrivateKey::PrivateKey(const std::string& password)
: _pswd(password)
, _impl(0)
{
}


PrivateKey::PrivateKey(const PrivateKey& pkey)
: _impl( pkey._impl )
, _pswd( pkey._pswd )
{
    if(_impl)
      _impl->ref();
}


PrivateKey::~PrivateKey()
{
    if( _impl && 0 == _impl->unref() )
    {
        delete _impl;
    }
}


PrivateKey& PrivateKey::operator=(const PrivateKey& key)
{
    if( _impl && 0 == _impl->unref() )
    {
        delete _impl;
    }

    _impl = key._impl;

    if(_impl)
        _impl->ref();

    _pswd = key._pswd;
    return *this;
}


void PrivateKey::fromPem(const char* data, std::size_t len)
{
    detach();
    _impl->fromPem(data, len);
}


void PrivateKey::fromPem(std::istream& is)
{
    detach();
    _impl->fromPem(is);
}


void PrivateKey::fromPemFile(const char* fileName)
{
    detach();
    _impl->fromPemFile(fileName);
}


void PrivateKey::toPem(std::ostream& os) const
{
    _impl->toPem(os);
}


void PrivateKey::detach()
{
    PrivateKeyImpl* new_impl = new PrivateKeyImpl(_pswd);

    if( _impl && 0 == _impl->unref() )
    {
        delete _impl;
    }
    
    _impl = new_impl;
}


evp_pkey_st* PrivateKey::impl() const
{ 
    return _impl? _impl->evp() : 0; 
}

} // namespace Ssl

} // namespace Pt
