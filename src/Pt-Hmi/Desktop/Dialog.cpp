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
	_defModel->ShowMaximizeButton.set(false);
	_defModel->ShowSysMenu.set(true);

	_defController->addInputDevice(&_mouseDevice);
	_defController->addInputDevice(&_keyboardDevice);
	_defController->addOutputDevice(&_gfxOutputDevice);
	_defController->setRenderer(_defRenderer);
	_defController->setModel(_defModel);

	setController(*_defController);
}

void Dialog::show(Dialog* parent)
{
	show((WindowController*) &parent->controller());
}

void Dialog::show(Window* parent)
{
	show((WindowController*) &parent->controller());
}

void Dialog::show(WindowController* parent)
{
	DialogController* myController = (DialogController*) &controller();
	
	myController->doModal(parent);	
}

Pt::Hmi::DialogResultType::Type Dialog::result() const
{
	return dialogModel().Result.get();
}

void Dialog::setResult(DialogResultType::Type r)
{
	dialogModel().Result = r;
}

DialogController& Dialog::dialogController()
{
	return *((DialogController*) & controller());
}

DialogModel& Dialog::dialogModel()
{
	return *((DialogModel*)dialogController().model());
}

const DialogController& Dialog::dialogController() const
{
	return *((DialogController*) & controller());
}

const DialogModel& Dialog::dialogModel() const
{
	return *((DialogModel*)dialogController().model());
}

void Dialog::setSize(const Pt::Gfx::SizeF& size)
{
	dialogModel().WinSize = size;
}

const Pt::Gfx::SizeF& Dialog::size() const
{
	return dialogModel().WinSize.get();
}


void Dialog::setPosition(const Pt::Gfx::PointF& position)
{
	dialogModel().WinPos = position;
}

const Pt::Gfx::PointF& Dialog::position() const
{
	return dialogModel().WinPos.get();
}

Dialog::~Dialog()
{
	delete _defController;
	delete _defModel;
	delete _defRenderer;
}

}}}
