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

#ifndef PT_LUA_CALL_H
#define PT_LUA_CALL_H

#include <Pt/Lua/Api.h>
#include <Pt/Reflex/MethodInfo.h>
#include <Pt/Reflex/PropertyInfo.h>
#include <Pt/Reflex/ConstructorInfo.h>
#include <Pt/Reflex/Argument.h>
#include <Pt/Any.h>

#include <string>
#include <vector>

struct lua_State;

namespace Pt {

namespace Lua {

class Context;


class Call
{
  public:
    virtual ~Call()
    {}

    virtual void execute() = 0;

    virtual void pushResult(lua_State* co, Context& ctx) = 0;

    bool hasError() const
    { return ! _errorMsg.empty(); }

    const std::string& errorMessage() const
    { return _errorMsg; }

  protected:
    void setError(const std::string& msg)
    { _errorMsg = msg; }

  private:
    std::string _errorMsg;
};


class MethodCall : public Call
{
  public:
    MethodCall(Pt::Reflex::MethodInfo* mi, void* self,
               std::vector<Pt::Reflex::Argument> args)
    : _mi(mi)
    , _self(self)
    , _args(args)
    {}

    void execute() override
    {
      try
      {
        Pt::Reflex::ArgumentList arglist(
          _args.empty() ? static_cast<Pt::Reflex::Argument*>(0) : &_args[0],
          _args.size());
        _result = _mi->call(_self, arglist);
      }
      catch(const std::exception& e) { setError(e.what()); }
    }

    void pushResult(lua_State* co, Context& ctx) override;

  private:
    Pt::Reflex::MethodInfo*           _mi;
    void*                             _self;
    std::vector<Pt::Reflex::Argument> _args;
    Pt::Any                           _result;
};


class PropertyGetCall : public Call
{
  public:
    PropertyGetCall(Pt::Reflex::PropertyInfo* pi, void* self)
    : _pi(pi)
    , _self(self)
    {}

    void execute() override
    {
      try { _result = _pi->get(_self); }
      catch(const std::exception& e) { setError(e.what()); }
    }

    void pushResult(lua_State* co, Context& ctx) override;

  private:
    Pt::Reflex::PropertyInfo* _pi;
    void*                     _self;
    Pt::Any                   _result;
};


class PropertySetCall : public Call
{
  public:
    PropertySetCall(Pt::Reflex::PropertyInfo* pi, void* self,
                    Pt::Reflex::Argument value)
    : _pi(pi)
    , _self(self)
    , _value(value)
    {}

    void execute() override
    {
      try { _pi->set(_self, _value.toAny(), _value.type()); }
      catch(const std::exception& e) { setError(e.what()); }
    }

    void pushResult(lua_State* /*co*/, Context& /*ctx*/) override
    {}

  private:
    Pt::Reflex::PropertyInfo* _pi;
    void*                     _self;
    Pt::Reflex::Argument      _value;
};


class ConstructorCall : public Call
{
  public:
    ConstructorCall(Pt::Reflex::ConstructorInfo* ci, void* instance,
                    std::vector<Pt::Reflex::Argument> args)
    : _ci(ci)
    , _instance(instance)
    , _args(args)
    {}

    void execute() override
    {
      try
      {
        Pt::Reflex::ArgumentList arglist(
          _args.empty() ? static_cast<Pt::Reflex::Argument*>(0) : &_args[0],
          _args.size());
        _ci->call(_instance, arglist);
      }
      catch(const std::exception& e) { setError(e.what()); }
    }

    // The userdata is already on the coroutine stack; the continuation returns it.
    void pushResult(lua_State* /*co*/, Context& /*ctx*/) override
    {}

  private:
    Pt::Reflex::ConstructorInfo*      _ci;
    void*                             _instance;
    std::vector<Pt::Reflex::Argument> _args;
};

} // namespace

} // namespace

#endif // PT_LUA_CALL_H
