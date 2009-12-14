/*
 * Copyright (C) 2007 by Marc Boris Drner
 * Copyright (C) 2007 by Laurentiu-Gheorghe Crisan
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
#ifndef Pt_Reflex_PropertyInfo_h
#define Pt_Reflex_PropertyInfo_h

#include <Pt/Any.h>
#include <Pt/Method.h>
#include <Pt/ConstMethod.h>
#include <Pt/SerializationInfo.h>
#include <Pt/Reflex/Api.h>
#include <Pt/Reflex/MemberInfo.h>
#include <Pt/Reflex/PropertyValue.h>
#include <memory>
#include <iostream>

namespace Pt {

namespace Reflex {

class PT_REFLEX_API PropertyNotReadable : public std::logic_error
{
    public:
        PropertyNotReadable(const std::string& propertyName, const SourceInfo& si);

        ~PropertyNotReadable() throw();
};


class PT_REFLEX_API PropertyNotWritable : public std::logic_error
{
    public:
        PropertyNotWritable(const std::string& propertyName, const SourceInfo& si);

        ~PropertyNotWritable() throw();
};


/** @brief Reflection support for properties
    @ingroup Reflection
*/
class PropertyInfo  : public MemberInfo
{
    public:
        PropertyInfo()
        {}

        virtual ~PropertyInfo()
        {}

        virtual bool isWritable() const = 0;

        virtual Pt::Any get() const = 0;

        virtual void set(const Pt::Any& value) = 0;

        virtual void serialize(Pt::SerializationInfo& si) const = 0;

        virtual void deserialize(const Pt::SerializationInfo& si) = 0;

        virtual void fixupT(const Pt::FixupInfo& fi)
        {}
};

inline void operator<<=(Pt::SerializationInfo& si, const PropertyInfo& pi)
{
    pi.serialize(si);
}

inline void operator>>=(const Pt::SerializationInfo& si, PropertyInfo& pi)
{
    pi.deserialize(si);
}


template <typename T>
class ReadPropertyInfo : virtual public PropertyInfo
{
    public:
        template <typename Object, typename ObjectBase>
        ReadPropertyInfo( const std::string& name, Object& parent, T (ObjectBase::*getter)() const )
        : _name(name)
        {
            _getter = new Pt::ConstMethod<T, Object>( parent, getter );
        }

        template <typename Object, typename ObjectBase>
        ReadPropertyInfo( const std::string& name, Object& parent, T (ObjectBase::*getter)() )
        : _name(name)
        {
            _getter = new Pt::Method<T, Object>( parent, getter );
        }

        ~ReadPropertyInfo()
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
class ReadWritePropertyInfo : public PropertyInfo
{
    public:
        template <typename R2, typename Object, typename ObjectBase>
        ReadWritePropertyInfo(const std::string& name, Object& parent, R (ObjectBase::*getter)() const, R2 (ObjectBase::*setter)(A type) )
        : _name(name)
        {
            std::auto_ptr< Pt::ConstMethod<R, ObjectBase> > ap( new Pt::ConstMethod<R, ObjectBase>( parent, getter ) );
            _setter = new Pt::Method<R2, ObjectBase, A>(parent, setter);
            _getter = ap.release();
        }

        template <typename R2, typename Object, typename ObjectBase>
        ReadWritePropertyInfo(const std::string& name, Object& parent, R (ObjectBase::*getter)(), R2 (ObjectBase::*setter)(A type) )
        : _name(name)
        {
            std::auto_ptr< Method<R, ObjectBase> > ap( new Method<R, ObjectBase>( parent, getter ) );
            _setter = new Pt::Method<R2, ObjectBase, A>(parent, setter);
            _getter = ap.release();
        }

        ~ReadWritePropertyInfo()
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
            std::cerr << "DESERIALIZE PROPERTY BEGIN " << std::endl;
            typedef typename Pt::TypeTraits<A>::Value ValueT;
            ValueT value;
            si >>= value;
            _setter->invoke(value);
            std::cerr << "DESERIALIZE PROPERTY END" << std::endl;
        }

        virtual void fixupT(const FixupInfo& fi)
        {
            std::cerr << "FIXUP PROPERTY BEGIN " << std::endl;
            typedef typename Pt::TypeTraits<A>::Value ValueT;
            ValueT value;
            fixup(value, fi);
            _setter->invoke(value);
            std::cerr << "FIXUP PROPERTY END " << std::endl;
        }

    private:
        std::string _name;
        Pt::Callable<R>* _getter;
        Pt::Invokable<A>* _setter;
};


template <typename T>
class ReadProperty : public PropertyInfo
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
class ReadWriteProperty : public PropertyInfo
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

inline void fixup(PropertyInfo& fixme, const Pt::FixupInfo& fi)
{
    std::cerr << "FIXUP PROPERTYINFO " << fi.targetType().name() << std::endl;
    fixme.fixupT(fi);
}

inline void operator >>=(const LoadInfo& li, PropertyInfo& pi)
{
    std::cerr << "LOAD PROPERTYINFO BEGIN" << std::endl;
    if(li.in().category() == Pt::SerializationInfo::Reference)
    {
        li.in().loadReference(pi);
        std::cerr << "LOAD REFERENCE" << std::endl;
    }
    else
    {
        std::cerr << "LOAD VALUE" << std::endl;
        //li.load(sp);
    }
    std::cerr << "LOAD PROPERTYINFO END" << std::endl;
}

} // namespace Reflex

} // namespace Pt

#endif
