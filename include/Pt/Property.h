/***************************************************************************
 *   Copyright (C) 2007 by Marc Boris Dürner                               *
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
#ifndef Pt_Property_h
#define Pt_Property_h

#include <Pt/Exception.h>
#include <Pt/TypeInfo.h>
#include <Pt/Any.h>
#include <Pt/Method.h>
#include <Pt/ConstMethod.h>
#include <Pt/Signal.h>
#include <Pt/PropertyProxy.h>


namespace Pt {

/** @brief Value type registerable as property
    @ingroup Reflection
*/
template <typename T>
class Property
{
    public:
        Property(const std::string& name, const T& value = T() )
        : _name(name)
        , _value(value)
        {}

        const T& get()const
        {  return any_cast<const T&>(_value); }

        void set( const T& value )
        { _value = value; }

        const std::string& name() const
        { return _name; }

        const Pt::Any value() const
        { return _value; }

    protected:
        std::string _name;
        Pt::Any _value;
};


template <typename T>
class ReadProperty : public IProperty
{
    public:
        template <typename Object, typename ObjectBase>
        ReadProperty( Property<T>& value, Object* parent, T (ObjectBase::*getter)() const )
        : _value(&value)
        {
            _getter = new Pt::ConstMethod<T, Object>( parent, getter );
        }

        template <typename Object, typename ObjectBase>
        ReadProperty( Property<T>& value, Object* parent, T (ObjectBase::*getter)() )
        : IProperty()
        , _value(&value)
        {
            _getter = new Pt::Method<T, Object>( parent, getter );
        }

        ~ReadProperty()
        { delete _getter; }

        virtual Pt::Any value()
        { return _value->value(); }

    private:
        Property<T>* _value;
        Pt::Callable<T>* _getter;
};


template <typename T>
class WriteProperty : virtual public IProperty
{
    public:
        template <typename R, typename Object, typename ObjectBase>
        WriteProperty(Object* parent, R (ObjectBase::*setter)(T type) )
        {
            _setter = new Pt::Method<R, Object, T>(parent, setter);
        }

        ~WriteProperty()
        { delete _setter; }

        virtual void setValue(const Pt::Any& a)
        {
            try {
                const T& value = Pt::any_cast<const T&>(a) ;
                _setter->invoke( value );
            }
            catch(...) {
                std::cerr << "WriteProperty: Type mismatch: " << a.typeName() << std::endl;
            }
        }

    private:
        Pt::Invokable<T>* _setter;
};


template <typename T, typename U = T>
class ReadWriteProperty : public IProperty
{
    public:
        template <typename R, typename Object, typename ObjectBase>
        ReadWriteProperty(Property<T>& value, Object* parent, T (ObjectBase::*getter)() const, R (ObjectBase::*setter)(U type) )
        : _value(&value)
        {
            _getter = new Pt::ConstMethod<T, Object>(parent, getter) ;
            _setter = new Pt::Method<R, Object, U>(parent, setter);
        }

        template <typename R, typename Object, typename ObjectBase>
        ReadWriteProperty(Property<T>& value, Object* parent, T (ObjectBase::*getter)(), R (ObjectBase::*setter)(U type) )
        : IProperty()
        , _value(&value)
        {
            _getter = new Pt::Method<T, Object>(parent, getter);
            _setter = new Pt::Method<R, Object, U>(parent, setter);
        }

        ~ReadWriteProperty()
        {
            delete _getter;
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
                std::cerr << "WritePropertyProxy: Type mismatch: " << a.typeName() << std::endl;
            }
        }

    private:
        Property<T>* _value;
        Pt::Callable<T>* _getter;
        Pt::Invokable<T>* _setter;
};

}


/*
namespace Pt {


template <typename T>
class ReadProperty : public IProperty
{
    public:
        ReadProperty( const std::string& name, Reflectable* parent, const T& value = T() )
        : PropertyValue()
        {
            parent->registerProperty( name, this, &ReadProperty<T>::get );
            _value = value;
        }

        const T& get() const
        {  return any_cast<const T&>(_value); }

        virtual Pt::Any value()
        {  return _value; }

        virtual void setValue(const Pt::Any& value)
        {
            _value = value;
            onValueChanged.send();
        }

    protected:
        Pt::Any _value;
};



template <typename T>
class WriteProperty : public IProperty
{
    public:
        WriteProperty( const std::string& name, Reflectable* parent, const T& value = T() )
        : PropertyValue()
        {
            parent->registerWriteProperty( name, this, &WriteProperty<T>::set );
            _value = value;
        }

        void set( const T& value )
        {
            _value = value;
            onValueChanged.send();
        }

        virtual Pt::Any value()
        {  return _value; }

        virtual void setValue(const Pt::Any& value)
        {
            _value = value;
            onValueChanged.send();
        }

    protected:
        Pt::Any _value;
};

template <typename T>
class Property : public IProperty
{
    public:
        Property( const std::string& name, Reflectable* parent, const T& value = T() )
        : PropertyValue()
        {
            parent->registerProperty( name, this, &Property<T>::get, &Property<T>::set );
            _value = value;
        }

        const T& get()const
        {  return any_cast<const T&>(_value); }

        void set( const T& value )
        {
            _value = value;
            onValueChanged.send();
        }

        virtual Pt::Any value()
        {  return _value; }

        virtual void setValue(const Pt::Any& value)
        {
            _value = value;
            onValueChanged.send();
        }

    protected:
        Pt::Any _value;
};


} // namespace Pt
*/
#endif
