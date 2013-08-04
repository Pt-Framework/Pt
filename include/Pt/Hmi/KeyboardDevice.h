#ifndef Pt_Hmi_KeyboardDevice_H
#define Pt_Hmi_KeyboardDevice_H

#include <Pt/Hmi/InputDevice.h>
#include <Pt/Hmi/KeyEvent.h>
#include <Pt/System/MainLoop.h>
#include <Pt/Singleton.h>

namespace Pt{
namespace Hmi{

class KeyboardDeviceImpl;

class PT_HMI_API KeyboardDevice : public InputDevice
{
public:
	KeyboardDevice();
	virtual ~KeyboardDevice();
	
	void start(Pt::System::EventLoop& loop);
	void stop();


	Pt::Signal<const KeyEvent&> Event;

protected:	
    virtual void onCancel();
    virtual bool onRun();
	
private:
	KeyboardDevice& getMe()
	{
		return *this;
	}

	KeyboardDeviceImpl* _impl;
};

}}
#endif
