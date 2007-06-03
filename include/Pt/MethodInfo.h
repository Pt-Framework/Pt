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
#include <Pt/MethodInfoBase.h>


namespace Pt {

template < typename R,
           class C,
           typename A1 = Pt::Void,
           typename A2 = Pt::Void,
           typename A3 = Pt::Void,
           typename A4 = Pt::Void,
           typename A5 = Pt::Void>
class MethodInfo : public MethodInfoBase<R, C, A1, A2, A3, A4, A5>
                 , private Method<R, C, A1, A2, A3, A4, A5>
{
    public:
        typedef C ClassT;
        typedef R (C::*MemFuncT)(A1, A2, A3, A4, A5);

    public:
        MethodInfo(C* object, MemFuncT memFunc)
        : Method<R, C, A1, A2, A3, A4, A5>(object, memFunc)
        {}

        Pt::Any call(const Args& a)
        {
            return Method<R, C, A1, A2, A3, A4, A5>::call( any_cast<A1>( a.get(0) ),
                                                           any_cast<A2>( a.get(1) ),
                                                           any_cast<A3>( a.get(2) ),
                                                           any_cast<A4>( a.get(3) ),
                                                           any_cast<A5>( a.get(4) ));
        }
};


template < class C,
           typename A1,
           typename A2,
           typename A3,
           typename A4,
           typename A5>
class MethodInfo<void, C, A1, A2, A3, A4, A5> : public MethodInfoBase<void, C, A1, A2, A3, A4, A5>
                                              , private Method<void, C, A1, A2, A3, A4, A5>
{
    public:
        typedef C ClassT;
        typedef void (C::*MemFuncT)(A1, A2, A3, A4, A5);

    public:
        MethodInfo(C* object, MemFuncT memFunc)
        : Method<void, C, A1, A2, A3, A4, A5>(object, memFunc)
        {}

        Pt::Any call(const Args& a)
        {
            Method<void, C, A1, A2, A3, A4, A5>::call( any_cast<A1>( a.get(0) ),
                                                       any_cast<A2>( a.get(1) ),
                                                       any_cast<A3>( a.get(2) ),
                                                       any_cast<A4>( a.get(3) ),
                                                       any_cast<A5>( a.get(4) ) );

            return Any();
        }
};


template < typename R,
           class C,
           typename A1,
           typename A2,
           typename A3,
           typename A4>
class MethodInfo<R, C, A1, A2, A3, A4, Pt::Void> : public MethodInfoBase<R, C, A1, A2, A3, A4>
                                                 , private Method<R, C, A1, A2, A3, A4>
{
    public:
        typedef C ClassT;
        typedef R (C::*MemFuncT)(A1, A2, A3, A4);

    public:
        MethodInfo(C* object, MemFuncT memFunc)
        : Method<R, C, A1, A2, A3, A4>(object, memFunc)
        {}

        Pt::Any call(const Args& a)
        {
            return Method<R, C, A1, A2, A3, A4>::call( any_cast<A1>( a.get(0) ),
                                                any_cast<A2>( a.get(1) ),
                                                any_cast<A3>( a.get(2) ),
                                                any_cast<A4>( a.get(3) ) );
        }
};


template < class C,
           typename A1,
           typename A2,
           typename A3,
           typename A4>
class MethodInfo<void, C, A1, A2, A3, A4, Pt::Void> : public MethodInfoBase<void, C, A1, A2, A3, A4>
                                                    , private Method<void, C, A1, A2, A3, A4>
{
    public:
        typedef C ClassT;
        typedef void (C::*MemFuncT)(A1, A2, A3, A4);

    public:
        MethodInfo(C* object, MemFuncT memFunc)
        : Method<void, C, A1, A2, A3, A4>(object, memFunc)
        {}

        Pt::Any call(const Args& a)
        {
            Method<void, C, A1, A2, A3, A4>::call( any_cast<A1>( a.get(0) ),
                                                   any_cast<A2>( a.get(1) ),
                                                   any_cast<A3>( a.get(2) ),
                                                   any_cast<A4>( a.get(3) ) );
            return Any();
        }
};


template < typename R,
           class C,
           typename A1,
           typename A2,
           typename A3>
class MethodInfo<R, C, A1, A2, A3, Pt::Void, Pt::Void> : public MethodInfoBase<R, C, A1, A2, A3>
                                                       , private Method<R, C, A1, A2, A3>
{
    public:
        typedef C ClassT;
        typedef R (C::*MemFuncT)(A1, A2, A3);

    public:
        MethodInfo(C* object, MemFuncT memFunc)
        : Method<R, C, A1, A2, A3>(object, memFunc)
        {}

        Pt::Any call(const Args& a)
        {
            return Method<R, C, A1, A2, A3>::call( any_cast<A1>( a.get(0) ),
                                                   any_cast<A2>( a.get(1) ),
                                                   any_cast<A3>( a.get(2) ));
        }
};


template < class C,
           typename A1,
           typename A2,
           typename A3>
class MethodInfo<void, C, A1, A2, A3, Pt::Void, Pt::Void> : public MethodInfoBase<void, C, A1, A2, A3>
                                                          , private Method<void, C, A1, A2, A3>
{
    public:
        typedef C ClassT;
        typedef void (C::*MemFuncT)(A1, A2, A3);

    public:
        MethodInfo(C* object, MemFuncT memFunc)
        : Method<void, C, A1, A2, A3>(object, memFunc)
        {}

        Pt::Any call(const Args& a)
        {
            Method<void, C, A1, A2, A3>::call( any_cast<A1>( a.get(0) ),
                                               any_cast<A2>( a.get(1) ),
                                               any_cast<A3>( a.get(2) ));
            return Any();
        }
};


template < typename R,
           class C,
           typename A1,
           typename A2>
class MethodInfo<R, C, A1, A2, Pt::Void, Pt::Void, Pt::Void> : public MethodInfoBase<R, C, A1, A2>
                                                             , private Method<R, C, A1, A2>
{
    public:
        typedef C ClassT;
        typedef R (C::*MemFuncT)(A1, A2);

    public:
        MethodInfo(C* object, MemFuncT memFunc)
        : Method<R, C, A1, A2>(object, memFunc)
        {}

        Pt::Any call(const Args& a)
        {
            return Method<R, C, A1, A2>::call( any_cast<A1>( a.get(0) ),
                                               any_cast<A2>( a.get(1) ) );
        }
};


template < class C,
           typename A1,
           typename A2>
class MethodInfo<void, C, A1, A2, Pt::Void, Pt::Void, Pt::Void> : public MethodInfoBase<void, C, A1, A2>
                                                                , private Method<void, C, A1, A2>
{
    public:
        typedef C ClassT;
        typedef void (C::*MemFuncT)(A1, A2);

    public:
        MethodInfo(C* object, MemFuncT memFunc)
        : Method<void, C, A1, A2>(object, memFunc)
        {}

        Pt::Any call(const Args& a)
        {
            Method<void, C, A1, A2>::call( any_cast<A1>( a.get(0) ),
                                           any_cast<A2>( a.get(1) ) );

            return Any();
        }
};


template < typename R,
           class C,
           typename A1>
class MethodInfo<R, C, A1, Pt::Void, Pt::Void, Pt::Void, Pt::Void> : public MethodInfoBase<R, C, A1>
                                                                   , private Method<R, C, A1>
{
    public:
        typedef C ClassT;
        typedef R (C::*MemFuncT)(A1);

    public:
        MethodInfo(C* object, MemFuncT memFunc)
        : Method<R, C, A1>(object, memFunc)
        {}

        Pt::Any call(const Args& a)
        {
            return Method<R, C, A1>::call( any_cast<A1>( a.get(0) ) );
        }
};


template < class C,
           typename A1>
class MethodInfo<void, C, A1, Pt::Void, Pt::Void, Pt::Void, Pt::Void> : public MethodInfoBase<void, C, A1>
                                                                      , private Method<void , C, A1>
{
    public:
        typedef C ClassT;
        typedef void (C::*MemFuncT)(A1);

    public:
        MethodInfo(C* object, MemFuncT memFunc)
        : Method<void, C, A1>(object, memFunc)
        {}

        Pt::Any call(const Args& a)
        {
            Method<void, C, A1>::call( any_cast<A1>( a.get(0) ) );
            return Any();
        }
};


template < typename R,
           class C >
class MethodInfo<R, C, Pt::Void, Pt::Void, Pt::Void, Pt::Void, Pt::Void> : public MethodInfoBase<R, C>
                                                                         , private Method<R, C>
{
    public:
        typedef C ClassT;
        typedef R (C::*MemFuncT)();

    public:
        MethodInfo(C* object, MemFuncT memFunc)
        :  Method<R, C>(object, memFunc)
        {}

        Pt::Any call(const Args& a)
        {
            return Method<R, C>::call();
        }
};


template < class C >
class MethodInfo<void, C, Pt::Void, Pt::Void, Pt::Void, Pt::Void, Pt::Void> : public MethodInfoBase<void, C>
                                                                            , private Method<void, C>
{
    public:
        typedef C ClassT;
        typedef void (C::*MemFuncT)();

    public:
        MethodInfo(C* object, MemFuncT memFunc)
        :  Method<void, C>(object, memFunc)
        {}

        Pt::Any call(const Args& a)
        {
            Method<void, C>::call();
            return Any();
        }
};

} // namespace Pt

#endif
