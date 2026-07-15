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

#include <Pt/Lua/Script.h>
#include <Pt/Lua/Context.h>
#include <Pt/Lua/AsyncCall.h>
#include <Pt/Lua/Call.h>

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include <stdexcept>
#include <string>

/*
  TODO:
    - Error handling: error in lua script and ScriptError exception,
    - exceptions from native c++ calls
    - Is Call and its derivateives still needed?
      - only used for exception strings which are handled locally anyways.
    - C++ value from Result. currently only get int
    - resolve lua script includes
    - Context reset more efficient?
    - Context bindings cleanup
*/

namespace {

static const char* const PT_LUA_SCRIPT_KEY  = "Script";

static const char* const PT_LUA_SCRIPT_CO   = "LuaScript_co";

static const int PT_LUA_INSTRUCTIONS_PER_YIELD = 100000;

} // namespace

namespace Pt {

namespace Lua {

Script::Script(Context& ctx, const char* script)
: _ctx(ctx)
, _co(0)
, _isCancelled(false)
, _lastStatus(Yield)
, _pendingCall(0)
, _pendingAsyncCall(0)
, _activeAsyncCall(0)
{
  lua_State* L = ctx.state();

  // Enforce single-script-per-context rule.
  lua_getfield(L, LUA_REGISTRYINDEX, PT_LUA_SCRIPT_KEY);
  bool alreadyActive = ! lua_isnil(L, -1);
  lua_pop(L, 1);
  if(alreadyActive)
    throw std::logic_error("context in use");

  // Store this in the registry so closures can find it.
  lua_pushlightuserdata(L, this);
  lua_setfield(L, LUA_REGISTRYINDEX, PT_LUA_SCRIPT_KEY);

  // Create and anchor coroutine.
  _co = lua_newthread(L);
  lua_pushvalue(L, -1);
  lua_setfield(L, LUA_REGISTRYINDEX, PT_LUA_SCRIPT_CO);
  lua_pop(L, 1);

  if(luaL_loadstring(_co, script) != LUA_OK)
  {
    const char* msg = lua_tostring(_co, -1);
    _errorMsg = msg ? msg : "syntax error";
    _lastStatus = ScriptError;
    // Clean up registry so context can be reused.
    lua_pushnil(L);
    lua_setfield(L, LUA_REGISTRYINDEX, PT_LUA_SCRIPT_KEY);
    lua_pushnil(L);
    lua_setfield(L, LUA_REGISTRYINDEX, PT_LUA_SCRIPT_CO);
    return;
  }

  lua_sethook(_co, &Script::yieldHook, LUA_MASKCOUNT, PT_LUA_INSTRUCTIONS_PER_YIELD);
}


Script::~Script()
{
  delete _pendingCall;
  _pendingCall = 0;
  delete _pendingAsyncCall;
  _pendingAsyncCall = 0;
  delete _activeAsyncCall;
  _activeAsyncCall = 0;

  lua_State* L = _ctx.state();

  // Release the coroutine anchor and Script registry entries.
  lua_pushnil(L);
  lua_setfield(L, LUA_REGISTRYINDEX, PT_LUA_SCRIPT_KEY);
  lua_pushnil(L);
  lua_setfield(L, LUA_REGISTRYINDEX, PT_LUA_SCRIPT_CO);
  lua_pushnil(L);
  lua_setfield(L, LUA_REGISTRYINDEX, "LuaPendingCall");
  lua_pushnil(L);
  lua_setfield(L, LUA_REGISTRYINDEX, "LuaPendingAsyncCall");
}


void Script::yieldHook(lua_State* L, lua_Debug* /*ar*/)
{
  Script* script = Script::fromState(L);
  if( ! script)
    return;

  if( script->isCancelled() )
  {
    lua_pushliteral(L, "cancelled");
    lua_error(L);
    return;
  }

  // Yield the coroutine so the event loop can process other work.
  lua_yieldk(L, 0, 0, [](lua_State* /*L*/, int /*status*/, lua_KContext /*ctx*/) -> int
  {
    return 0;
  });
}


Script* Script::fromState(lua_State* L)
{
  lua_getfield(L, LUA_REGISTRYINDEX, PT_LUA_SCRIPT_KEY);
  Script* script = static_cast<Script*>(lua_touserdata(L, -1));
  lua_pop(L, 1);
  return script;
}


void Script::beginAdvance()
{
  post();
}


Script::Status Script::endAdvance() const
{
  return _lastStatus;
}


Pt::Signal<>& Script::advanced()
{
  return _advanced;
}


Script::Status Script::advance()
{
  if(_lastStatus == ScriptOk || _lastStatus == ScriptError)
    return _lastStatus;

  if( isCancelled() )
  {
    _errorMsg = "cancelled";
    _lastStatus = ScriptError;
    return _lastStatus;
  }

  if( ! onCall() )
    return _lastStatus;

  resume();

  if(_pendingAsyncCall)
  {
    delete _pendingAsyncCall;
    _pendingAsyncCall = 0;
    throw std::logic_error("async call not supported");
  }

  return _lastStatus;
}


bool Script::onRun()
{
  if(_activeAsyncCall)
  {
    bool isOk = onAsyncCallReady();
    if(isOk)
      resume();

    _advanced.send();
  }
  else if(_pendingAsyncCall)
  {
    onAsyncCall();
  }
  else if(_pendingCall)
  {
    bool isOk = onCall();
    if(isOk)
      resume();

    _advanced.send();
  }
  else
  {
    resume();
    _advanced.send();
  }

  return false;
}


bool Script::onCall()
{
  if(_pendingCall)
  {
    Pt::Any result = _pendingCall->call();

    if( _pendingCall->hasError() )
    {
      _errorMsg = _pendingCall->errorMessage();
      _lastStatus = ScriptError;

      delete _pendingCall;
      _pendingCall = 0;

      return false;
    }

    if(_pendingCall->rtype())
      pushResult(result, *_pendingCall->rtype());

    delete _pendingCall;
    _pendingCall = 0;
  }

  return true;
}


void Script::onAsyncCall()
{
  _activeAsyncCall = _pendingAsyncCall;
  _pendingAsyncCall = 0;
  _activeAsyncCall->bind(_ctx.typeManager());
  _activeAsyncCall->finished() += Pt::slot(*this, &Script::onAsyncCallFinished);
  _activeAsyncCall->beginCall(*parent());
}


void Script::onAsyncCallFinished()
{
  post();
}


bool Script::onAsyncCallReady()
{
  if( _activeAsyncCall->hasError() )
  {
    _errorMsg = _activeAsyncCall->errorMessage();
    _lastStatus = ScriptError;

    delete _activeAsyncCall;
    _activeAsyncCall = 0;

    delete _pendingCall;
    _pendingCall = 0;
    return false;
  }

  Pt::Any result = _activeAsyncCall->getResult();
  if(_activeAsyncCall->rtype())
    pushResult(result, *_activeAsyncCall->rtype());

  delete _activeAsyncCall;
  _activeAsyncCall = 0;
  return true;
}


void Script::onAttach(Pt::System::EventLoop& /*loop*/)
{}


void Script::onDetach(Pt::System::EventLoop& /*loop*/)
{}


void Script::onCancel()
{
  _isCancelled = true;

  if(_activeAsyncCall)
    _activeAsyncCall->cancel();
}


bool Script::isCancelled() const
{
  return _isCancelled;
}


void Script::resume()
{
  int nres = 0;
  int status = lua_resume(_co, NULL, 0, &nres);

  if(status == LUA_YIELD)
  {
    lua_State* L = _ctx.state();

    lua_getfield(L, LUA_REGISTRYINDEX, "LuaPendingCall");
    Call* syncCall = static_cast<Call*>(lua_touserdata(L, -1));
    lua_pop(L, 1);

    if(syncCall)
    {
      delete _pendingCall;
      _pendingCall = syncCall;
      lua_pushnil(L);
      lua_setfield(L, LUA_REGISTRYINDEX, "LuaPendingCall");
      _lastStatus = NativeCall;
      return;
    }

    lua_getfield(L, LUA_REGISTRYINDEX, "LuaPendingAsyncCall");
    AsyncCall* asyncCall = static_cast<AsyncCall*>(lua_touserdata(L, -1));
    lua_pop(L, 1);

    if(asyncCall)
    {
      delete _pendingAsyncCall;
      _pendingAsyncCall = asyncCall;
      lua_pushnil(L);
      lua_setfield(L, LUA_REGISTRYINDEX, "LuaPendingAsyncCall");
      _lastStatus = NativeCall;
      return;
    }

    _lastStatus = Yield;
  }
  else if(status == LUA_OK)
  {
    _lastStatus = ScriptOk;
  }
  else
  {
    const char* msg = lua_tostring(_co, -1);
    _errorMsg = msg ? msg : "script error";
    _lastStatus = ScriptError;
  }
}


int Script::pushResult(Pt::Any& value, Pt::Reflex::Type& type)
{
  const std::type_info* ti = type.id();

  if( ! ti || type.name() == "void")
    return 0;

  if(*ti == typeid(bool))
  {
    lua_pushboolean(_co, *static_cast<bool*>(value.get()) ? 1 : 0);
    return 1;
  }
  if(*ti == typeid(int))
  {
    lua_pushinteger(_co, *static_cast<int*>(value.get()));
    return 1;
  }
  if(*ti == typeid(long))
  {
    lua_pushinteger(_co, *static_cast<long*>(value.get()));
    return 1;
  }
  if(*ti == typeid(double))
  {
    lua_pushnumber(_co, *static_cast<double*>(value.get()));
    return 1;
  }
  if(*ti == typeid(float))
  {
    lua_pushnumber(_co, *static_cast<float*>(value.get()));
    return 1;
  }
  if(*ti == typeid(std::string))
  {
    const std::string* s = static_cast<const std::string*>(value.get());
    lua_pushstring(_co, s->c_str());
    return 1;
  }

  // Object type: always copy-construct into owned Lua userdata.
  const std::vector<Pt::Reflex::Type*>& boundTypes = _ctx.typeManager().boundTypes();
  bool isBound = false;
  for(std::size_t i = 0; i < boundTypes.size(); ++i)
  {
    if(boundTypes[i] == &type)
    {
      isBound = true;
      break;
    }
  }

  if( ! isBound )
  {
    lua_pushnil(_co);
    return 1;
  }

  if( ! type.hasCopyConstruct() )
  {
    return luaL_error(_co, "type '%s' cannot be returned by value: no copy constructor",
                      type.name().c_str());
  }

  std::size_t totalSize = LUAOBJECT_DATA_OFFSET + type.size();
  LuaObjectHeader* hdr =
    static_cast<LuaObjectHeader*>(lua_newuserdatauv(_co, totalSize, 0));
  hdr->instance   = static_cast<char*>(static_cast<void*>(hdr)) + LUAOBJECT_DATA_OFFSET;
  hdr->type       = &type;
  type.copyConstruct(hdr->instance, value.get());
  luaL_getmetatable(_co, type.name().c_str());
  lua_setmetatable(_co, -2);
  return 1;
}

} // namespace

} // namespace
