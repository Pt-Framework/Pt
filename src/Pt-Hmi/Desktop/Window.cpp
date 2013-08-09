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

	_mouseDevice.start(app.loop());
	_keyboardDevice.start(app.loop());
	_gfxOutputDevice.start(app.loop());

	_defModel->Caption.set("New Window");
	_defModel->ShowInTaskbar.set(true);
	_defModel->Border.set(BorderStyle::Sizebale);
	_defModel->WinPos.set(Pt::Gfx::PointF(20,20));
	_defModel->WinSize.set( Pt::Gfx::SizeF(800,800));


	_defController->addInputDevice(&_mouseDevice);
	_defController->addInputDevice(&_keyboardDevice);
	_defController->addOutputDevice(&_gfxOutputDevice);
	_defController->setRenderer(_defRenderer);
	_defController->setModel(_defModel);

	setController(*_defController);
}

void Window::show()
{
	Pt::Hmi::WindowModel* m = (Pt::Hmi::WindowModel*) controller().model();
	m->Closed = false;
	Widget::show();
}

void Window::close()
{
	Pt::Hmi::WindowController* ctrl = (Pt::Hmi::WindowController*) &controller();
	ctrl->close();
}

Window::~Window()
{
	delete _defController;
	delete _defRenderer;
	delete _defModel;
	
}
		
}}}
