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

#ifndef PT_LUA_ASYNCCALL_H
#define PT_LUA_ASYNCCALL_H

#include <Pt/Lua/Api.h>
#include <Pt/Reflex/MethodInfo.h>
#include <Pt/Reflex/TypeManager.h>
#include <Pt/Reflex/FunctionInfo.h>
#include <Pt/Reflex/Argument.h>
#include <Pt/Reflex/ArgumentTraits.h>
#include <Pt/System/EventLoop.h>
#include <Pt/Any.h>

#include <exception>
#include <string>

namespace Pt {

namespace Lua {

class PT_LUA_API AsyncCall
{
  public:
    typedef void (*ReadyCallback)(void* user);

    AsyncCall();

    virtual ~AsyncCall();

    void bind(Pt::Reflex::TypeManager& tm);

    Pt::Reflex::Type* rtype() const;

    void beginCall(Pt::System::EventLoop& loop,
                   ReadyCallback readyCb, void* user);

    Pt::Any getResult();

    void cancel();

    bool hasError() const;

    const std::string& errorMessage() const;

  protected:
    void setReady();

    void setError(const std::string& msg);

  private:
    virtual Pt::Reflex::Type* onBind(Pt::Reflex::TypeManager& tm) = 0;

    virtual void onBeginCall(Pt::System::EventLoop& loop) = 0;

    virtual Pt::Any onGetResult() = 0;

    virtual void onCancel() = 0;

  private:
    ReadyCallback     _readyCb;
    void*             _readyUser;
    bool              _hasError;
    std::string       _errorMsg;
    Pt::Reflex::Type* _rtype;
};


class AsyncMethodInfo : public Pt::Reflex::MethodInfo
{
  public:
    explicit AsyncMethodInfo(const char* name)
    : Pt::Reflex::MethodInfo(name)
    {}

    virtual ~AsyncMethodInfo()
    {}
};


template <typename T,
          typename A1 = Pt::Reflex::Void,
          typename A2 = Pt::Reflex::Void>
class AsyncMethod : public AsyncMethodInfo
{
  public:
    typedef AsyncCall* (T::*MemFunc)(A1, A2);

    AsyncMethod(Pt::Reflex::TypeManager& tm, const char* name, MemFunc mf)
    : AsyncMethodInfo(name)
    , _mf(mf)
    {
      Pt::Reflex::Type* rtype = tm.getType(typeid(AsyncCall*));
      _params[0] = tm.getType(typeid(A1));
      _params[1] = tm.getType(typeid(A2));
      this->init(*rtype, _params, 2);
    }

    Pt::Any call(void* instance, const Pt::Reflex::ArgumentList& args) override
    {
      T* self = static_cast<T*>(instance);
      Pt::Reflex::ArgumentIterator arg = args.begin();
      A1 a1 = Pt::Reflex::ArgumentTraits<A1>::cast(*_params[0], arg->type(), arg->get());
      ++arg;
      A2 a2 = Pt::Reflex::ArgumentTraits<A2>::cast(*_params[1], arg->type(), arg->get());
      return Pt::Any((self->*_mf)(a1, a2));
    }

  private:
    MemFunc           _mf;
    Pt::Reflex::Type* _params[2];
};


template <typename T, typename A1>
class AsyncMethod<T, A1, Pt::Reflex::Void> : public AsyncMethodInfo
{
  public:
    typedef AsyncCall* (T::*MemFunc)(A1);

    AsyncMethod(Pt::Reflex::TypeManager& tm, const char* name, MemFunc mf)
    : AsyncMethodInfo(name)
    , _mf(mf)
    {
      Pt::Reflex::Type* rtype = tm.getType(typeid(AsyncCall*));
      _params[0] = tm.getType(typeid(A1));
      this->init(*rtype, _params, 1);
    }

    Pt::Any call(void* instance, const Pt::Reflex::ArgumentList& args) override
    {
      T* self = static_cast<T*>(instance);
      Pt::Reflex::ArgumentIterator arg = args.begin();
      A1 a1 = Pt::Reflex::ArgumentTraits<A1>::cast(*_params[0], arg->type(), arg->get());
      return Pt::Any((self->*_mf)(a1));
    }

  private:
    MemFunc           _mf;
    Pt::Reflex::Type* _params[1];
};


template <typename T>
class AsyncMethod<T, Pt::Reflex::Void, Pt::Reflex::Void> : public AsyncMethodInfo
{
  public:
    typedef AsyncCall* (T::*MemFunc)();

