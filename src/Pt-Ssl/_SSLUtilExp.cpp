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

class FreeBIO {
    protected:
        void destroy(BIO* ptr)
        { BIO_free(ptr); }
};

class FreeX509 {
    protected:
        void destroy(X509* ptr)
        { X509_free(ptr); }
};

class FreeEVP_PKEY {
    protected:
        void destroy(EVP_PKEY* ptr)
        { EVP_PKEY_free(ptr); }
};

typedef Pt::AutoPtr<BIO,      FreeBIO     > BioAutoPtr;
typedef Pt::AutoPtr<X509,     FreeX509    > X509AutoPtr;
typedef Pt::AutoPtr<EVP_PKEY, FreeEVP_PKEY> EvpPKeyAutoPtr;


////////////////////////////////////////////////////////////////////////////////////////////////////

static void readFileToString(const char *file, std::string& dst)
{
    dst.clear();

    std::ifstream ifs;
    char          rbuf[4096];

    ifs.open(file, std::ios::binary);
    while(ifs) {
        ifs.read( rbuf, sizeof(rbuf) );
        dst += std::string( rbuf, ifs.gcount() );
    }
}

static int passwordCallback(char* buff, int num, int /*rwflag*/, void* userdata)
{
    // Get the password
    const std::string& password = *((std::string*) userdata);

    // If the wanted length is not the same with the given password length, just return 0
    if((unsigned) num < password.length() + 1) return 0;

    // Copy the password to the buffer and return the length
    strcpy(buff, &password[0]);
    return password.length();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void pt_ssl_load_certificate_chain_file(ssl_ctx_st* ctx, const char* file)
{
    std::string data;
    readFileToString(file, data);
    pt_ssl_load_certificate_chain_string(ctx, data);
}

void pt_ssl_load_certificate_chain_string(ssl_ctx_st* ctx, const std::string& certData)
{
    // Create a read-only memory BIO from the given string
    BioAutoPtr in( BIO_new_mem_buf( (void*) certData.c_str(), certData.length() ) );

    // Try to read/parse the X509 certificate
    X509AutoPtr x509( PEM_read_bio_X509_AUX(in.get(), 0, 0, 0) );
    if( ! x509 )
        throw SSLRuntimeError("Could not read/parse certificate data!", PT_SOURCEINFO);

    // Try to use the X509 certificate
    ERR_clear_error();
    if( ! SSL_CTX_use_certificate( ctx, x509.get() ) || ERR_peek_error() )
        throw SSLRuntimeError("Invalid/mismatched certificate!", PT_SOURCEINFO);

    // Clear the previous CA certificates (if any)
    if(ctx->extra_certs) {
        sk_X509_pop_free(ctx->extra_certs, X509_free);
        ctx->extra_certs = 0;
    }

    // Load CA certificates (if any)
    X509AutoPtr ca;
    while( ca = PEM_read_bio_X509(in.get(), 0, 0, 0) ) {
        if( ! SSL_CTX_add_extra_chain_cert( ctx, ca.get() ) )
            throw SSLRuntimeError("Could not read/parse CA certificate data!", PT_SOURCEINFO);
        ca.release();
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void pt_ssl_load_private_key_file(ssl_ctx_st* ctx, const char* file, const char* password)
{
    std::string data;
    readFileToString(file, data);
    pt_ssl_load_private_key_string(ctx, data, password ? password : "");
}

void pt_ssl_load_private_key_string(ssl_ctx_st* ctx, const std::string& keyData, const std::string& password)
{
    // Create a read-only memory BIO from the given string
    BioAutoPtr in( BIO_new_mem_buf( (void*) keyData.c_str(), keyData.length() ) );

    // Try to read/parse the private key
    EvpPKeyAutoPtr pkey( PEM_read_bio_PrivateKey(in.get(), 0, passwordCallback, (void*) &password) );
    if( ! pkey )
        throw SSLRuntimeError("Could not read/parse/decode private-key data!", PT_SOURCEINFO);

    // Try to use the private key
    if( ! SSL_CTX_use_PrivateKey( ctx, pkey.get() ) )
        throw SSLRuntimeError("Invalid private-key!", PT_SOURCEINFO);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void pt_ssl_load_trusted_ca_list_file(ssl_ctx_st* ctx, const char* file)
{
    std::string data;
    readFileToString(file, data);
    pt_ssl_load_trusted_ca_list_string(ctx, data);
}

void pt_ssl_load_trusted_ca_list_string(ssl_ctx_st* ctx, const std::string& certData)
{
    // Create a read-only memory BIO from the given string
    BioAutoPtr in( BIO_new_mem_buf( (void*) certData.c_str(), certData.length() ) );

    // Try to read/parse the X509 certificate
    X509AutoPtr x509;

    int count = 0;
    while(true) {
        x509 = PEM_read_bio_X509_AUX(in.get(), 0, 0, 0);
        if( ! x509 ) {
            if((ERR_GET_REASON(ERR_peek_last_error()) != PEM_R_NO_START_LINE) && (count > 0))
                throw SSLRuntimeError("Could not read/parse CA certificate data!", PT_SOURCEINFO);
            ERR_clear_error();
            break;
        }

        if( ! X509_STORE_add_cert(ctx->cert_store, x509.get()) )
            throw SSLRuntimeError("Could not store the CA certificate!", PT_SOURCEINFO);

        ++count;
    }
}

} // namespace Pt
} // namespace Ssl
