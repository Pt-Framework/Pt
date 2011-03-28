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

#include "Utils.h"
#include <cstdio>

namespace Pt {
namespace Ssl {

const std::string i2s(const ASN1_INTEGER* asn1Val)
{
    long a = ASN1_INTEGER_get(asn1Val);
    char buf[1024];

    sprintf(buf, "%ld", a);

    return buf;
}

const std::string s2s(const ASN1_STRING* asn1Val)
{
    BioAutoPtr out( BIO_new(BIO_s_mem()) );
    if(!ASN1_STRING_print(out.get(), asn1Val)) return "";

    char      buf[1024];
    const int len = BIO_read(out.get(), buf, sizeof(buf));

    return len ? std::string(buf, len) : "";
}

const std::string t2s(const ASN1_TIME* asn1Val)
{
    BioAutoPtr out( BIO_new(BIO_s_mem()) );
    if(!ASN1_TIME_print(out.get(), asn1Val)) return "";

    char      buf[1024];
    const int len = BIO_read(out.get(), buf, sizeof(buf));

    return len ? std::string(buf, len) : "";
}

const std::string n2s(const X509_NAME* x509Val)
{
    BioAutoPtr out( BIO_new(BIO_s_mem()) );
    if(!X509_NAME_print(out.get(), (X509_NAME*) x509Val, 0)) return "";

    char      buf[1024];
    const int len = BIO_read(out.get(), buf, sizeof(buf));

    return len ? std::string(buf, len) : "";
}

} // namespace Pt
} // namespace Ssl
