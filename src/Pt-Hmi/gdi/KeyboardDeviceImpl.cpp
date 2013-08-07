#include "KeyboardDeviceImpl.h"
#include <Windowsx.h>
#include <Pt/hmi/Application.h>
#include "ApplicationImpl.h"

namespace Pt{
namespace Hmi{

KeyboardDeviceImpl::KeyboardDeviceImpl(KeyboardDevice& device)
: _device(device)
, _loop(0)
{
	
	Application* app = (Application*) &Application::instance();
	app->impl()->KeyBoardEvent += Pt::slot(*this, &KeyboardDeviceImpl::onKeyEvent);
}

void KeyboardDeviceImpl::onKeyEvent (int nCode, WPARAM wParam, LPARAM lParam)
{ 
	if(lParam & 0x80000000)
		_keyEvent.setState(KeyEvent::KeyUp);
	else
		_keyEvent.setState(KeyEvent::KeyDown);
	
	_keyEvent.setVirtualCode(wParam);
	_keyEvent.setRepeatCount((int)(lParam & 0xFFFF));
	_keyEvent.setScancode((int)((lParam & 0x7F0000)>>16));
	_keyEvent.setExtCode( (lParam & 0x1000000) != 0);
	_keyEvent.setAlt( (lParam & 0x20000000) != 0); 
		
	if(_keyEvent.virtualCode() == 16 )
	{
		_keyEvent.setShift(_keyEvent.state() == KeyEvent::KeyDown);
	}
			
	if(_keyEvent.virtualCode() == 17 )
	{
		_keyEvent.setCtrl(_keyEvent.state() == KeyEvent::KeyDown);
	}

	_loop->setReady(_device);
	_loop->wake();			    
}


KeyboardDeviceImpl::~KeyboardDeviceImpl()
{
	stop();	
}

void KeyboardDeviceImpl::start(Pt::System::EventLoop& loop)
{
	_loop = &loop;

	_device.setActive(*_loop);
}

void KeyboardDeviceImpl::stop()
{
	_device.detach();
}

bool KeyboardDeviceImpl::onRun()
{

	Application& app = *((Application*)&Application::instance());
	
	Pt::Gfx::Point pixPos((size_t)_x,(size_t)_y);

	Pt::Gfx::PointF pos = app.toUnit(pixPos);
	
	_device.Event.send(_keyEvent);			
	
	return true;	
}

}}
