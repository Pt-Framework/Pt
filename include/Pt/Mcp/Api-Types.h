/* Copyright (C) 2020-2026 by Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
*/

#ifndef PT_MCP_API_TYPES_H
#define PT_MCP_API_TYPES_H

/** @addtogroup Pt-Mcp-Types

    @brief JSON Schema descriptors for MCP tool parameters.

    A tool parameter is described by a %Type, which is a JSON Schema
    fragment the declaration writes into tools/list. The schema is
    what the client sees. The C++ argument type of the procedure is
    what the remoting layer deserializes. Those two descriptions must
    agree: an integer schema belongs on an int parameter, an object
    schema on a serializable struct.

    Primitive schemas are process-wide singletons.
    %nullType(), %integerType(), %numberType(), %stringType(), and
    %booleanType() return them. They are never constructed by the
    caller and they outlive every tool.

    Composed schemas are values the caller owns. %ObjectType is a JSON
    object with named properties. %ArrayType is an array whose items
    have one element type. %EnumType is a string restricted to a fixed
    set of values. %NullableType wraps another type so null is also
    accepted. The composed type stores a pointer to each inner %Type;
    it does not copy it and it does not own it. Keep every inner type
    alive for as long as the composed type, and keep the composed type
    alive for as long as any %Tool that names it.

    %Property is the named slot both objects and tools use: a name, a
    %Type, a description, and a required flag. Properties and tool
    parameters are required by default. %ObjectType::setOptional() and
    %Tool::setOptional() clear that flag by name. %ObjectType::setStrict()
    forbids additional properties in the schema.

    %Type::toSchema() writes the JSON Schema object to a stream. The
    declaration calls it when it formats tools/list. Application code
    rarely calls it directly. %Type is not copyable.

    The example builds an object schema with a required string and an
    optional integer, then uses it as a tool parameter. The object type
    must outlive the declaration.

    @code
    Pt::Mcp::ObjectType query;
    query.addProperty("name", Pt::Mcp::stringType(), "Name to look up");
    query.addProperty("limit", Pt::Mcp::integerType(), "Maximum rows");
    query.setOptional("limit");

    decl.addTool("search", "Search by name")
        .addParam("query", query, "Search parameters");
    @endcode
*/

#endif
