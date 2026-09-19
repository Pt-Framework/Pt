/* Copyright (C) 2020-2026 by Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
*/

#ifndef PT_LUA_RESULT_H
#define PT_LUA_RESULT_H

#include <Pt/Lua/Api.h>
#include <string>

struct lua_State;

namespace Pt {

namespace Lua {

/** @brief Result of a finished Lua script.

    After a %Script has reached %ScriptOk, %Result reads a named
    Lua global as an int from that context's state. Construct it
    with %Context::state() and call %get() with the global name
    the script assigned. A missing or non-numeric global converts
    as Lua does for integers, typically to zero.

    The result does not own the state. The context that produced
    the state must outlive the result.

    @ingroup Pt-Lua-Scripts
*/
class PT_LUA_API Result
{
  public:
    /** @brief Creates a result with no Lua state.
    */
    Result();

    /** @brief Creates a result that reads globals from @a L.
    */
    explicit Result(lua_State* L);

    /** @brief Returns the integer value of the global @a name.
    */
    int get(const std::string& name) const;

  private:
    lua_State* _L;
};

} // namespace

} // namespace

#endif // include guard
