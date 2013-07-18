#ifndef Pt_Hmi_Input_Input2DDevice_H
#define Pt_Hmi_Input_Input2DDevice_H

#include <Pt/Hmi/InputDevice.h>
#include <Pt/Hmi/Event2D.h>

namespace Pt{
namespace Hmi{

class PT_HMI_API Input2DDevice : public InputDevice
{
public:
	virtual ~Input2DDevice()
	{ }

public:
	Pt::Signal<const Event2D&> Event;

protected:	
	Input2DDevice()
	{ }

};

}}
#endif