/* Copyright (C) 2020-2026 by Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
*/

#ifndef PT_MCP_API_H
#define PT_MCP_API_H

#include <Pt/Api.h>

#define PT_MCP_VERSION_MAJOR PT_VERSION_MAJOR
#define PT_MCP_VERSION_MINOR PT_VERSION_MINOR
#define PT_MCP_VERSION_REVISION PT_VERSION_REVISION
#define PT_MCP_VERSION_PRERELEASE PT_VERSION_PRERELEASE

#if defined(PT_MCP_API_EXPORT)
#    define PT_MCP_API PT_EXPORT
#  else
#    define PT_MCP_API PT_IMPORT
#  endif

namespace Pt {

/** @namespace Pt::Mcp
    @brief Model Context Protocol (MCP) services.

    The MCP module (Pt::Mcp) of the %Pt framework provides an implementation
    of the Model Context Protocol for exposing tool-based services. It builds
    on top of the JSON-RPC module (Pt::JsonRpc) and uses its Formatter and
    Fault infrastructure. Custom data-types can be used in MCP tool procedures
    if they are serializable using %Pt's serialization API.
*/
namespace Mcp {

} // namespace Mcp

} // namespace Pt

#endif
