#ifndef Pt_Hmi_PointingEvent_h
#define Pt_Hmi_PointingEvent_h

#include <Pt/Hmi/Api.h>
#include <Pt/Hmi/DeviceButton.h>
#include <Pt/Hmi/DeviceControlDial.h>
#include <Pt/Hmi/Event.h>
#include <vector>

namespace Pt{
namespace Hmi{

class PT_HMI_API PointingEvent : public Pt::BasicEvent<PointingEvent>
{
public:	
	explicit PointingEvent()
	{
	}

	virtual ~PointingEvent()
	{
	}

	inline void setX(double x)
	{
		_x = x;
	}
	
	inline void setY(double y)
	{
		_y = y;
	}

	inline void addX(double x)
	{
		_x += x;
	}
	
	inline void addY(double y)
	{
		_y += y;
	}

	inline double x() const
	{
		return _x;
	}

	inline double y() const
	{
		return _y;
	}

	inline const std::vector<DeviceButton>& buttons() const
	{
		return _buttons;
	}

	inline const std::vector<DeviceControlDial>& controlDial() const 
	{
		return _controlDial;
	}

	inline std::vector<DeviceButton>& buttons()
	{
		return _buttons;
	}

	inline std::vector<DeviceControlDial>& controlDial()
	{
		return _controlDial;
	}	

private:
	double _x;
	double _y;
	std::vector<DeviceButton>	  _buttons;
	std::vector<DeviceControlDial> _controlDial;	
};

}}

#endif

