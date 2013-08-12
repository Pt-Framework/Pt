#include <Pt/Hmi/KeyboardDevice.h>
#include <Pt/Hmi/Application.h>

namespace Pt{
namespace Hmi{

KeyboardDevice::KeyboardDevice()
{
	Application::instance().keyDeviceEvent() += Pt::slot(*this, &KeyboardDevice::handleKeyEvent);	
}

KeyboardDevice::~KeyboardDevice()
{		
}

void KeyboardDevice::handleKeyEvent(Controller* source, const KeyEvent& ev)
{
	onKeyEvent(source, ev);
}

}}
