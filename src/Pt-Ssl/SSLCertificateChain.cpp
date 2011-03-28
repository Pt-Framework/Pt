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

#include <Pt/Ssl/SSLCertificateChain.h>
#include <fstream>

#include "Utils.h"

namespace Pt {
namespace Ssl {

SSLCertificateChain::SSLCertificateChain()
: _cert(0)
{}

SSLCertificateChain::SSLCertificateChain(const std::string& certData)
: _cert(0)
{ loadFromString(certData); }

SSLCertificateChain::~SSLCertificateChain()
{ clear(); }

void SSLCertificateChain::loadFromString(const std::string& certData)
{
    clear();

    // Create a read-only memory BIO from the given string
    BioAutoPtr in( BIO_new_mem_buf( (void*) certData.c_str(), certData.length() ) );

    // Try to read/parse the X509 certificate
    _cert = PEM_read_bio_X509_AUX(in.get(), 0, 0, 0);
    if(!_cert)
        throw SSLRuntimeError("Could not read/parse certificate data!", PT_SOURCEINFO);

    // Load CA certificates (if any)
    while(true) {
        X509* ca = PEM_read_bio_X509_AUX(in.get(), 0, 0, 0);
        if(!ca) break;
        _caCert.push_back(ca);
    }
}

void SSLCertificateChain::loadFromFile(const std::string& fileName)
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

void SSLCertificateChain::clear()
{
    if(_cert) {
        X509_free(_cert);
        _cert = 0;
    }

    for(std::vector<x509_st*>::const_iterator it = _caCert.begin(); it != _caCert.end(); ++it) {
        X509_free(*it);
    }
    _caCert.clear();
}

} // namespace Pt
} // namespace Ssl
