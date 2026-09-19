/* Copyright (C) 2020-2026 by Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
*/

#ifndef PT_LUA_TYPEMANAGER_H
#define PT_LUA_TYPEMANAGER_H

#include <Pt/Lua/Api.h>
#include <Pt/Lua/AsyncCall.h>
#include <Pt/Reflex/TypeManager.h>
#include <Pt/Reflex/Function.h>
#include <Pt/Reflex/Type.h>

namespace Pt {

namespace Lua {

/** @brief Reflex function that returns an asynchronous Lua call.

    Alias for a %Pt::Reflex::BasicFunction whose result type is
    %AsyncCall*. Register an instance, or a free function with that
    return type, so Lua can invoke asynchronous native work.

    @ingroup Pt-Lua-Calls
*/
template <typename... As>
using AsyncFunction = Pt::Reflex::BasicFunction<AsyncCall*, As...>;


/** @brief Type catalog for Lua bindings.

    %TypeManager is the Lua catalog the runtime group described. It
    is a %Pt::Reflex::TypeManager that already registers int, long,
    float, double, bool, string, void, and %AsyncCall*. Those
    builtins exist so argument conversion and asynchronous calls
    have types before any user type is added.

    Register user types and functions with the inherited Reflex
    operations before a %Context is constructed. The manager does
    not own those user types. A parent type manager is visible to
    binding; put shared types on the parent and Lua-specific
    functions on a child if the same catalog serves more than one
    context.

    %voidType() is the void entry of the builtin set, used when an
    asynchronous call has no result. The manager is not copyable.

    @ingroup Pt-Lua-Runtime
*/
class PT_LUA_API TypeManager : public Pt::Reflex::TypeManager
{
  public:
    /** @brief Creates a catalog with Lua builtin types.
    */
    TypeManager();

    TypeManager(const TypeManager&) = delete;

    TypeManager& operator=(const TypeManager&) = delete;

    /** @brief Destroys the catalog and its builtin types.
    */
    ~TypeManager();

    /** @brief Returns the builtin void type.
    */
    Pt::Reflex::Type& voidType() const
    { return *_voidType; }

  private:
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
