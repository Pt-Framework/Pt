#include <Pt/Forms/Dialog.h>
#include <Pt/Hmi/Application.h>

namespace Pt {
namespace Forms {

Dialog::Dialog()
: _defController(_defModel, _defView)
{
	Pt::Hmi::Application& app = Pt::Hmi::Application::instance();
	
	_defModel.Caption.set("New Dialog");
	_defModel.ShowInTaskbar.set(true);
	_defModel.Border.set(Hmi::WindowBorderType::Dialog);
	_defModel.Position.set(Pt::Gfx::PointF(20,20));
	_defModel.Size.set( Pt::Gfx::SizeF(800,800));
	_defModel.ShowMaximizeButton.set(false);
	_defModel.ShowSysMenu.set(true);

	_defController.addOutput(&_defView);
	setDialog(_defController);
}

void Dialog::setDialog(Hmi::Dialog& controller)
{
	_currController = & controller;
	Window::setWindow(controller);
}

void Dialog::show(Dialog* parent)
{
	show(parent->dialog());
}

void Dialog::show(Window* parent)
{
	show(parent->window());
}

void Dialog::show(Hmi::Window& parent)
{
	dialog().doModal(&parent);	
}

Pt::Hmi::DialogResultType::Type Dialog::result() const
{
	return dialogModel().Result.get();
}

void Dialog::setResult(Hmi::DialogResultType::Type r)
{
	dialogModel().Result = r;
}

Hmi::Dialog& Dialog::dialog()
{
	return *_currController;
}

Hmi::DialogModel& Dialog::dialogModel()
{
	return _currController->dialogModel();
}

const Hmi::Dialog& Dialog::dialog() const
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
