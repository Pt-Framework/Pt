/*
 * Copyright (C) 2005-2007 by Marc Boris Duerner
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

#ifndef Pt_Reflex_Reflectable_h
#define Pt_Reflex_Reflectable_h

#include <Pt/Reflex/Api.h>
#include <Pt/SerializationInfo.h>
#include <Pt/SourceInfo.h>
#include <Pt/Method.h>
#include <Pt/ConstMethod.h>
#include <Pt/Any.h>
#include <stdexcept>
#include <memory>

namespace Pt {

class SerializationInfo;

namespace Reflex {

class PT_REFLEX_API NoSuchProperty : public std::runtime_error
{
    public:
        NoSuchProperty(const std::string& propertyName, const SourceInfo& si);

        ~NoSuchProperty() throw();
};


class PT_REFLEX_API PropertyNotReadable : public std::runtime_error
{
    public:
        PropertyNotReadable(const std::string& propertyName, const SourceInfo& si);

        ~PropertyNotReadable() throw();
};


class PT_REFLEX_API PropertyNotWritable : public std::runtime_error
{
    public:
        PropertyNotWritable(const std::string& propertyName, const SourceInfo& si);

        ~PropertyNotWritable() throw();
};


/** @brief Value type registerable as property
    @ingroup Reflection
*/
template <typename T>
class PropertyValue
{
    public:
        PropertyValue( const T& value = T() )
        : _value(value)
        {}

        const T& get() const
        { 
            const Any::Value* v = _value.value();
            const Any::BasicValue<T>* b = static_cast< const Any::BasicValue<T>* >(v);
            return b->value();
        }

        T& get()
        {
            Any::Value* v = _value.value();
            Any::BasicValue<T>* b = static_cast<Any::BasicValue<T>* >(v);
            return b->value();
        }

        void set( const T& value )
        { _value = value; }

        const Pt::Any& value() const
        { return _value; }

    protected:
        Pt::Any _value;
};


/** @brief Reflection support for properties
    @ingroup Reflection
*/
class Property
{
    public:
        Property()
        {}

        virtual ~Property()
        {}

        virtual const char* name() const = 0;

        virtual bool isWritable() const = 0;

        virtual Pt::Any get() const = 0;

        virtual void set(const Pt::Any& value) = 0;

        virtual void serialize(Pt::SerializationInfo& si) const = 0;

        virtual void deserialize(const Pt::SerializationInfo& si) = 0;
};

inline void operator<<=(Pt::SerializationInfo& si, const Property& pi)
{
    pi.serialize(si);
}

inline void operator>>=(const Pt::SerializationInfo& si, Property& pi)
{
    pi.deserialize(si);
}


template <typename T>
class ReadPropertyProxy : virtual public Property
{
    public:
        template <typename Object, typename ObjectBase>
        ReadPropertyProxy( const std::string& name, Object& parent, T (ObjectBase::*getter)() const )
        : _name(name)
        {
            _getter = new Pt::ConstMethod<T, Object>( parent, getter );
        }

        template <typename Object, typename ObjectBase>
        ReadPropertyProxy( const std::string& name, Object& parent, T (ObjectBase::*getter)() )
        : _name(name)
        {
            _getter = new Pt::Method<T, Object>( parent, getter );
        }

        ~ReadPropertyProxy()
        { delete _getter; }

        const char* name() const
        { return _name.c_str(); }

        virtual bool isWritable() const
        { return false; }

        virtual Pt::Any get() const
        {
            Pt::Any any;
            any = _getter->operator()();
            return any;
        }

        virtual void set(const Pt::Any& value)
        { throw PropertyNotWritable(this->name(), PT_SOURCEINFO); }

        void serialize(Pt::SerializationInfo& si) const
        {
            si <<= _getter->operator()();
        }

        void deserialize(const Pt::SerializationInfo& si)
        { throw PropertyNotWritable(this->name(), PT_SOURCEINFO); }

    private:
        std::string _name;
        Pt::Callable<T>* _getter;
};


