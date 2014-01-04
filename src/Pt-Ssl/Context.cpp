/*
 * Copyright (C) 2010-2010 by Aloysius Indrayanto
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

#include "ContextImpl.h"
#include <Pt/Ssl/Context.h>

namespace Pt {

namespace Ssl {


SSLInit::SSLInit()
{
    SSLInitImpl();
}


SSLInit::~SSLInit()
{
    SSLExitImpl();
}


Context::Context()
: _impl(0)
{
    _impl = new ContextImpl(SSLv3);
}


Context::Context(Protocol protocol)
: _impl(0)
{
    _impl = new ContextImpl(protocol);
}


Context::~Context()
{
    delete _impl;
}


Protocol Context::protocol() const
{ 
    return _impl->protocol(); 
}


void Context::setProtocol(Protocol protocol)
{
    _impl->setProtocol(protocol);
}


void Context::setVerifyDepth(int n)
{
    _impl->setVerifyDepth(n);
}


VerifyMode Context::verifyMode() const
{
    return _impl->verifyMode();
}


void Context::setVerifyMode(VerifyMode m)
{
    _impl->setVerifyMode(m);
}


void Context::assign(const Context& ctx)
{
    const ContextImpl* impl = ctx.impl();
    _impl->assign( *impl );
}


void Context::addCACertificate(const Certificate& trustedCert)
{
    _impl->addCACertificate(trustedCert);
}


void Context::setIdentity(const Certificate& cert)
{
    _impl->setIdentity(cert);
}


void Context::addCertificate(const Certificate& cert)
{
    _impl->addCertificate(cert);
}


ContextImpl* Context::impl()
{ 
    return _impl; 
}


const ContextImpl* Context::impl() const
{ 
    return _impl; 
}

} // namespace Ssl

} // namespace Pt
