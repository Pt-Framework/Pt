#include <Pt/Hmi/Desktop/Window.h>
#include <Pt/Hmi/Model/Window.h>
#include <Pt/Hmi/Output/Gfx.h>
#include <Pt/Hmi/Input/Mouse.h>
#include <Pt/Hmi/Renderer/Window.h>


namespace Pt{
namespace Hmi{
namespace Desktop{

Window::Window()
:_controller(0)
{		
	Pt::Hmi::Controller::Windows* ctrl = new Pt::Hmi::Controller::Windows();
	ctrl.setModel(new Pt::Hmi::Model::Window());
	ctrl.addInputDevice(new Pt::Hmi::Input::Mouse());
	ctrl.addOutputDevice(new Pt::Hmi::Output::Gfx());
	ctrl.setRenderer(new  Pt::Hmi::Renderer::Window());

	setController(ctrl)
}

void setController(Pt::Hmi::Controller::Windows* controller)
{
	if(_controller != 0)
		delete _controller;

	_controller = controller;	
	_controller->start();
}

Window::~Window()
{
	delete _controller;
}
		
}}}
