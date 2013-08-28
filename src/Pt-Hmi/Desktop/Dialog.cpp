#include <Pt/Hmi/Desktop/Dialog.h>
#include <Pt/Hmi/DialogController.h>
#include <Pt/Hmi/DialogModel.h>
#include <Pt/Hmi/DialogRenderer.h>
#include <Pt/Hmi/Application.h>

namespace Pt{
namespace Hmi{
namespace Desktop{

Dialog::Dialog()
: _defController(new DialogController())
, _defModel( new DialogModel())
, _defRenderer(new DialogRenderer())
{
	Pt::Hmi::Application& app = Pt::Hmi::Application::instance();

	_gfxOutputDevice.start(app.loop());

	_defModel->Caption.set("New Dialog");
	_defModel->ShowInTaskbar.set(true);
	_defModel->Border.set(WindowBorderType::Dialog);
	_defModel->WinPos.set(Pt::Gfx::PointF(20,20));
	_defModel->WinSize.set( Pt::Gfx::SizeF(800,800));

	_defController->addInputDevice(&_mouseDevice);
	_defController->addInputDevice(&_keyboardDevice);
	_defController->addOutputDevice(&_gfxOutputDevice);
	_defController->setRenderer(_defRenderer);
	_defController->setModel(_defModel);

	setController(*_defController);
}

void Dialog::show(Dialog* parent)
{
	Pt::Hmi::WindowModel* m = (Pt::Hmi::WindowModel*) controller().model();
	m->Closed.set(false);
	Widget::show();
	DialogController* ctrl = (DialogController*) &controller();
	ctrl->modal((Pt::Hmi::WindowController*) &parent->controller());
}

void Dialog::show(Window* parent)
{
	Pt::Hmi::WindowModel* m = (Pt::Hmi::WindowModel*) controller().model();
	m->Closed.set(false);
	Widget::show();
	DialogController* ctrl = (DialogController*) &controller();
	ctrl->modal((Pt::Hmi::WindowController*) &parent->controller());
}

Dialog::~Dialog()
{
	delete _defController;
	delete _defModel;
	delete _defRenderer;
}

Pt::Hmi::DialogResult::Type Dialog::result() const
{
	return Pt::Hmi::DialogResult::OK;
}

}}}
