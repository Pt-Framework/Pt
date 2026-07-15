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
#ifndef PT_REFLEX_METHOD_H
#define PT_REFLEX_METHOD_H

#include <Pt/Reflex/MethodInfo.h>
#include <Pt/Reflex/TypeManager.h>
#include <Pt/Reflex/ArgumentTraits.h>
#include <Pt/Reflex/Type.h>
#include <Pt/Any.h>
#include <string>

namespace Pt {

namespace Reflex {

template < typename R,
           class C,
           typename A1 = Void,
           typename A2 = Void,
           typename A3 = Void,
           typename A4 = Void>
class Method : public MethodInfo
{
    public:
        typedef R (C::*MemFunc)(A1, A2, A3, A4);

    public:
        Method(TypeManager& ctx, const std::string& name, MemFunc mf)
        : MethodInfo(name)
        , _mf(mf)
        {
            Type* rtype = ctx.getType( typeid(R) );
            _params[0] = ctx.getType( typeid(A1) );
            _params[1] = ctx.getType( typeid(A2) );
            _params[2] = ctx.getType( typeid(A3) );
            _params[3] = ctx.getType( typeid(A4) );
            this->init(*rtype, _params, 4);
        }

        Any call(void* instance, const ArgumentList& args)
        {
            C* t = reinterpret_cast<C*>( instance );

            ArgumentIterator arg = args.begin();
            A1 a1 = ArgumentTraits<A1>::cast( *_params[0], arg->type(), arg->get() );

            ++arg;
            A2 a2 = ArgumentTraits<A2>::cast( *_params[1], arg->type(), arg->get() );

            ++arg;
            A3 a3 = ArgumentTraits<A3>::cast( *_params[2], arg->type(), arg->get() );

            ++arg;
            A4 a4 = ArgumentTraits<A4>::cast( *_params[3], arg->type(), arg->get() );

            R r = (t->*_mf)(a1, a2, a3, a4);
            return ReturnTraits<R>::make(r);
        }

    private:
        MemFunc _mf;
        Type* _params[4];
};


template < class C,
           typename A1,
           typename A2,
           typename A3,
           typename A4>
class Method<void, C,
             A1,
             A2,
             A3,
             A4> : public MethodInfo
{
    public:
        typedef void (C::*MemFunc)(A1, A2, A3, A4);

    public:
        Method(TypeManager& ctx, const std::string& name, MemFunc mf)
        : MethodInfo(name)
        , _mf(mf)
        {
            Type* rtype = ctx.getType( typeid(void) );
            _params[0] = ctx.getType( typeid(A1) );
            _params[1] = ctx.getType( typeid(A2) );
            _params[2] = ctx.getType( typeid(A3) );
            _params[3] = ctx.getType( typeid(A4) );
            this->init(*rtype, _params, 4);
        }

        Any call(void* instance, const ArgumentList& args)
        {
            C* t = reinterpret_cast<C*>( instance );

            ArgumentIterator arg = args.begin();
            A1 a1 = ArgumentTraits<A1>::cast( *_params[0], arg->type(), arg->get() );

            ++arg;
            A2 a2 = ArgumentTraits<A2>::cast( *_params[1], arg->type(), arg->get() );

            ++arg;
            A3 a3 = ArgumentTraits<A3>::cast( *_params[2], arg->type(), arg->get() );

            ++arg;
            A4 a4 = ArgumentTraits<A4>::cast( *_params[3], arg->type(), arg->get() );

            (t->*_mf)(a1, a2, a3, a4);
            return Any();
        }

    private:
        MemFunc _mf;
        Type* _params[4];
};


template < typename R,
           class C,
           typename A1,
           typename A2,
           typename A3>
class Method<R, C,
             A1,
             A2,
             A3> : public MethodInfo
{
    public:
        typedef R (C::*MemFunc)(A1, A2, A3);

    public:
        Method(TypeManager& ctx, const std::string& name, MemFunc mf)
        : MethodInfo(name)
        , _mf(mf)
        {
            Type* rtype = ctx.getType( typeid(R) );
            _params[0] = ctx.getType( typeid(A1) );
            _params[1] = ctx.getType( typeid(A2) );
            _params[2] = ctx.getType( typeid(A3) );
            this->init(*rtype, _params, 3);
        }

        Any call(void* instance, const ArgumentList& args)
        {
            C* t = reinterpret_cast<C*>( instance );

            ArgumentIterator arg = args.begin();
            A1 a1 = ArgumentTraits<A1>::cast( *_params[0], arg->type(), arg->get() );

            ++arg;
            A2 a2 = ArgumentTraits<A2>::cast( *_params[1], arg->type(), arg->get() );

            ++arg;
            A3 a3 = ArgumentTraits<A3>::cast( *_params[2], arg->type(), arg->get() );

            R r = (t->*_mf)(a1, a2, a3);
            return ReturnTraits<R>::make(r);
        }

    private:
        MemFunc _mf;
        Type* _params[3];
};


template < class C,
           typename A1,
           typename A2,
           typename A3>
class Method<void, C,
             A1,
             A2,
             A3> : public MethodInfo
{
    public:
        typedef void (C::*MemFunc)(A1, A2, A3);

    public:
        Method(TypeManager& ctx, const std::string& name, MemFunc mf)
        : MethodInfo(name)
        , _mf(mf)
        {
            Type* rtype = ctx.getType( typeid(void) );
            _params[0] = ctx.getType( typeid(A1) );
            _params[1] = ctx.getType( typeid(A2) );
            _params[2] = ctx.getType( typeid(A3) );
            this->init(*rtype, _params, 3);
        }

        Any call(void* instance, const ArgumentList& args)
        {
            C* t = reinterpret_cast<C*>( instance );

            ArgumentIterator arg = args.begin();
            A1 a1 = ArgumentTraits<A1>::cast( *_params[0], arg->type(), arg->get() );

            ++arg;
            A2 a2 = ArgumentTraits<A2>::cast( *_params[1], arg->type(), arg->get() );

            ++arg;
            A3 a3 = ArgumentTraits<A3>::cast( *_params[2], arg->type(), arg->get() );

            (t->*_mf)(a1, a2, a3);
            return Pt::Any();
        }

    private:
        MemFunc _mf;
        Type* _params[3];
};


template < typename R,
           class C,
           typename A1,
           typename A2>
class Method<R, C,
             A1,
             A2> : public MethodInfo
{
    public:
        typedef R (C::*MemFunc)(A1, A2);

    public:
        Method(TypeManager& ctx, const std::string& name, MemFunc mf)
        : MethodInfo(name)
        , _mf(mf)
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

            R r = (t->*_mf)(a1,a2);
            return ReturnTraits<R>::make(r);
        }

