#ifndef Pt_Hmi_KeyboardDeviceImpl_H
#define Pt_Hmi_KeyboardDeviceImpl_H

#include <Pt/System/Api.h>
#include <Pt/System/IODevice.h>
#include <Pt/System/IOError.h>
#include <Pt/System/EventLoop.h>
#include <Pt/System/Thread.h>
#include <Pt/System/Mutex.h>
#include <Pt/Hmi/KeyboardDevice.h>
#include <Pt/Hmi/PointingEvent.h>

#include <string>
#include <windows.h>
#include <map>

namespace Pt{
namespace Hmi{

class KeyboardDeviceImpl : public Pt::Connectable
{
public:
    KeyboardDeviceImpl(KeyboardDevice& device);        
    virtual ~KeyboardDeviceImpl();
	
	void start(Pt::System::EventLoop& loop);
	void stop();
	bool onRun();

private:
	void onInput(Pt::System::IODevice& dev);	
	void onKeyEvent (int nCode, WPARAM wParam, LPARAM lParam);

private:	
	bool _running;
	int _x;
	int _y;
	int _delta;
	KeyboardDevice&			_device;	
	KeyEvent				_keyEvent;
	Pt::System::EventLoop*	_loop;
};

}}

#endif
