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

#ifndef Pt_Reflectable_h
#define Pt_Reflectable_h

#include <Pt/Args.h>
#include <Pt/ICallable.h>
#include <Pt/PropertyProxy.h>
#include <Pt/Method.h>
#include <map>


namespace Pt {


template < typename R,
           class C,
           typename A1 = Pt::Void,
           typename A2 = Pt::Void,
           typename A3 = Pt::Void,
           typename A4 = Pt::Void,
           typename A5 = Pt::Void>
class MethodProxy : public ICallable, private Method<R, C, A1, A2, A3, A4, A5>
{
    public:
        typedef C ClassT;
        typedef R (C::*MemFuncT)(A1, A2, A3, A4, A5);

    public:
        MethodProxy(C* object, MemFuncT memFunc)
        : Method<R, C, A1, A2, A3, A4, A5>(object, memFunc)
        {}

        size_t argSize() const
        { return 5; }

        const char* argName(size_t index) const
        {
            switch(index)
            {
                case 0: return TypeTraits<A1>::typeName();
                case 1: return TypeTraits<A2>::typeName();
                case 2: return TypeTraits<A3>::typeName();
                case 3: return TypeTraits<A4>::typeName();
                case 4: return TypeTraits<A5>::typeName();
            }

            throw std::invalid_argument("No such argument" + PT_SOURCEINFO);
        }

        const std::type_info& argType(size_t index) const
        {
            switch(index)
            {
                case 0: return typeid(A1);
                case 1: return typeid(A2);
                case 2: return typeid(A3);
                case 3: return typeid(A4);
                case 4: return typeid(A5);
            }

            throw std::invalid_argument("No such argument" + PT_SOURCEINFO);
        }

