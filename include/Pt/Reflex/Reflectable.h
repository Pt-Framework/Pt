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
#include <Pt/Reflex/MethodInfo.h>
#include <Pt/Reflex/PropertyInfo.h>
#include <Pt/Deserializer.h>
#include <Pt/SerializationInfo.h>
#include <Pt/SourceInfo.h>
#include <stdexcept>

namespace Pt {

class SerializationInfo;

namespace Reflex {

class PT_REFLEX_API NoSuchProperty : public std::logic_error
{
    public:
        NoSuchProperty(const std::string& propertyName, const SourceInfo& si);

        ~NoSuchProperty() throw();
};


class PT_REFLEX_API NoSuchMethod : public std::logic_error
{
    public:
        NoSuchMethod(const std::string& methodName, const SourceInfo& si);

        ~NoSuchMethod() throw();
};


/** @brief Make objects reflectable
    @ingroup Reflection
*/
class PT_REFLEX_API Reflectable
{
    public:
        class MethodIterator;

        class ConstMethodIterator;

        class PropertyIterator;

        class ConstPropertyIterator;

    public:
        Reflectable(const std::string& name);

        virtual ~Reflectable();

        const std::string& objectName() const;

        virtual Pt::Any property(const std::string& name) const;

        virtual void setProperty(const std::string& name, const Pt::Any& value);

        void invoke(const std::string& name, const Any* args, size_t argCount);

        Pt::Any call(const std::string& name, const Any* args, size_t argCount);

        PropertyInfo& propertyInfo(const std::string& name);

        CallableInfo& methodInfo(const std::string& name);

        const CallableInfo& methodInfo(const std::string& name) const;

        PropertyIterator propertiesBegin();

        PropertyIterator propertiesEnd();

        ConstPropertyIterator propertiesBegin() const;

        ConstPropertyIterator propertiesEnd() const;

        MethodIterator methodsBegin();

        MethodIterator methodsEnd();

        ConstMethodIterator methodsBegin() const;

        ConstMethodIterator methodsEnd() const;

        template <typename R, typename Parent, typename Object>
        void registerProperty(const std::string& name, Parent& parent, R (Object::*getter)() const)
        {
            this->registerPropertyInfo( new ReadPropertyInfo<R>(name, parent, getter) );
        }

        template <typename R, typename Parent, typename Object>
        void registerProperty(const std::string& name, Parent& parent, R (Object::*getter)())
        {
            this->registerPropertyInfo( new ReadPropertyInfo<R>(name, parent, getter) );
        }

        template <typename R1, typename R2, typename A, typename Parent, typename Object>
        void registerProperty(const std::string& name, Parent& parent, R1 (Object::*getter)() const, R2 (Object::*setter)(A type))
        {
           this->registerPropertyInfo( new ReadWritePropertyInfo<R1, A>(name, parent, getter, setter) );
        }

        template <typename R1, typename R2, typename A, typename Parent, typename Object>
        void registerProperty(const std::string& name, Parent& parent, R1 (Object::*getter)(), R2 (Object::*setter)(A type))
        {
            this->registerPropertyInfo( new ReadWritePropertyInfo<R1, A>(name, parent, getter, setter) );
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

        template <typename R, typename ParentT>
        void registerMethod(const std::string& name, ParentT& parent, R (ParentT::*memFunc)() )
        {
            CallableInfo* cb = new MethodInfo<R, ParentT>(name, parent, memFunc);
            this->registerCallableInfo(cb);
        }

        template <class ParentT, typename A1>
        void registerMethod(const std::string& name, ParentT& parent, void (ParentT::*memFunc)(A1) )
        {
            CallableInfo* cb =  new MethodInfo<void, ParentT, A1>(name, parent, memFunc);
            this->registerCallableInfo(cb);
        }

        template <class ParentT, typename A1>
        void registerMethod(const std::string& name, ParentT& parent, void (ParentT::*memFunc)(A1) const )
        {
            CallableInfo* cb =  new MethodInfo<void, ParentT, A1>(name, parent, memFunc);
            this->registerCallableInfo(cb);
        }

        template <class ParentT, typename A1, typename A2>
        void registerMethod(const std::string& name, ParentT& parent, void (ParentT::*memFunc)(A1, A2) )
        {
            CallableInfo* cb =  new MethodInfo<void, ParentT, A1, A2>(name, parent, memFunc);
            this->registerCallableInfo(cb);
        }

        template <class ParentT, typename A1, typename A2, typename A3>
        void registerMethod(const std::string& name, ParentT& parent, void (ParentT::*memFunc)(A1, A2, A3) )
        {
            CallableInfo* cb =  new MethodInfo<void, ParentT, A1, A2, A3>(name, parent, memFunc);
            this->registerCallableInfo(cb);
        }

        template <class ParentT, typename A1, typename A2, typename A3, typename A4>
        void registerMethod(const std::string& name, ParentT& parent, void (ParentT::*memFunc)(A1, A2, A3, A4) )
        {
            CallableInfo* cb =  new MethodInfo<void, ParentT, A1, A2, A3, A4>(name, parent, memFunc);
            this->registerCallableInfo(cb);
        }

        template <class ParentT, typename A1, typename A2, typename A3, typename A4, typename A5>
        void registerMethod(const std::string& name, ParentT& parent, void (ParentT::*memFunc)(A1, A2, A3, A4, A5) )
        {
            CallableInfo* cb =  new MethodInfo<void, ParentT, A1, A2, A3, A4, A5>(name, parent, memFunc);
            this->registerCallableInfo(cb);
        }

        template <class ParentT, typename A1, typename A2, typename A3, typename A4,
                                 typename A5, typename A6, typename A7, typename A8>
        void registerMethod(const std::string& name, ParentT& parent, void (ParentT::*memFunc)(A1, A2, A3, A4, A5, A6, A7, A8) )
        {
            CallableInfo* cb =  new MethodInfo<void, ParentT, A1, A2, A3, A4, A5, A6, A7, A8>(name, parent, memFunc);
            this->registerCallableInfo(cb);
        }

        void registerCallableInfo(CallableInfo* ci);

        void deserialize(const SerializationInfo& si);

        void serialize(SerializationInfo& si) const;

    protected:
        Reflectable(const Reflectable& other);

        Reflectable& operator=(const Reflectable& other);

        void registerPropertyInfo(PropertyInfo* ci);

        void include(Reflectable& r);

    private:
        struct ReflectableData* _data;
};

PT_REFLEX_API void operator >>= (const SerializationInfo& si, Reflectable& r);

PT_REFLEX_API void operator <<= (SerializationInfo& si, const Reflectable& r);


class Reflectable::MethodIterator
{
    public:
        MethodIterator()
        : _ci(0)
        {}

