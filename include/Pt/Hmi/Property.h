#ifndef Pt_Hmi_Base_Property_h
#define Pt_Hmi_Base_Property_h


#include <Pt/Signal.h>

namespace Pt {
namespace Hmi {


class PropertyBase
{
public:
	PropertyBase(void* parent)
	: _sender(parent)
	{
	
	}

protected:
	void* _sender;
};

template<typename T>
class  Property  : public PropertyBase
{
public:
    Property(void* parent)
	: PropertyBase(parent)
	{
	}

	Property(void* parent, const T& value)
	: PropertyBase(parent)
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
		PropertyChanged.send(_sender, this);
		return _value;
	}

public:	
	Pt::Signal<const void*, const PropertyBase&> PropertyChanged;

private:
	T _value;
};

}}
#endif

