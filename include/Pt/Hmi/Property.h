#ifndef Pt_Hmi_Base_Property_h
#define Pt_Hmi_Base_Property_h


#include <Pt/Signal.h>

namespace Pt {
namespace Hmi {

template<typename T>
class  Property 
{
public:
    Property()
	{
	}

	Property(const T& value)
	: _value(value)
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
		if(_value != value)
		{
			_value = value;
			PropertyChanged.send(_value);
		}
		return _value;
	}

public:	
	Pt::Signal<T&> PropertyChanged;

private:
	T _value;
};

}}
#endif

