// Copyright (C) 2000-2004 Marc Boris Drner <marcd __at arklinux __dot org>
// All rights reserved.
#ifndef Pt_Reflectable_h
#define Pt_Reflectable_h

#include <Pt/PropertyProxy.h>
#include <map>


namespace Pt {

typedef std::multimap<std::string, AbstractProperty*> PropertyMap;


class PT_EXPORT Reflectable {
	public:
		Reflectable(const std::string& typeName = "Reflectable");

		virtual ~Reflectable();

		const std::string& typeName() const
		{ return _typeName; }

		Pt::Any property(const std::string& name);

		void setProperty(const std::string& name, const Pt::Any& value);

		const PropertyMap& properties() const
		{return _properties;}

		template <typename R, typename Parent, typename Object>
		void registerWriteProperty(const std::string& name, Parent* parent, R (Object::*setter)() )
		{ _properties.insert( std::make_pair(name, new WritePropertyProxy<R>(parent, setter)) ); }

		template <typename R, typename Parent, typename Object>
		void registerProperty(const std::string& name, Parent* parent, R (Object::*getter)() const)
		{ _properties.insert( std::make_pair(name, new ReadPropertyProxy<R>(parent, getter)) ); }

		template <typename R, typename Parent, typename Object>
		void registerProperty(const std::string& name, Parent* parent, R (Object::*getter)())
		{ _properties.insert( std::make_pair(name, new ReadPropertyProxy<R>(parent, getter)) ); }

		template <typename R1, typename R2, typename A, typename Parent, typename Object>
		void registerProperty(const std::string& name, Parent* parent, R1 (Object::*getter)() const, R2 (Object::*setter)(A type))
		{ _properties.insert( std::make_pair(name, new PropertyProxy<R1, A>(parent, getter, setter)) ); }

		template <typename R1, typename R2, typename A, typename Parent, typename Object>
		void registerProperty(const std::string& name, Parent* parent, R1 (Object::*getter)(), R2 (Object::*setter)(A type))
		{ _properties.insert( std::make_pair(name, new PropertyProxy<R1, A>(parent, getter, setter)) ); }

	private:
		std::string _typeName;
		PropertyMap _properties;
};

} // namespace Pt

#endif
