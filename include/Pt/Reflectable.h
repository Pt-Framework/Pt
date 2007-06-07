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

#include <Pt/Api.h>
#include <Pt/Exception.h>
#include <Pt/MethodInfo.h>
#include <Pt/PropertyValue.h>
#include <Pt/PropertyInfo.h>
#include <map>


namespace Pt {


class NoSuchProperty : public std::logic_error
{
    public:
        NoSuchProperty(const std::string& propertyName, const SourceInfo& si);

        ~NoSuchProperty() throw();

    private:
        std::string _propertyName;
};


class NoSuchMethod : public std::logic_error
{
    public:
        NoSuchMethod(const std::string& methodName, const SourceInfo& si);

        ~NoSuchMethod() throw();

    private:
        std::string _methodName;
};


typedef std::multimap<std::string, PropertyInfo*> PropertyMap;
typedef std::multimap<std::string, CallableInfo*> MethodMap;


/** @brief Make objects reflectable
    @ingroup Reflection

    TODO: PropertyMap and MethodMap: MemberInfo mapping?
    TODO: rename CallableInfo to MethodInfo
    TODO: registerXXX names for Read Write properties
    TODO: rename method() to methodInfo()
    TODO: remove self()
    TODO: rename getIdentifierName()
*/
class PT_API Reflectable
{
    public:
        Reflectable(const std::string& name);

        virtual ~Reflectable();

        Reflectable* self()
        { return this; }

        const std::string& getIdentifierName() const
        { return _identiferName; }

        Pt::Any property(const std::string& name);

        void setProperty(const std::string& name, const Pt::Any& value);

        Pt::Any call(const std::string& name, const Args& args);

        const CallableInfo& method(const std::string& name) const;

        const PropertyMap& properties() const
        { return _properties; }

        PropertyMap& properties()
        { return _properties; }

        const MethodMap& methods() const
        { return _methods; }

        template <typename R, typename Parent, typename Object>
        void registerWriteProperty(const std::string& name, Parent* parent, R (Object::*setter)() )
        {
            _properties.insert( std::make_pair(name, new WritePropertyInfo<R>(parent, setter)) );
        }

        template <typename R, typename Parent, typename Object>
        void registerProperty(const std::string& name, Parent* parent, R (Object::*getter)() const)
        {
            _properties.insert( std::make_pair(name, new ReadPropertyInfo<R>(parent, getter)) );
        }

        template <typename R, typename Parent, typename Object>
        void registerProperty(const std::string& name, Parent* parent, R (Object::*getter)())
        {
            _properties.insert( std::make_pair(name, new ReadPropertyInfo<R>(parent, getter)) );
        }

        template <typename R1, typename R2, typename A, typename Parent, typename Object>
        void registerProperty(const std::string& name, Parent* parent, R1 (Object::*getter)() const, R2 (Object::*setter)(A type))
        {
            _properties.insert( std::make_pair(name, new ReadWritePropertyInfo<R1, A>(parent, getter, setter)) );
        }

        template <typename R1, typename R2, typename A, typename Parent, typename Object>
        void registerProperty(const std::string& name, Parent* parent, R1 (Object::*getter)(), R2 (Object::*setter)(A type))
        {
            _properties.insert( std::make_pair(name, new ReadWritePropertyInfo<R1, A>(parent, getter, setter)) );
        }


        template <typename R, typename Parent>
        void registerProperty(const std::string& name, Parent* parent, PropertyValue<R>& value)
        {
            _properties.insert( std::make_pair(name, new InternalReadPropertyInfo<R>(parent, value)) );
        }


        template <typename T, typename R, typename A, typename Parent, typename Object>
        void registerProperty(const std::string& name, Parent* parent, PropertyValue<T>& value, R (Object::*setter)(A type))
        {
            _properties.insert( std::make_pair(name, new InternalReadWritePropertyInfo<T, A>(parent, value, setter)) );
        }

        template <typename ParentT>
        void registerMethod(const std::string& name, ParentT& parent, void (ParentT::*memFunc)() )
        {
            CallableInfo* cb = new MethodInfo<void, ParentT>(&parent, memFunc);
            _methods.insert( std::make_pair(name, cb) );
        }

        template <class ParentT, typename A1>
        void registerMethod(const std::string& name, ParentT& parent, void (ParentT::*memFunc)(A1) )
        {
            CallableInfo* cb =  new MethodInfo<void, ParentT, A1>(&parent, memFunc);
            _methods.insert( std::make_pair(name, cb) );
        }

        template <class ParentT, typename A1>
        void registerMethod(const std::string& name, ParentT& parent, void (ParentT::*memFunc)(A1) const )
        {
            CallableInfo* cb =  new MethodInfo<void, ParentT, A1>(&parent, memFunc);
            _methods.insert( std::make_pair(name, cb) );
        }

        template <class ParentT, typename A1, typename A2>
        void registerMethod(const std::string& name, ParentT& parent, void (ParentT::*memFunc)(A1, A2) )
        {
            CallableInfo* cb =  new MethodInfo<void, ParentT, A1, A2>(&parent, memFunc);
            _methods.insert( std::make_pair(name, cb) );
        }

        template <class ParentT, typename A1, typename A2, typename A3>
        void registerMethod(const std::string& name, ParentT& parent, void (ParentT::*memFunc)(A1, A2, A3) )
        {
            CallableInfo* cb =  new MethodInfo<void, ParentT, A1, A2, A3>(&parent, memFunc);
            _methods.insert( std::make_pair(name, cb) );
        }

        template <class ParentT, typename A1, typename A2, typename A3, typename A4>
        void registerMethod(const std::string& name, ParentT& parent, void (ParentT::*memFunc)(A1, A2, A3, A4) )
        {
            CallableInfo* cb =  new MethodInfo<void, ParentT, A1, A2, A3, A4>(&parent, memFunc);
            _methods.insert( std::make_pair(name, cb) );
        }

        template <class ParentT, typename A1, typename A2, typename A3, typename A4, typename A5>
        void registerMethod(const std::string& name, ParentT& parent, void (ParentT::*memFunc)(A1, A2, A3, A4, A5) )
        {
            CallableInfo* cb =  new MethodInfo<void, ParentT, A1, A2, A3, A4, A5>(&parent, memFunc);
            _methods.insert( std::make_pair(name, cb) );
        }

    private:
        MethodMap   _methods;
        PropertyMap _properties;
        std::string _identiferName;
        void* _reserved;
};

} // namespace Pt

#endif