template <typename R, typename A = R>
class ReadWritePropertyProxy : public Property
{
    public:
        template <typename R2, typename Object, typename ObjectBase>
        ReadWritePropertyProxy(const std::string& name, Object& parent, R (ObjectBase::*getter)() const, R2 (ObjectBase::*setter)(A type) )
        : _name(name)
        {
            std::auto_ptr< Pt::ConstMethod<R, ObjectBase> > ap( new Pt::ConstMethod<R, ObjectBase>( parent, getter ) );
            _setter = new Pt::Method<R2, ObjectBase, A>(parent, setter);
            _getter = ap.release();
        }

        template <typename R2, typename Object, typename ObjectBase>
        ReadWritePropertyProxy(const std::string& name, Object& parent, R (ObjectBase::*getter)(), R2 (ObjectBase::*setter)(A type) )
        : _name(name)
        {
            std::auto_ptr< Method<R, ObjectBase> > ap( new Method<R, ObjectBase>( parent, getter ) );
            _setter = new Pt::Method<R2, ObjectBase, A>(parent, setter);
            _getter = ap.release();
        }

        ~ReadWritePropertyProxy()
        {
            delete _setter;
            delete _getter;
        }

        const char* name() const
        { return _name.c_str(); }

        virtual bool isWritable() const
        { return true; }

        virtual Pt::Any get() const
        {
            Pt::Any any;
            any = _getter->operator()();
            return any;
        }

        virtual void set(const Pt::Any& a)
        {
            typedef typename Pt::TypeTraits<A>::ConstReference ConstRefT ;

            try {
                ConstRefT val = Pt::any_cast<ConstRefT>(a) ;
                _setter->invoke( val );
            }
            catch(const std::bad_cast&) {
                std::cerr << "WritePropertyInfo: Type mismatch: " << _name << std::endl;
            }
        }

        void serialize(Pt::SerializationInfo& si) const
        {
            si <<= _getter->operator()();
        }

        void deserialize(const Pt::SerializationInfo& si)
        {
            typedef typename Pt::TypeTraits<A>::Value ValueT;
            ValueT value;
            si >>= value;
            _setter->invoke(value);
        }

    private:
        std::string _name;
        Pt::Callable<R>* _getter;
        Pt::Invokable<A>* _setter;
};


template <typename T>
class ReadProperty : public Property
{
    public:
        template <typename Object>
        ReadProperty( const std::string& name, Object& parent, PropertyValue<T>& value )
        : _name(name)
        , _value(&value)
        { }

        ~ReadProperty()
        { }

        const char* name() const
        { return _name.c_str(); }

        virtual bool isWritable() const
        { return false; }

        virtual Pt::Any get() const
        { return _value->value(); }

        virtual void set(const Pt::Any& value)
        { throw PropertyNotWritable(this->name(), PT_SOURCEINFO); }

        void serialize(Pt::SerializationInfo& si) const
        {
            si <<=  _value->get();
        }

        void deserialize(const Pt::SerializationInfo& si)
        {
            throw PropertyNotWritable(this->name(), PT_SOURCEINFO);
        }

    private:
        std::string _name;
        PropertyValue<T>* _value;
};


template <typename T, typename A = T>
class ReadWriteProperty : public Property
{
    public:
        template <typename R,typename Class, typename Base>
        ReadWriteProperty( const std::string& name, Class& parent, PropertyValue<T>& value,  R (Base::*setter)(A type) )
        : _name(name)
        , _value(&value)
        {
            _setter = new Pt::Method<R, Base, A>(parent, setter);
        }

        ~ReadWriteProperty()
        {
            delete _setter;
        }

        const char* name() const
        { return _name.c_str(); }

        virtual bool isWritable() const
        { return true; }

        virtual Pt::Any get() const
        { return _value->value(); }

        virtual void set(const Pt::Any& a)
        {
            try {
                const T& value = Pt::any_cast<const T&>(a) ;
                _setter->invoke( value );
            }
            catch(const std::bad_cast&) {
                std::cerr << "WritePropertyInfo: Type mismatch: " << _name << std::endl;
            }
        }

        void serialize(Pt::SerializationInfo& si) const
        {
            si <<= _value->get();
        }

        void deserialize(const Pt::SerializationInfo& si)
        {
            T value = T();
            si >>= value;
            _setter->invoke( value );
        }

    private:
        std::string _name;
        PropertyValue<T>* _value;
        Pt::Invokable<A>* _setter;
};


