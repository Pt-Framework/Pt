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

#ifndef PT_LUA_TYPEMANAGER_H
#define PT_LUA_TYPEMANAGER_H

#include <Pt/Lua/Api.h>
#include <Pt/Lua/AsyncCall.h>
#include <Pt/Lua/Type.h>
#include <Pt/Reflex/TypeManager.h>
#include <Pt/Reflex/Type.h>

#include <vector>

namespace Pt {

namespace Lua {

// Extends Pt::Reflex::TypeManager to additionally capture the destructor
// thunk (void(*)(void*)) for each user-defined C++ class registered for Lua
// export. The TypeBinding list is later consumed by Context to wire __gc.
//
// Also pre-registers the common primitive types (int, long, float, double,
// bool, std::string, void) so that Type::registerMethod() and
// Type::registerConstructor() can resolve their parameter and return types
// from this manager without manual setup.
//
// Usage:
//   TypeManager tm;
//   tm.registerType<Counter>(counterType);  // Phase 1: register all types
//   tm.registerType<Point>(pointType);
//   counterType.define(tm);                 // Phase 2: define (may ref other types)
//   pointType.define(tm);
//   Context ctx(tm);                     // exports all registered types
class PT_LUA_API TypeManager : public Pt::Reflex::TypeManager
{
  public:
    struct TypeBinding
    {
      Type*          type;
      void (*destructor)(void*);
    };

    TypeManager();

    ~TypeManager();

    // Register a type. Captures the destructor thunk for T.
    // Must be called before Context construction.
    // After all types are registered, call type.define() in a second pass
    // so that inter-type references can be resolved.
    template <typename T>
    void registerType(Type& type)
    {
      Pt::Reflex::TypeManager::registerType(type);
      TypeBinding b;
      b.type       = &type;
      b.destructor = &destroyLuaObject<T>;
      _bindings.push_back(b);
    }

    const std::vector<TypeBinding>& bindings() const
    { return _bindings; }

    // Register an async global function by name (0 arguments).
    // Must be called before Context construction.
    void registerAsyncFunction(const char* name,
                               AsyncCall* (*func)());

    // Register an async global function by name (1 typed argument).
    // Must be called before Context construction.
    template <typename A1>
    void registerAsyncFunction(const char* name, AsyncCall* (*func)(A1))
    {
      AsyncFunction<A1>* fi = new AsyncFunction<A1>(name, func, *this, asyncCallType());
      _ownedFunctions.push_back(fi);
      Pt::Reflex::TypeManager::registerFunction(fi);
    }

    // Register an async global function by name (2 typed arguments).
    // Must be called before Context construction.
    template <typename A1, typename A2>
    void registerAsyncFunction(const char* name, AsyncCall* (*func)(A1, A2))
    {
      AsyncFunction<A1, A2>* fi = new AsyncFunction<A1, A2>(name, func, *this, asyncCallType());
      _ownedFunctions.push_back(fi);
      Pt::Reflex::TypeManager::registerFunction(fi);
    }

    Pt::Reflex::Type& voidType() const
    { return *_voidType; }

    Pt::Reflex::Type& asyncCallType() const
    { return *_asyncCallType; }

  private:
    // Destructor thunk: calls T's destructor on the raw object pointer.
    // Used by Context __gc to destroy owned Lua userdata objects.
    template <typename T>
    static void destroyLuaObject(void* p)
    {
      static_cast<T*>(p)->~T();
    }

  private:
    std::vector<TypeBinding>         _bindings;
    // Owns AsyncFunctionInfo objects registered via registerAsyncFunction().
    // refs=1 prevents cross-DLL delete by TypeManager; we delete in ~TypeManager.
    std::vector<AsyncFunctionInfo*>  _ownedFunctions;

    // Owned primitive types — pre-registered in the constructor.
    Pt::Reflex::BasicType<int>*         _intType;
    Pt::Reflex::BasicType<long>*        _longType;
    Pt::Reflex::BasicType<float>*       _floatType;
    Pt::Reflex::BasicType<double>*      _doubleType;
    Pt::Reflex::BasicType<bool>*        _boolType;
    Pt::Reflex::BasicType<std::string>* _stringType;
    Pt::Reflex::Type*                   _voidType;
    Pt::Reflex::BasicType<AsyncCall*>*  _asyncCallType;
};

} // namespace

} // namespace

#endif // include guard
