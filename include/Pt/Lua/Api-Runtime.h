/* Copyright (C) 2020-2026 by Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
*/

#ifndef PT_LUA_API_RUNTIME_H
#define PT_LUA_API_RUNTIME_H

/** @addtogroup Pt-Lua-Runtime

    @brief Prepare a Lua state and bind a type catalog.

    Runtime work is a catalog, then a bind. %TypeManager is the
    catalog: a %Pt::Reflex::TypeManager that already registers the
    scalar types Lua can convert, void, and %AsyncCall*. Register
    user types and asynchronous functions on that manager, or on a
    parent manager it can see, before a %Context is constructed.
    The context does not watch the catalog afterwards.

    %Context is the bind step. It opens a Lua state, loads the
    standard libraries, and walks the type manager, including any
    parent, to install globals. A type is bound when it has at
    least one constructor, method, or property. Its name becomes a
    callable global that constructs userdata. Methods are looked up
    on the instance, properties are fields, and Reflex methods
    named +, -, *, /, ==, <, <=, or # become the matching Lua
    metamethods. A free function is bound only when it returns
    %AsyncCall*; a synchronous free function stays in the catalog
    and does not become a Lua global.

    The context does not own the type manager. The manager, and
    every type and function registered on it, must outlive the
    context. The context owns the Lua state and is not copyable.
    %state() is that Lua state, for %Result and for the Lua C API.
    %reset() removes globals that are neither bindings nor
    standard-library names, then collects garbage, so the same
    context can load another script after the previous %Script has
    been destroyed.

    A name that is already a binding or a standard Lua global
    throws %std::logic_error when a different type or function
    would take it. Binding the same type or function twice is
    ignored. Types in a parent manager are visible to a child;
    two different types that share a name across that chain fail
    at context construction.
*/

#endif
