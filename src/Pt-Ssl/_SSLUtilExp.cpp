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
#include <fstream>

#include <openssl/ssl.h>
#include <openssl/err.h>

namespace Pt {
namespace Ssl {

static BIO* pt_ssl_new_string_bio()
{
    BIO_METHOD method;
    method.type          = BIO_TYPE_NONE;
    method.name          = "std::string";
    method.bwrite        = 0;
    method.bread         = 0;
    method.bputs         = 0;
    method.bgets         = 0;
    method.ctrl          = 0;
    method.create        = 0;
    method.destroy       = 0;
    method.callback_ctrl = 0;
}

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

void pt_ssl_load_certificate_chain_string(ssl_ctx_st* ctx, const std::string& certData)
{
}


#if 0
int SSL_CTX_use_certificate_chain_file(SSL_CTX *ctx, const char *file)
{
    BIO *in;
    int ret=0;
    X509 *x=NULL;

    ERR_clear_error(); /* clear error stack for SSL_CTX_use_certificate() */

    in=BIO_new(BIO_s_file_internal());
    if (in == NULL)
        {
        SSLerr(SSL_F_SSL_CTX_USE_CERTIFICATE_CHAIN_FILE,ERR_R_BUF_LIB);
        goto end;
        }

    if (BIO_read_filename(in,file) <= 0)
        {
        SSLerr(SSL_F_SSL_CTX_USE_CERTIFICATE_CHAIN_FILE,ERR_R_SYS_LIB);
        goto end;
        }

    x=PEM_read_bio_X509_AUX(in,NULL,ctx->default_passwd_callback,ctx->default_passwd_callback_userdata);
    if (x == NULL)
        {
        SSLerr(SSL_F_SSL_CTX_USE_CERTIFICATE_CHAIN_FILE,ERR_R_PEM_LIB);
        goto end;
        }

    ret=SSL_CTX_use_certificate(ctx,x);
    if (ERR_peek_error() != 0)
        ret = 0;  /* Key/certificate mismatch doesn't imply ret==0 ... */
    if (ret)
        {
        /* If we could set up our certificate, now proceed to
         * the CA certificates.
         */
        X509 *ca;
        int r;
        unsigned long err;

        if (ctx->extra_certs != NULL)
            {
            sk_X509_pop_free(ctx->extra_certs, X509_free);
            ctx->extra_certs = NULL;
            }

        while ((ca = PEM_read_bio_X509(in,NULL,ctx->default_passwd_callback,ctx->default_passwd_callback_userdata))
            != NULL)
            {
            r = SSL_CTX_add_extra_chain_cert(ctx, ca);
            if (!r)
                {
                X509_free(ca);
                ret = 0;
                goto end;
                }
            /* Note that we must not free r if it was successfully
             * added to the chain (while we must free the main
             * certificate, since its reference count is increased
             * by SSL_CTX_use_certificate). */
            }
        /* When the while loop ends, it's usually just EOF. */
        err = ERR_peek_last_error();
        if (ERR_GET_LIB(err) == ERR_LIB_PEM && ERR_GET_REASON(err) == PEM_R_NO_START_LINE)
            ERR_clear_error();
        else
            ret = 0; /* some real error */
        }

end:
    if (x != NULL) X509_free(x);
    if (in != NULL) BIO_free(in);
    return(ret);
}
#endif


} // namespace Pt
} // namespace Ssl
