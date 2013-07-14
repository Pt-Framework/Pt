#ifndef Pt_System_MouseDevievImpl_H
#define Pt_System_MouseDevievImpl_H

#include "Pt/WinVer.h"
#include "Pt/System/Api.h"
#include "Pt/System/IODevice.h"
#include "Pt/System/IOError.h"
#include <Pt/System/MouseDevice.h>
#include <Pt/System/EventLoop.h>
#include <Pt/System/Input2DEvent.h>
#include <Pt/System/Thread.h>
#include "Selector.h"
#include "IODeviceImpl.h"
#include <string>
#include <windows.h>

namespace Pt{
namespace System{

class MouseDeviceImpl : public Pt::System::IODeviceImpl, public Pt::Connectable
{
public:
    MouseDeviceImpl(MouseDevice& device);        
    virtual ~MouseDeviceImpl();
	
	size_t beginRead(Pt::System::EventLoop& loop, char* buffer, size_t n, bool& eof);    

    size_t endRead(Pt::System::EventLoop& loop, char* buffer, size_t n, bool& eof);

private:
	void onInput(Pt::System::IODevice& dev);	
	void mouseCapture();

	static LRESULT CALLBACK mouseProc(int nCode, WPARAM wParam, LPARAM lParam);
	LRESULT onMouseEvent (int nCode, WPARAM wParam, LPARAM lParam);
	
private:	
	static MouseDeviceImpl* _me;
	HHOOK _hDesktopHook;
	bool _running;
	MouseDevice& _device;
	//state
	int _button;
	Input2DEvent::Action _action;
	int _modifier;
	double _x;
	double _y;	
	Pt::System::AttachedThread* _monitorThread;
};

}}

#endif
