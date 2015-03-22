#ifndef Pt_Hmi_Event_h
#define Pt_Hmi_Event_h

#include <Pt/Hmi/Api.h>
#include <Pt/Event.h>

namespace Pt{
namespace Hmi{

class Window;

class PT_HMI_API Event : public Pt::Event
{
public:	
	Event(Window* controller)
	: _controller(controller)
	{
	}	

	inline void setController(Window* controller)
	{
		_controller = controller;
	}

	inline Window* controller()
	{
		return _controller;
	}

	inline const  Window* controller() const
	{
		return _controller;
	}
		
private:
	Window* _controller;	
};

}}

#endif

