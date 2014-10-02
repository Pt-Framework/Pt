#include <Pt/Forms/Dialog.h>
#include <Pt/Hmi/DialogController.h>
#include <Pt/Hmi/DialogModel.h>
#include <Pt/Hmi/DialogRenderer.h>
#include <Pt/Hmi/Application.h>

namespace Pt {
namespace Forms {

Dialog::Dialog()
: _defController(_defModel, _defRenderer)
{
	Pt::Hmi::Application& app = Pt::Hmi::Application::instance();

	_View.start(app.loop());

	_defModel.Caption.set("New Dialog");
	_defModel.ShowInTaskbar.set(true);
	_defModel.Border.set(Hmi::WindowBorderType::Dialog);
	_defModel.Position.set(Pt::Gfx::PointF(20,20));
	_defModel.Size.set( Pt::Gfx::SizeF(800,800));
	_defModel.ShowMaximizeButton.set(false);
	_defModel.ShowSysMenu.set(true);

	_defController.addOutputDevice(&_View);
	setDialogController(_defController);
}

void Dialog::setDialogController(Hmi::DialogController& controller)
{
	_currController = & controller;
	Window::setWindowController(controller);
}

void Dialog::show(Dialog* parent)
{
	show(parent->dialogController());
}

void Dialog::show(Window* parent)
{
	show(parent->windowController());
}

void Dialog::show(Hmi::WindowController& parent)
{
	dialogController().doModal(&parent);	
}

Pt::Hmi::DialogResultType::Type Dialog::result() const
{
	return dialogModel().Result.get();
}

void Dialog::setResult(Hmi::DialogResultType::Type r)
{
	dialogModel().Result = r;
}

Hmi::DialogController& Dialog::dialogController()
{
	return *_currController;
}

Hmi::DialogModel& Dialog::dialogModel()
{
	return _currController->dialogModel();
}

const Hmi::DialogController& Dialog::dialogController() const
{
	return *_currController;
}

const Hmi::DialogModel& Dialog::dialogModel() const
{
	return _currController->dialogModel();
}

void Dialog::setSize(const Pt::Gfx::SizeF& size)
{
	dialogModel().Size = size;
}

const Pt::Gfx::SizeF& Dialog::size() const
{
	return dialogModel().Size.get();
}

void Dialog::setPosition(const Pt::Gfx::PointF& position)
{
	dialogModel().Position = position;
}

const Pt::Gfx::PointF& Dialog::position() const
{
	return dialogModel().Position.get();
}

Dialog::~Dialog()
{

}

}}
