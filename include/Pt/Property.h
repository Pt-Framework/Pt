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
#include <Pt/Reflectable.h>


namespace Pt {

class PT_API PropertyValue : public AbstractProperty
{
    public:
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


/** @brief Read-only property
    @ingroup Reflection
*/
template <typename T>
class ReadProperty : public PropertyValue
{
    public:
        ReadProperty( const std::string& name, Reflectable* parent, const T& value = T() )
        : PropertyValue()
        {
            parent->registerProperty( name, this, &ReadProperty<T>::get );
            _value = value;
        }

        AbstractProperty* clone() const
        {  return new ReadProperty<T>(*this); }

        const T& get() const
        {  return any_cast<const T&>(_value); }
};


/** @brief Write-only property
    @ingroup Reflection
*/
template <typename T>
class WriteProperty : public PropertyValue
{
    public:
        WriteProperty( const std::string& name, Reflectable* parent, const T& value = T() )
        : PropertyValue()
        {
            parent->registerWriteProperty( name, this, &WriteProperty<T>::set );
            _value = value;
        }

        AbstractProperty* clone() const
        { return new WriteProperty<T>(*this); }

        void set( const T& value )
        {
            _value = value;
            onValueChanged.send();
        }
};

/** @brief Readable and writable property
    @ingroup Reflection
*/
template <typename T>
class Property : public PropertyValue
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

        AbstractProperty* clone() const
        { return new Property<T>(*this); }
};


} // namespace Pt

#endif