/** @brief Make objects reflectable
    @ingroup Reflection
*/
class PT_REFLEX_API Reflectable
{
    public:
        class PropertyIterator;

        class ConstPropertyIterator;

    public:
        Reflectable();

        Reflectable(const std::string& name);

        virtual ~Reflectable();

        const std::string& objectName() const;

        void setName(const std::string& name);

        virtual Pt::Any property(const std::string& name) const;

        virtual void setProperty(const std::string& name, const Pt::Any& value);

        Property& propertyInfo(const std::string& name);

        PropertyIterator propertiesBegin();

        PropertyIterator propertiesEnd();

        ConstPropertyIterator propertiesBegin() const;

        ConstPropertyIterator propertiesEnd() const;

        template <typename R, typename Parent, typename Object>
        void registerProperty(const std::string& name, Parent& parent, R (Object::*getter)() const)
        {
            this->registerPropertyInfo( new ReadPropertyProxy<R>(name, parent, getter) );
        }

        template <typename R, typename Parent, typename Object>
        void registerProperty(const std::string& name, Parent& parent, R (Object::*getter)())
        {
            this->registerPropertyInfo( new ReadPropertyProxy<R>(name, parent, getter) );
        }

        template <typename R1, typename R2, typename A, typename Parent, typename Object>
        void registerProperty(const std::string& name, Parent& parent, R1 (Object::*getter)() const, R2 (Object::*setter)(A type))
        {
           this->registerPropertyInfo( new ReadWritePropertyProxy<R1, A>(name, parent, getter, setter) );
        }

        template <typename R1, typename R2, typename A, typename Parent, typename Object>
        void registerProperty(const std::string& name, Parent& parent, R1 (Object::*getter)(), R2 (Object::*setter)(A type))
        {
            this->registerPropertyInfo( new ReadWritePropertyProxy<R1, A>(name, parent, getter, setter) );
        }

        template <typename R, typename Parent>
        void registerReadProperty(const std::string& name, Parent& parent, PropertyValue<R>& value)
        {
            this->registerPropertyInfo( new ReadProperty<R>(name, parent, value) );
        }

        template <typename T, typename R, typename A, typename Parent, typename Object>
        void registerProperty(const std::string& name, Parent& parent, PropertyValue<T>& value, R (Object::*setter)(A type))
        {
            this->registerPropertyInfo( new ReadWriteProperty<T, A>(name, parent, value, setter) );
        }

        void deserialize(const SerializationInfo& si);

        void serialize(SerializationInfo& si) const;

    protected:
        Reflectable(const Reflectable& other);

        Reflectable& operator=(const Reflectable& other);

        void registerPropertyInfo(Property* ci);

        void include(Reflectable& r);

    private:
        struct ReflectableData* _data;
};

PT_REFLEX_API void operator >>= (const SerializationInfo& si, Reflectable& r);

PT_REFLEX_API void operator <<= (SerializationInfo& si, const Reflectable& r);


class Reflectable::PropertyIterator
{
    public:
        PropertyIterator(Property** pi)
        : _pi(pi)
        { }

        PropertyIterator()
        : _pi(0)

        {}

        PropertyIterator& operator++()
        {
            ++_pi;
            return *this;
        }

        Property& operator*()
        { return **_pi; }

        Property* operator->()
        { return *_pi; }

        bool operator!=(const PropertyIterator& other) const
        { return _pi != other._pi; }

        bool operator==(const PropertyIterator& other) const
        { return _pi == other._pi; }

    private:
        Property** _pi;
};


class Reflectable::ConstPropertyIterator
{
    public:
        ConstPropertyIterator(Property* const* pi)
        : _pi(pi)
        { }

        ConstPropertyIterator()
        : _pi(0)

        {}

        ConstPropertyIterator& operator++()
        {
            ++_pi;
            return *this;
        }

        const Property& operator*()
        { return **_pi; }

        const Property* operator->()
        { return *_pi; }

        bool operator!=(const ConstPropertyIterator& other) const
        { return _pi != other._pi; }

        bool operator==(const ConstPropertyIterator& other) const
        { return _pi == other._pi; }

    private:
        Property* const* _pi;
};

} // Reflex

} // namespace Pt

#endif
