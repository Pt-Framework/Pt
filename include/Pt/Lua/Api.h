/* Copyright (C) 2020-2026 by Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
*/

#ifndef PT_LUA_API_H
#define PT_LUA_API_H

#include <Pt/Api.h>

#define PT_LUA_VERSION_MAJOR PT_VERSION_MAJOR
#define PT_LUA_VERSION_MINOR PT_VERSION_MINOR
#define PT_LUA_VERSION_REVISION PT_VERSION_REVISION
#define PT_LUA_VERSION_PRERELEASE PT_VERSION_PRERELEASE

#if defined(PT_LUA_API_EXPORT)
#    define PT_LUA_API PT_EXPORT
#  else
#    define PT_LUA_API PT_IMPORT
#  endif

namespace Pt {

/** @namespace Pt::Lua
    @brief Lua Runtime and Bindiungs.
*/
namespace Lua {

class Script;
class Context;
class Result;
class Call;
class AsyncCall;
class Type;
class TypeManager;

} // namespace

} // namespace

#endif