    AsyncMethod(Pt::Reflex::TypeManager& tm, const char* name, MemFunc mf)
    : AsyncMethodInfo(name)
    , _mf(mf)
    {
      Pt::Reflex::Type* rtype = tm.getType(typeid(AsyncCall*));
      this->init(*rtype, 0, 0);
    }

    Pt::Any call(void* instance, const Pt::Reflex::ArgumentList& /*args*/) override
    {
      T* self = static_cast<T*>(instance);
      return Pt::Any((self->*_mf)());
    }

  private:
    MemFunc _mf;
};


class AsyncFunctionInfo : public Pt::Reflex::FunctionInfo
{
  public:
    explicit AsyncFunctionInfo(const char* name)
    : Pt::Reflex::FunctionInfo(1, std::string(name))
    {}

    virtual ~AsyncFunctionInfo()
    {}
};


template <typename A1 = Pt::Reflex::Void,
          typename A2 = Pt::Reflex::Void>
class AsyncFunction : public AsyncFunctionInfo
{
  public:
    typedef AsyncCall* (*FuncPtr)(A1, A2);

    AsyncFunction(const char* name, FuncPtr func,
                  Pt::Reflex::TypeManager& tm, Pt::Reflex::Type& returnType)
    : AsyncFunctionInfo(name)
    , _func(func)
    {
      _params[0] = tm.getType(typeid(A1));
      _params[1] = tm.getType(typeid(A2));
      this->init(returnType, _params, 2);
    }

    Pt::Any call(const Pt::Reflex::ArgumentList& args) override
    {
      Pt::Reflex::ArgumentIterator arg = args.begin();
      A1 a1 = Pt::Reflex::ArgumentTraits<A1>::cast(*_params[0], arg->type(), arg->get());
      ++arg;
      A2 a2 = Pt::Reflex::ArgumentTraits<A2>::cast(*_params[1], arg->type(), arg->get());
      return Pt::Any(_func(a1, a2));
    }

  private:
    FuncPtr           _func;
    Pt::Reflex::Type* _params[2];
};


template <typename A1>
class AsyncFunction<A1, Pt::Reflex::Void> : public AsyncFunctionInfo
{
  public:
    typedef AsyncCall* (*FuncPtr)(A1);

    AsyncFunction(const char* name, FuncPtr func,
                  Pt::Reflex::TypeManager& tm, Pt::Reflex::Type& returnType)
    : AsyncFunctionInfo(name)
    , _func(func)
    {
      _params[0] = tm.getType(typeid(A1));
      this->init(returnType, _params, 1);
    }

    Pt::Any call(const Pt::Reflex::ArgumentList& args) override
    {
      Pt::Reflex::ArgumentIterator arg = args.begin();
      A1 a1 = Pt::Reflex::ArgumentTraits<A1>::cast(*_params[0], arg->type(), arg->get());
      return Pt::Any(_func(a1));
    }

  private:
    FuncPtr           _func;
    Pt::Reflex::Type* _params[1];
};


template <>
class AsyncFunction<Pt::Reflex::Void, Pt::Reflex::Void> : public AsyncFunctionInfo
{
  public:
    typedef AsyncCall* (*FuncPtr)();

    AsyncFunction(const char* name, FuncPtr func, Pt::Reflex::Type& returnType)
    : AsyncFunctionInfo(name)
    , _func(func)
    {
      this->init(returnType, 0, 0);
    }

    Pt::Any call(const Pt::Reflex::ArgumentList& /*args*/) override
    {
      return Pt::Any(_func());
    }

  private:
    FuncPtr _func;
};


template <typename R>
class BasicAsyncCall : public AsyncCall
{
  protected:
    virtual Pt::Reflex::Type* onBind(Pt::Reflex::TypeManager& tm) override
    {
      return tm.getType( typeid(R) );
    }

    virtual R onResult() = 0;

  private:
    Pt::Any onGetResult() override
    {
      R r = onResult();
      return Pt::Any(r);
    }
};


template <>
class BasicAsyncCall<void> : public AsyncCall
{
  protected:
    virtual Pt::Reflex::Type* onBind(Pt::Reflex::TypeManager& tm) override
    {
      return tm.getType( typeid(void) );
    }

  private:
    Pt::Any onGetResult() override
    { return Pt::Any(); }
};

} // namespace

} // namespace

#endif // PT_LUA_ASYNCCALL_H
