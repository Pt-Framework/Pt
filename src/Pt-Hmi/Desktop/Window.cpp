#include <Pt/Hmi/Desktop/Window.h>
#include <Pt/Hmi/WindowController.h>
#include <Pt/Hmi/WindowModel.h>
#include <Pt/Hmi/WindowRenderer.h>
#include <Pt/Hmi/Application.h>
#include <algorithm>

namespace Pt{
namespace Hmi{
namespace Desktop{

Window::Window()
: _defController(_defModel, _defRenderer)
, _currController(0)
{		
	
	Pt::Hmi::Application& app = Pt::Hmi::Application::instance();

	_gfxOutputDevice.start(app.loop());

	_defModel.Caption.set("New Window");
	_defModel.ShowInTaskbar.set(true);
	_defModel.Border.set(WindowBorderType::Sizeable);
	_defModel.Position.set(Pt::Gfx::PointF(20,20));
	_defModel.Size.set( Pt::Gfx::SizeF(800,800));

	_defController.addInputDevice(&_mouseDevice);
	_defController.addInputDevice(&_keyboardDevice);
	_defController.addOutputDevice(&_gfxOutputDevice);

	setWindowController(_defController);
}

void Window::setWindowController(WindowController& controller)
{
	_currController = &controller;
}


WindowController& Window::windowController()
{
	return *_currController;
}

void Window::addChild(Widget* w)
{

	windowController().addChild(&w->widgetController());		
}

WindowModel& Window::windowModel()
{
	return _currController->windowModel();
}

const WindowController& Window::windowController() const
{
	return *_currController;
}

const WindowModel& Window::windowModel() const
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
	windowController().invalidate();
}

void Window::close()
{
	windowController().close();
}

Window::~Window()
{
}
		
}}}
