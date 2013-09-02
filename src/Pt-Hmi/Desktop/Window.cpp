#include <Pt/Hmi/Desktop/Window.h>
#include <Pt/Hmi/WindowController.h>
#include <Pt/Hmi/WindowModel.h>
#include <Pt/Hmi/WindowRenderer.h>
#include <Pt/Hmi/Application.h>

namespace Pt{
namespace Hmi{
namespace Desktop{

Window::Window()
: _defController(new WindowController())
, _defModel( new WindowModel())
, _defRenderer(new WindowRenderer())
{		
	
	Pt::Hmi::Application& app = Pt::Hmi::Application::instance();

	_gfxOutputDevice.start(app.loop());

	_defModel->Caption.set("New Window");
	_defModel->ShowInTaskbar.set(true);
	_defModel->Border.set(WindowBorderType::Sizeable);
	_defModel->WinPos.set(Pt::Gfx::PointF(20,20));
	_defModel->WinSize.set( Pt::Gfx::SizeF(800,800));

	_defController->addInputDevice(&_mouseDevice);
	_defController->addInputDevice(&_keyboardDevice);
	_defController->addOutputDevice(&_gfxOutputDevice);
	_defController->setRenderer(_defRenderer);
	_defController->setModel(_defModel);

	setController(*_defController);
}

WindowController& Window::windowController()
{
	return *((WindowController*)&controller());
}

WindowModel& Window::windowModel()
{
	return *((WindowModel*)windowController().model());
}

const WindowController& Window::windowController() const
{
	return *((WindowController*)&controller());
}

const WindowModel& Window::windowModel() const
{
	return *((WindowModel*)windowController().model());
}

void Window::setSize(const Pt::Gfx::SizeF& size)
{
	windowModel().WinSize = size;
}

const Pt::Gfx::SizeF& Window::size() const
{
	return windowModel().WinSize.get();
}

void Window::setPosition(const Pt::Gfx::PointF& position)
{
	windowModel().WinPos = position;
}

const Pt::Gfx::PointF& Window::position() const
{
	return windowModel().WinPos.get();
}

void Window::show()
{
	Pt::Hmi::WindowModel* m = (Pt::Hmi::WindowModel*) controller().model();
	m->Closed.set(false);
	Widget::show();
}

void Window::close()
{
	Pt::Hmi::WindowController* ctrl = (Pt::Hmi::WindowController*) &controller();
	ctrl->close();
}

Window::~Window()
{
	if(_defController != 0)
		delete _defController;


	if(_defRenderer != 0)
		delete _defRenderer;

	if( _defModel != 0)
		delete _defModel;	
}
		
}}}
