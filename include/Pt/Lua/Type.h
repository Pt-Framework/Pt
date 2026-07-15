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

#ifndef PT_LUA_TYPE_H
#define PT_LUA_TYPE_H

#include <Pt/Lua/Api.h>
#include <Pt/Lua/AsyncCall.h>
#include <Pt/Reflex/Type.h>
#include <vector>

namespace Pt {

namespace Lua {

class TypeManager;


class Type : public Pt::Reflex::Type
{
  public:
    Type(const std::type_info& ti, const std::string& name)
    : Pt::Reflex::Type(ti, name)
    { }

    template <typename T>
    void registerAsyncMethod(TypeManager& tm, const char* name,
                             AsyncCall* (T::*method)())
    {
      this->registerMethod(new AsyncMethod<T>(tm, name, method));
    }

    template <typename T, typename A1>
    void registerAsyncMethod(TypeManager& tm, const char* name,
                             AsyncCall* (T::*method)(A1))
    {
      this->registerMethod(new AsyncMethod<T, A1>(tm, name, method));
    }

    template <typename T, typename A1, typename A2>
    void registerAsyncMethod(TypeManager& tm, const char* name,
                             AsyncCall* (T::*method)(A1, A2))
    {
      this->registerMethod(new AsyncMethod<T, A1, A2>(tm, name, method));
    }
};


template <typename T>
class BasicType : public Pt::Reflex::BasicType<T>
{
  public:
    explicit BasicType(const std::string& name)
    : Pt::Reflex::BasicType<T>(name)
    {}

    template <typename U>
    void registerAsyncMethod(TypeManager& tm, const char* name,
                             AsyncCall* (U::*method)())
    {
      this->registerMethod(new AsyncMethod<U>(tm, name, method));
    }

    template <typename U, typename A1>
    void registerAsyncMethod(TypeManager& tm, const char* name,
                             AsyncCall* (U::*method)(A1))
    {
      this->registerMethod(new AsyncMethod<U, A1>(tm, name, method));
    }

    template <typename U, typename A1, typename A2>
    void registerAsyncMethod(TypeManager& tm, const char* name,
                             AsyncCall* (U::*method)(A1, A2))
    {
      this->registerMethod(new AsyncMethod<U, A1, A2>(tm, name, method));
    }
};

} // namespace

} // namespace

#endif // include guard
