/* Copyright (C) 2015 Marc Boris Duerner 
   Copyright (C) 2015 Laurentiu-Gheorghe Crisan
  
  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.
  
  As a special exception, you may use this file as part of a free
  software library without restriction. Specifically, if other files
  instantiate templates or use macros or inline functions from this
  file, or you compile this file and link it with other files to
  produce an executable, this file does not by itself cause the
  resulting executable to be covered by the GNU General Public
  License. This exception does not however invalidate any other
  reasons why the executable file might be covered by the GNU Library
  General Public License.
  
  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.
  
  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA*/
#ifndef Pt_Hmi_Property_h
#define Pt_Hmi_Property_h

#include <Pt/Any.h>
#include <Pt/Signal.h>
#include <Pt/Hmi/PropertyBase.h>
#include <string>

namespace Pt {
namespace Hmi {

template<typename C, typename A>
class Property  : public PropertyBase
{
public:
  Property(const char* name,  C& parent, const A& (C::*getter)() const, void (C::*setter)(const A& type) )
	: PropertyBase(name)
  , _obj(&parent)
	, _getter( getter)
	, _setter(setter)        
	{

	}


  Pt::Any getValue() const
  {
      return  (_obj->*_getter)();
  }


  void setValue(const Pt::Any& a, bool notify = true)
  {
    typedef typename Pt::TypeTraits<A>::ConstReference ConstRefT ;

    ConstRefT val = Pt::any_cast<ConstRefT>(a) ;
    (_obj->*_setter)(val);
		
		if( notify ) 
			_changed.send(val);
  }
		


	const A& get() const
	{
		return (_obj->*_getter)();
	}
	

	void set(const A& value) 
	{
		(_obj->*_setter)(value);		
	}


	Property<C,A>& operator=(const A& value)
	{
		(_obj->*_setter)(value);
		_changed.send(value);
		return *this;
	}

	Pt::Signal< const A& >& changed()
	{
		return _changed;
	}

private:
	C* _obj;
	const A& (C::*_getter)() const;
	void (C::*_setter)(const A&);
	Pt::Signal<const A&> _changed;	
};


#define PT_HMI_INIT_PROPERTY_VALUE(prop, value) prop(#prop, value)
#define PT_HMI_INIT_PROPERTY(prop) prop(#prop)

template<typename T>
class ValueProperty  : public PropertyBase
{
public:
  ValueProperty(const char* name)
	: PropertyBase(name)
	{

	}


  ValueProperty(const char* name, const T& value )
	: PropertyBase(name)
	, _value(value)
	{

	}


  Pt::Any getValue() const
  {
      return  Pt::Any(_value);
  }


  void setValue(const Pt::Any& a, bool notify = true)
  {
		typedef typename Pt::TypeTraits<T>::ConstReference ConstRefT ;
		_value = Pt::any_cast<ConstRefT>(a) ;
		
		if( notify ) 
			_changed.send(_value);
  }
		

	T& get()
	{
		return _value;
	}


	const T& get() const
	{
		return _value;
	}
	

	void set(const T& value) 
	{
		_value = value;
	}


	ValueProperty<T>& operator=(const T& value)
	{
		_value = value;
		_changed.send(_value);
		return *this;
	}

	Pt::Signal< const T& >& changed()
	{
		return _changed;
	}

private:
	T _value;
	Pt::Signal<const T&> _changed;	
};

}}
#endif

