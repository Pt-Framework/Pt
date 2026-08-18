/*
  Copyright (C) 2004-2010 by Marc Boris Duerner

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

#ifndef PT_REFLEX_CONSTRUCTOR_PROXY_H
#define PT_REFLEX_CONSTRUCTOR_PROXY_H

#include <Pt/Reflex/ConstructorInfo.h>
#include <Pt/Reflex/ArgumentTraits.h>
#include <Pt/Reflex/TypeManager.h>
#include <Pt/Reflex/Type.h>
#include <Pt/TypeTraits.h>
#include <utility>

namespace Pt {

namespace Reflex {

template <class T, typename... As>
class ConstructorProxy : public ConstructorInfo
{
    public:
        typedef void (T::*Ctor)(void*, As...);

    public:
        ConstructorProxy(TypeManager& ctx, T& type, Ctor ctor)
        : ConstructorInfo()
        , _type(&type)
        , _ctor(ctor)
        , _params{ ctx.getType( typeid(As) )...,
                   static_cast<Type*>(0) }
        {
            this->init(sizeof...(As) ? _params : 0, sizeof...(As));
        }

        void call(void* to, const ArgumentList& args) override
        {
            this->callWith(to, args.begin(), std::index_sequence_for<As...>());
        }

    protected:
        template <std::size_t I, typename... Ts>
        using NthArgType = typename NthType<I, Ts...>::type;

        template <std::size_t I, typename... Ts>
        using ArgTr = ArgumentTraits< NthArgType<I, Ts...> >;

        template <std::size_t... Is>
        void callWith(void* to, ArgumentIterator arg, std::index_sequence<Is...>)
        {
            (_type->*_ctor)(to, ArgTr<Is, As...>::cast(*_params[Is],
                                                       (arg + Is)->type(),
                                                       (arg + Is)->get() )... );
        }

    private:
        T* _type;
        Ctor _ctor;
        Type* _params[sizeof...(As) + 1];
};

}

}

#endif
