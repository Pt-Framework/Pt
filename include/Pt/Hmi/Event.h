#ifndef Pt_Hmi_Event_h
#define Pt_Hmi_Event_h

#include <Pt/Hmi/Api.h>
#include <Pt/Event.h>

namespace Pt{
namespace Hmi{

class Controller;

class PT_HMI_API Event : public Pt::Event
{
public:	
	Event(Controller* controller)
	: _controller(controller)
	{
	}	

	inline void setController(Controller* controller)
	{
		_controller = controller;
	}

	inline Controller* controller()
	{
		return _controller;
	}

	inline const  Controller* controller() const
	{
		return _controller;
	}
		
private:
	Controller* _controller;	
};

}}

#endif

