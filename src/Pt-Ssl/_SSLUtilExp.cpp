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

#include <Pt/Ssl/_SSLUtilExp.h>
#include <Pt/SmartPtr.h>
#include <fstream>

#include <openssl/ssl.h>
#include <openssl/err.h>

namespace Pt {
namespace Ssl {

////////////////////////////////////////////////////////////////////////////////////////////////////

void pt_ssl_load_certificate_chain_file(ssl_ctx_st* ctx, const char *file)
{
    std::ifstream ifs;
    char          rbuf[4096];
    std::string   data;

    ifs.open(file, std::ios::binary);
    while(ifs) {
        ifs.read( rbuf, sizeof(rbuf) );
        data += std::string( rbuf, ifs.gcount() );
    }

    pt_ssl_load_certificate_chain_string(ctx, data);
}


class FreeBIO
{
    protected:
        void destroy(BIO* ptr)
        { BIO_free(ptr); }
};

class FreeX509
{
    protected:
        void destroy(X509* ptr)
        { X509_free(ptr); }
};

typedef Pt::AutoPtr<BIO, FreeBIO> BioAutoPtr;
typedef Pt::AutoPtr<X509, FreeX509> X509AutoPtr;

void pt_ssl_load_certificate_chain_string(ssl_ctx_st* ctx, const std::string& certData)
{
    // Create a read-only memory BIO from the given string
    BioAutoPtr in;
    in = BIO_new_mem_buf( (void*) certData.c_str(), certData.length() );

    // Try to read/parse the X509 certificate
    X509AutoPtr x509;
    x509 = PEM_read_bio_X509_AUX(in.get(), 0,0, 0);
    if( ! x509 ) {
        // TODO: How to prevent writing these freeing code multiple times?
        //BIO_free(in);
        throw SSLRuntimeError("Could not read/parse certificate data!", PT_SOURCEINFO);
    }

    // Try to use the X509 certificate
    ERR_clear_error();
    if( ! SSL_CTX_use_certificate( ctx, x509.get() ) || ERR_peek_error() ) {
        // TODO: How to prevent writing these freeing code multiple times?
        /// BIO_free(in);
        //X509_free(x509);
        throw SSLRuntimeError("Invalid/mismatched certificate!", PT_SOURCEINFO);
    }

    // Clear the previous CA certificates (if any)
    // TODO: Accessing a structure member!
    //           1. Can we sure that extra_certs will always exist
    //           2. Is there any public API that can work directly on the SSL_CTX object?
    if(ctx->extra_certs) {
        sk_X509_pop_free(ctx->extra_certs, X509_free);
        ctx->extra_certs = 0;
    }

    // Load CA certificates (if any)
    X509AutoPtr ca;
    while( ca = PEM_read_bio_X509(in.get(), 0, 0, 0) ) {
        if( ! SSL_CTX_add_extra_chain_cert( ctx, ca.get() ) ) {
            // TODO: How to prevent writing these freeing code multiple times?
            ///BIO_free(in);
            //X509_free(x509);
            //X509_free(ca);
            throw SSLRuntimeError("Could not read/parse CA certificate data!", PT_SOURCEINFO);
            break;
        }
        ca.release();
    }

    // Done
    // TODO: How to prevent writing these freeing code multiple times?
    ///BIO_free(in);
    //X509_free(x509);
}

} // namespace Pt
} // namespace Ssl
