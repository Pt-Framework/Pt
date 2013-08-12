#ifndef Pt_Hmi_KeyboardDevice_H
#define Pt_Hmi_KeyboardDevice_H

#include <Pt/Hmi/InputDevice.h>
#include <Pt/Hmi/KeyEvent.h>
#include <Pt/Hmi/Controller.h>
#include <Pt/System/MainLoop.h>
#include <Pt/Signal.h>

namespace Pt{
namespace Hmi{

class PT_HMI_API KeyboardDevice : public InputDevice
{
public:
	KeyboardDevice();
	virtual ~KeyboardDevice();

	Pt::Signal<Controller*, const KeyEvent&> Event;

protected:
	virtual void onKeyEvent(Controller* source, const KeyEvent& ev)
	{
		Event.send(source, ev);
	}

private:
	void handleKeyEvent(Controller* source, const KeyEvent& ev);
};

}}
#endif
