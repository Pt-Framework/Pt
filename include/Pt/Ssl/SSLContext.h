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

#include <Pt/Ssl/SSLCipherInfo.h>
#include <Pt/System/Logger.h>
#include <string>
#include <vector>

#ifndef NLOG
#define PT_SSL_LOGGER_CATEGORY "Pt.SSL.Logger"
#define PT_SSL_LOG_INFO(NAME, CODE) log_info(Pt::Ssl::SSLContext::pt_ssl_gen_call_info(NAME, PT_FUNCTION) << CODE)
#else
#define PT_SSL_LOGGER_CATEGORY
#define PT_SSL_LOG_INFO(NAME, CODE)
#endif

namespace Pt {
namespace Ssl {

//! @internal Library initialization.
static struct PT_SSL_API SSLInit {
    SSLInit();
    ~SSLInit();
} ssl_init;

//! \brief SSL context.
class PT_SSL_API SSLContext {
    public:
        //! \brief Available protocol.
        enum Protocol {
            DefaultProtocol, //!< Select the default protocol (for now it is SSL version 3).
            TLSv1,           //!< Select TLS version 1 (it is the latest standard for secure TCP communication).
            SSLv3,           //!< Select SSL version 3 (recommended for modern system).
            SSLv3or2,        //!< Select SSL version 3 if available, if not, fallback to version 2 (recommended for the most compatibility).
            SSLv2            //!< Select SSL version 2 (unsecure, not recommended).
        };

    public:
        /** \brief Construct an SSL context that uses the given certificate-key file and password.
         *
         * The 'caCertFile' is needed if you would like to check if the other peer's certificate
         * is signed by a valid Certificate Authority.
         * \n
         * The 'certFile' and 'keyFile' is mandatory for a server context.
         * \n
         * A client context will only need 'certFile' and 'keyFile' if it is to be used for 
         * certificate-based client authentication.
         * \n
         * The 'password' is only needed if the 'keyFile' is encrypted.
         */
        SSLContext(const char* caCertFile = 0,
                   const char* certFile   = 0, const char* keyFile = 0, const char* password = 0,
                   const char* sessionID  = 0,
                   Protocol    protocol   = DefaultProtocol);

        //! \brief Standard dtor.
        ~SSLContext();

        /** \brief Return the current protocol. */
        inline Protocol protocol() const
        { return _protocol; }

        /** \brief Set the current protocol. */
        void setProtocol(Protocol protocol);

        /** \brief Return a list of available ciphers for the current protocol. */
        inline const std::vector<SSLCipherInfo>& availableCiphers() const
        { return _availCiphers; }

        /** \brief Return a list of enabled ciphers. */
        inline const std::vector<SSLCipherInfo>& enabledCiphers() const
        { return _enabledCiphers; }

        /** \brief Set the list of enabled ciphers. */
        void setEnabledCiphers(const std::vector<SSLCipherInfo>& ciphers);

        friend class SSLStreamBuf;

    private:
        // Password callback to feed the password to OpenSSL
        static int passwordCallback(char* buf, int num, int rwflag, void* userdata);

        // Get available ciphers
        void getAvailableCiphers();

    private:
        ssl_ctx_st*                _ctx;            // OpenSSL's SSL context
        std::string                _pswd;           // The password
        Protocol                   _protocol;       // Selected SSL protocol
        std::vector<SSLCipherInfo> _availCiphers;   // List of all available ciphers for the current protocol
        std::vector<SSLCipherInfo> _enabledCiphers; // List of enabled ciphers

#ifndef NLOG
    public:
        // Generate call information for logging purposes
        static const std::string pt_ssl_gen_call_info(const char* className, const std::string& funcName);
#endif
};


} // namespace Pt
} // namespace Ssl

#endif
