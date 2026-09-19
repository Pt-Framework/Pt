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

    An MCP server that exposes C++ procedures as tools. Parameter
    schemas, a tool catalog, result content, and HTTP or standard-I/O
    transport. Procedures live on %Pt::Remoting::ServiceDefinition.
    Arguments and results must be serializable.
*/
namespace Mcp {

class Type;
class Property;
class ObjectType;
class ArrayType;
class EnumType;
class NullableType;
class Tool;
class ToolDeclaration;
class ContentFormatter;
class ContentType;
class TextContent;
class ImageContent;
class HttpService;
class Service;
class StdioService;

} // namespace Mcp

} // namespace Pt

#endif
