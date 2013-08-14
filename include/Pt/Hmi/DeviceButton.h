#ifndef Pt_Hmi_DeviceButton_h
#define Pt_Hmi_DeviceButton_h

#include <Pt/Hmi/Api.h>

namespace Pt{
namespace Hmi{

class PT_HMI_API DeviceButton
{

public:
	enum State 
	{
		Released,
		Pressed
	};


	DeviceButton()
	: _state(Released)
	{
	}
	
	virtual ~DeviceButton()
	{
	}

	inline State state() const
	{
		return _state;
	}

	inline void setState(State s)
	{
		_state = s;
	}

private:
	State _state;
};

}}

#endif
