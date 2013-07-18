#ifndef Pt_Hmi_Device_H
#define Pt_Hmi_Device_H

#include <Pt/System/Selectable.h>
#include <Pt/Hmi/Api.h>

namespace Pt{
namespace Hmi{

class PT_HMI_API Device  : public Pt::System::Selectable
{
protected:
	Device();

public:
	virtual ~Device();

	virtual void start()
	{}

	virtual void start(Pt::System::EventLoop& loop)
	{}

	virtual void stop()
	{}
};

}}
#endif