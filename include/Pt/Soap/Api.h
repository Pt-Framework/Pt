/* Copyright (C) 2009-2014 by Dr. Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
*/

#ifndef PT_SOAP_API_H
#define PT_SOAP_API_H

#include <Pt/Api.h>

#define PT_SOAP_VERSION_MAJOR PT_VERSION_MAJOR
#define PT_SOAP_VERSION_MINOR PT_VERSION_MINOR
#define PT_SOAP_VERSION_REVISION PT_VERSION_REVISION
#define PT_SOAP_VERSION_PRERELEASE PT_VERSION_PRERELEASE

#if defined(PT_SOAP_API_EXPORT)
#    define PT_SOAP_API PT_EXPORT
#  else
#    define PT_SOAP_API PT_IMPORT
#  endif

namespace Pt {

/** @namespace Pt::Soap
    @brief SOAP services and clients.

    The SOAP module (Pt::Soap) of the %Pt framework provides a client API to
    call remote procedures and a server API to implement remote procedures.
    Blocking and non-blocking APIs exist in either case. The SOAP module builds
    on top of the HTTP module (Pt::Http) and uses the HTTP client and server 
    implementation thereof. Custom data-types can be used in SOAP procedures
    if they are serializable using %Pt's serialization API.
*/
namespace Soap {

} // namespace Soap

} // namespace Pt

#endif
