/***************************************************************************
 *   Copyright (C) 2007 by Marc Boris Drner                               *
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
#ifndef Pt_PropertyInfo_h
#define Pt_PropertyInfo_h

#include <Pt/Api.h>
#include <Pt/Any.h>
#include <Pt/Signal.h>
#include <Pt/Exception.h>
#include <Pt/Method.h>
#include <Pt/MemberInfo.h>
#include <Pt/ConstMethod.h>
#include <Pt/PropertyValue.h>
#include <Pt/SerializationData.h>


namespace Pt {

/** @brief Property interface
    @ingroup Reflection
*/
class PT_API PropertyInfo  : public MemberInfo
{
    public:
        PropertyInfo()
        {}

        virtual ~PropertyInfo()
        {}

        virtual const char* typeName() const = 0;

        virtual Pt::Any get() const
        { throw std::logic_error("Property is not readable" + PT_SOURCEINFO); }

        virtual void set(const Pt::Any& value)
        { throw std::logic_error("Property is not writable" + PT_SOURCEINFO); }

        virtual void set(const Pt::Variant& v)
        {  throw std::logic_error("Property is not writable" + PT_SOURCEINFO); }

        virtual void set(const Pt::SerializationData& sd)
        { throw std::logic_error("Property is not writable" + PT_SOURCEINFO); }

        Signal<> valueChanged;
};


template <typename T>
struct PropertyTraits
{
    static void set(const Pt::Variant& v, T& t)
    { throw std::logic_error("Conversion error"); }

    static void set(const Pt::SerializationData& sd, T& t)
    { sd >> t; }
};


template <>
struct PropertyTraits<bool>
{
    static void set(const Pt::Variant& v, bool& b)
    { v.get<bool>(b); }

    static void set(const Pt::SerializationData&, bool&)
    { throw std::logic_error("Serialization error"); }
};


template <>
struct PropertyTraits<int>
{
    static void set(const Pt::Variant& v, int& val)
    { v.get<int>(val); }

    static void set(const Pt::SerializationData&, int&)
    { throw std::logic_error("Serialization error"); }
};


template <>
struct PropertyTraits<float>
{
    static void set(const Pt::Variant& v, float& val)
    { v.get<float>(val); }

    static void set(const Pt::SerializationData&, float&)
    { throw std::logic_error("Serialization error"); }
};


template <>
struct PropertyTraits<double>
{
    static void set(const Pt::Variant& v, double& val)
    { v.get<double>(val); }

    static void set(const Pt::SerializationData&, double&)
    { throw std::logic_error("Serialization error"); }
};


template <>
struct PropertyTraits<std::string>
{
    static void set(const Pt::Variant& v, std::string& s)
    { v.get<std::string>(s); }

    static void set(const Pt::SerializationData&, std::string&)
    { throw std::logic_error("Serialization error"); }
};


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

        virtual const char* typeName() const
        {
            return TypeTraits<T>::typeName();
        }

        virtual Pt::Any get() const
        {
            Pt::Any any;
            any = _getter->operator()();;
            return any;
        }

    private:
        Pt::Callable<T>* _getter;
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

        virtual const char* typeName() const
        {
            return TypeTraits<T>::typeName();
        }

        virtual void set(const Pt::Any& a)
        {
            typedef typename Pt::TypeInfo<T>::ConstReference ConstRefT ;

            try {
                ConstRefT val = Pt::any_cast<ConstRefT>(a) ;
                this->set( val );
            }
            catch(const std::exception& e) {
                std::cerr << e.what() << std::endl;
                std::cerr << "WritePropertyInfo: Type mismatch: " << a.typeName() << std::endl;
            }
        }

        virtual void set(const Pt::Variant& variant)
        {
            typedef typename Pt::TypeInfo<T>::Value ValueT ;

            ValueT value;
            PropertyTraits<ValueT>::set(variant, value);
            this->set( value );
        }

        virtual void set(const Pt::SerializationData& sd)
        {
            typedef typename Pt::TypeInfo<T>::Value ValueT ;

            ValueT value;
            PropertyTraits<ValueT>::set(sd, value);
            this->set( value );
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

        virtual const char* typeName() const
        {
            return TypeTraits<R>::typeName();
        }

        virtual Pt::Any get() const
        {
            Pt::Any any;
            any = ReadPropertyInfo<R>::get();
            return any;
        }

        virtual void set(const Pt::Any& any)
        { WritePropertyInfo<A>::set(any); }
};


template <typename T>
class ReadProperty : public PropertyInfo
{
    public:
        template <typename Object>
        ReadProperty( Object* parent, PropertyValue<T>& value )
        : _value(&value)
        { }

        ~ReadProperty()
        { }

        virtual const char* typeName() const
        {
            return TypeTraits<T>::typeName();
        }

        virtual Pt::Any value()
        { return _value->value(); }

    private:
        PropertyValue<T>* _value;
};


template <typename T, typename A = T>
class ReadWriteProperty : public PropertyInfo
{
    public:
        template <typename R,typename Class, typename Base>
        ReadWriteProperty( Class* parent, PropertyValue<T>& value,  R (Base::*setter)(A type) )
        : _value(&value)
        {
            _setter = new Pt::Method<R, Base, A>(parent, setter);
        }

        ~ReadWriteProperty()
        {
            delete _setter;
        }

        virtual const char* typeName() const
        {
            return TypeTraits<T>::typeName();
        }

        virtual Pt::Any value()
        { return _value->value(); }

        virtual void set(const Pt::Any& a)
        {
            try {
                const T& value = Pt::any_cast<const T&>(a) ;
                _setter->invoke( value );
            }
            catch(...) {
                std::cerr << "WritePropertyInfo: Type mismatch: " << a.typeName() << std::endl;
            }
        }

        virtual void set(const Pt::Variant& variant)
        {
            typedef typename Pt::TypeInfo<T>::Value ValueT ;

            ValueT value;
            PropertyTraits<ValueT>::set(variant, value);
            _setter->invoke( value );
        }

        virtual void set(const Pt::SerializationData& sd)
        {
            typedef typename Pt::TypeInfo<T>::Value ValueT ;

            ValueT value;
            PropertyTraits<ValueT>::set(sd, value);
            this->set( value );
        }

    private:
        PropertyValue<T>* _value;
        Pt::Invokable<A>* _setter;
};

} // namespace Pt

#endif
