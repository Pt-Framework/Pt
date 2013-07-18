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

class PT_HMI_API Event2D : public Pt::Event
{

public:	
	Event2D();
	Event2D(const Event2D& copy);
	virtual ~Event2D();


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

