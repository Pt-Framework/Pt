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

template <typename T>
class PT_EXPORT ReadProperty : public AbstractProperty 
{
	public:
		ReadProperty( const std::string& name, Reflectable* parent, const T& value = T() )
		: AbstractProperty()
		, _value( value )
		{ parent->registerProperty( name, this, &ReadProperty<T>::get ); }

		~ReadProperty()
		{ }

		AbstractProperty* clone() const
		{  return new Property<T>(*this); }

		virtual Pt::Any value()
		{  return _value; }

		T get() const
		{  return any_cast<T>(_value); }

	private:
	    Pt::Any _value;
};


template <typename T>
class PT_EXPORT WriteProperty : public AbstractProperty 
{
	public:
		WriteProperty( const std::string& name, Reflectable* parent, const T& value = T() )
		: AbstractProperty()
		, _value( value )
		{ parent->registerProperty( name, this, &WriteProperty<T>::get, &WriteProperty<T>::set ); }

		~WriteProperty()
		{ }

		AbstractProperty* clone() const
		{ return new WriteProperty<T>(*this); }


		virtual void setValue(const Pt::Any& value)
		{ _value = value; }

		void set( T value )
		{ 
			_value = value;
			onValueChanged.send();
		}		

	private:

		T get() const
		{  throw LogicError("A WriteProperty can not be readed", PT_SOURCEINFO); }

	    Pt::Any _value;
};

template <typename T>
class PT_EXPORT Property : public AbstractProperty 
{
	public:
		Property( const std::string& name, Reflectable* parent, const T& value = T() )
		: AbstractProperty()
		, _value( value )
		{ parent->registerProperty( name, this, &Property<T>::get, &Property<T>::set ); }

		~Property()
		{ }

		AbstractProperty* clone() const
		{ return new Property<T>(*this); }

		virtual Pt::Any value()
		{ return _value; }

		virtual void setValue(const Pt::Any& value)
		{ _value = value; }

		T get() const
		{ return any_cast<T>(_value); }

		void set( T value )
		{
			_value = value;
			onValueChanged.send();
		}		

	private:
	    Pt::Any _value;
};


} // namespace Pt

#endif
