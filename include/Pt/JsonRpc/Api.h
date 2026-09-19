/* Copyright (C) 2020 Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
*/

#ifndef PT_JSONRPC_API_H
#define PT_JSONRPC_API_H

#include <Pt/Api.h>

#define PT_JSONRPC_VERSION_MAJOR PT_VERSION_MAJOR
#define PT_JSONRPC_VERSION_MINOR PT_VERSION_MINOR
#define PT_JSONRPC_VERSION_REVISION PT_VERSION_REVISION
#define PT_JSONRPC_VERSION_PRERELEASE PT_VERSION_PRERELEASE

#if defined(PT_JSONRPC_API_EXPORT)
#    define PT_JSONRPC_API PT_EXPORT
#  else
#    define PT_JSONRPC_API PT_IMPORT
#  endif

namespace Pt {

/** @namespace Pt::JsonRpc
    @brief JSON-RPC 2.0 services and clients.

    The JSON-RPC module (Pt::JsonRpc) of the %Pt framework provides a client API to
    call remote procedures and a server API to implement remote procedures.
    Blocking and non-blocking APIs exist in either case. The JSON-RPC module builds
    on top of the HTTP module (Pt::Http) and uses the HTTP client and server 
    implementation thereof. Custom data-types can be used in JSON-RPC procedures
    if they are serializable using %Pt's serialization API.
*/
namespace JsonRpc {

} // namespace

} // namespace

#endif // include guard
