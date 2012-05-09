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

#include "Utils.h"
#include <fstream>
#include <cstdio>

namespace Pt {

namespace Ssl {

std::string asn1str2string(ASN1_STRING* asn1Val)
{
    BioAutoPtr out( BIO_new(BIO_s_mem()) );
    if(!ASN1_STRING_print(out.get(), asn1Val)) return "";

    char      buf[1024];
    const int len = BIO_read(out.get(), buf, sizeof(buf));

    return len ? std::string(buf, len) : "";
}

std::string asn1tim2string(ASN1_TIME* asn1Val)
{
    BioAutoPtr out( BIO_new(BIO_s_mem()) );
    if(!ASN1_TIME_print(out.get(), asn1Val)) return "";

    char      buf[1024];
    const int len = BIO_read(out.get(), buf, sizeof(buf));

    return len ? std::string(buf, len) : "";
}

std::string x509nam2string(const X509_NAME* x509Val)
{
    BioAutoPtr out( BIO_new(BIO_s_mem()) );
    if(!X509_NAME_print(out.get(), (X509_NAME*) x509Val, 0)) return "";

    char      buf[1024];
    const int len = BIO_read(out.get(), buf, sizeof(buf));

    return len ? std::string(buf, len) : "";
}

std::string sslhash2string(long md)
{
    char buf[1024];
    sprintf(buf, "%08lx", md);
    return buf;
}

std::string sslhash2string(const unsigned char* md, unsigned int n)
{
    std::string hash;

    char buf[1024];
    for(unsigned int i = 0; i < n; ++i) {
        sprintf(buf, "%02X", md[i]);
        if(!hash.empty()) hash += ':';
        hash += buf;
    }

    return hash;
}

std::string ssldata2string(const unsigned char* md, unsigned int n)
{
    std::string hash;

    char buf[1024];
    for(unsigned int i = 0; i < n; ++i) {
        sprintf(buf, "%02X", md[i]);
        hash += buf;
    }

    return hash;
}

unsigned int string2ssldata(const std::string& str, unsigned char* md, unsigned int nmax)
{
    const char*          ptrcur = str.c_str();
    const char*          ptrmax = ptrcur + str.length();
    unsigned char*       mdcur  = md;
    const unsigned char* mdmax  = mdcur + nmax;
    char                 cnv[3] = { 0, 0, 0 };

    for(;;) {
        if(ptrcur >= ptrmax || md >= mdmax) break;

        cnv[0] = *ptrcur++;
        cnv[1] = (ptrcur < ptrmax) ? (*ptrcur++) : 0;

        *mdcur++ = static_cast<unsigned char>( strtoul(cnv, 0, 16) );
    }

    return mdcur - md;
}

unsigned int string2ssldata(const char* str, int slen, unsigned char* md, unsigned int nmax)
{
    const char*          ptrcur = str;
    const char*          ptrmax = ptrcur + slen;
          unsigned char* mdcur  = md;
    const unsigned char* mdmax  = mdcur + nmax;
    char                 cnv[3] = { 0, 0, 0 };

    for(;;) {
        if(ptrcur >= ptrmax || md >= mdmax) break;

        cnv[0] = *ptrcur++;
        cnv[1] = (ptrcur < ptrmax) ? (*ptrcur++) : 0;

        *mdcur++ = static_cast<unsigned char>( strtoul(cnv, 0, 16) );
    }

    return mdcur - md;
}

} // namespace Pt

} // namespace Ssl
