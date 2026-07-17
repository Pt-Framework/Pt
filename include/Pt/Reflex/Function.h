/*
 * Copyright (C) 2004-2026 by Marc Boris Duerner
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */
#ifndef PT_REFLEX_FUNCTION_H
#define PT_REFLEX_FUNCTION_H

#include <Pt/Reflex/FunctionInfo.h>
#include <Pt/Reflex/Type.h>
#include <Pt/Reflex/Argument.h>
#include <Pt/Reflex/ArgumentTraits.h>
#include <Pt/Any.h>

namespace Pt {

namespace Reflex {

///////////////////////////////////////////////////////////////////////
// BasicFunction
///////////////////////////////////////////////////////////////////////

template <typename R,
          typename A1 = Void,
          typename A2 = Void>
class BasicFunction : public FunctionInfo
{
  public:
    BasicFunction(const char* name, TypeManager& tm)
    : FunctionInfo(1, std::string(name))
    {
      Type* rtype = tm.getType(typeid(R));
      _params[0] = tm.getType(typeid(A1));
      _params[1] = tm.getType(typeid(A2));
      this->init(*rtype, _params, 2);
    }

    Any call(const ArgumentList& args) override
    {
      ArgumentIterator arg = args.begin();
      A1 a1 = ArgumentTraits<A1>::cast(*_params[0], arg->type(), arg->get());
      ++arg;
      A2 a2 = ArgumentTraits<A2>::cast(*_params[1], arg->type(), arg->get());
      return Any( this->onCall(a1, a2) );
    }

  protected:
    virtual R onCall(A1, A2) = 0;

  private:
    Type* _params[2];
};


template <typename R, typename A1>
class BasicFunction<R, A1, Void> : public FunctionInfo
{
  public:
    BasicFunction(const char* name, TypeManager& tm)
    : FunctionInfo(1, std::string(name))
    {
      Type* rtype = tm.getType(typeid(R));
      _params[0] = tm.getType(typeid(A1));
      this->init(*rtype, _params, 1);
    }

    Any call(const ArgumentList& args) override
    {
      ArgumentIterator arg = args.begin();
      A1 a1 = ArgumentTraits<A1>::cast(*_params[0], arg->type(), arg->get());
      return Any( this->onCall(a1) );
    }

  protected:
    virtual R onCall(A1) = 0;

  private:
    Type* _params[1];
};


template <typename R>
class BasicFunction<R, Void, Void> : public FunctionInfo
{
  public:
    BasicFunction(const char* name, TypeManager& tm)
    : FunctionInfo(1, std::string(name))
    {
      Type* rtype = tm.getType(typeid(R));
      this->init(*rtype, 0, 0);
    }

    Any call(const ArgumentList& /*args*/) override
    {
      return Any( this->onCall() );
    }

  protected:
    virtual R onCall() = 0;
};


template <typename A1, typename A2>
class BasicFunction<void, A1, A2> : public FunctionInfo
{
  public:
    BasicFunction(const char* name, TypeManager& tm)
    : FunctionInfo(1, std::string(name))
    {
      Type* rtype = tm.getType(typeid(void));
      _params[0] = tm.getType(typeid(A1));
      _params[1] = tm.getType(typeid(A2));
      this->init(*rtype, _params, 2);
    }

    Any call(const ArgumentList& args) override
    {
      ArgumentIterator arg = args.begin();
      A1 a1 = ArgumentTraits<A1>::cast(*_params[0], arg->type(), arg->get());
      ++arg;
      A2 a2 = ArgumentTraits<A2>::cast(*_params[1], arg->type(), arg->get());
      this->onCall(a1, a2);
      return Any();
    }

  protected:
    virtual void onCall(A1, A2) = 0;

  private:
    Type* _params[2];
};


template <typename A1>
class BasicFunction<void, A1, Void> : public FunctionInfo
{
  public:
    BasicFunction(const char* name, TypeManager& tm)
    : FunctionInfo(1, std::string(name))
    {
      Type* rtype = tm.getType(typeid(void));
      _params[0] = tm.getType(typeid(A1));
      this->init(*rtype, _params, 1);
    }

    Any call(const ArgumentList& args) override
    {
      ArgumentIterator arg = args.begin();
      A1 a1 = ArgumentTraits<A1>::cast(*_params[0], arg->type(), arg->get());
      this->onCall(a1);
      return Any();
    }

  protected:
    virtual void onCall(A1) = 0;

  private:
    Type* _params[1];
};


template <>
class BasicFunction<void, Void, Void> : public FunctionInfo
{
  public:
    BasicFunction(const char* name, TypeManager& tm)
    : FunctionInfo(1, std::string(name))
    {
      Type* rtype = tm.getType(typeid(void));
      this->init(*rtype, 0, 0);
    }

    Any call(const ArgumentList& /*args*/) override
    {
      this->onCall();
      return Any();
    }

  protected:
    virtual void onCall() = 0;
};

///////////////////////////////////////////////////////////////////////
// Function
///////////////////////////////////////////////////////////////////////

// Non-void return, 2 args (primary template)
template <typename R,
          typename A1 = Void,
          typename A2 = Void>
class Function : public FunctionInfo
{
  public:
    typedef R (*FuncPtr)(A1, A2);

