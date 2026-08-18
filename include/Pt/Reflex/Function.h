/*
  Copyright (C) 2004-2026 by Marc Boris Duerner

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

#ifndef PT_REFLEX_FUNCTION_H
#define PT_REFLEX_FUNCTION_H

#include <Pt/Reflex/FunctionInfo.h>
#include <Pt/Reflex/Type.h>
#include <Pt/Reflex/Argument.h>
#include <Pt/Reflex/ArgumentTraits.h>
#include <Pt/TypeTraits.h>
#include <Pt/Any.h>
#include <utility>

namespace Pt {

namespace Reflex {

///////////////////////////////////////////////////////////////////////
// BasicFunction
///////////////////////////////////////////////////////////////////////

template <typename R, typename... As>
class BasicFunction : public FunctionInfo
{
    public:
        BasicFunction(const char* name, TypeManager& tm)
        : FunctionInfo(1, std::string(name))
        {
            Type* rtype = tm.getType(typeid(R));
            Type* ptypes[] = { tm.getType(typeid(As))...,
                               static_cast<Type*>(0) };

            for(std::size_t n = 0; n < sizeof...(As); ++n)
                _params[n] = ptypes[n];

            this->init(*rtype, sizeof...(As) ? _params : 0, sizeof...(As));
        }

        Any call(const ArgumentList& args) override
        {
            return this->callWith(args.begin(), std::index_sequence_for<As...>());
        }

    protected:
        virtual R onCall(As...) = 0;

    private:
        template <std::size_t I, typename... Ts>
        using NthArgType = typename NthType<I, Ts...>::type;

        template <std::size_t I, typename... Ts>
        using ArgTr = ArgumentTraits< NthArgType<I, Ts...> >;

        template <std::size_t... Is>
        Any callWith(ArgumentIterator arg, std::index_sequence<Is...>)
        {
            R r = this->onCall( ArgTr<Is, As...>::cast(*_params[Is],
                                                       (arg + Is)->type(),
                                                       (arg + Is)->get() )... );
            return ReturnTraits<R>::make(r);
        }

    private:
        Type* _params[sizeof...(As) + 1];
};


template <typename... As>
class BasicFunction<void, As...> : public FunctionInfo
{
    public:
         BasicFunction(const char* name, TypeManager& tm)
        : FunctionInfo(1, std::string(name))
        {
            Type* rtype = tm.getType(typeid(void));
            Type* ptypes[] = { tm.getType(typeid(As))...,
                               static_cast<Type*>(0) };

            for(std::size_t n = 0; n < sizeof...(As); ++n)
                _params[n] = ptypes[n];

            this->init(*rtype, sizeof...(As) ? _params : 0, sizeof...(As));
        }

        Any call(const ArgumentList& args) override
        {
            return this->callWith(args.begin(), std::index_sequence_for<As...>());
        }

    protected:
          virtual void onCall(As...) = 0;

    private:
        template <std::size_t I, typename... Ts>
        using NthArgType = typename NthType<I, Ts...>::type;

        template <std::size_t I, typename... Ts>
        using ArgTr = ArgumentTraits< NthArgType<I, Ts...> >;

        template <std::size_t... Is>
        Any callWith(ArgumentIterator arg, std::index_sequence<Is...>)
        {
            this->onCall( ArgTr<Is, As...>::cast(*_params[Is],
                                                 (arg + Is)->type(),
                                                 (arg + Is)->get() )... );
            return Any();
        }

    private:
        Type* _params[sizeof...(As) + 1];
};

///////////////////////////////////////////////////////////////////////
// Function
///////////////////////////////////////////////////////////////////////

template <typename R, typename... As>
class Function : public FunctionInfo
{
    public:
        typedef R (*FuncPtr)(As...);

    public:
        Function(TypeManager& tm, const char* name, FuncPtr func)
        : FunctionInfo(0, std::string(name))
        , _func(func)
        {
            Type* rtype = tm.getType(typeid(R));
            Type* ptypes[] = { tm.getType(typeid(As))...,
                               static_cast<Type*>(0) };

            for(std::size_t n = 0; n < sizeof...(As); ++n)
                _params[n] = ptypes[n];

            this->init(*rtype, sizeof...(As) ? _params : 0, sizeof...(As));
        }

        Any call(const ArgumentList& args) override
        {
            return this->callWith(args.begin(), std::index_sequence_for<As...>());
        }

    private:
        template <std::size_t I, typename... Ts>
        using NthArgType = typename NthType<I, Ts...>::type;

        template <std::size_t I, typename... Ts>
        using ArgTr = ArgumentTraits< NthArgType<I, Ts...> >;

        template <std::size_t... Is>
        Any callWith(ArgumentIterator arg, std::index_sequence<Is...>)
        {
            R r = _func( ArgTr<Is, As...>::cast(*_params[Is],
                                                (arg + Is)->type(),
                                                (arg + Is)->get() )... );
            return ReturnTraits<R>::make(r);
        }

    private:
        FuncPtr _func;
        Type* _params[sizeof...(As) + 1];
};


template <typename... As>
class Function<void, As...> : public FunctionInfo
{
    public:
        typedef void (*FuncPtr)(As...);

    public:
        Function(TypeManager& tm, const char* name, FuncPtr func)
        : FunctionInfo(0, std::string(name))
        , _func(func)
        {
            Type* rtype = tm.getType(typeid(void));
            Type* ptypes[] = { tm.getType(typeid(As))...,
                               static_cast<Type*>(0) };

            for(std::size_t n = 0; n < sizeof...(As); ++n)
                _params[n] = ptypes[n];

            this->init(*rtype, sizeof...(As) ? _params : 0, sizeof...(As));
        }

        Any call(const ArgumentList& args) override
        {
            return this->callWith(args.begin(), std::index_sequence_for<As...>());
        }

    private:
        template <std::size_t I, typename... Ts>
        using NthArgType = typename NthType<I, Ts...>::type;

        template <std::size_t I, typename... Ts>
        using ArgTr = ArgumentTraits< NthArgType<I, Ts...> >;

        template <std::size_t... Is>
        Any callWith(ArgumentIterator arg, std::index_sequence<Is...>)
        {
            _func( ArgTr<Is, As...>::cast(*_params[Is],
                                          (arg + Is)->type(),
                                          (arg + Is)->get() )... );
            return Any();
        }

    private:
        FuncPtr _func;
        Type* _params[sizeof...(As) + 1];
};

} // namespace Reflex

} // namespace Pt

#endif // PT_REFLEX_FUNCTION_H
