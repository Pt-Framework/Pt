#include <Pt/Hmi/KeyboardDevice.h>

#include "KeyboardDeviceImpl.h"

namespace Pt{
namespace Hmi{

KeyboardDevice::KeyboardDevice()
: _impl( new KeyboardDeviceImpl(getMe()))
{

}

KeyboardDevice::~KeyboardDevice()
{	
	delete _impl;
}
	
void KeyboardDevice::start(Pt::System::EventLoop& loop)
{
	_impl->start(loop);
}

void KeyboardDevice::stop()
{
	_impl->stop();
}

void KeyboardDevice::onCancel()
{	
}

bool KeyboardDevice::onRun()
{
	return _impl->onRun();
}	

}}
