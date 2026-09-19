/* Copyright (C) 2009 by Dr. Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
*/

#ifndef PT_XMLRPC_API_H
#define PT_XMLRPC_API_H

#include <Pt/Api.h>

#define PT_XMLRPC_VERSION_MAJOR PT_VERSION_MAJOR
#define PT_XMLRPC_VERSION_MINOR PT_VERSION_MINOR
#define PT_XMLRPC_VERSION_REVISION PT_VERSION_REVISION
#define PT_XMLRPC_VERSION_PRERELEASE PT_VERSION_PRERELEASE

#if defined(PT_XMLRPC_API_EXPORT)
#    define PT_XMLRPC_API PT_EXPORT
#  else
#    define PT_XMLRPC_API PT_IMPORT
#  endif

namespace Pt {

/** @namespace Pt::XmlRpc
    @brief XML RPC services and clients.

    The XML-RPC module (Pt::XmlRpc) of the %Pt framework provides a client API to
    call remote procedures and a server API to implement remote procedures.
    Blocking and non-blocking APIs exist in either case. The XML-RPC module builds
    on top of the HTTP module (Pt::Http) and uses the HTTP client and server 
    implementation thereof. Custom data-types can be used in XML-RPC procedures
    if they are serializable using %Pt's serialization API.
*/
namespace XmlRpc {

} // namespace XmlRpc

} // namespace Pt

#endif
