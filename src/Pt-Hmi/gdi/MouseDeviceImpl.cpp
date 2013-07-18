#include "MouseDeviceImpl.h"
#include <Windowsx.h>
#include <Pt/hmi/Application.h>
#include "ApplicationImpl.h"

namespace Pt{
namespace Hmi{

MouseDeviceImpl::MouseDeviceImpl(MouseDevice& device)
: _device(device)
, _loop(0)
{
	std::vector<DeviceButton>&  buttons = _mouseEvent.buttons();
	
	for( size_t i = 0; i < 3; ++i)
		buttons.push_back(DeviceButton());
		
	std::vector<DeviceControlDial>& controlDials = 	_mouseEvent.controlDial();

	for( size_t i = 0; i < 1; ++i)
		controlDials.push_back(DeviceControlDial());


	Application* app = (Application*) &Application::instance();
	app->impl()->MouseEvent += Pt::slot(*this, &MouseDeviceImpl::onMouseEvent);
}

void MouseDeviceImpl::onMouseEvent (int nCode, WPARAM wParam, LPARAM lParam)
{
    MOUSEHOOKSTRUCT* mouseStruct = (MOUSEHOOKSTRUCT *)lParam;
    
	if (mouseStruct == NULL)
		return;
	
	//Clear the stats
	for( size_t i = 0; i < _mouseEvent.buttons().size(); ++i)
		_mouseEvent.buttons()[i].setState(DeviceButton::None);

	for( size_t i = 0; i < _mouseEvent.controlDial().size(); ++i)
	{
		_mouseEvent.controlDial()[i].setState(DeviceButton::None);
		_mouseEvent.controlDial()[i].setDelta(0);
	}

    switch(wParam)		
    {
		case WM_LBUTTONDOWN:
			_mouseEvent.buttons()[0].setState(DeviceButton::Pressed);			
		break;

		case WM_LBUTTONUP:				
			_mouseEvent.buttons()[0].setState(DeviceButton::Released);
		break;

		case WM_RBUTTONDOWN:
			_mouseEvent.buttons()[1].setState(DeviceButton::Pressed);
		break;

		case WM_RBUTTONUP:				
			_mouseEvent.buttons()[1].setState(DeviceButton::Released);
		break;

		case WM_MOUSEWHEEL:
			_delta = 0;
			//_mouseEvent.controlDial()[0].setDelta(
		break;
    }
		
	_x = mouseStruct->pt.x;
	_y = mouseStruct->pt.y;
		
	_loop->setReady(_device);
	_loop->wake();			    
}


MouseDeviceImpl::~MouseDeviceImpl()
{
	stop();	
}

void MouseDeviceImpl::start(Pt::System::EventLoop& loop)
{
	_loop = &loop;

	_device.setActive(*_loop);
}

void MouseDeviceImpl::stop()
{
	_device.detach();
}

bool MouseDeviceImpl::onRun()
{

	Application& app = *((Application*)&Application::instance());
	
	Pt::Gfx::Point pixPos((size_t)_x,(size_t)_y);

	Pt::Gfx::PointF pos = app.toUnit(pixPos);
	
	_mouseEvent.setX(pos.x());
	_mouseEvent.setY(pos.y());
	_mouseEvent.controlDial()[0].setDelta(app.toUnit(_delta));

	_device.Event.send(_mouseEvent);			
	
	return true;	
}

}}
