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



namespace Pt {

class Reflectable;

class PT_EXPORT Property : public Clonable<Property> {
	public:
		Property()
		{}

		virtual ~Property()
		{}

		virtual Property* clone() const = 0;

		virtual Pt::Any value() = 0;

		virtual void setValue(const Pt::Any& value) = 0;
};

template <typename T>
class PT_EXPORT ValueProperty : virtual public Property 
{
	public:
		ValueProperty( const std::string& name, Reflectable* parent, const T& value = T() )
		: Property()
		, _value(value)
		{ 				
			parent->registerProperty( name, this, &ValueProperty<T>::get, &ValueProperty<T>::set ); 
		}

		~ValueProperty()
		{ }

		Property* clone() const
		{ return new ValueProperty<T>(*this); }

		virtual Pt::Any value()
		{ return _value; }

		virtual void setValue(const Pt::Any& value)
		{ 
			_value = value;
			onValueChanged.send();
		}

		T get() const
		{  return any_cast<T>(_value); }

		void set( T value )
		{
			_value = value;
			onValueChanged.send();
		}

		virtual void closed(const Connection& c)
		{ }

		Signal<> onValueChanged;

	private:
	    Pt::Any _value;
};

template <typename T>
class PT_EXPORT ReadProperty : virtual public Property {
	public:
		template <typename Object, typename ObjectBase>
		ReadProperty(Object* parent, T (ObjectBase::*getter)() const)
		: Property()
		{ _getter = new Pt::ConstMethod<T, Object>(parent, getter); }

		template <typename Object, typename ObjectBase>
		ReadProperty(Object* parent, T (ObjectBase::*getter)() )
		: Property()
		{ _getter = new Pt::Method<T, Object>(parent, getter); }

		ReadProperty(const ReadProperty& property)
		: Property()
		{ _getter = property._getter->clone(); }

		~ReadProperty()
		{ delete _getter; }

		Property* clone() const
		{ return new ReadProperty<T>(*this); }

		virtual Pt::Any value()
		{
			Pt::Any any;
			any = this->get();
			return any;
		}

		virtual void setValue(const Pt::Any&)
		{ }

		T get() const
		{ return _getter->operator()(); }

		T operator()() const
		{ return get(); }

	private:
		Pt::Callable<T>* _getter;
};


template <typename T>
class PT_EXPORT WriteProperty : virtual public Property {
	public:
		template <typename R, typename Object, typename ObjectBase>
		WriteProperty(Object* parent, R (ObjectBase::*setter)(T type) )
		{ _setter = new Pt::Method<R, Object, T>(parent, setter); }

		WriteProperty(const WriteProperty& property)
		: Property()
		{ 
			
		//	_setter = property._setter->clone(); 
			_setter = property._setter; 
		
		}

		~WriteProperty()
		{ delete _setter; }

		Property* clone() const
		{ return new WriteProperty(*this); }

		virtual Pt::Any value()
		{ return Pt::Any(); }

		virtual void setValue(const Pt::Any& a)
		{
			typedef typename Pt::TypeInfo<T>::ConstReference ConstRefT ;
			try {
				ConstRefT val = Pt::any_cast<ConstRefT>(a) ;
				this->set( val );
			}
			catch(...) {
				std::cerr << "WriteProperty: Type mismatch: " << a.typeName() << std::endl;
			}
		}

		void operator=(T type)
		{ this->set(type); }

		void set(T type)
		{ _setter->invoke(type); }

	private:
		Pt::Invokable<T>* _setter;
};


template <typename R, typename A = R>
class PT_EXPORT ReadWriteProperty : public ReadProperty<R>, public WriteProperty<A> {
	public:
		template <typename R2, typename Object, typename ObjectBase>
		ReadWriteProperty(Object* parent, R (ObjectBase::*getter)() const, R2 (ObjectBase::*setter)(A type) )
		: ReadProperty<R>(parent, getter), WriteProperty<A>(parent, setter)
		{}

		template <typename R2, typename Object, typename ObjectBase>
		ReadWriteProperty(Object* parent, R (ObjectBase::*getter)(), R2 (ObjectBase::*setter)(A type) )
		: ReadProperty<R>(parent, getter), WriteProperty<A>(parent, setter)
		{ }

		ReadWriteProperty(const ReadWriteProperty& property)
		: Property(), ReadProperty<R>(property), WriteProperty<A>(property)
		{}

		Property* clone() const
		{ return new ReadWriteProperty(*this);}

		virtual Pt::Any value()
		{
			Pt::Any any;
			any = ReadProperty<R>::get();
			return any;
		}

		virtual void setValue(const Pt::Any& any)
		{ WriteProperty<A>::setValue(any); }
};


} // namespace Pt

#endif
