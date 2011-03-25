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
}

/*
#define X509_LOOKUP_load_file(x,name,type) X509_LOOKUP_ctrl((x),X509_L_FILE_LOAD,(name),(long)(type),NULL)

int X509_LOOKUP_ctrl(X509_LOOKUP *ctx, int cmd, const char *argc, long argl, char **ret)
{
    if (ctx->method == NULL) return -1;
    if (ctx->method->ctrl != NULL)
        return ctx->method->ctrl(ctx,cmd,argc,argl,ret);
    else
        return 1;
}

int SSL_CTX_load_verify_locations(SSL_CTX *ctx, const char *CAfile, const char *CApath)
{
    return(X509_STORE_load_locations(ctx->cert_store,CAfile,CApath));
}

int X509_STORE_load_locations(X509_STORE *ctx, const char *file, const char *path)
{
    X509_LOOKUP *lookup;

    if (file != NULL) {
        lookup=X509_STORE_add_lookup(ctx,X509_LOOKUP_file());
        if (lookup == NULL) return(0);
        if (X509_LOOKUP_load_file(lookup,file,X509_FILETYPE_PEM) != 1)
            return(0);
    }

    if (path != NULL) {
        lookup=X509_STORE_add_lookup(ctx,X509_LOOKUP_hash_dir());
        if (lookup == NULL) return(0);
        if (X509_LOOKUP_add_dir(lookup,path,X509_FILETYPE_PEM) != 1)
            return(0);
    }

    if ((path == NULL) && (file == NULL)) return(0);

    return(1);
}

X509_LOOKUP *X509_STORE_add_lookup(X509_STORE *v, X509_LOOKUP_METHOD *m)
{
    int i;
    STACK_OF(X509_LOOKUP) *sk;
    X509_LOOKUP *lu;

    sk=v->get_cert_methods;
    for (i=0; i<sk_X509_LOOKUP_num(sk); i++) {
        lu=sk_X509_LOOKUP_value(sk,i);
        if (m == lu->method) return lu;
    }

    // a new one
    lu=X509_LOOKUP_new(m);
    if (lu == NULL)
        return NULL;
    else {
        lu->store_ctx=v;
        if (sk_X509_LOOKUP_push(v->get_cert_methods,lu))
            return lu;
        else {
            X509_LOOKUP_free(lu);
            return NULL;
        }
    }
}

int X509_load_cert_file(X509_LOOKUP *ctx, const char *file, int type) // crypto/x509/by_file.c
    {
    int ret=0;
    BIO *in=NULL;
    int i,count=0;
    X509 *x=NULL;

    if (file == NULL) return(1);
    in=BIO_new(BIO_s_file_internal());

    if ((in == NULL) || (BIO_read_filename(in,file) <= 0)) {
        X509err(X509_F_X509_LOAD_CERT_FILE,ERR_R_SYS_LIB);
        goto err;
    }

    if (type == X509_FILETYPE_PEM) {
        for (;;) {
            x=PEM_read_bio_X509_AUX(in,NULL,NULL,NULL);
            if (x == NULL)
                {
                if ((ERR_GET_REASON(ERR_peek_last_error()) ==
                    PEM_R_NO_START_LINE) && (count > 0))
                    {
                    ERR_clear_error();
                    break;
                    }
                else
                    {
                    X509err(X509_F_X509_LOAD_CERT_FILE,
                        ERR_R_PEM_LIB);
                    goto err;
                    }
                }
            i=X509_STORE_add_cert(ctx->store_ctx,x);
            if (!i) goto err;
            count++;
            X509_free(x);
            x=NULL;
        }
        ret=count;
    }
    ...
*/
} // namespace Pt
} // namespace Ssl
