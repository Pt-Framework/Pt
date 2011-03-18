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
#ifndef PT_SSL_SSLCONTEXT_H
#define PT_SSL_SSLCONTEXT_H

//#undef NLOG

#include <string>
#include <openssl/ssl.h>
#include <openssl/err.h>

#include <Pt/Ssl/Exception.h>
#include <Pt/System/Logger.h>

namespace Pt {
namespace Ssl {

//! @internal Library initialization.
static struct Init
{
    Init();
    ~Init();
} ssl_init;

//! \brief SSL context.
class PT_SSL_API SSLContext {
    public:
        typedef std::streambuf::int_type int_type;

    public:
        /** \brief Construct an SSL context that uses the given certificate-key file and password.
         *
         * The 'caCertFile' is needed if you would like to check if the other peer's certificate is signed by a valid Certificate Authority.
         * The 'certFile' and 'keyFile' is mandatory for a server context.
         * A client context will only need 'certFile' and 'keyFile' if it is to be used for certificate-based client authentication.
         * The 'password' is only needed if the 'keyFile' is encrypted.
         */
        SSLContext(const char* caCertFile, const char* certFile, const char* keyFile, const char* password, const char* sessionID = 0);

        //! \brief Standard dtor.
        ~SSLContext();

        friend class SSLStreamBuf;

    private:
        SSL_CTX*    _ctx;  // OpenSSL's SSL context
        std::string _pswd; // The password

        // Password callback to feed the password to OpenSSL
        static int _passwordCallback(char* buf, int num, int rwflag, void* userdata);

#ifndef NLOG
    public:
        static const std::string pt_ssl_gen_call_info(const char* className, const std::string& funcName);
#endif
};


} // namespace Pt
} // namespace Ssl

#endif
