// Copyright (C) 2000-2006 Marc Boris Drner <marcd __at arklinux __dot org>
// All rights reserved.
#ifndef Pt_Property_h
#define Pt_Property_h

#include <Pt/Exception.h>
#include <Pt/TypeInfo.h>
#include <Pt/Clonable.h>
#include <Pt/Any.h>
#include <Pt/Method.h>
#include <Pt/ConstMethod.h>
#include <Pt/Signal.h>
#include <Pt/PropertyProxy.h>
#include <Pt/Reflectable.h>


namespace Pt {


class PT_EXPORT PropertyValue : public AbstractProperty
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


template <typename T>
class PT_EXPORT ReadProperty : public PropertyValue
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

		T get() const
		{  return any_cast<T>(_value); }
};


template <typename T>
class PT_EXPORT WriteProperty : public PropertyValue
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

		void set( T value )
		{
			_value = value;
			onValueChanged.send();
		}
};


template <typename T>
class PT_EXPORT Property : public PropertyValue
{
	public:
		Property( const std::string& name, Reflectable* parent, const T& value = T() )
		: PropertyValue()
		{
			parent->registerProperty( name, this, &Property<T>::get, &Property<T>::set );
			_value = value;
		}

		T get() const
		{  return any_cast<T>(_value); }

		void set( T value )
		{
			_value = value;
			onValueChanged.send();
		}

		AbstractProperty* clone() const
		{ return new Property<T>(*this); }
};


} // namespace Pt

#endif
