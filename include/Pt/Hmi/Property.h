#ifndef Pt_Hmi_Property_h
#define Pt_Hmi_Property_h

#include <Pt/Signal.h>
#include <string>
#include <Pt/Hmi/PropertyBase.h>

namespace Pt {
namespace Hmi {

template<typename T>
class  Property  : public PropertyBase
{
public:
    Property(const char* name)
	: PropertyBase(name)
	{
	}

	Property(const char* name, const T& value)
	: PropertyBase(name)
	, _value(value)
	{
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

	Property<T>& operator=(const T& value)
	{
		_value = value;		
		Changed.send(*this);
		return *this;
	}

public:	
	Pt::Signal< const Property<T>&> Changed;

private:
	T _value;
};

}}
#endif