        void call(const Args& a)
        {
            Method<R, C, A1, A2, A3, A4, A5>::call( any_cast<A1>( a.get(0) ),
                                                    any_cast<A2>( a.get(1) ),
                                                    any_cast<A3>( a.get(2) ),
                                                    any_cast<A4>( a.get(3) ),
                                                    any_cast<A5>( a.get(4) ));
        }
};


template < typename R,
           class C,
           typename A1,
           typename A2,
           typename A3,
           typename A4>
class MethodProxy<R, C, A1, A2, A3, A4, Pt::Void> : public ICallable
                                                            , private Method<R, C, A1, A2, A3, A4>
{
    public:
        typedef C ClassT;
        typedef R (C::*MemFuncT)(A1, A2, A3, A4);

    public:
        MethodProxy(C* object, MemFuncT memFunc)
        : Method<R, C, A1, A2, A3, A4>(object, memFunc)
        {}

        size_t argSize() const
        { return 4; }

        const char* argName(size_t index) const
        {
            switch(index)
            {
                case 0: return TypeTraits<A1>::typeName();
                case 1: return TypeTraits<A2>::typeName();
                case 2: return TypeTraits<A3>::typeName();
                case 3: return TypeTraits<A4>::typeName();
            }

            throw std::invalid_argument("No such argument" + PT_SOURCEINFO);
        }

        const std::type_info& argType(size_t index) const
        {
            switch(index)
            {
                case 0: return typeid(A1);
                case 1: return typeid(A2);
                case 2: return typeid(A3);
                case 3: return typeid(A4);
            }

            throw std::invalid_argument("No such argument" + PT_SOURCEINFO);
        }

        void call(const Args& a)
        {
            Method<R, C, A1, A2, A3, A4>::call( any_cast<A1>( a.get(0) ),
                                                any_cast<A2>( a.get(1) ),
                                                any_cast<A3>( a.get(2) ),
                                                any_cast<A4>( a.get(3) ) );
        }
};


template < typename R,
           class C,
           typename A1,
           typename A2,
           typename A3>
class MethodProxy<R, C, A1, A2, A3, Pt::Void, Pt::Void> : public ICallable
                                                                  , private Method<R, C, A1, A2, A3>
{
    public:
        typedef C ClassT;
        typedef R (C::*MemFuncT)(A1, A2, A3);

    public:
        MethodProxy(C* object, MemFuncT memFunc)
        : Method<R, C, A1, A2, A3>(object, memFunc)
        {}

        size_t argSize() const
        { return 3; }

        const char* argName(size_t index) const
        {
            switch(index)
            {
                case 0: return TypeTraits<A1>::typeName();
                case 1: return TypeTraits<A2>::typeName();
                case 2: return TypeTraits<A3>::typeName();
            }

            throw std::invalid_argument("No such argument" + PT_SOURCEINFO);
        }

        const std::type_info& argType(size_t index) const
        {
            switch(index)
            {
                case 0: return typeid(A1);
                case 1: return typeid(A2);
                case 2: return typeid(A3);
            }

            throw std::invalid_argument("No such argument" + PT_SOURCEINFO);
        }

        void call(const Args& a)
        {
            Method<R, C, A1, A2, A3>::call( any_cast<A1>( a.get(0) ),
                                            any_cast<A2>( a.get(1) ),
                                            any_cast<A3>( a.get(2) ));
        }
};


template < typename R,
           class C,
           typename A1,
           typename A2>
class MethodProxy<R, C, A1, A2, Pt::Void, Pt::Void, Pt::Void> : public ICallable
                                                                        , private Method<R, C, A1, A2>
{
    public:
        typedef C ClassT;
        typedef R (C::*MemFuncT)(A1, A2);

    public:
        MethodProxy(C* object, MemFuncT memFunc)
        : Method<R, C, A1, A2>(object, memFunc)
        {}

        size_t argSize() const
        { return 2; }

        const char* argName(size_t index) const
        {
            switch(index)
            {
                case 0: return TypeTraits<A1>::typeName();
                case 1: return TypeTraits<A2>::typeName();
            }

            throw std::invalid_argument("No such argument" + PT_SOURCEINFO);
        }

        const std::type_info& argType(size_t index) const
        {
            switch(index)
            {
                case 0: return typeid(A1);
                case 1: return typeid(A2);
            }

            throw std::invalid_argument("No such argument" + PT_SOURCEINFO);
        }

        void call(const Args& a)
        {
            Method<R, C, A1, A2>::call( any_cast<A1>( a.get(0) ),
                                        any_cast<A2>( a.get(1) ) );
        }
};


template < typename R,
           class C,
           typename A1>
class MethodProxy<R, C, A1, Pt::Void, Pt::Void, Pt::Void, Pt::Void> : public ICallable
                                                                              , private Method<R, C, A1>
{
    public:
        typedef C ClassT;
        typedef R (C::*MemFuncT)(A1);

    public:
        MethodProxy(C* object, MemFuncT memFunc)
        : Method<R, C, A1>(object, memFunc)
        {}

        size_t argSize() const
        { return 1; }

        const char* argName(size_t index) const
        {
            switch(index)
            {
                case 0: return TypeTraits<A1>::typeName();
            }

            throw std::invalid_argument("No such argument" + PT_SOURCEINFO);
        }

        const std::type_info& argType(size_t index) const
        {
            switch(index)
            {
                case 0: return typeid(A1);
            }

            throw std::invalid_argument("No such argument" + PT_SOURCEINFO);
        }

        void call(const Args& a)
        {
            Method<R, C, A1>::call( any_cast<A1>( a.get(0) ) );
        }
};


template < typename R,
           class C>
class MethodProxy<R, C, Pt::Void, Pt::Void, Pt::Void, Pt::Void, Pt::Void> : public ICallable
                                                                                    , private Method<R, C>
{
    public:
        typedef C ClassT;
        typedef R (C::*MemFuncT)();

    public:
        MethodProxy(C* object, MemFuncT memFunc)
        :  Method<R, C>(object, memFunc)
        {}

        size_t argSize() const
        { return 0; }

        const char* argName(size_t index) const
        { throw std::invalid_argument("No such argument" + PT_SOURCEINFO); }

        const std::type_info& argType(size_t index) const
        { throw std::invalid_argument("No such argument" + PT_SOURCEINFO); }

        void call(const Args& a)
        {
            Method<R, C>::call();
        }
};




typedef std::multimap<std::string, AbstractProperty*> PropertyMap;
typedef std::multimap<std::string, ICallable*> MethodMap;

/** @brief Make objects reflectable
    @ingroup Reflection

    This module is the base module for all other modules and has no dependency
    to any system specific libraries except the standard c++ library. It
    provides some basic types, support for byte-order handling, a CTTI system,
    an unicode string and character class, reflection and a signals/delegates
    based callback mechanism.
*/
class PT_API Reflectable {
    public:
        Reflectable(const std::string& name);
//        Reflectable(const std::string& name = "Reflectable");

