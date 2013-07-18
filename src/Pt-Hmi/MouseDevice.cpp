#include <Pt/Hmi/MouseDevice.h>
#include "MouseDeviceImpl.h"

namespace Pt{
namespace Hmi{

MouseDevice::MouseDevice()
: _impl(new MouseDeviceImpl(getMe()))
{
}

MouseDevice::~MouseDevice()
{
}

void MouseDevice::start(Pt::System::EventLoop& loop)
{
	_impl->start(loop);
}

void MouseDevice::stop()
{
	_impl->stop();
}

void MouseDevice::onCancel()
{	
}

bool MouseDevice::onRun()
{
	return _impl->onRun();
}

}}
