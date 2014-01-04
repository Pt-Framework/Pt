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

#include "CertificateStoreImpl.h"
#include <Pt/Ssl/SslError.h>
#include <Pt/Ssl/CertificateStore.h>
#include <Pt/System/Logger.h>
#include <vector>
#include <cassert>

log_define("Pt.Ssl.CertificateStore")

namespace Pt {

namespace Ssl {

CertificateStore::CertificateStore()
: _impl(0)
{
    _impl = new CertificateStoreImpl;
}


CertificateStore::~CertificateStore()
{
    delete _impl;
}


void CertificateStore::loadPkcs12(std::istream& is, const char* passwd)
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


void CertificateStore::loadPkcs12(const char* data, std::size_t len, const char* passwd)
{
    _impl->loadPkcs12(data, len, passwd);
}


const Certificate* CertificateStore::findCertificate(const std::string& subject)
{
    return _impl->findCertificate(subject); 
}


const Certificate& CertificateStore::getCertificate(const std::string& subject)
{
    const Certificate* cert = _impl->findCertificate(subject);
    if( ! cert)
        throw InvalidCertificate("missing certificate");

    return *cert;
}


std::size_t CertificateStore::size() const
{
    return _impl->size(); 
}

CertificateStore::ConstIterator CertificateStore::begin() const
{ 
    return _impl->begin(); 
}


CertificateStore::ConstIterator CertificateStore::end() const
{
    return _impl->end(); 
}


CertificateStore::ConstIterator::ConstIterator()
: _cert(0)
{}


CertificateStore::ConstIterator::ConstIterator(const CertificateStore::ConstIterator& other)
: _cert(other._cert)
{}


CertificateStore::ConstIterator::ConstIterator(Certificate* const* cert)
: _cert(cert)
{}


CertificateStore::ConstIterator& CertificateStore::ConstIterator::operator=(const CertificateStore::ConstIterator& other)
{
    _cert = other._cert;
    return *this;
}


CertificateStore::ConstIterator& CertificateStore::ConstIterator::operator++()
{
    _cert++;
    return *this;
}


const Certificate& CertificateStore::ConstIterator::operator*() const
{ 
    return **_cert; 
}


const Certificate* CertificateStore::ConstIterator::operator->() const
{ 
    return *_cert; 
}


bool CertificateStore::ConstIterator::operator!=(const CertificateStore::ConstIterator& other) const
{ 
    return _cert != other._cert; 
}


bool CertificateStore::ConstIterator::operator==(const CertificateStore::ConstIterator& other) const
{ 
    return _cert == other._cert; 
}

} // namespace Ssl

} // namespace Pt
