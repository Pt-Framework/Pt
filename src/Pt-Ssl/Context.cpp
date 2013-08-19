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

#ifndef __APPLE__
    #include "OpenSsl.h"
#endif

#include "ContextImpl.h"
#include "CertificateImpl.h"
#include <Pt/Ssl/Context.h>
#include <Pt/Ssl/SslError.h>
#include <Pt/System/Mutex.h>
#include <Pt/System/Logger.h>
#include <openssl/ssl.h>
#include <openssl/crypto.h>
#include <openssl/err.h>
#include <cstdio>

log_define("Pt.Ssl.Context")

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


Context::Context(Protocol protocol)
: _impl(0)
{
    _impl = new ContextImpl(protocol);
}


Context::~Context()
{
    delete _impl;
}


Context::Protocol Context::protocol() const
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


Context::VerifyMode Context::verification() const
{
    return _impl->verification();
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


//void Context::setCACertificates(const CertificateStore& caCerts)
//{
//    _impl->setCACertificates(caCerts);
//}


void Context::addCACertificate(const Certificate& trustedCert)
{
    _impl->addCACertificate(trustedCert);
}


void Context::setCertificate(const Certificate& cert)
{
    _impl->setCertificate(cert);
}


void Context::loadPkcs12(const char* data, size_t len, const char* passwd)
{
    _impl->loadPkcs12(data, len, passwd);
}


const Certificate* Context::findCertificate(const std::string& subject)
{
    return _impl->findCertificate(subject);
}


ContextImpl* Context::impl()
{ 
    return _impl; 
}


const ContextImpl* Context::impl() const
{ 
    return _impl; 
}


void Context::loadPkcs12(std::istream& is, const char* passwd)
{
    std::vector<char> data;
    char rbuf[4096];
    const std::streamsize rbufSize = sizeof(rbuf);

    while( is )
    {
        is.read( rbuf, rbufSize );
        data.insert( data.end(), rbuf, rbuf + is.gcount() );
    }

    if( data.empty() )
        return;

    loadPkcs12(&data[0], data.size(), passwd);
}

} // namespace Ssl

} // namespace Pt
