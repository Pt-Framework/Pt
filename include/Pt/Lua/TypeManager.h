/*
  Copyright (C) 2020-2026 by Marc Boris Duerner

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  As a special exception, you may use this file as part of a free
  software library without restriction. Specifically, if other files
  instantiate templates or use macros or inline functions from this
  file, or you compile this file and link it with other files to
  produce an executable, this file does not by itself cause the
  resulting executable to be covered by the GNU General Public
  License. This exception does not however invalidate any other
  reasons why the executable file might be covered by the GNU Library
  General Public License.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the:
  Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
  Boston, MA 02110-1301 USA
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

template <typename... As>
using AsyncFunction = Pt::Reflex::BasicFunction<AsyncCall*, As...>;


class PT_LUA_API TypeManager : public Pt::Reflex::TypeManager
{
  public:
    TypeManager();

    ~TypeManager();

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
