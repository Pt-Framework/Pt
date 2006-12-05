// Copyright (C) 2000-2004 Marc Boris Drner <marcd __at arklinux __dot org>
// All rights reserved.

#include "Pt/Reflectable.h"


namespace Pt {

Reflectable::Reflectable(const std::string& typeName)
: _typeName(typeName)
{
	this->registerProperty( "name", this, &Reflectable::name );
}


Reflectable::~Reflectable()
{
	for(PropertyMap::iterator it = _properties.begin(); it != _properties.end(); ++it) {
		delete it->second;
	}
	_properties.clear();

	MethodMap::iterator it;
	for( it = _methods.begin(); it != _methods.end(); ++it)
	{
		delete it->second;
	}

	_methods.clear();
}


Pt::Any Reflectable::property(const std::string& name)
{
	PropertyMap::iterator it = _properties.find(name);
	if(it == _properties.end())
		return Any();

	return it->second->value();
}


void Reflectable::setProperty(const std::string& name, const Pt::Any& value)
{
	PropertyMap::iterator it = _properties.find(name);
	if( it == _properties.end() ) {
		std::cerr << "Reflectable: Could not set '" << name << "' = " << value << std::endl;
		return;
	}

	it->second->setValue(value);
}


const ICallable& Reflectable::method(const std::string& name) const
{
	MethodMap::const_iterator it = _methods.find(name);
	if( it == _methods.end() )
		throw Pt::IllegalArgument("No such method.", PT_SOURCEINFO);

	return *(it->second);
}


void Reflectable::call(const std::string& name, const Args& args)
{
	MethodMap::iterator it = _methods.find(name);
	if( it == _methods.end() )
		throw Pt::IllegalArgument("No such method:" + name, PT_SOURCEINFO);

	it->second->call(args);
}

} // namespace Pt

