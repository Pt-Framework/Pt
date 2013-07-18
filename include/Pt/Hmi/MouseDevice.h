#ifndef Pt_Hmi_Input_Mouse_H
#define Pt_Hmi_Input_Mouse_H

#include <Pt/Hmi/Input2DDevice.h>
#include <Pt/System/MainLoop.h>
#include <Pt/Singleton.h>

namespace Pt{
namespace Hmi{

class MouseDeviceImpl;

class PT_HMI_API MouseDevice : public Input2DDevice
{

public:
	MouseDevice();
	virtual ~MouseDevice();
	
	void start(Pt::System::EventLoop& loop);
	void stop();

protected:	
    virtual void onCancel();
    virtual bool onRun();
	
private:
	MouseDevice& getMe()
	{
		return *this;
	}

	MouseDeviceImpl* _impl;
};

}}
#endif
