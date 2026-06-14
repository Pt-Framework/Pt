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

#include <Pt/Lua/TypeManager.h>
#include <Pt/Reflex/Type.h>
#include <lua.hpp>

namespace Pt {

namespace Lua {

namespace {

class VoidType : public Pt::Reflex::Type
{
  public:
    VoidType()
    : Pt::Reflex::Type(typeid(void), "void")
    {}

    std::size_t size() const
    { return 0; }
};

} // anonymous namespace


TypeManager::TypeManager()
: _intType(new Pt::Reflex::BasicType<int>("int"))
, _longType(new Pt::Reflex::BasicType<long>("long"))
, _floatType(new Pt::Reflex::BasicType<float>("float"))
, _doubleType(new Pt::Reflex::BasicType<double>("double"))
, _boolType(new Pt::Reflex::BasicType<bool>("bool"))
, _stringType(new Pt::Reflex::BasicType<std::string>("string"))
, _voidType(new VoidType())
, _luaCallPtrType(new Pt::Reflex::BasicType<AsyncCall*>("AsyncCall*"))
{
  Pt::Reflex::TypeManager::registerType(*_intType);
  Pt::Reflex::TypeManager::registerType(*_longType);
  Pt::Reflex::TypeManager::registerType(*_floatType);
  Pt::Reflex::TypeManager::registerType(*_doubleType);
  Pt::Reflex::TypeManager::registerType(*_boolType);
  Pt::Reflex::TypeManager::registerType(*_stringType);
  Pt::Reflex::TypeManager::registerType(*_voidType);
  Pt::Reflex::TypeManager::registerType(*_luaCallPtrType);
}


TypeManager::~TypeManager()
{
  delete _voidType;
  delete _luaCallPtrType;
  delete _stringType;
  delete _boolType;
  delete _doubleType;
  delete _floatType;
  delete _longType;
  delete _intType;

  for(std::size_t i = 0; i < _ownedFunctions.size(); ++i)
    delete _ownedFunctions[i];
}


void TypeManager::registerAsyncFunction(const char* name,
                                            AsyncCall* (*func)())
{
  AsyncFunction<Pt::Reflex::Void, Pt::Reflex::Void>* fi =
    new AsyncFunction<Pt::Reflex::Void, Pt::Reflex::Void>(name, func, luaCallPtrType());
  _ownedFunctions.push_back(fi);
  Pt::Reflex::TypeManager::registerFunction(fi);
}

} // namespace

} // namespace
