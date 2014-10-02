#ifndef Pt_Hmi_Output_Device_H
#define Pt_Hmi_Output_Device_H

#include <Pt/Hmi/Api.h>
#include <Pt/Hmi/Model.h>
#include <Pt/Hmi/Device.h>

namespace Pt{
namespace Hmi{

class PT_HMI_API OutputDevice : public Pt::Hmi::Device
{
public:
	virtual ~OutputDevice()
	{ }

	virtual void output(Pt::Hmi::Controller* controller, Pt::Hmi::Model* model) = 0;

protected:
	OutputDevice()
	{ }
};

}}
#endif