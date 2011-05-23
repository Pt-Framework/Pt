/*
 * Copyright (C) 2004-2010 by Marc Boris Duerner
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
#ifndef PT_REFLEX_CONSTRUCTOR_PROXY_H
#define PT_REFLEX_CONSTRUCTOR_PROXY_H

#include <Pt/Reflex/ConstructorInfo.h>
#include <Pt/Reflex/ArgumentTraits.h>
#include <Pt/Reflex/TypeManager.h>
#include <Pt/Reflex/Type.h>

namespace Pt {

namespace Reflex {

template < class T,
           typename A1 = Void,
           typename A2 = Void,
           typename A3 = Void>
class ConstructorProxy : public ConstructorInfo
{
    public:
        typedef void (T::*Ctor)(void*, A1, A2, A3);

    public:
        ConstructorProxy(TypeManager& ctx, T& type, Ctor ctor)
        : ConstructorInfo()
        , _type(&type)
        , _ctor(ctor)
        {
            _params[0] = ctx.getType( typeid(A1) );
            _params[1] = ctx.getType( typeid(A2) );
            _params[2] = ctx.getType( typeid(A3) );
            this->init(_params, 3);
        }

        void call(void* to, const ArgumentList& args)
        {
            ArgumentIterator arg = args.begin();
            A1 a1 = ArgumentTraits<A1>::cast( *_params[0], arg->type(), arg->get() );

            ++arg;
            A2 a2 = ArgumentTraits<A2>::cast( *_params[1], arg->type(), arg->get() );

            ++arg;
            A3 a3 = ArgumentTraits<A2>::cast( *_params[2], arg->type(), arg->get() );

            (_type->*_ctor)(to, a1, a2, a3);
        }

    private:
        T* _type;
        Ctor _ctor;
        Type* _params[3];
};

template < class T,
           typename A1,
           typename A2>
class ConstructorProxy<T,
                       A1,
                       A2,
                       Void> : public ConstructorInfo
{
    public:
        typedef void (T::*Ctor)(void*, A1, A2);

    public:
        ConstructorProxy(TypeManager& ctx, T& type, Ctor ctor)
        : ConstructorInfo()
        , _type(&type)
        , _ctor(ctor)
        {
            _params[0] = ctx.getType( typeid(A1) );
            _params[1] = ctx.getType( typeid(A2) );
            this->init(_params, 2);
        }

        void call(void* to, const ArgumentList& args)
        {
            ArgumentIterator arg = args.begin();
            A1 a1 = ArgumentTraits<A1>::cast( *_params[0], arg->type(), arg->get() );

            ++arg;
            A2 a2 = ArgumentTraits<A2>::cast( *_params[1], arg->type(), arg->get() );

            (_type->*_ctor)(to, a1, a2);
        }

    private:
        T* _type;
        Ctor _ctor;
        Type* _params[2];
};


template < class T,
           typename A1>
class ConstructorProxy<T,
                       A1,
                       Void,
                       Void> : public ConstructorInfo
{
    public:
        typedef void (T::*Ctor)( void*, A1);

    public:
        ConstructorProxy(TypeManager& ctx, T& type, Ctor ctor)
        : ConstructorInfo()
        , _type(&type)
        , _ctor(ctor)
        {
            _params[0] = ctx.getType( typeid(A1) );
            this->init(_params, 1);
        }

        void call(void* to, const ArgumentList& args)
        {
            ArgumentIterator arg = args.begin();
            A1 a1 = ArgumentTraits<A1>::cast( *_params[0], arg->type(), arg->get() );

            (_type->*_ctor)(to, a1);
        }

    private:
        T* _type;
        Ctor _ctor;
        Type* _params[1];
};


template < class T>
class ConstructorProxy<T,
                       Void,
                       Void,
                       Void> : public ConstructorInfo
{
    public:
        typedef void (T::*Ctor)(void*);

    public:
        ConstructorProxy(TypeManager& ctx, T& type, Ctor ctor)
        : ConstructorInfo()
        , _type(&type)
        , _ctor(ctor)
        {
            this->init(0, 0);
        }

        void call(void* to, const ArgumentList& args)
        {
            (_type->*_ctor)(to);
        }

    private:
        T* _type;
        Ctor _ctor;
};

}

}

#endif
