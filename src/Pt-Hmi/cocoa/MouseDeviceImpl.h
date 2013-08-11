#ifndef Pt_Hmi_Input_MouseDevievImpl_H
#define Pt_Hmi_Input_MouseDevievImpl_H

#include <Pt/System/Api.h>
#include <Pt/System/IODevice.h>
#include <Pt/System/IOError.h>
#include <Pt/System/EventLoop.h>
#include <Pt/System/Thread.h>
#include <Pt/System/Mutex.h>
#include <Pt/Hmi/MouseDevice.h>
#include <Pt/Hmi/PointingEvent.h>

#include <string>
#include <map>

namespace Pt{
namespace Hmi{

class MouseDeviceImpl : public Pt::Connectable
{
public:
    MouseDeviceImpl(MouseDevice& device);        
    virtual ~MouseDeviceImpl();
	
	void start(Pt::System::EventLoop& loop);
	void stop();
	bool onRun();


private:
	void onInput(Pt::System::IODevice& dev);	
	void mouseCapture();	


private:	
	bool _running;
	int _x;
	int _y;
	int _delta;
	MouseDevice&			_device;	
	PointingEvent			_mouseEvent;
	Pt::System::EventLoop*	_loop;
};

}}

#endif
