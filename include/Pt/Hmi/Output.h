#ifndef Pt_Hmi_Output_Device_H
#define Pt_Hmi_Output_Device_H

#include <Pt/Hmi/Api.h>
#include <Pt/Hmi/Model.h>

namespace Pt{
namespace Hmi{

class PT_HMI_API Output
{
public:
	virtual ~Output()
	{ }

	virtual void output(Pt::Hmi::Model* model) = 0;

protected:
	Output()
	{ }
	
};

}}
#endif