        virtual ~Reflectable();

        Pt::Any property(const std::string& name);

        void setProperty(const std::string& name, const Pt::Any& value);

        const PropertyMap& properties() const
        { return _properties; }

        template <typename R, typename Parent, typename Object>
        void registerWriteProperty(const std::string& name, Parent* parent, R (Object::*setter)() )
        {
            _properties.insert( std::make_pair(name, new WritePropertyProxy<R>(parent, setter)) );
        }

        template <typename R, typename Parent, typename Object>
        void registerProperty(const std::string& name, Parent* parent, R (Object::*getter)() const)
        {
            _properties.insert( std::make_pair(name, new ReadPropertyProxy<R>(parent, getter)) );
        }

        template <typename R, typename Parent, typename Object>
        void registerProperty(const std::string& name, Parent* parent, R (Object::*getter)())
        {
            _properties.insert( std::make_pair(name, new ReadPropertyProxy<R>(parent, getter)) );
        }

        template <typename R1, typename R2, typename A, typename Parent, typename Object>
        void registerProperty(const std::string& name, Parent* parent, R1 (Object::*getter)() const, R2 (Object::*setter)(A type))
        {
            _properties.insert( std::make_pair(name, new PropertyProxy<R1, A>(parent, getter, setter)) );
        }

        template <typename R1, typename R2, typename A, typename Parent, typename Object>
        void registerProperty(const std::string& name, Parent* parent, R1 (Object::*getter)(), R2 (Object::*setter)(A type))
        {
            _properties.insert( std::make_pair(name, new PropertyProxy<R1, A>(parent, getter, setter)) );
        }

        template <typename ParentT>
        void registerMethod(const std::string& name, ParentT& parent, void (ParentT::*memFunc)() )
        {
            ICallable* cb = new MethodProxy<void, ParentT>(&parent, memFunc);
            _methods.insert( std::make_pair(name, cb) );
        }

        template <class ParentT, typename A1>
        void registerMethod(const std::string& name, ParentT& parent, void (ParentT::*memFunc)(A1) )
        {
            ICallable* cb =  new MethodProxy<void, ParentT, A1>(&parent, memFunc);
            _methods.insert( std::make_pair(name, cb) );
        }

        template <class ParentT, typename A1, typename A2>
        void registerMethod(const std::string& name, ParentT& parent, void (ParentT::*memFunc)(A1, A2) )
        {
            ICallable* cb =  new MethodProxy<void, ParentT, A1, A2>(&parent, memFunc);
            _methods.insert( std::make_pair(name, cb) );
        }

        template <class ParentT, typename A1, typename A2, typename A3>
        void registerMethod(const std::string& name, ParentT& parent, void (ParentT::*memFunc)(A1, A2, A3) )
        {
            ICallable* cb =  new MethodProxy<void, ParentT, A1, A2, A3>(&parent, memFunc);
            _methods.insert( std::make_pair(name, cb) );
        }

        template <class ParentT, typename A1, typename A2, typename A3, typename A4>
        void registerMethod(const std::string& name, ParentT& parent, void (ParentT::*memFunc)(A1, A2, A3, A4) )
        {
            ICallable* cb =  new MethodProxy<void, ParentT, A1, A2, A3, A4>(&parent, memFunc);
            _methods.insert( std::make_pair(name, cb) );
        }

        template <class ParentT, typename A1, typename A2, typename A3, typename A4, typename A5>
        void registerMethod(const std::string& name, ParentT& parent, void (ParentT::*memFunc)(A1, A2, A3, A4, A5) )
        {
            ICallable* cb =  new MethodProxy<void, ParentT, A1, A2, A3, A4, A5>(&parent, memFunc);
            _methods.insert( std::make_pair(name, cb) );
        }

        const MethodMap& methods() const
        { return _methods; }

        const ICallable& method(const std::string& name) const;

        void call(const std::string& name, const Args& args);

        Reflectable* self()
        { return this; }

        const std::string& getIdentifierName()
        {
            return _identiferName;
        }

    private:
        MethodMap   _methods;
        PropertyMap _properties;
        std::string _identiferName;
};

} // namespace Pt

#endif
