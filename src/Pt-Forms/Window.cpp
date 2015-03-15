#include <Pt/Forms/Window.h>
#include <Pt/Hmi/Window.h>
#include <Pt/Hmi/WindowModel.h>
#include <Pt/Hmi/Application.h>
#include <algorithm>

namespace Pt {
namespace Forms {

Window::Window()
: _defController(_defModel, _defView)
, _currController(0)
{		
	
	Pt::Hmi::Application& app = Pt::Hmi::Application::instance();	

	_defModel.Caption.set("New Window");
	_defModel.ShowInTaskbar.set(true);
	_defModel.Border.set(Hmi::WindowBorderType::Sizeable);
	_defModel.Position.set(Pt::Gfx::PointF(20,20));
	_defModel.Size.set( Pt::Gfx::SizeF(800,800));
	_defController.addOutput(&_defView);

	setWindow(_defController);
}

void Window::setWindow(Hmi::Window& controller)
{
	_currController = &controller;
}


Hmi::Window& Window::window()
{
	return *_currController;
}

void Window::addChild(Widget* w)
{

	window().addChild(&w->widgetController());		
}

Hmi::WindowModel& Window::windowModel()
{
	return _currController->windowModel();
}

const Hmi::Window& Window::window() const
{
	return *_currController;
}

const Hmi::WindowModel& Window::windowModel() const
{
	return _currController->windowModel();
}

void Window::setSize(const Pt::Gfx::SizeF& size)
{
	windowModel().Size = size;
}

const Pt::Gfx::SizeF& Window::size() const
{
	return windowModel().Size.get();
}

void Window::setPosition(const Pt::Gfx::PointF& position)
{
	windowModel().Position = position;
}

const Pt::Gfx::PointF& Window::position() const
{
	return windowModel().Position.get();
}

void Window::show()
{
	windowModel().Closed.set(false);
	windowModel().Visible = true;
	window().invalidate();
}

void Window::close()
{
	window().close();
}

Window::~Window()
{
}
		
}}
