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
#ifndef PT_SSL_EXCEPTION_H
#define PT_SSL_EXCEPTION_H

#include <Pt/Ssl/Api.h>
#include <Pt/SourceInfo.h>

#include <string>
#include <exception>
#include <stdexcept>


namespace Pt {
namespace Ssl {

    /** @brief Generic SSL run-time error.
     */
    class PT_SSL_API SSLRuntimeError : public std::runtime_error {
        public:
            //! @see Exception()
            SSLRuntimeError(const std::string& what, const SourceInfo& si) throw();

            //! @brief Destructor.
            ~SSLRuntimeError() throw();
    };


    /** @brief SSL handshake-failed error.
     */
    class PT_SSL_API SSLHandshakeFailedError : public SSLRuntimeError {
        public:
            //! @see Exception()
            SSLHandshakeFailedError(const std::string& what, const SourceInfo& si) throw();

            //! @brief Destructor.
            ~SSLHandshakeFailedError() throw();
    };


} // namespace Ssl
} // namespace Pt

#endif