    private:
        MemFunc _mf;
        Type* _params[2];
};


template < class C,
           typename A1,
           typename A2>
class Method<void, C,
            A1,
            A2> : public MethodInfo
{
    public:
        typedef void (C::*MemFunc)(A1, A2);

    public:
        Method(TypeManager& ctx, const std::string& name, MemFunc mf)
        : MethodInfo(name)
        , _mf(mf)
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

            (t->*_mf)(a1,a2);
            return Any();
        }

    private:
        MemFunc _mf;
        Type* _params[2];
};


template < typename R,
           class C,
           typename A1>
class Method<R, C,
             A1,
             Void> : public MethodInfo
{
    public:
        typedef R (C::*MemFunc)(A1);

    public:
        Method(TypeManager& ctx, const std::string& name, MemFunc mf)
        : MethodInfo(name)
        , _mf(mf)
        {
            Type* rtype = ctx.getType( typeid(R) );
            _params[0] = ctx.getType( typeid(A1) );
            this->init(*rtype, _params, 1);
        }

        Any call(void* instance, const ArgumentList& args)
        {
            C* t = reinterpret_cast<C*>( instance );

            ArgumentIterator arg = args.begin();
            A1 a1 = ArgumentTraits<A1>::cast( *_params[0], arg->type(), arg->get() );

            R r = (t->*_mf)(a1);
            return ReturnTraits<R>::make(r);
        }

    private:
        MemFunc _mf;
        Type* _params[1];
};


template < class C,
           typename A1>
class Method<void, C,
             A1,
             Void> : public MethodInfo
{
    public:
        typedef void (C::*MemFunc)(A1);

    public:
        Method(TypeManager& ctx, const std::string& name, MemFunc mf)
        : MethodInfo(name)
        , _mf(mf)
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

            (t->*_mf)(a1);
            return Any();
        }

    private:
        MemFunc _mf;
        Type* _params[1];
};


template < typename R,
           class C>
class Method<R, C,
             Void,
             Void> : public MethodInfo
{
    public:
        typedef R (C::*MemFunc)();

    public:
        Method(TypeManager& ctx, const std::string& name, MemFunc mf)
        : MethodInfo(name)
        , _mf(mf)
        {
            Type* rtype = ctx.getType( typeid(R) );
            this->init(*rtype, 0, 0);
        }

        Any call(void* instance, const ArgumentList& args)
        {
            C* t = reinterpret_cast<C*>( instance );

            R r = (t->*_mf)();
            return ReturnTraits<R>::make(r);
        }

    private:
        MemFunc _mf;
};


template < class C>
class Method<void, C,
             Void,
             Void> : public MethodInfo
{
    public:
        typedef void (C::*MemFunc)();

    public:
        Method(TypeManager& ctx, const std::string& name, MemFunc mf)
        : MethodInfo(name)
        , _mf(mf)
        {
            Type* rtype = ctx.getType( typeid(void) );
            this->init(*rtype, 0, 0);
        }

        Any call(void* instance, const ArgumentList& args)
        {
            C* t = reinterpret_cast<C*>( instance );

            (t->*_mf)();
            return Any();
        }

    private:
        MemFunc _mf;
};

} // namespace Reflex

} // namespace Pt

#endif // PT_REFLEX_METHOD_H