    Function(TypeManager& tm, const char* name, FuncPtr func)
    : FunctionInfo(0, std::string(name))
    , _func(func)
    {
      Pt::Reflex::Type* rtype = tm.getType(typeid(R));
      _params[0] = tm.getType(typeid(A1));
      _params[1] = tm.getType(typeid(A2));
      this->init(*rtype, _params, 2);
    }

    Pt::Any call(const Pt::Reflex::ArgumentList& args) override
    {
      Pt::Reflex::ArgumentIterator arg = args.begin();
      A1 a1 = Pt::Reflex::ArgumentTraits<A1>::cast(*_params[0], arg->type(), arg->get());
      ++arg;
      A2 a2 = Pt::Reflex::ArgumentTraits<A2>::cast(*_params[1], arg->type(), arg->get());
      R r = _func(a1, a2);
      return Pt::Any(r);
    }

  private:
    FuncPtr           _func;
    Pt::Reflex::Type* _params[2];
};


// Non-void return, 1 arg
template <typename R, typename A1>
class Function<R, A1, Void> : public FunctionInfo
{
  public:
    typedef R (*FuncPtr)(A1);

    Function(TypeManager& tm, const char* name, FuncPtr func)
    : FunctionInfo(0, std::string(name))
    , _func(func)
    {
      Pt::Reflex::Type* rtype = tm.getType(typeid(R));
      _params[0] = tm.getType(typeid(A1));
      this->init(*rtype, _params, 1);
    }

    Pt::Any call(const Pt::Reflex::ArgumentList& args) override
    {
      Pt::Reflex::ArgumentIterator arg = args.begin();
      A1 a1 = Pt::Reflex::ArgumentTraits<A1>::cast(*_params[0], arg->type(), arg->get());
      R r = _func(a1);
      return Pt::Any(r);
    }

  private:
    FuncPtr           _func;
    Pt::Reflex::Type* _params[1];
};


// Non-void return, 0 args
template <typename R>
class Function<R, Void, Void> : public FunctionInfo
{
  public:
    typedef R (*FuncPtr)();

    Function(TypeManager& tm, const char* name, FuncPtr func)
    : FunctionInfo(0, std::string(name))
    , _func(func)
    {
      Pt::Reflex::Type* rtype = tm.getType(typeid(R));
      this->init(*rtype, 0, 0);
    }

    Pt::Any call(const Pt::Reflex::ArgumentList& /*args*/) override
    {
      R r = _func();
      return Pt::Any(r);
    }

  private:
    FuncPtr _func;
};


// Void return, 2 args
template <typename A1, typename A2>
class Function<void, A1, A2> : public FunctionInfo
{
  public:
    typedef void (*FuncPtr)(A1, A2);

    Function(TypeManager& tm, const char* name, FuncPtr func)
    : FunctionInfo(0, std::string(name))
    , _func(func)
    {
      Pt::Reflex::Type* rtype = tm.getType(typeid(void));
      _params[0] = tm.getType(typeid(A1));
      _params[1] = tm.getType(typeid(A2));
      this->init(*rtype, _params, 2);
    }

    Pt::Any call(const Pt::Reflex::ArgumentList& args) override
    {
      Pt::Reflex::ArgumentIterator arg = args.begin();
      A1 a1 = Pt::Reflex::ArgumentTraits<A1>::cast(*_params[0], arg->type(), arg->get());
      ++arg;
      A2 a2 = Pt::Reflex::ArgumentTraits<A2>::cast(*_params[1], arg->type(), arg->get());
      _func(a1, a2);
      return Pt::Any();
    }

  private:
    FuncPtr           _func;
    Pt::Reflex::Type* _params[2];
};


// Void return, 1 arg
template <typename A1>
class Function<void, A1, Void> : public FunctionInfo
{
  public:
    typedef void (*FuncPtr)(A1);

    Function(TypeManager& tm, const char* name, FuncPtr func)
    : FunctionInfo(0, std::string(name))
    , _func(func)
    {
      Pt::Reflex::Type* rtype = tm.getType(typeid(void));
      _params[0] = tm.getType(typeid(A1));
      this->init(*rtype, _params, 1);
    }

    Pt::Any call(const Pt::Reflex::ArgumentList& args) override
    {
      Pt::Reflex::ArgumentIterator arg = args.begin();
      A1 a1 = Pt::Reflex::ArgumentTraits<A1>::cast(*_params[0], arg->type(), arg->get());
      _func(a1);
      return Pt::Any();
    }

  private:
    FuncPtr           _func;
    Pt::Reflex::Type* _params[1];
};


// Void return, 0 args
template <>
class Function<void, Void, Void> : public FunctionInfo
{
  public:
    typedef void (*FuncPtr)();

    Function(TypeManager& tm, const char* name, FuncPtr func)
    : FunctionInfo(0, std::string(name))
    , _func(func)
    {
      Pt::Reflex::Type* rtype = tm.getType(typeid(void));
      this->init(*rtype, 0, 0);
    }

    Pt::Any call(const Pt::Reflex::ArgumentList& /*args*/) override
    {
      _func();
      return Pt::Any();
    }

  private:
    FuncPtr _func;
};

} // namespace Reflex

} // namespace Pt

#endif // PT_REFLEX_FUNCTION_H
