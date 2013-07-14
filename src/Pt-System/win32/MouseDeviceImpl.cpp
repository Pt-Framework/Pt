#include "MouseDeviceImpl.h"
#include <Windowsx.h>

namespace Pt{
namespace System{

MouseDeviceImpl* MouseDeviceImpl::_me = 0;

LRESULT CALLBACK MouseDeviceImpl::mouseProc (int nCode, WPARAM wParam, LPARAM lParam)
{
	return _me->onMouseEvent(nCode, wParam, lParam);
}

LRESULT MouseDeviceImpl::onMouseEvent (int nCode, WPARAM wParam, LPARAM lParam)
{
    MOUSEHOOKSTRUCT * pMouseStruct = (MOUSEHOOKSTRUCT *)lParam;
    
	if (pMouseStruct != NULL)
	{
        switch(wParam)		
        {
			case WM_LBUTTONDOWN:
				_action = Input2DEvent::Press;
				_button = Input2DEvent::LeftButton;
				_modifier = Input2DEvent::LeftButtonDown;
			break;

			case WM_LBUTTONUP:				
				_button = Input2DEvent::LeftButton;
				_action = Input2DEvent::Release;
				_modifier = Input2DEvent::LeftButtonDown;
			break;

			case WM_RBUTTONDOWN:
				_action = Input2DEvent::Press;
				_button = Input2DEvent::RightButton;
				_modifier = Input2DEvent::RightButtonDown;
			break;

			case WM_RBUTTONUP:				
				_button = Input2DEvent::RightButton;
				_action = Input2DEvent::Release;
				_modifier = Input2DEvent::RightButtonDown;
			break;
        }
		
		_x = pMouseStruct->pt.x;
		_y = pMouseStruct->pt.y;


        printf("Mouse position X = %d  Mouse Position Y = %d\n", pMouseStruct->pt.x,pMouseStruct->pt.y);
				
		_device.setReady();
		SetEvent(Pt::System::IODeviceImpl::handle());
		
    }	
	
    return CallNextHookEx(_hDesktopHook, nCode, wParam, lParam);
}

MouseDeviceImpl::MouseDeviceImpl(MouseDevice& device)
: _device(device)
, _button(0)
, _action(Input2DEvent::Action::None)
, _modifier(0)
, _x(0)
, _y(0)
{
	_me = this;	
	_running = true;
	Pt::System::IODeviceImpl::setHandle(CreateEvent(NULL, FALSE,FALSE,NULL));

	_monitorThread  =  new Pt::System::AttachedThread( Pt::callable(*this, &MouseDeviceImpl::mouseCapture) );
	_monitorThread->start();
                           
}

MouseDeviceImpl::~MouseDeviceImpl()
{
	_running = false;
	_monitorThread->join();
	delete _monitorThread;
}


size_t MouseDeviceImpl::beginRead(Pt::System::EventLoop& loop, char* buffer, size_t n, bool& eof)
{
	return 0;	
}

size_t MouseDeviceImpl::endRead(Pt::System::EventLoop& loop, char* buffer, size_t n, bool& eof)
{
	return 0;
}

void MouseDeviceImpl::mouseCapture()
{
    HINSTANCE hInstance = GetModuleHandle(NULL);

    // here I put WH_MOUSE instead of WH_MOUSE_LL
   _hDesktopHook = SetWindowsHookEx( WH_MOUSE_LL, mouseProc, hInstance, NULL );

    MSG message;
    while (GetMessage(&message,NULL,0,0) && _running) {
        TranslateMessage( &message );
        DispatchMessage( &message );
    }

    UnhookWindowsHookEx(_hDesktopHook);
}

void MouseDeviceImpl::onInput(Pt::System::IODevice& dev)
{
	Input2DEvent ev(_button, _action, _modifier, _x, _y);	
	_device.InputEvent.send(ev);
	
}


}}
