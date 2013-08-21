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
#ifndef PT_SSL_OPENSSL_H
#define PT_SSL_OPENSSL_H

#include <Pt/Ssl/Api.h>
#include <Pt/SmartPtr.h>
#include <openssl/ssl.h>
#include <openssl/crypto.h>
#include <openssl/err.h>
#include <openssl/pkcs12.h>
//#include <openssl/pem.h>

namespace Pt {

namespace Ssl {

class FreeBIO {
    protected:
        void destroy(BIO* ptr)
        { BIO_free(ptr); }
};

typedef Pt::AutoPtr<BIO, FreeBIO> BioAutoPtr;


class FreeX509 {
    protected:
        void destroy(X509* ptr)
        { X509_free(ptr); }
};

typedef Pt::AutoPtr<X509, FreeX509> X509AutoPtr;


class FreeX509_STORE {
    protected:
        void destroy(X509_STORE* ptr)
        { X509_STORE_free(ptr); }
};

typedef Pt::AutoPtr<X509_STORE, FreeX509_STORE> X509StoreAutoPtr;


class FreeX509_STACK {
    protected:
        void destroy(STACK_OF(X509)* ptr)
        { sk_X509_pop_free(ptr, X509_free); }
};

typedef Pt::AutoPtr<STACK_OF(X509), FreeX509_STACK> X509StackAutoPtr;

} // namespace Ssl

} // namespace Pt

/*   
std::string asn1str2string(ASN1_STRING* asn1Val)
{
    BioAutoPtr out( BIO_new(BIO_s_mem()) );
    if(!ASN1_STRING_print(out.get(), asn1Val)) return "";

    char      buf[1024];
    const int len = BIO_read(out.get(), buf, sizeof(buf));

    return len ? std::string(buf, len) : "";
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

void dumpCertInfo(X509* ptrCert)

    // For calculating the fingerprint hash of the certificate
    const EVP_MD* fdig = EVP_sha1();
    unsigned char md[EVP_MAX_MD_SIZE];
    unsigned int  n;

    if( ! X509_digest(ptrCert, fdig, md, &n) )
        throw InvalidCertificate("Could not calculate the certificate's fingerprint hash!");

    X509_get_version(ptrCert),
    sslhash2string  ( X509_issuer_name_hash (ptrCert) ),
    sslhash2string  ( X509_subject_name_hash(ptrCert) ),
    OBJ_nid2sn( EVP_MD_type(fdig) ),
    sslhash2string(md, n) );
}
*/

#endif // PT_SSL_OPENSSL_H
