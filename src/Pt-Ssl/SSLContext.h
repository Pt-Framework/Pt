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

#include "Api.h"

#include <string>
#include <openssl/ssl.h>
#include <openssl/err.h>

namespace Pt {
namespace Ssl {

//! \brief SSL context.
class PT_SSL_API SSLContext {
    public:
        //! \brief Construct an SSL context that uses the given certificate-key file and password.
        SSLContext(const char* caFile, const char* keyFile, const char* password, const char* sessionID);

        //! \brief Standard dtor.
        ~SSLContext();

        friend class SSLConnector2;
        friend class SSLStreamBuffer;
        friend class SSLStreamBuf;

    private:
        SSL_CTX*    _ctx;    // OpenSSL's SSL context
        std::string _pswd;   // The password
        static BIO* _bioErr; // Error BIO for OpenSSL

        // Password callback to feed the password to OpenSSL
        static int _passwordCallback(char* buf, int num, int rwflag, void* userdata);
};

} // namespace Pt
} // namespace Ssl

#endif
