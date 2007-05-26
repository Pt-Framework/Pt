/***************************************************************************
 *   Copyright (C) 2007 by Marc Boris Drner                                *
 *   Copyright (C) 2007 by Laurentiu-Gheorghe Crisan                       *
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
#ifndef Pt_PropertyProxy_h
#define Pt_PropertyProxy_h

#include <Pt/Api.h>
#include <Pt/Any.h>
#include <Pt/Exception.h>
#include <Pt/Method.h>
#include <Pt/ConstMethod.h>
#include <Pt/Property.h>
#include <Pt/IProperty.h>


namespace Pt {

template <typename T>
class ReadPropertyInfo : virtual public PropertyInfo
{
    public:
        template <typename Object, typename ObjectBase>
        ReadPropertyInfo( Object* parent, T (ObjectBase::*getter)() const )
        : PropertyInfo()
        {
            _getter = new Pt::ConstMethod<T, Object>( parent, getter );
        }

        template <typename Object, typename ObjectBase>
        ReadPropertyInfo( Object* parent, T (ObjectBase::*getter)() )
        : PropertyInfo()
        {
            _getter = new Pt::Method<T, Object>( parent, getter );
        }

        ~ReadPropertyInfo()
        { delete _getter; }

        virtual Pt::Any value()
        {
            Pt::Any any;
            any = this->get();
            return any;
        }

        T get() const
        { return _getter->operator()(); }

        T operator()() const
        { return get(); }

    private:
        Pt::Callable<T>* _getter;
};


template <typename T>
class InternalReadPropertyInfo : public PropertyInfo
{
    public:
        template <typename Object, typename ObjectBase>
        InternalReadPropertyInfo( PropertyValue<T>& value, Object* parent, T (ObjectBase::*getter)() const )
        : _value(&value)
        { }

        template <typename Object, typename ObjectBase>
        InternalReadPropertyInfo( PropertyValue<T>& value, Object* parent, T (ObjectBase::*getter)() )
        : _value(&value)
        { }

        ~InternalReadPropertyInfo()
        { }

        virtual Pt::Any value()
        { return _value->value(); }

    private:
        PropertyValue<T>* _value;
};


template <typename T>
class WritePropertyInfo : virtual public PropertyInfo
{
    public:
        template <typename R, typename Object, typename ObjectBase>
        WritePropertyInfo(Object* parent, R (ObjectBase::*setter)(T type) )
        : PropertyInfo()
        {
            _setter = new Pt::Method<R, Object, T>(parent, setter);
        }

        ~WritePropertyInfo()
        {
            delete _setter;
        }

        virtual void setValue(const Pt::Any& a)
        {
            typedef typename Pt::TypeInfo<T>::ConstReference ConstRefT ;

            try {
                ConstRefT val = Pt::any_cast<ConstRefT>(a) ;
                this->set( val );
            }
            catch(...) {
                std::cerr << "WritePropertyInfo: Type mismatch: " << a.typeName() << std::endl;
            }
        }

        void operator=(T type)
        { this->set(type); }

        void set(T type)
        {
            _setter->invoke(type);
        }

    private:
        Pt::Invokable<T>* _setter;
};


template <typename R, typename A = R>
class ReadWritePropertyInfo : public ReadPropertyInfo<R>, public WritePropertyInfo<A> {
    public:
        template <typename R2, typename Object, typename ObjectBase>
        ReadWritePropertyInfo(Object* parent, R (ObjectBase::*getter)() const, R2 (ObjectBase::*setter)(A type) )
        : ReadPropertyInfo<R>(parent, getter)
        , WritePropertyInfo<A>(parent, setter)
        { }

        template <typename R2, typename Object, typename ObjectBase>
        ReadWritePropertyInfo(Object* parent, R (ObjectBase::*getter)(), R2 (ObjectBase::*setter)(A type) )
        : ReadPropertyInfo<R>(parent, getter)
        , WritePropertyInfo<A>(parent, setter)
        { }

        virtual Pt::Any value()
        {
            Pt::Any any;
            any = ReadPropertyInfo<R>::get();
            return any;
        }

        virtual void setValue(const Pt::Any& any)
        { WritePropertyInfo<A>::setValue(any); }
};


template <typename T, typename U = T>
class InternalReadWritePropertyInfo : public PropertyInfo
{
    public:
        template <typename R, typename Object, typename ObjectBase>
        InternalReadWritePropertyInfo(PropertyValue<T>& value, Object* parent, T (ObjectBase::*getter)() const, R (ObjectBase::*setter)(U type) )
        : _value(&value)
        {
            _setter = new Pt::Method<R, Object, U>(parent, setter);
        }

        template <typename R, typename Object, typename ObjectBase>
        InternalReadWritePropertyInfo(PropertyValue<T>& value, Object* parent, T (ObjectBase::*getter)(), R (ObjectBase::*setter)(U type) )
        : PropertyInfo()
        , _value(&value)
        {
            _setter = new Pt::Method<R, Object, U>(parent, setter);
        }

        ~InternalReadWritePropertyInfo()
        {
            delete _setter;
        }

        virtual Pt::Any value()
        { return _value->value(); }

        virtual void setValue(const Pt::Any& a)
        {
            try {
                const T& value = Pt::any_cast<const T&>(a) ;
                _setter->invoke( value );
            }
            catch(...) {
                std::cerr << "WritePropertyInfo: Type mismatch: " << a.typeName() << std::endl;
            }
        }

    private:
        PropertyValue<T>* _value;
        Pt::Invokable<T>* _setter;
};

} // namespace Pt

#endif
