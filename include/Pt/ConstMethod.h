/***************************************************************************
 *   Copyright (C) 2005-2006 by Dr. Marc Boris Dürner                      *
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

#ifndef Pt_ConstMethod_h
#define Pt_ConstMethod_h

#include <Pt/Callable.h>
#include <Pt/Connectable.h>
#include <Pt/Slot.h>


namespace Pt {

template < typename R,
           class Object,
           typename A1 = Pt::Void,
           typename A2 = Pt::Void,
           typename A3 = Pt::Void,
           typename A4 = Pt::Void,
           typename A5 = Pt::Void >
class ConstMethod : public Callable<R, A1, A2, A3, A4, A5> {
    public:
        typedef Object ObjectT;
        typedef R (Object::*MethodT)(A1, A2, A3, A4, A5) const;

        ConstMethod(Object* object, MethodT ptr) throw()
        : _object(object), _method(ptr)
        { }

        ConstMethod(const ConstMethod& method) throw()
        : Callable<R, A1, A2, A3, A4, A5>()
        { this->operator=(method); }

        Object& object()
        { return *_object;}

        const Object& object() const
        { return *_object;}

        R operator()(A1 a1, A2 a2, A3 a3, A4 a4, A4 a5) const
        { return (_object->*_method)(a1, a2, a3, a4, a5); }

        ConstMethod<R, Object, A1, A2, A3, A4, A5>* clone() const
        { return new ConstMethod(*this); }

    private:
        Object* _object;
        MethodT _method;
};


template <typename R, class Object, typename A1, typename A2, typename A3, typename A4, typename A5>
ConstMethod<R, Object, A1, A2, A3, A4, A5> callable( Object* obj, R (Object::*ptr)(A1, A2, A3, A4, A5) const ) throw()
{ return ConstMethod<R, Object, A1, A2, A3, A4, A5>(obj, ptr); }


template < typename R,
           class Object,
           typename A1,
           typename A2,
           typename A3,
           typename A4 >
class ConstMethod<R, Object, A1, A2, A3, A4, Pt::Void> : public Callable<R, A1, A2, A3, A4> {
    public:
        typedef Object ObjectT;
        typedef R (Object::*MethodT)(A1, A2, A3, A4) const;

        ConstMethod(Object* object, MethodT ptr) throw()
        : _object(object), _method(ptr)
        { }

        ConstMethod(const ConstMethod& method) throw()
        : Callable<R, A1, A2, A3>()
        { this->operator=(method); }

        Object& object()
        { return *_object;}

        const Object& object() const
        { return *_object;}

        R operator()(A1 a1, A2 a2, A3 a3, A4 a4) const
        { return (_object->*_method)(a1, a2, a3, a4); }

        ConstMethod<R, Object, A1, A2, A3, A4>* clone() const
        { return new ConstMethod(*this); }

    private:
        Object* _object;
        MethodT _method;
};


template <typename R, class Object, typename A1, typename A2, typename A3, typename A4>
ConstMethod<R, Object, A1, A2, A3, A4> callable( Object* obj, R (Object::*ptr)(A1, A2, A3, A4) const ) throw()
{ return ConstMethod<R, Object, A1, A2, A3, A4>(obj, ptr); }


template < typename R,
           class Object,
           typename A1,
           typename A2,
           typename A3 >
class ConstMethod<R, Object, A1, A2, A3, Pt::Void, Pt::Void> : public Callable<R, A1, A2, A3> {
    public:
        typedef Object ObjectT;
        typedef R (Object::*MethodT)(A1, A2, A3) const;

        ConstMethod(Object* object, MethodT ptr) throw()
        : _object(object), _method(ptr)
        { }

        ConstMethod(const ConstMethod& method) throw()
        : Callable<R, A1, A2, A3>()
        { this->operator=(method); }

        Object& object()
        { return *_object;}

        const Object& object() const
        { return *_object;}

        R operator()(A1 a1, A2 a2, A3 a3) const
        { return (_object->*_method)(a1, a2, a3); }

        ConstMethod<R, Object, A1, A2, A3>* clone() const
        { return new ConstMethod(*this); }

    private:
        Object* _object;
        MethodT _method;
};


template <typename R, class Object, typename A1, typename A2, typename A3>
ConstMethod<R, Object, A1, A2, A3> callable( Object* obj, R (Object::*ptr)(A1, A2, A3) const ) throw()
{ return ConstMethod<R, Object, A1, A2, A3>(obj, ptr); }


template < typename R,
           class Object,
           typename A1,
           typename A2 >
class ConstMethod<R, Object, A1, A2, Pt::Void, Pt::Void, Pt::Void> : public Callable<R, A1, A2> {
    public:
        typedef Object ObjectT;
        typedef R (Object::*MethodT)(A1, A2) const;

        ConstMethod(Object* object, MethodT ptr) throw()
        : _object(object), _method(ptr)
        { }

        ConstMethod(const ConstMethod& method) throw()
        : Callable<R, A1, A2, Pt::Void>()
        { this->operator=(method); }

        Object& object()
        { return *_object;}

        const Object& object() const
        { return *_object;}

        R operator()(A1 a1, A2 a2) const
        { return (_object->*_method)(a1, a2); }

        ConstMethod<R, Object, A1, A2>* clone() const
        { return new ConstMethod(*this); }

    private:
        Object* _object;
        MethodT _method;
};


template <typename R, class Object, typename A1, typename A2>
ConstMethod<R, Object, A1, A2> callable( Object* obj, R (Object::*ptr)(A1, A2) const ) throw()
{ return ConstMethod<R, Object, A1, A2>(obj, ptr); }


template < typename R,
           class Object,
           typename A1 >
class ConstMethod<R, Object, A1, Pt::Void, Pt::Void, Pt::Void, Pt::Void> : public Callable<R, A1> {
    public:
        typedef Object ObjectT;
        typedef R (Object::*MethodT)(A1) const;

        ConstMethod(Object* object, MethodT ptr) throw()
        : _object(object), _method(ptr)
        { }

        Object& object()
        { return *_object;}

        const Object& object() const
        { return *_object;}

        ConstMethod(const ConstMethod& method) throw()
        : Callable<R, A1, Pt::Void, Pt::Void>()
        { this->operator=(method); }

        R operator()(A1 a1) const
        { return (_object->*_method)(a1); }

        ConstMethod<R, Object, A1>* clone() const
        { return new ConstMethod(*this); }

    private:
        Object* _object;
        MethodT _method;
};


template <typename R, class Object, typename A1>
ConstMethod<R,Object, A1> callable( Object* obj, R (Object::*ptr)(A1) const ) throw()
{ return ConstMethod<R, Object, A1>(obj, ptr); }


template < typename R,
           class C >
class ConstMethod<R, C, Pt::Void, Pt::Void, Pt::Void, Pt::Void, Pt::Void> : public Callable<R> {
    public:
        typedef C ClassT;
        typedef R (C::*MemFuncT)() const;

        ConstMethod(ClassT* object, MemFuncT memFunc) throw()
        : _object(object), _memFunc(memFunc)
        { }

        ConstMethod(const ConstMethod& method) throw()
        : Callable<R, Pt::Void, Pt::Void, Pt::Void>()
        { this->operator=(method); }

        ClassT& object()
        { return *_object; }

        const ClassT& object() const
        { return _object;}

        const MemFuncT& method() const
        { return _memFunc;}

        inline R operator()() const
        { return this->call(); }

        inline R call() const
        { return (_object->*_memFunc)(); }

        ConstMethod* clone() const
        { return new ConstMethod(*this); }

    private:
        ClassT* _object;
        MemFuncT _memFunc;
};


template <typename R, class Object>
ConstMethod<R,Object> callable( Object* obj, R (Object::*ptr)() const ) throw()
{ return ConstMethod<R, Object>(obj, ptr); }




template < typename R,
           class C,
           typename A1 = Pt::Void,
           typename A2 = Pt::Void,
           typename A3 = Pt::Void,
           typename A4 = Pt::Void,
           typename A5 = Pt::Void
         >
class ConstMethodSlot : public BasicSlot<R, A1, A2, A3, A4, A5> {
    public:
        ConstMethodSlot(const ConstMethod<R, C, A1, A2, A3, A4, A5>& method)
        : _method( method )
        {}

        Slot* clone() const
        { return new ConstMethodSlot(*this); }

        virtual const void* callable() const
        { return &_method; }

        virtual void opened(const Connection& c)
        {
            Connectable& connectable = _method.object();
            connectable.opened(c);
        }

        virtual void closed(const Connection& c) 
        {
            Connectable& connectable = _method.object();
            connectable.closed(c);
        }

    private:
        ConstMethod<R, C, A1, A2, A3, A4, A5> _method;
};


template <typename R, class BaseT, class ClassT>
ConstMethodSlot<R, ClassT> slot( ClassT* obj, R (BaseT::*memFunc)() const ) throw()
{ return ConstMethodSlot<R, ClassT>( callable(obj, memFunc) ); }


template <typename R, class BaseT, class ClassT, typename A1>
ConstMethodSlot<R, ClassT, A1> slot( ClassT* obj, R (BaseT::*method)(A1) const ) throw()
{ return ConstMethodSlot<R, ClassT, A1>( callable(obj, method) ); }


template <typename R, class BaseT, class ClassT, typename A1, typename A2>
ConstMethodSlot<R, ClassT, A1, A2> slot( ClassT* obj, R (BaseT::*method)(A1, A2) const ) throw()
{ return ConstMethodSlot<R, ClassT, A1, A2>( callable(obj, method) ); }


template <typename R, class BaseT, class ClassT, typename A1, typename A2, typename A3>
ConstMethodSlot<R, ClassT, A1, A2, A3> slot( ClassT* obj, R (BaseT::*method)(A1, A2, A3) const ) throw()
{ return ConstMethodSlot<R, ClassT, A1, A2, A3>( callable(obj, method) ); }


template <typename R, class BaseT, class ClassT, typename A1, typename A2, typename A3, typename A4>
ConstMethodSlot<R, ClassT, A1, A2, A3, A4> slot( ClassT* obj, R (BaseT::*method)(A1, A2, A3, A4) const ) throw()
{ return ConstMethodSlot<R, ClassT, A1, A2, A3, A4>( callable(obj, method) ); }


template <typename R, class BaseT, class ClassT, typename A1, typename A2, typename A3, typename A4, typename A5>
ConstMethodSlot<R, ClassT, A1, A2, A3, A4, A5> slot( ClassT* obj, R (BaseT::*method)(A1, A2, A3, A4, A5) const ) throw()
{ return ConstMethodSlot<R, ClassT, A1, A2, A3, A4, A5>( callable(obj, method) ); }

} // !namespace Pt


#endif
