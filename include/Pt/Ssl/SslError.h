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
#ifndef PT_SSL_SSLERROR_H
#define PT_SSL_SSLERROR_H

#include <Pt/Ssl/Api.h>
#include <string>
#include <stdexcept>

namespace Pt {

namespace Ssl {

/** @brief Generic SSL run-time error.
  */
class PT_SSL_API SslError : public std::runtime_error 
{
    public:
        //! @brief Contructs with message.
        SslError(const std::string& what);

        //! @brief Destructor.
        ~SslError() throw();
};

/** @brief SSL Handshake failed.
  */
class PT_SSL_API HandshakeFailed : public SslError 
{
    public:
        //! @brief Contructs with message.
        HandshakeFailed(const std::string& what) throw();

        //! @brief Destructor.
        ~HandshakeFailed() throw();
};

/** @brief Invalid public or private key.
  */
class PT_SSL_API InvalidKey : public SslError 
{
    public:
        //! @brief Contructs with message.
        InvalidKey(const std::string& what) throw();

        //! @brief Destructor.
        ~InvalidKey() throw();
};

/** @brief Invalid SSL certificate or certificate chain.
  */
class PT_SSL_API InvalidCertificate : public SslError 
{
    public:
        //! @brief Contructs with message.
        InvalidCertificate(const std::string& what) throw();

        //! @brief Destructor.
        ~InvalidCertificate() throw();
};

/** @brief SSL session failure.
  */
class PT_SSL_API SessionFailed : public SslError 
{
    public:
        //! @brief Contructs with message.
        SessionFailed(const std::string& what) throw();

        //! @brief Destructor.
        ~SessionFailed() throw();
};

} // namespace Ssl

} // namespace Pt

#endif // PT_SSL_SSLERROR_H
