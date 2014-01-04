/*
 * Copyright (C) 2013 by Marc Boris Duerner
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

#ifndef PT_SSL_API_H
#define PT_SSL_API_H

#include <Pt/Api.h>

#define PT_SSL_VERSION_MAJOR 1
#define PT_SSL_VERSION_MINOR 0
#define PT_SSL_VERSION_REVISION 0

#if defined(PT_SSL_API_EXPORT)
#    define PT_SSL_API PT_EXPORT
#  else
#    define PT_SSL_API PT_IMPORT
#  endif

namespace Pt {

/** @namespace Pt::Ssl
    @brief Secure Communication using SSL/TLS.
*/
namespace Ssl {
    
class Certificate;
class CertificateStore;
class Context;
class HandshakeFailed;
class InvalidCertificate;
class IOStream;
class SslError;
class StreamBuffer;

} // namespace Ssl

} // namespace Pt

#endif // PT_SSL_API_H
