/*
 * Copyright (C) 2005 by Marc Boris Duerner
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

#ifndef Pt_Reflex_MethodInfo_h
#define Pt_Reflex_MethodInfo_h

#include <Pt/Reflex/Api.h>
#include <Pt/Reflex/MethodInfoBase.h>
#include <Pt/SourceInfo.h>
#include <Pt/ConstMethod.h>
#include <Pt/Method.h>
#include <stdexcept>


namespace Pt {

namespace Reflex {

template < typename R,
           class C,
           typename A1 = Pt::Void,
           typename A2 = Pt::Void,
           typename A3 = Pt::Void,
           typename A4 = Pt::Void,
           typename A5 = Pt::Void,
           typename A6 = Pt::Void,
           typename A7 = Pt::Void,
           typename A8 = Pt::Void >
class MethodInfo : public MethodInfoBase<R, C, A1, A2, A3, A4, A5, A6, A7, A8>
{
    public:
        typedef C ClassT;
        typedef R (C::*MemFuncT)(A1, A2, A3, A4, A5, A6, A7, A8);
        typedef R (C::*ConstMemFuncT)(A1, A2, A3, A4, A5, A6, A7, A8) const;

    public:
        MethodInfo(const std::string& name, C& object, MemFuncT memFunc)
        : MethodInfoBase<R, C, A1, A2, A3, A4, A5, A6, A7, A8>(name)
        , _cb( new Method<R, C, A1, A2, A3, A5, A6, A7, A8>(object, memFunc) )
        {}

        MethodInfo(const std::string& name, C& object, ConstMemFuncT memFunc)
        : MethodInfoBase<R, C, A1, A2, A3, A4, A5, A6, A7, A8>(name)
        , _cb( new ConstMethod<R, C, A1, A2, A3, A4, A5, A6, A7, A8>(object, memFunc) )
        {}

        ~MethodInfo()
        { delete _cb; }

        Pt::Any call(const Any* args, size_t argCount)
        {
            if(argCount < 8)
                throw std::invalid_argument("Not enough arguments");

            return _cb->call( any_cast<A1>( args[0] ),
                              any_cast<A2>( args[1] ),
                              any_cast<A3>( args[2] ),
                              any_cast<A4>( args[3] ),
                              any_cast<A5>( args[4] ),
                              any_cast<A6>( args[5] ),
                              any_cast<A7>( args[6] ),
                              any_cast<A8>( args[7] ) );
        }

    private:
        Callable<R, A1, A2, A3, A4, A5, A6, A7, A8>* _cb;
};


template < class C,
           typename A1,
           typename A2,
           typename A3,
           typename A4,
           typename A5,
           typename A6,
           typename A7,
           typename A8>
class MethodInfo<void, C, A1, A2, A3, A4, A5, A6, A7, A8> : public MethodInfoBase<void, C, A1, A2, A3, A4, A5, A6, A7, A8>
{
    public:
        typedef C ClassT;
        typedef void (C::*MemFuncT)(A1, A2, A3, A4, A5, A6, A7, A8);
        typedef void (C::*ConstMemFuncT)(A1, A2, A3, A4, A5, A6, A7, A8) const;

    public:
        MethodInfo(const std::string& name, C& object, MemFuncT memFunc)
        : MethodInfoBase<void, C, A1, A2, A3, A4, A5, A6, A7, A8>(name)
        , _cb( new Method<void, C, A1, A2, A3, A4, A5, A6, A7, A8>(object, memFunc) )
        {}

        MethodInfo(const std::string& name, C& object, ConstMemFuncT memFunc)
        : MethodInfoBase<void, C, A1, A2, A3, A4, A5, A6, A7, A8>(name)
        , _cb( new ConstMethod<void, C, A1, A2, A3, A4, A5, A6, A7, A8>(object, memFunc) )
        {}

        ~MethodInfo()
        { delete _cb; }

        Pt::Any call(const Any* args, size_t argCount)
        {
            if(argCount < 8)
                throw std::invalid_argument("Not enough arguments");

            _cb->call( any_cast<A1>( args[0] ),
                       any_cast<A2>( args[1] ),
                       any_cast<A3>( args[2] ),
                       any_cast<A4>( args[3] ),
                       any_cast<A5>( args[4] ),
                       any_cast<A6>( args[5] ),
                       any_cast<A7>( args[6] ),
                       any_cast<A8>( args[7] ) );

            return Any();
        }

    private:
        Callable<void, A1, A2, A3, A4, A5, A6, A7, A8>* _cb;
};



template < typename R,
           class C,
           typename A1,
           typename A2,
           typename A3,
           typename A4,
           typename A5>
class MethodInfo<R, C, A1, A2, A3, A4, A5, Pt::Void, Pt::Void, Pt::Void> : public MethodInfoBase<R, C, A1, A2, A3, A4, A5, Pt::Void, Pt::Void, Pt::Void>
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

    private:
        Callable<R, A1, A2, A3, A4, A5, Pt::Void, Pt::Void, Pt::Void>* _cb;
};


template < class C,
           typename A1,
           typename A2,
           typename A3,
           typename A4,
           typename A5>
class MethodInfo<void, C, A1, A2, A3, A4, A5, Pt::Void, Pt::Void, Pt::Void> : public MethodInfoBase<void, C, A1, A2, A3, A4, A5>
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

    private:
        Callable<void, A1, A2, A3, A4, A5, Pt::Void, Pt::Void, Pt::Void>* _cb;
};


template < typename R,
           class C,
           typename A1,
           typename A2,
           typename A3,
           typename A4>
class MethodInfo<R, C, A1, A2, A3, A4, Pt::Void, Pt::Void, Pt::Void, Pt::Void> : public MethodInfoBase<R, C, A1, A2, A3, A4>
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

    private:
        Callable<R, A1, A2, A3, A4, Pt::Void, Pt::Void, Pt::Void, Pt::Void>* _cb;
};


template < class C,
           typename A1,
           typename A2,
           typename A3,
           typename A4>
class MethodInfo<void, C, A1, A2, A3, A4, Pt::Void, Pt::Void, Pt::Void, Pt::Void> : public MethodInfoBase<void, C, A1, A2, A3, A4>
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

    private:
        Callable<void, A1, A2, A3, A4, Pt::Void, Pt::Void, Pt::Void, Pt::Void>* _cb;
};


template < typename R,
           class C,
           typename A1,
           typename A2,
           typename A3>
class MethodInfo<R, C, A1, A2, A3, Pt::Void, Pt::Void, Pt::Void, Pt::Void, Pt::Void> : public MethodInfoBase<R, C, A1, A2, A3>
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

    private:
        Callable<R, A1, A2, A3, Pt::Void, Pt::Void, Pt::Void, Pt::Void, Pt::Void>* _cb;
};


template < class C,
           typename A1,
           typename A2,
           typename A3>
class MethodInfo<void, C, A1, A2, A3, Pt::Void, Pt::Void, Pt::Void, Pt::Void, Pt::Void> : public MethodInfoBase<void, C, A1, A2, A3>
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

    private:
        Callable<void, A1, A2, A3, Pt::Void, Pt::Void, Pt::Void, Pt::Void, Pt::Void>* _cb;
};


template < typename R,
           class C,
           typename A1,
           typename A2>
class MethodInfo<R, C, A1, A2, Pt::Void, Pt::Void, Pt::Void, Pt::Void, Pt::Void, Pt::Void> : public MethodInfoBase<R, C, A1, A2>
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

    private:
        Callable<void, A1, A2, Pt::Void, Pt::Void, Pt::Void, Pt::Void, Pt::Void, Pt::Void>* _cb;
};


template < class C,
           typename A1,
           typename A2>
class MethodInfo<void, C, A1, A2, Pt::Void, Pt::Void, Pt::Void, Pt::Void, Pt::Void, Pt::Void> : public MethodInfoBase<void, C, A1, A2>
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

    private:
        Callable<void, A1, A2, Pt::Void, Pt::Void, Pt::Void, Pt::Void, Pt::Void, Pt::Void>* _cb;
};


template < typename R,
           class C,
           typename A1>
class MethodInfo<R, C, A1, Pt::Void, Pt::Void, Pt::Void, Pt::Void, Pt::Void, Pt::Void, Pt::Void> : public MethodInfoBase<R, C, A1>
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

    private:
        Callable<void, A1, Pt::Void, Pt::Void, Pt::Void, Pt::Void, Pt::Void, Pt::Void, Pt::Void>* _cb;
};


template < class C,
           typename A1>
class MethodInfo<void, C, A1, Pt::Void, Pt::Void, Pt::Void, Pt::Void, Pt::Void, Pt::Void, Pt::Void> : public MethodInfoBase<void, C, A1>
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
                throw std::invalid_argument("Not enough arguments YYY");

            _cb->call( any_cast<A1>(*args) );
            return Any();
        }

    private:
        Callable<void, A1, Pt::Void, Pt::Void, Pt::Void, Pt::Void, Pt::Void, Pt::Void, Pt::Void>* _cb;
};


template < typename R,
           class C >
class MethodInfo<R, C, Pt::Void, Pt::Void, Pt::Void, Pt::Void, Pt::Void, Pt::Void, Pt::Void, Pt::Void> : public MethodInfoBase<R, C>
                                                                         , public Connectable
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

        virtual Slot* createSlot()
        {
            return Pt::slot( *this, &MethodInfo::do_call ).clone();
        }

        void do_call()
        { _cb->invoke(); }

    private:
        Callable<R, Pt::Void, Pt::Void, Pt::Void, Pt::Void, Pt::Void, Pt::Void, Pt::Void, Pt::Void>* _cb;
};


template < class C >
class MethodInfo<void, C, Pt::Void, Pt::Void, Pt::Void, Pt::Void, Pt::Void, Pt::Void, Pt::Void, Pt::Void> : public MethodInfoBase<void, C>
                                                                            , public Connectable
{
    public:
        typedef C ClassT;
        typedef void (C::*MemFuncT)();
        typedef void (C::*ConstMemFuncT)() const;

    public:
        MethodInfo(const std::string& name, C& object, MemFuncT memFunc)
        : MethodInfoBase<void, C>(name)
        ,_cb( new Method<void, C>(object, memFunc) )
        { }

        MethodInfo(C& object, ConstMemFuncT memFunc)
        : _cb( new ConstMethod<void, C>(object, memFunc) )
        { }

        ~MethodInfo()
        {
            delete _cb;
        }

        Pt::Any call(const Any* args, size_t argCount)
        {
            _cb->invoke();
            return Any();
        }

        virtual Slot* createSlot()
        {
            return Pt::slot( *this, &MethodInfo::do_call ).clone();
        }

        void do_call()
        { _cb->invoke(); }

    private:
        Callable<void, Pt::Void, Pt::Void, Pt::Void, Pt::Void, Pt::Void, Pt::Void, Pt::Void, Pt::Void>* _cb;
};

} // namespace Reflex

} // namespace Pt

#endif