        MethodIterator(CallableInfo** ci)
        : _ci(ci)
        {}

        MethodIterator& operator++()
        {
            _ci += 1;
            return *this;
        }

        CallableInfo& operator*()
        { return **_ci; }

        CallableInfo* operator->()
        { return *_ci; }

        bool operator!=(const MethodIterator& other) const
        { return _ci != other._ci; }

    private:
        CallableInfo** _ci;
};


class Reflectable::ConstMethodIterator
{
    public:
        ConstMethodIterator()
        : _ci(0)
        {}

        ConstMethodIterator(CallableInfo* const* ci)
        : _ci(ci)
        {}

        ConstMethodIterator& operator++()
        {
            ++_ci;
            return *this;
        }

        const CallableInfo& operator*() const
        { return **_ci; }

        const CallableInfo* operator->() const
        { return *_ci; }

        bool operator!=(const ConstMethodIterator& other) const
        { return _ci != other._ci; }

    private:
        CallableInfo* const* _ci;
};


class Reflectable::PropertyIterator
{
    public:
        PropertyIterator(PropertyInfo** pi)
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

        PropertyInfo& operator*()
        { return **_pi; }

        PropertyInfo* operator->()
        { return *_pi; }

        bool operator!=(const PropertyIterator& other) const
        { return _pi != other._pi; }

        bool operator==(const PropertyIterator& other) const
        { return _pi == other._pi; }

    private:
        PropertyInfo** _pi;
};


class Reflectable::ConstPropertyIterator
{
    public:
        ConstPropertyIterator(PropertyInfo* const* pi)
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

        const PropertyInfo& operator*()
        { return **_pi; }

        const PropertyInfo* operator->()
        { return *_pi; }

        bool operator!=(const ConstPropertyIterator& other) const
        { return _pi != other._pi; }

        bool operator==(const ConstPropertyIterator& other) const
        { return _pi == other._pi; }

    private:
        PropertyInfo* const* _pi;
};

} // Reflex

template <>
class Composer<Reflex::Reflectable> : public IComposer
{
    public:
        Composer()
        : _reflectable(0)
        , _property(0)
        , _current(&_si)
        {}

        void begin(Reflex::Reflectable& t)
        {
            if(_reflectable)
                this->clear();

            _reflectable = &t;
        }

        virtual void clear()
        {
            _si.clear();
            _reflectable = 0;
            _property = 0;
            _current = &_si;
        }

        virtual void setContext(SerializationContext* context)
        {
            _si.setContext(context);
        }

        virtual void setName(const std::string& name)
        {
            if(_property)
                _current->setName(name);
            //else
            //    _reflectable->setObjectName
        }

        virtual void setId(const std::string& id)
        {
            //_current->setId(id);
        }

        virtual void setTypeName(const std::string& type)
        { }

        virtual void setValue(const Pt::String& value)
        {
            if(_current)
                _current->setValue(value);
        }

        virtual void setBool(bool value)
        {
            if(_current)
                _current->setValue(value);
        }

        virtual void setInt(long value)
        {
            if(_current)
                _current->setValue(value);
        }

        virtual void setUInt(unsigned long value)
        {
            if(_current)
                _current->setValue(value);
        }

        virtual void setFloat(double value)
        {
            if(_current)
                _current->setValue(value);
        }

        virtual void setReference(const std::string& id)
        {
            // always an error
            //_si.setReference(id);
        }

        virtual IComposer* beginMember(const std::string& name)
        {
            if(_property)
            {
                _current = &( _current->addMember(name) );
                return this;
            }

            _property = &( _reflectable->propertyInfo(name) );

            if(_property)
            {
                _current = &_si;
                return this;
            }

            // try to find reflectable child and set _reflectable
            return this;
        }

        virtual IComposer* beginElement()
        {
            return this; // always an error
        }

        virtual IComposer* finish()
        {
            if(_property)
            {
                if( _current->parent() )
                {
                    _current = _current->parent();
                    return this;
                }

                _property->deserialize(_si);
                _si.clear();
                _property = 0;
                return this;
            }

            // if( _reflectable->parent() )
            // {
            //     _reflectable = _reflectable->parent();
            //     return this;
            // }

            return 0;
        }

    private:
        Reflex::Reflectable* _reflectable;
        Reflex::PropertyInfo* _property;
        Pt::SerializationInfo _si;
        Pt::SerializationInfo* _current;
};

} // namespace Pt

#endif
