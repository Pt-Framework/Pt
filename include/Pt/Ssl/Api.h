/* Copyright (C) 2013 by Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
*/

#ifndef PT_SSL_API_H
#define PT_SSL_API_H

#include <Pt/Api.h>

#define PT_SSL_VERSION_MAJOR PT_VERSION_MAJOR
#define PT_SSL_VERSION_MINOR PT_VERSION_MINOR
#define PT_SSL_VERSION_REVISION PT_VERSION_REVISION
#define PT_SSL_VERSION_PRERELEASE PT_VERSION_PRERELEASE

#if defined(PT_SSL_API_EXPORT)
#    define PT_SSL_API PT_EXPORT
#  else
#    define PT_SSL_API PT_IMPORT
#  endif

namespace Pt {

/** @namespace Pt::Ssl
    @brief SSL/TLS streams, certificates and contexts.

    Portable SSL/TLS on top of an iostream, including certificate
    stores and shared connection contexts.
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
