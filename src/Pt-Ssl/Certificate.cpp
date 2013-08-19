/*
 * Copyright (C) 2010-2013 by Marc Duerner
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
#include "CertificateImpl.h"
#include <Pt/Ssl/Certificate.h>

namespace Pt {

namespace Ssl {

//Certificate::Certificate()
//: _impl(0)
//{
//}


Certificate::Certificate(CertificateImpl* impl)
: _impl(impl)
{
}


//Certificate::Certificate(const Certificate& cert)
//: _impl(cert._impl)
//{
//    if(_impl)
//        _impl->ref();
//}


Certificate::~Certificate()
{
    if( _impl && 0 == _impl->unref() )
    {
        delete _impl;
    }
}


//Certificate& Certificate::operator=(const Certificate& cert)
//{
//    if( _impl && 0 == _impl->unref() )
//    {
//        delete _impl;
//    }
//
//    _impl = cert._impl;
//
//    if(_impl)
//        _impl->ref();
//
//    return *this;
//}


int Certificate::serialNumber() const
{
    if( ! _impl)
        return 0;

    return _impl->serialNumber();
}


std::string Certificate::issuer() const
{
    if( ! _impl)
        return std::string();

    return _impl->issuer();
}


std::string Certificate::subject() const
{
    if( ! _impl)
        return std::string();

    return _impl->subject();
}
   
        
std::string Certificate::notBefore() const
{
    if( ! _impl)
        return std::string();

    return _impl->notBefore();
}


std::string Certificate::notAfter() const
{
    if( ! _impl)
        return std::string();

    return _impl->notAfter();
}


CertificateImpl* Certificate::impl() const
{
    if( ! _impl)
        throw std::logic_error("invalid certificate implementation");

    return _impl;
}

} // namespace Ssl

} // namespace Pt


