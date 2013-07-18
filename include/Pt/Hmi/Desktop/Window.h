#ifndef Pt_Hmi_Desktop_Window_H
#define Pt_Hmi_Desktop_Window_H

#include <Pt/Hmi/Controller/Window.h>
#include <Pt/Hmi/Property.h>
#include <Pt/Hmi/Model/Window.h>

namespace Pt{
namespace Hmi{
namespace Desktop{

class PT_HMI_DESKTOP_API Window 
{
public:
	Window();
	virtual ~Window();

	inline Pt::Hmi::Controller::Windows& controller()
	{
		return  *_controller;
	}
	
	
	inline Pt::Hmi::Controller::Windows& controller()
	{
		return  *_controller;
	}
	
	inline setController(Pt::Hmi::Controller::Windows* controller)
	{
		delete _controller;
		_controller = controller;
	}

	inline Pt::Hmi::Model::Window& model()
	{
		Pt::Hmi::Model::Window* winMod = dynamic_cast<Pt::Hmi::Model::Window*>(_controller->model())
		return *winMod;
	}

private:
	Pt::Hmi::Controller::Windows* _controller;
};

}}}
#endif