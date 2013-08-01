#ifndef Pt_Hmi_Input_Event2D_h
#define Pt_Hmi_Input_Event2D_h

#include <Pt/Hmi/Api.h>
#include <Pt/Event.h>
#include <Pt/Allocator.h>
#include <cstddef>
#include <Pt/Hmi/DeviceButton.h>
#include <Pt/Hmi/DeviceControlDial.h>
#include <vector>

namespace Pt{
namespace Hmi{

class PT_HMI_API PointingEvent : public Pt::Event
{

public:	
	PointingEvent();
	PointingEvent(const PointingEvent& copy);
	virtual ~PointingEvent();

	bool operator==(const PointingEvent& cmp)
	{
		return false;
	}
	
	bool operator!=(const PointingEvent& cmp)
	{
		return true;
	}

	inline void setX(double x)
	{
		_x = x;
	}
	
	inline void setY(double y)
	{
		_y = y;
	}

	inline double x() const
	{
		return _x;
	}

	inline double y() const
	{
		return _y;
	}

	const inline std::vector<DeviceButton>& buttons() const
	{
		return _buttons;
	}

	const inline std::vector<DeviceControlDial>& controlDial() const 
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


	void operator =(const PointingEvent& copy);

protected:
    virtual Pt::Event& onClone(Pt::Allocator& allocator) const;
    virtual void onDestroy(Pt::Allocator& allocator);
    virtual const std::type_info& onTypeInfo() const;

private:
	double _x;
	double _y;
	std::vector<DeviceButton>	  _buttons;
	std::vector<DeviceControlDial> _controlDial;
};

}}

#endif

