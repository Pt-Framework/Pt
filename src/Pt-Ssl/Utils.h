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
#ifndef PT_SSL_UTILS_H
#define PT_SSL_UTILS_H

#include <Pt/SmartPtr.h>

#include <openssl/ssl.h>
#include <openssl/pem.h>
#include <openssl/err.h>

namespace Pt {
namespace Ssl {

//
// Internal SSL classes, typedefs, and functions
//

// Used to automatically free a BIO*
class FreeBIO {
    protected:
        void destroy(BIO* ptr)
        { BIO_free(ptr); }
};
typedef Pt::AutoPtr<BIO, FreeBIO> BioAutoPtr;

// Used to automatically free an X509*
class FreeX509 {
    protected:
        void destroy(X509* ptr)
        { X509_free(ptr); }
};
typedef Pt::AutoPtr<X509, FreeX509> X509AutoPtr;

// Used to automatically free an EVP_PKEY*
class FreeEVP_PKEY {
    protected:
        void destroy(EVP_PKEY* ptr)
        { EVP_PKEY_free(ptr); }
};
typedef Pt::AutoPtr<EVP_PKEY, FreeEVP_PKEY> EvpPKeyAutoPtr;

// Used to automatically free an EVP_MD_CTX*
class FreeEVP_MD_CTX {
    protected:
        void destroy(EVP_MD_CTX* mctx)
        { EVP_MD_CTX_cleanup(mctx); }
};
typedef Pt::AutoPtr<EVP_MD_CTX, FreeEVP_MD_CTX > EvpMdCtxAutoPtr;

// Used to automatically free an RSA*
class FreeRSA {
    protected:
        void destroy(RSA* rsa)
        { RSA_free(rsa); }
};
typedef Pt::AutoPtr<RSA, FreeRSA > RsaAutoPtr;

// Used to automatically free an EVP_CIPHER_CTX*
class FreeEVP_CIPHER_CTX {
    protected:
        void destroy(EVP_CIPHER_CTX* cctx)
        { EVP_CIPHER_CTX_cleanup(cctx); }
};
typedef Pt::AutoPtr<EVP_CIPHER_CTX, FreeEVP_CIPHER_CTX> EvpCipherCtxAutoPtr;

// Converter functions for converting OpenSSL values into STL strings
extern const std::string asn1int2string(const ASN1_INTEGER* asn1Val);
extern const std::string asn1str2string(const ASN1_STRING* asn1Val);
extern const std::string asn1tim2string(const ASN1_TIME* asn1Val);
extern const std::string x509nam2string(const X509_NAME* x509Val);

extern const std::string sslhash2string(long md);
extern const std::string sslhash2string(const unsigned char* md, unsigned int n);

extern const std::string ssldata2string(const unsigned char* md, unsigned int n);
extern       void        string2ssldata(const std::string& str, unsigned char* md, unsigned int nmax);

} // namespace Pt
} // namespace Ssl

#endif
