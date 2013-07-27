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

	_defController->setModel(_defModel);
	_defController->setRenderer(_defRenderer);
	_defController->addInputDevice(&_mouseDevice);
	_defController->addOutputDevice(&_gfxOutputDevice);
	
	_mouseDevice.start(app.loop());
	_gfxOutputDevice.start(app.loop());

	setController(*_defController);
}

void Window::show()
{
	GfxModel* m = dynamic_cast<GfxModel*>(controller().model());
	m->Visible = true;
	controller().invalidate();
}

void Window::hide()
{
	GfxModel* m = dynamic_cast<GfxModel*>(controller().model());
	m->Visible = false;
	controller().invalidate();	
}


Window::~Window()
{
	delete _defController;
	delete _defModel;
	delete _defRenderer;
}
		
}}}
