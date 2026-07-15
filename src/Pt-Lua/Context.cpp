/*
 * Copyright (C) 2020-2026 by Marc Boris Duerner
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * As a special exception, you may use this file as part of a free
 * software library without restriction. Specifically, if other files
 * instantiate templates or use macros or inline functions from this
 * file, or you compile this file and link it with other files to
 * produce an executable, this file does not by itself cause the
 * resulting executable to be covered by the GNU General Public
 * License. This exception does not however invalidate any other
 * reasons why the executable file might be covered by the GNU Library
 * General Public License.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301 USA
 */

#include <Pt/Lua/Context.h>
#include <Pt/Lua/Script.h>
#include <Pt/Lua/Call.h>

#include <Pt/Reflex/Type.h>
#include <Pt/Reflex/MethodInfo.h>
#include <Pt/Reflex/PropertyInfo.h>
#include <Pt/Reflex/ConstructorInfo.h>
#include <Pt/Reflex/Argument.h>
#include <Pt/Any.h>

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include <cstring>
#include <typeinfo>
#include <vector>
#include <string>

namespace Pt {

namespace Lua {

namespace {

//
// Metamethods installed on every bound type's metatable.
//

int objectGc(lua_State* L)
{
  LuaObjectHeader* hdr = static_cast<LuaObjectHeader*>(lua_touserdata(L, 1));
  hdr->type->destroy(hdr->instance);
  return 0;
}

int objectToString(lua_State* L)
{
  LuaObjectHeader* hdr = static_cast<LuaObjectHeader*>(lua_touserdata(L, 1));
  lua_pushfstring(L, "%s: %p", hdr->type->name().c_str(), hdr->instance);
  return 1;
}


static Pt::Reflex::Argument extractArg(lua_State* L, int idx,
                                       Pt::Reflex::Type& expectedType)
{
  Pt::Any value;
  const std::type_info* ti = expectedType.id();

  if(ti && *ti == typeid(bool))
  {
    bool v = lua_toboolean(L, idx) != 0;
    value = v;
  }
  else if(ti && *ti == typeid(int))
  {
    int v = static_cast<int>(luaL_checkinteger(L, idx));
    value = v;
  }
  else if(ti && *ti == typeid(long))
  {
    long v = static_cast<long>(luaL_checkinteger(L, idx));
    value = v;
  }
  else if(ti && *ti == typeid(double))
  {
    double v = luaL_checknumber(L, idx);
    value = v;
  }
  else if(ti && *ti == typeid(float))
  {
    float v = static_cast<float>(luaL_checknumber(L, idx));
    value = v;
  }
  else if(ti && *ti == typeid(std::string))
  {
    std::size_t len = 0;
    const char* s = luaL_checklstring(L, idx, &len);
    value = std::string(s, len);
  }
  else
  {
    if(lua_type(L, idx) != LUA_TUSERDATA)
    {
      luaL_error(L, "expected object of type '%s', got %s",
                 expectedType.name().c_str(),
                 lua_typename(L, lua_type(L, idx)));
    }
    else
    {
      LuaObjectHeader* hdr =
        static_cast<LuaObjectHeader*>(lua_touserdata(L, idx));
      if( ! hdr->type->isTypeOf(expectedType))
      {
        luaL_error(L, "expected type '%s', got '%s'",
                   expectedType.name().c_str(),
                   hdr->type->name().c_str());
      }
      else
      {
        void* ptr = hdr->instance;
        value = ptr;
      }
    }
  }

  return Pt::Reflex::Argument(value, expectedType);
}


static int nativeCallContinuation(lua_State* L, int /*status*/, lua_KContext origTop)
{
  int newTop = lua_gettop(L);
  return newTop - static_cast<int>(origTop);
}


static int constructContinuation(lua_State* L, int /*status*/, lua_KContext origTop)
{
  int newTop = lua_gettop(L);
  return newTop - static_cast<int>(origTop);
}


static int yieldWithCall(lua_State* L, Call* call,
                         lua_KFunction cont, lua_KContext ctxArg)
{
  lua_pushlightuserdata(L, call);
  lua_setfield(L, LUA_REGISTRYINDEX, "LuaPendingCall");
  return lua_yieldk(L, 0, ctxArg, cont);
}


static int yieldWithAsyncCall(lua_State* L, AsyncCall* call,
                              lua_KFunction cont, lua_KContext ctxArg)
{
  lua_pushlightuserdata(L, call);
  lua_setfield(L, LUA_REGISTRYINDEX, "LuaPendingAsyncCall");
  return lua_yieldk(L, 0, ctxArg, cont);
}


static int methodCallClosure(lua_State* L)
{
  Pt::Reflex::MethodInfo* mi =
    static_cast<Pt::Reflex::MethodInfo*>(lua_touserdata(L, lua_upvalueindex(1)));

  if(lua_type(L, 1) != LUA_TUSERDATA)
    return luaL_error(L, "method '%s': expected object as first argument", mi->name());

  LuaObjectHeader* hdr = static_cast<LuaObjectHeader*>(lua_touserdata(L, 1));

  int nstack = lua_gettop(L) - 1;
  if(nstack < 0) nstack = 0;

  if(static_cast<std::size_t>(nstack) != mi->psize())
  {
    return luaL_error(L, "method '%s': expected %d argument(s), got %d",
                      mi->name(), static_cast<int>(mi->psize()), nstack);
  }

  std::vector<Pt::Reflex::Argument> args;
  args.reserve(mi->psize());
  for(std::size_t i = 0; i < mi->psize(); ++i)
    args.push_back(extractArg(L, static_cast<int>(2 + i), *mi->params()[i]));

  Script* script = Script::fromState(L);
  if( ! script)
    return luaL_error(L, "method '%s': no active script", mi->name());

  lua_KContext origTop = static_cast<lua_KContext>(lua_gettop(L));
  Call* call = new MethodCall(mi, hdr->instance, args);
  return yieldWithCall(L, call, nativeCallContinuation, origTop);
}


static int objectIndex(lua_State* L)
{
  const char* key = luaL_checkstring(L, 2);

  // Method lookup
  lua_pushvalue(L, lua_upvalueindex(1));
  lua_getfield(L, -1, key);
  if( ! lua_isnil(L, -1))
    return 1;
  lua_pop(L, 2);

  // Property lookup
  lua_pushvalue(L, lua_upvalueindex(2));
  lua_getfield(L, -1, key);
  if( ! lua_isnil(L, -1))
  {
    Pt::Reflex::PropertyInfo* pi =
      static_cast<Pt::Reflex::PropertyInfo*>(lua_touserdata(L, -1));
    lua_pop(L, 2);

    LuaObjectHeader* hdr = static_cast<LuaObjectHeader*>(lua_touserdata(L, 1));

    Script* script = Script::fromState(L);
    if( ! script)
      return luaL_error(L, "property '%s': no active script", key);

    lua_KContext origTop = static_cast<lua_KContext>(lua_gettop(L));
    Call* call = new PropertyGetCall(pi, hdr->instance);
    return yieldWithCall(L, call, nativeCallContinuation, origTop);
  }
  lua_pop(L, 2);

  return 0;
}


static int objectNewIndex(lua_State* L)
{
  const char* key = luaL_checkstring(L, 2);

  lua_pushvalue(L, lua_upvalueindex(1));
  lua_getfield(L, -1, key);
  if(lua_isnil(L, -1))
  {
    lua_pop(L, 2);
    return luaL_error(L, "no writable property '%s'", key);
  }

  Pt::Reflex::PropertyInfo* pi =
    static_cast<Pt::Reflex::PropertyInfo*>(lua_touserdata(L, -1));
  lua_pop(L, 2);

  LuaObjectHeader* hdr = static_cast<LuaObjectHeader*>(lua_touserdata(L, 1));
  Pt::Reflex::Argument val = extractArg(L, 3, pi->type());

  Script* script = Script::fromState(L);
  if( ! script)
    return luaL_error(L, "property '%s': no active script", key);

  lua_KContext origTop = static_cast<lua_KContext>(lua_gettop(L));
  Call* call = new PropertySetCall(pi, hdr->instance, val);
  return yieldWithCall(L, call, nativeCallContinuation, origTop);
}


static int constructClosure(lua_State* L)
{
  Pt::Reflex::Type* type =
    static_cast<Pt::Reflex::Type*>(lua_touserdata(L, lua_upvalueindex(1)));

  int nstack = lua_gettop(L) - 1;
  if(nstack < 0) nstack = 0;

  Pt::Reflex::ConstructorInfo* ci = 0;
  {
    Pt::Reflex::ConstructorTable::Iterator it = type->constructors().begin();
    for( ; it != type->constructors().end(); ++it)
    {
      if(it->psize() == static_cast<std::size_t>(nstack))
      {
        ci = &(*it);
        break;
      }
    }
  }

  if( ! ci)
  {
    return luaL_error(L, "no constructor for '%s' with %d argument(s)",
                      type->name().c_str(), nstack);
  }

  // Extract args BEFORE allocating userdata.
  std::vector<Pt::Reflex::Argument> args;
  args.reserve(ci->psize());
  for(std::size_t i = 0; i < ci->psize(); ++i)
    args.push_back(extractArg(L, static_cast<int>(2 + i), *ci->params()[i]));

  // Allocate the userdata now; it will be the return value after the yield.
  std::size_t totalSize = LUAOBJECT_DATA_OFFSET + type->size();
  LuaObjectHeader* hdr =
    static_cast<LuaObjectHeader*>(lua_newuserdatauv(L, totalSize, 0));
  hdr->instance   = static_cast<char*>(static_cast<void*>(hdr)) + LUAOBJECT_DATA_OFFSET;
  hdr->type       = type;
  luaL_getmetatable(L, type->name().c_str());
  lua_setmetatable(L, -2);

  // The userdata is now at the top. Record its position for the continuation.
  // After yield + resume the continuation returns 1 (the userdata).
  lua_KContext origTop = static_cast<lua_KContext>(lua_gettop(L) - 1);

  Script* script = Script::fromState(L);
  if( ! script)
    return luaL_error(L, "constructor for '%s': no active script",
                      type->name().c_str());

  Call* call = new ConstructorCall(ci, hdr->instance, args);
  return yieldWithCall(L, call, constructContinuation, origTop);
}


static int luaFunctionDispatchClosure(lua_State* L)
{
  AsyncFunctionInfo* info =
    static_cast<AsyncFunctionInfo*>(lua_touserdata(L, lua_upvalueindex(1)));

  std::vector<Pt::Reflex::Argument> args;
  args.reserve(info->psize());
  for(std::size_t i = 0; i < info->psize(); ++i)
    args.push_back(extractArg(L, static_cast<int>(1 + i), *info->params()[i]));

  Script* script = Script::fromState(L);
  if( ! script)
    return luaL_error(L, "function '%s': no active script", info->name());

  Pt::Reflex::ArgumentList argList(args.empty() ? 0 : &args[0], args.size());
  Pt::Any result = info->call(argList);
  AsyncCall* call = static_cast<AsyncCall*>(result.get());
  lua_KContext origTop = static_cast<lua_KContext>(lua_gettop(L));
  return yieldWithAsyncCall(L, call, nativeCallContinuation, origTop);
}


static int luaAsyncDispatchClosure(lua_State* L)
{
  AsyncMethodInfo* info =
    static_cast<AsyncMethodInfo*>(lua_touserdata(L, lua_upvalueindex(1)));

  LuaObjectHeader* hdr = static_cast<LuaObjectHeader*>(lua_touserdata(L, 1));
  void* instance = hdr ? hdr->instance : 0;

  std::vector<Pt::Reflex::Argument> args;
  args.reserve(info->psize());
  for(std::size_t i = 0; i < info->psize(); ++i)
    args.push_back(extractArg(L, static_cast<int>(2 + i), *info->params()[i]));

  Script* script = Script::fromState(L);
  if( ! script)
    return luaL_error(L, "method '%s': no active script", info->name());

  Pt::Reflex::ArgumentList argList(args.empty() ? 0 : &args[0], args.size());
  Pt::Any result = info->call(instance, argList);
  AsyncCall* call = static_cast<AsyncCall*>(result.get());
  lua_KContext origTop = static_cast<lua_KContext>(lua_gettop(L));
  return yieldWithAsyncCall(L, call, nativeCallContinuation, origTop);
}


//
// Operator metamethods
//

struct OperatorEntry
{
  const char* reflexName;
  const char* luaMetamethod;
};

static const OperatorEntry s_operators[] =
{
  { "+",  "__add" },
  { "-",  "__sub" },
  { "*",  "__mul" },
  { "/",  "__div" },
  { "==", "__eq"  },
  { "<",  "__lt"  },
  { "<=", "__le"  },
  { "#",  "__len" },
  { 0,    0       }
};

void installOperators(lua_State* L, Pt::Reflex::Type& type, int metatableIdx,
                      Context* ctx)
{
  if(metatableIdx < 0)
    metatableIdx = lua_gettop(L) + metatableIdx + 1;

  Pt::Reflex::MethodTable& mtab = type.methods();

  for(const OperatorEntry* op = s_operators; op->reflexName; ++op)
  {
    Pt::Reflex::MethodTable::Iterator it = mtab.begin();
    for( ; it != mtab.end(); ++it)
    {
      if(std::string(it->name()) == op->reflexName)
      {
        lua_pushlightuserdata(L, &(*it));
        lua_pushlightuserdata(L, ctx);
        lua_pushcclosure(L, &methodCallClosure, 2);
        lua_setfield(L, metatableIdx, op->luaMetamethod);
        break;
      }
    }
  }
}

} // anonymous namespace

Context::Context(TypeManager& tm)
: _tm(tm)
, _L(luaL_newstate())
{
  luaL_openlibs(_L);

  // Snapshot standard-lib globals so reset() knows what to keep.
  lua_pushglobaltable(_L);
  lua_pushnil(_L);
  while(lua_next(_L, -2) != 0)
  {
    lua_pop(_L, 1);  // pop value, keep key
    if(lua_type(_L, -1) == LUA_TSTRING)
      _bindingKeys.push_back(lua_tostring(_L, -1));
  }
  lua_pop(_L, 1);  // pop global table

  const std::vector<Pt::Reflex::Type*>& boundTypes = tm.boundTypes();
  for(std::size_t i = 0; i < boundTypes.size(); ++i)
    bindType(*boundTypes[i]);

  // Snapshot type binding globals (class tables).
  lua_pushglobaltable(_L);
  lua_pushnil(_L);
  while(lua_next(_L, -2) != 0)
  {
    lua_pop(_L, 1);
    if(lua_type(_L, -1) == LUA_TSTRING)
    {
      std::string key = lua_tostring(_L, -1);
      bool found = false;
      for(std::size_t i = 0; i < _bindingKeys.size(); ++i)
        if(_bindingKeys[i] == key) { found = true; break; }
      if( ! found)
        _bindingKeys.push_back(key);
    }
  }
  lua_pop(_L, 1);

  // Install global async functions.
  Pt::Reflex::FunctionTable::Iterator fit = tm.functions().begin();
  for( ; fit != tm.functions().end(); ++fit)
  {
    AsyncFunctionInfo* info = dynamic_cast<AsyncFunctionInfo*>(&(*fit));
    if( ! info)
      continue;
    lua_pushlightuserdata(_L, info);
    lua_pushcclosure(_L, &luaFunctionDispatchClosure, 1);
    lua_setglobal(_L, info->name());
    _bindingKeys.push_back(info->name());
  }
}


void Context::bindType(Pt::Reflex::Type& type)
{
  // Method table
  lua_newtable(_L);
  int methodTableIdx = lua_gettop(_L);

  {
    Pt::Reflex::MethodTable::Iterator it = type.methods().begin();
    for( ; it != type.methods().end(); ++it)
    {
      Pt::Reflex::MethodInfo& mi = *it;
      if(AsyncMethodInfo* ami = dynamic_cast<AsyncMethodInfo*>(&mi))
      {
        lua_pushlightuserdata(_L, ami);
        lua_pushcclosure(_L, &luaAsyncDispatchClosure, 1);
      }
      else
      {
        lua_pushlightuserdata(_L, &mi);
        lua_pushlightuserdata(_L, this);
        lua_pushcclosure(_L, &methodCallClosure, 2);
      }
      lua_setfield(_L, methodTableIdx, mi.name());
    }
  }

  // Property table
  lua_newtable(_L);
  int propTableIdx = lua_gettop(_L);

  {
    Pt::Reflex::PropertyTable::Iterator it = type.properties().begin();
    for( ; it != type.properties().end(); ++it)
    {
      Pt::Reflex::PropertyInfo& pi = *it;
      lua_pushlightuserdata(_L, &pi);
      lua_setfield(_L, propTableIdx, pi.name());
    }
  }

  // Instance metatable
  luaL_newmetatable(_L, type.name().c_str());
  int metatableIdx = lua_gettop(_L);

  lua_pushcfunction(_L, &objectGc);
  lua_setfield(_L, metatableIdx, "__gc");

  lua_pushcfunction(_L, &objectToString);
  lua_setfield(_L, metatableIdx, "__tostring");

  // __index closure(methodTable, propTable, Context*)
  lua_pushvalue(_L, methodTableIdx);
  lua_pushvalue(_L, propTableIdx);
  lua_pushlightuserdata(_L, this);
  lua_pushcclosure(_L, &objectIndex, 3);
  lua_setfield(_L, metatableIdx, "__index");

  // __newindex closure(propTable)
  lua_pushvalue(_L, propTableIdx);
  lua_pushcclosure(_L, &objectNewIndex, 1);
  lua_setfield(_L, metatableIdx, "__newindex");

  installOperators(_L, type, metatableIdx, this);

  lua_pop(_L, 1);  // pop metatable

  // Global class table with __call -> construct
  lua_newtable(_L);
  int classTableIdx = lua_gettop(_L);

  lua_newtable(_L);
  lua_pushlightuserdata(_L, &type);
  lua_pushcclosure(_L, &constructClosure, 1);
  lua_setfield(_L, -2, "__call");
  lua_setmetatable(_L, classTableIdx);

  lua_setglobal(_L, type.name().c_str());

  lua_pop(_L, 2);  // propTable, methodTable
}


Context::~Context()
{
  lua_close(_L);
}


void Context::reset()
{
  // Remove all globals not in the binding snapshot.
  lua_pushglobaltable(_L);
  lua_pushnil(_L);

  std::vector<std::string> toRemove;
  while(lua_next(_L, -2) != 0)
  {
    lua_pop(_L, 1);
    if(lua_type(_L, -1) == LUA_TSTRING)
    {
      std::string key = lua_tostring(_L, -1);
      bool keep = false;
      for(std::size_t i = 0; i < _bindingKeys.size(); ++i)
        if(_bindingKeys[i] == key) { keep = true; break; }
      if( ! keep)
        toRemove.push_back(key);
    }
  }
  lua_pop(_L, 1);

  for(std::size_t i = 0; i < toRemove.size(); ++i)
  {
    lua_pushnil(_L);
    lua_setglobal(_L, toRemove[i].c_str());
  }

  lua_gc(_L, LUA_GCCOLLECT, 0);
}

} // namespace

} // namespace
