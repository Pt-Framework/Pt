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

    virtual ~Property()
	{
	}

	inline T& get()
	{
		return _value;
	}

	inline const T& get() const
	{
		return _value;
	}
	
	inline void set(const T& value) 
	{
		_value = value;
	}

	T& operator=(const T& value)
	{
		_value = value;		
		PropertyChanged.send(parent(), *this);
         changed();
		return _value;
	}

public:	
	Pt::Signal<const void*, const PropertyBase&> PropertyChanged;

private:
	T _value;
};

}}
#endif

