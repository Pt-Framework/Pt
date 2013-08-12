#ifndef Pt_Hmi_Input_Input2DDevice_H
#define Pt_Hmi_Input_Input2DDevice_H

#include <Pt/Hmi/InputDevice.h>
#include <Pt/Hmi/PointingEvent.h>
#include <Pt/Hmi/Controller.h>

namespace Pt{
namespace Hmi{

class PT_HMI_API PointingDevice : public InputDevice
{
public:
	virtual ~PointingDevice()
	{ }

public:
	Pt::Signal<Controller*, const PointingEvent&> Event;

protected:	
	PointingDevice()
	{ }

};

}}
#endif