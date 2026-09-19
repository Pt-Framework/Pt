/* Copyright (C) 2020-2026 by Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
*/

#ifndef PT_LUA_CONTEXT_H
#define PT_LUA_CONTEXT_H

#include <Pt/Lua/Api.h>
#include <Pt/Lua/AsyncCall.h>
#include <Pt/Reflex/TypeManager.h>
#include <Pt/Any.h>

#include <map>
#include <string>
#include <vector>

struct lua_State;

namespace Pt {

namespace Lua {

/** @brief Lua state with a bound type catalog.

    %Context is the bind step of the runtime model. It opens a Lua
    state, loads the standard libraries, and installs globals for
    every eligible type and asynchronous function in @a tm,
    including a parent type manager. The manager is not owned; it
    must outlive the context, together with every type and function
    registered on it.

    A type becomes a Lua class when it has a constructor, a method,
    or a property. The type name is a callable global. An instance
    is userdata that Lua owns. Methods use the Lua method syntax,
    properties are fields, and selected Reflex operator names map
    to metamethods. Scalar values convert in both directions. A
    returned object is copy-constructed into userdata; a type
    without a copy constructor cannot be returned by value.

    A free function is bound only when it returns %AsyncCall*. Two
    different types or functions that share a name, or a name that
    is already a standard Lua global, throw %std::logic_error.
    Binding the same object twice is ignored.

    %state() is the Lua state, for %Result and for the Lua C API.
    %reset() drops script-created globals and collects garbage; the
    bindings remain. Destroy the %Script first. The context is not
    copyable.

    @ingroup Pt-Lua-Runtime
*/
class PT_LUA_API Context
{
  public:
    /** @brief Creates a Lua state and binds @a tm.

        @throw %std::logic_error if a name is already bound.
        @throw %std::bad_alloc if the Lua state cannot be created.
    */
    explicit Context(Pt::Reflex::TypeManager& tm);

    /** @brief Closes the Lua state.
    */
    ~Context();

    /** @brief Returns the Lua state of this context.
    */
    lua_State* state() const
    { return _L; }

    /** @brief Returns the type manager this context bound.
    */
    Pt::Reflex::TypeManager& typeManager()
    { return _tm; }

    /** @brief Removes script-created globals and collects garbage.
    */
    void reset();

  private:
    Context(const Context&);

    Context& operator=(const Context&);

    void bindType(Pt::Reflex::Type& type);

    void bindFunction(Pt::Reflex::FunctionInfo& function);

    void bind(Pt::Reflex::TypeManager& tm);

    bool hasStandardGlobal(const std::string& name) const;

  private:
    Pt::Reflex::TypeManager&  _tm;
    struct lua_State* _L;
    std::map<std::string, void*> _bindings;
    std::vector<std::string> _standardGlobals;
};

/** @internal
*/
struct LuaObjectHeader
{
  void*             instance;
  Pt::Reflex::Type* type;
};

/** @internal
*/
static const std::size_t LUAOBJECT_DATA_OFFSET =
  (sizeof(LuaObjectHeader) + sizeof(void*) - 1) & ~(sizeof(void*) - 1);

} // namespace

} // namespace

#endif // include guard
