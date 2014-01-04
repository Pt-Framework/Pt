#ifndef Pt_Hmi_Input_Device_H
#define Pt_Hmi_Input_Device_H

#include <Pt/Hmi/Device.h>
#include <Pt/Hmi/Api.h>

namespace Pt{
namespace Hmi{

class PT_HMI_API InputDevice  : public Pt::Hmi::Device
{
protected:
	InputDevice()
	{ }

public:
	virtual ~InputDevice()
	{ }
};

}}

#endif