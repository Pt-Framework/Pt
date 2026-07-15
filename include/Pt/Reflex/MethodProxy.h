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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA  02110-1301  USA
 */
#ifndef PT_REFLEX_METHODPROXY_H
#define PT_REFLEX_METHODPROXY_H

#include <Pt/Reflex/MethodInfo.h>
#include <Pt/Reflex/TypeManager.h>
#include <Pt/Reflex/ArgumentTraits.h>
#include <Pt/Reflex/Type.h>
#include <Pt/Any.h>
#include <Pt/Reflex/GenericMethod.h>
#include <Pt/Reflex/Method.h>
#include <string>

namespace Pt {

namespace Reflex {

///////////////////////////////////////////////////////////////////////
// MethodProxy
///////////////////////////////////////////////////////////////////////

template < typename R,
           class C,
           typename A1 = Void,
           typename A2 = Void>
class MethodProxy : public MethodInfo
{
    public:
        typedef R (*FuncPtr)(C&, A1, A2);

    public:
        MethodProxy(TypeManager& ctx, const std::string& name, FuncPtr proxy)
        : MethodInfo(name)
        , _proxy( proxy )
        {
            Type* rtype = ctx.getType( typeid(R) );
            _params[0] = ctx.getType( typeid(A1) );
            _params[1] = ctx.getType( typeid(A2) );
            this->init(*rtype, _params, 2);
        }

        Any call(void* instance, const ArgumentList& args)
        {
            C* t = reinterpret_cast<C*>( instance );

            ArgumentIterator arg = args.begin();
            A1 a1 = ArgumentTraits<A1>::cast( *_params[0], arg->type(), arg->get() );

            ++arg;
            A2 a2 = ArgumentTraits<A2>::cast( *_params[1], arg->type(), arg->get() );

            R r = _proxy( *t, a1, a2 );
            return ReturnTraits<R>::make(r);
        }

    private:
        FuncPtr _proxy;
        Type* _params[2];
};


template < class C,
           typename A1,
           typename A2>
class MethodProxy<void, C, A1, A2> : public MethodInfo
{
    public:
        typedef void (*FuncPtr)(C&, A1, A2);

    public:
        MethodProxy(TypeManager& ctx, const std::string& name, FuncPtr proxy)
        : MethodInfo(name)
        , _proxy( proxy )
        {
            Type* rtype = ctx.getType( typeid(void) );
            _params[0] = ctx.getType( typeid(A1) );
            _params[1] = ctx.getType( typeid(A2) );
            this->init(*rtype, _params, 2);
        }

        Any call(void* instance, const ArgumentList& args)
        {
            C* t = reinterpret_cast<C*>( instance );

            ArgumentIterator arg = args.begin();
            A1 a1 = ArgumentTraits<A1>::cast( *_params[0], arg->type(), arg->get() );

            ++arg;
            A2 a2 = ArgumentTraits<A2>::cast( *_params[1], arg->type(), arg->get() );

            _proxy( *t, a1, a2 );
            return Any();
        }

    private:
        FuncPtr _proxy;
        Type* _params[2];
};


template < typename R,
           class C,
           typename A1>
class MethodProxy<R, C,
                 A1,
                 Void> : public MethodInfo
{
    public:
        typedef R (*FuncPtr)( C&, A1);

    public:
        MethodProxy(TypeManager& ctx, const std::string& name, FuncPtr proxy)
        : MethodInfo(name)
        , _proxy( proxy )
        {
            Type* rtype  = ctx.getType( typeid(R) );
            _params[0] = ctx.getType( typeid(A1) );
            this->init(*rtype, _params, 1);
        }

        Any call(void* instance, const ArgumentList& args)
        {
            C* t = reinterpret_cast<C*>( instance );

            ArgumentIterator arg = args.begin();
            A1 a1 = ArgumentTraits<A1>::cast( *_params[0], arg->type(), arg->get() );

            R r = _proxy( *t, a1 );
            return ReturnTraits<R>::make(r);
        }

    private:
        FuncPtr _proxy;
        Type* _params[1];
};


template < class C,
           typename A1>
class MethodProxy<void, C,
                 A1,
                 Void> : public MethodInfo
{
    public:
        typedef void (*FuncPtr)( C&, A1);

    public:
        MethodProxy(TypeManager& ctx, const std::string& name, FuncPtr proxy)
        : MethodInfo(name)
        , _proxy( proxy )
        {
            Type* rtype = ctx.getType( typeid(void) );
            _params[0] = ctx.getType( typeid(A1) );
            this->init(*rtype, _params, 1);
        }

        Any call(void* instance, const ArgumentList& args)
        {
            C* t = reinterpret_cast<C*>( instance );

            ArgumentIterator arg = args.begin();
            A1 a1 = ArgumentTraits<A1>::cast( *_params[0], arg->type(), arg->get() );

            _proxy( *t, a1 );
            return Any();
        }

    private:
        FuncPtr _proxy;
        Type* _params[1];
};


template < typename R,
           class C >
class MethodProxy<R, C,
                 Void,
                 Void> : public MethodInfo
{
    public:
        typedef R (*FuncPtr)( C&);

    public:
        MethodProxy(TypeManager& ctx, const std::string& name, FuncPtr proxy)
        : MethodInfo(name)
        , _proxy( proxy )
        {
            Type* rtype = ctx.getType( typeid(R) );
            this->init(*rtype, 0, 0);
        }

        Any call(void* instance, const ArgumentList&)
        {
            C* t = reinterpret_cast<C*>( instance );

            R r = _proxy(*t);
            return ReturnTraits<R>::make(r);
        }

    private:
        FuncPtr _proxy;
};


template < class C >
class MethodProxy<void, C,
                  Void,
                  Void> : public MethodInfo
{
    public:
        typedef void (*FuncPtr)( C&);

    public:
        MethodProxy(TypeManager& ctx, const std::string& name, FuncPtr proxy)
        : MethodInfo(name)
        , _proxy( proxy )
        {
            Type* rtype = ctx.getType( typeid(void) );
            this->init(*rtype, 0, 0);
        }

        Any call(void* instance, const ArgumentList& args)
        {
            C* t = reinterpret_cast<C*>( instance );

            _proxy(*t);
            return Any();
        }

    private:
        FuncPtr _proxy;
};

} // namespace Reflex

} // namespace Pt

#endif
