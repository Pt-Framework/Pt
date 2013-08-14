#ifndef Pt_Hmi_DeviceControllDial_h
#define Pt_Hmi_DeviceControllDial_h

#include <Pt/Hmi/DeviceButton.h>

namespace Pt{
namespace Hmi{

class PT_HMI_API DeviceControlDial : public DeviceButton
{
public:
	enum Direction
	{
		None,
		Left,
		Right
	};

public:
	DeviceControlDial()
	: _direction(None)
	{
	}
	
	virtual ~DeviceControlDial()
	{
	}

	inline Direction direction() const
	{
		return _direction;
	}

	inline void setDirection(Direction d)
	{
		_direction = d;
	}

	inline double delta() const
	{
		return  _delta;
	}

	inline void setDelta(double d)
	{
		_delta= d;
	}

private:
	Direction _direction;
	double _delta;

};

}}
#endif
