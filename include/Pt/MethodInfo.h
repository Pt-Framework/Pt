/***************************************************************************
 *   Copyright (C) 2005 by Marc Boris Dürner                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this program; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef Pt_MethodInfo_h
#define Pt_MethodInfo_h

#include <Pt/Args.h>
#include <Pt/Exception.h>
#include <Pt/Method.h>
#include <Pt/ConstMethod.h>
#include <Pt/MethodInfoBase.h>
#include <Pt/SerializationData.h>
#include <stdexcept>


namespace Pt {

template < typename R,
           class C,
           typename A1 = Pt::Void,
           typename A2 = Pt::Void,
           typename A3 = Pt::Void,
           typename A4 = Pt::Void,
           typename A5 = Pt::Void>
class MethodInfo : public MethodInfoBase<R, C, A1, A2, A3, A4, A5>
{
    public:
        typedef C ClassT;
        typedef R (C::*MemFuncT)(A1, A2, A3, A4, A5);
        typedef R (C::*ConstMemFuncT)(A1, A2, A3, A4, A5) const;

    public:
        MethodInfo(const std::string& name, C& object, MemFuncT memFunc)
        : MethodInfoBase<R, C, A1, A2, A3, A4, A5>(name)
        , _cb( new Method<R, C, A1, A2, A3, A5>(object, memFunc) )
        {}

        MethodInfo(const std::string& name, C& object, ConstMemFuncT memFunc)
        : MethodInfoBase<R, C, A1, A2, A3, A4, A5>(name)
        , _cb( new ConstMethod<R, C, A1, A2, A3, A4, A5>(object, memFunc) )
        {}

        ~MethodInfo()
        { delete _cb; }

        Pt::Any call(const Any* args, size_t argCount)
        {
            if(argCount < 5)
                throw std::invalid_argument("Not enough arguments");

            return _cb->call( any_cast<A1>( args[0] ),
                              any_cast<A2>( args[1] ),
                              any_cast<A3>( args[2] ),
                              any_cast<A4>( args[3] ),
                              any_cast<A5>( args[4] ) );
        }

        virtual void call(const SerializationData& args) const
        {
            typename Pt::TypeInfo<A1>::Value a1;
            args.getNode(0) >> a1;

            typename Pt::TypeInfo<A2>::Value a2;
            args.getNode(1) >> a2;

            typename Pt::TypeInfo<A3>::Value a3;
            args.getNode(2) >> a3;

            typename Pt::TypeInfo<A4>::Value a4;
            args.getNode(3) >> a4;

            typename Pt::TypeInfo<A5>::Value a5;
            args.getNode(4) >> a5;

            _cb->invoke(a1, a2, a3, a4, a5);
        }

    private:
        Callable<R, A1, A2, A3, A4, A5>* _cb;
};


template < class C,
           typename A1,
           typename A2,
           typename A3,
           typename A4,
           typename A5>
class MethodInfo<void, C, A1, A2, A3, A4, A5> : public MethodInfoBase<void, C, A1, A2, A3, A4, A5>
{
    public:
        typedef C ClassT;
        typedef void (C::*MemFuncT)(A1, A2, A3, A4, A5);
        typedef void (C::*ConstMemFuncT)(A1, A2, A3, A4, A5) const;

    public:
        MethodInfo(const std::string& name, C& object, MemFuncT memFunc)
        : MethodInfoBase<void, C, A1, A2, A3, A4, A5>(name)
        , _cb( new Method<void, C, A1, A2, A3, A4, A5>(object, memFunc) )
        {}

        MethodInfo(const std::string& name, C& object, ConstMemFuncT memFunc)
        : MethodInfoBase<void, C, A1, A2, A3, A4, A5>(name)
        , _cb( new ConstMethod<void, C, A1, A2, A3, A4, A5>(object, memFunc) )
        {}

        ~MethodInfo()
        { delete _cb; }

        Pt::Any call(const Any* args, size_t argCount)
        {
            if(argCount < 5)
                throw std::invalid_argument("Not enough arguments");

            _cb->call( any_cast<A1>( args[0] ),
                       any_cast<A2>( args[1] ),
                       any_cast<A3>( args[2] ),
                       any_cast<A4>( args[3] ),
                       any_cast<A5>( args[4] ) );

            return Any();
        }

        virtual void call(const SerializationData& args) const
        {
            typename Pt::TypeInfo<A1>::Value a1;
            args.getNode(0) >> a1;

            typename Pt::TypeInfo<A2>::Value a2;
            args.getNode(1) >> a2;

            typename Pt::TypeInfo<A3>::Value a3;
            args.getNode(2) >> a3;

            typename Pt::TypeInfo<A4>::Value a4;
            args.getNode(3) >> a4;

            typename Pt::TypeInfo<A5>::Value a5;
            args.getNode(4) >> a5;

            _cb->invoke(a1, a2, a3, a4, a5);
        }

    private:
        Callable<void, A1, A2, A3, A4, A5>* _cb;
};


template < typename R,
           class C,
           typename A1,
           typename A2,
           typename A3,
           typename A4>
class MethodInfo<R, C, A1, A2, A3, A4, Pt::Void> : public MethodInfoBase<R, C, A1, A2, A3, A4>
{
    public:
        typedef C ClassT;
        typedef R (C::*MemFuncT)(A1, A2, A3, A4);
        typedef R (C::*ConstMemFuncT)(A1, A2, A3, A4) const;

    public:
        MethodInfo(const std::string& name, C& object, MemFuncT memFunc)
        : MethodInfoBase<R, C, A1, A2, A3, A4>(name)
        , _cb( new Method<R, C, A1, A2, A3>(object, memFunc) )
        {}

        MethodInfo(const std::string& name, C& object, ConstMemFuncT memFunc)
        : MethodInfoBase<R, C, A1, A2, A3, A4>(name)
        , _cb( new ConstMethod<R, C, A1, A2, A3, A4>(object, memFunc) )
        {}

        ~MethodInfo()
        { delete _cb; }

        Pt::Any call(const Any* args, size_t argCount)
        {
            if(argCount < 4)
                throw std::invalid_argument("Not enough arguments");

            return _cb->call( any_cast<A1>( args[0] ),
                              any_cast<A2>( args[1] ),
                              any_cast<A3>( args[2] ),
                              any_cast<A4>( args[3] ) );
        }

        virtual void call(const SerializationData& args) const
        {
            typename Pt::TypeInfo<A1>::Value a1;
            args.getNode(0) >> a1;

            typename Pt::TypeInfo<A2>::Value a2;
            args.getNode(1) >> a2;

            typename Pt::TypeInfo<A3>::Value a3;
            args.getNode(2) >> a3;

            typename Pt::TypeInfo<A4>::Value a4;
            args.getNode(3) >> a4;

            _cb->invoke(a1, a2, a3, a4);
        }

    private:
        Callable<R, A1, A2, A3, A4>* _cb;
};


template < class C,
           typename A1,
           typename A2,
           typename A3,
           typename A4>
class MethodInfo<void, C, A1, A2, A3, A4, Pt::Void> : public MethodInfoBase<void, C, A1, A2, A3, A4>
{
    public:
        typedef C ClassT;
        typedef void (C::*MemFuncT)(A1, A2, A3, A4);
        typedef void (C::*ConstMemFuncT)(A1, A2, A3,A4) const;

    public:
        MethodInfo(const std::string& name, C& object, MemFuncT memFunc)
        : MethodInfoBase<void, C, A1, A2, A3, A4>(name)
        , _cb( new Method<void, C, A1, A2, A3, A4>(object, memFunc) )
        {}

        MethodInfo(const std::string& name, C& object, ConstMemFuncT memFunc)
        : MethodInfoBase<void, C, A1, A2, A3, A4>(name)
        , _cb( new ConstMethod<void, C, A1, A2, A3, A4>(object, memFunc) )
        {}

        ~MethodInfo()
        { delete _cb; }

        Pt::Any call(const Any* args, size_t argCount)
        {
            if(argCount < 4)
                throw std::invalid_argument("Not enough arguments");

            _cb->call( any_cast<A1>( args[0] ),
                       any_cast<A2>( args[1] ),
                       any_cast<A3>( args[2] ),
                       any_cast<A4>( args[3] ) );

            return Any();
        }

        virtual void call(const SerializationData& args) const
        {
            typename Pt::TypeInfo<A1>::Value a1;
            args.getNode(0) >> a1;

            typename Pt::TypeInfo<A2>::Value a2;
            args.getNode(1) >> a2;

            typename Pt::TypeInfo<A3>::Value a3;
            args.getNode(2) >> a3;

            typename Pt::TypeInfo<A4>::Value a4;
            args.getNode(3) >> a4;

            _cb->invoke(a1, a2, a3, a4);
        }

    private:
        Callable<void, A1, A2, A3, A4>* _cb;
};


template < typename R,
           class C,
           typename A1,
           typename A2,
           typename A3>
class MethodInfo<R, C, A1, A2, A3, Pt::Void, Pt::Void> : public MethodInfoBase<R, C, A1, A2, A3>
{
    public:
        typedef C ClassT;
        typedef R (C::*MemFuncT)(A1, A2, A3);
        typedef R (C::*ConstMemFuncT)(A1, A2, A3) const;

    public:
        MethodInfo(const std::string& name, C& object, MemFuncT memFunc)
        : MethodInfoBase<R, C, A1, A2, A3>(name)
        , _cb( new Method<R, C, A1, A2, A3>(object, memFunc) )
        {}

        MethodInfo(const std::string& name, C& object, ConstMemFuncT memFunc)
        : MethodInfoBase<R, C, A1, A2, A3>(name)
        , _cb( new ConstMethod<R, C, A1, A2, A3>(object, memFunc) )
        {}

        ~MethodInfo()
        { delete _cb; }

        Pt::Any call(const Any* args, size_t argCount)
        {
            if(argCount < 3)
                throw std::invalid_argument("Not enough arguments");

            return _cb->call( any_cast<A1>( args[0] ),
                              any_cast<A2>( args[1] ),
                              any_cast<A3>( args[2] ));
        }

        virtual void call(const SerializationData& args) const
        {
            typename Pt::TypeInfo<A1>::Value a1;
            args.getNode(0) >> a1;

            typename Pt::TypeInfo<A2>::Value a2;
            args.getNode(1) >> a2;

            typename Pt::TypeInfo<A3>::Value a3;
            args.getNode(2) >> a3;

            _cb->invoke(a1, a2, a3);
        }

    private:
        Callable<R, A1, A2, A3>* _cb;
};


template < class C,
           typename A1,
           typename A2,
           typename A3>
class MethodInfo<void, C, A1, A2, A3, Pt::Void, Pt::Void> : public MethodInfoBase<void, C, A1, A2, A3>
{
    public:
        typedef C ClassT;
        typedef void (C::*MemFuncT)(A1, A2, A3);
        typedef void (C::*ConstMemFuncT)(A1, A2, A3) const;

    public:
        MethodInfo(const std::string& name, C& object, MemFuncT memFunc)
        : MethodInfoBase<void, C, A1, A2, A3>(name)
        , _cb( new Method<void, C, A1, A2, A3>(object, memFunc) )
        {}

        MethodInfo(const std::string& name, C& object, ConstMemFuncT memFunc)
        : MethodInfoBase<void, C, A1, A2, A3>(name)
        , _cb( new ConstMethod<void, C, A1, A2, A3>(object, memFunc) )
        {}

        ~MethodInfo()
        { delete _cb; }

        Pt::Any call(const Any* args, size_t argCount)
        {
            if(argCount < 3)
                throw std::invalid_argument("Not enough arguments");

            _cb->call( any_cast<A1>( args[0] ),
                                               any_cast<A2>( args[1] ),
                                               any_cast<A3>( args[2] ));
            return Any();
        }

        virtual void call(const SerializationData& args) const
        {
            typename Pt::TypeInfo<A1>::Value a1;
            args.getNode(0) >> a1;

            typename Pt::TypeInfo<A2>::Value a2;
            args.getNode(1) >> a2;

            typename Pt::TypeInfo<A3>::Value a3;
            args.getNode(2) >> a3;

            _cb->invoke(a1, a2, a3);
        }

    private:
        Callable<void, A1, A2, A3>* _cb;
};


template < typename R,
           class C,
           typename A1,
           typename A2>
class MethodInfo<R, C, A1, A2, Pt::Void, Pt::Void, Pt::Void> : public MethodInfoBase<R, C, A1, A2>
{
    public:
        typedef C ClassT;
        typedef R (C::*MemFuncT)(A1, A2);
        typedef R (C::*ConstMemFuncT)(A1, A2) const;

    public:
        MethodInfo(const std::string& name, C& object, MemFuncT memFunc)
        : MethodInfoBase<R, C, A1, A2>(name)
        , _cb( new Method<R, C, A1, A2>(object, memFunc) )
        {}

        MethodInfo(const std::string& name, C& object, ConstMemFuncT memFunc)
        : MethodInfoBase<R, C, A1, A2>(name)
        , _cb( new ConstMethod<R, C, A1, A2>(object, memFunc) )
        {}

        ~MethodInfo()
        { delete _cb; }

        Pt::Any call(const Any* args, size_t argCount)
        {
            if(argCount < 2)
                throw std::invalid_argument("Not enough arguments");

            return _cb->call( any_cast<A1>( args[0] ),
                              any_cast<A2>( args[1] ) );
        }

        virtual void call(const SerializationData& args) const
        {
            typename Pt::TypeInfo<A1>::Value a1;
            args.getNode(0) >> a1;

            typename Pt::TypeInfo<A2>::Value a2;
            args.getNode(1) >> a2;

            _cb->invoke(a1, a2);
        }

    private:
        Callable<void, A1, A2>* _cb;
};


template < class C,
           typename A1,
           typename A2>
class MethodInfo<void, C, A1, A2, Pt::Void, Pt::Void, Pt::Void> : public MethodInfoBase<void, C, A1, A2>
{
    public:
        typedef C ClassT;
        typedef void (C::*MemFuncT)(A1, A2);
        typedef void (C::*ConstMemFuncT)(A1, A2) const;

    public:
        MethodInfo(const std::string& name, C& object, MemFuncT memFunc)
        : MethodInfoBase<void, C, A1, A2>(name)
        , _cb( new Method<void, C, A1, A2>(object, memFunc) )
        {}

        MethodInfo(const std::string& name, C& object, ConstMemFuncT memFunc)
        : MethodInfoBase<void, C, A1, A2>(name)
        , _cb( new ConstMethod<void, C, A1, A2>(object, memFunc) )
        {}

        ~MethodInfo()
        { delete _cb; }

        Pt::Any call(const Any* args, size_t argCount)
        {
            if(argCount < 2)
                throw std::invalid_argument("Not enough arguments");

            _cb->call( any_cast<A1>( args[0] ),
                       any_cast<A2>( args[1] ) );

            return Any();
        }

        virtual void call(const SerializationData& args) const
        {
            typename Pt::TypeInfo<A1>::Value a1;
            args.getNode(0) >> a1;

            typename Pt::TypeInfo<A2>::Value a2;
            args.getNode(1) >> a2;

            _cb->invoke(a1, a2);
        }

    private:
        Callable<void, A1, A2>* _cb;
};


template < typename R,
           class C,
           typename A1>
class MethodInfo<R, C, A1, Pt::Void, Pt::Void, Pt::Void, Pt::Void> : public MethodInfoBase<R, C, A1>
{
    public:
        typedef C ClassT;
        typedef R (C::*MemFuncT)(A1);
        typedef R (C::*ConstMemFuncT)(A1) const;

    public:
        MethodInfo(const std::string& name, C& object, MemFuncT memFunc)
        : MethodInfoBase<R, C, A1>(name)
        , _cb( new Method<R, C, A1>(object, memFunc) )
        {}

        MethodInfo(const std::string& name, C& object, ConstMemFuncT memFunc)
        : MethodInfoBase<R, C, A1>(name)
        , _cb( new ConstMethod<R, C, A1>(object, memFunc) )
        {}

        ~MethodInfo()
        { delete _cb; }


        Pt::Any call(const Any* args, size_t argCount)
        {
            if(argCount < 1)
                throw std::invalid_argument("Not enough arguments");

            return _cb->call( any_cast<A1>(*args) );
        }

        virtual void call(const SerializationData& args) const
        {
            typename Pt::TypeInfo<A1>::Value a1;
            args.getNode(0) >> a1;
            _cb->invoke(a1);
        }

    private:
        Callable<void, A1>* _cb;
};


template < class C,
           typename A1>
class MethodInfo<void, C, A1, Pt::Void, Pt::Void, Pt::Void, Pt::Void> : public MethodInfoBase<void, C, A1>
{
    public:
        typedef C ClassT;
        typedef void (C::*MemFuncT)(A1);
        typedef void (C::*ConstMemFuncT)(A1) const;

    public:
        MethodInfo(const std::string& name, C& object, MemFuncT memFunc)
        : MethodInfoBase<void, C, A1>(name)
        , _cb( new Method<void, C, A1>(object, memFunc) )
        {}

        MethodInfo(const std::string& name, C& object, ConstMemFuncT memFunc)
        : MethodInfoBase<void, C, A1>(name)
        , _cb( new ConstMethod<void, C, A1>(object, memFunc) )
        {}

        ~MethodInfo()
        { delete _cb; }

        Pt::Any call(const Any* args, size_t argCount)
        {
            if(argCount < 1)
                throw std::invalid_argument("Not enough arguments");

            _cb->call( any_cast<A1>(*args) );
            return Any();
        }

        virtual void call(const SerializationData& args) const
        {
            typename Pt::TypeInfo<A1>::Value a1;
            args.getNode(0) >> a1;
            _cb->invoke(a1);
        }

    private:
        Callable<void, A1>* _cb;
};


template < typename R,
           class C >
class MethodInfo<R, C, Pt::Void, Pt::Void, Pt::Void, Pt::Void, Pt::Void> : public MethodInfoBase<R, C>
{
    public:
        typedef C ClassT;
        typedef R (C::*MemFuncT)();
        typedef R (C::*ConstMemFuncT)() const;

    public:
        MethodInfo(const std::string& name, C& object, MemFuncT memFunc)
        : MethodInfoBase<R, C>(name)
        , _cb( new Method<R, C>(object, memFunc) )
        {}

        MethodInfo(const std::string& name, C& object, ConstMemFuncT memFunc)
        : MethodInfoBase<R, C>(name)
        , _cb( new ConstMethod<R, C>(object, memFunc) )
        {}

        ~MethodInfo()
        { delete _cb; }

        Pt::Any call(const Any* args, size_t argCount)
        {
            return _cb->call();
        }

        virtual void call(const SerializationData& args) const
        {
            _cb->invoke();
        }

    private:
        Callable<R>* _cb;
};


template < class C >
class MethodInfo<void, C, Pt::Void, Pt::Void, Pt::Void, Pt::Void, Pt::Void> : public MethodInfoBase<void, C>
{
    public:
        typedef C ClassT;
        typedef void (C::*MemFuncT)();
        typedef void (C::*ConstMemFuncT)() const;

    public:
        MethodInfo(const std::string& name, C& object, MemFuncT memFunc)
        : MethodInfoBase<void, C>(name)
        ,_cb( new Method<void, C>(object, memFunc) )
        {}

        MethodInfo(C& object, ConstMemFuncT memFunc)
        : _cb( new ConstMethod<void, C>(object, memFunc) )
        {}

        ~MethodInfo()
        { delete _cb; }

        Pt::Any call(const Any* args, size_t argCount)
        {
            _cb->invoke();
            return Any();
        }

        virtual void call(const SerializationData& args) const
        {
            _cb->invoke();
        }

    private:
        Callable<void>* _cb;
};

} // namespace Pt

#endif
