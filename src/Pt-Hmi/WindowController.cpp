#include <Pt/Hmi/WindowController.h>
#include <Pt/Hmi/PanelController.h>
#include <Pt/Hmi/Input2DDevice.h>
#include <Pt/Hmi/WindowModel.h>
#include <assert.h>

namespace Pt{
namespace Hmi{

WindowController::WindowController()
{
}

WindowController::~WindowController()
{
}

WidgetController* WindowController::mainWidget()
{
	if(Controller::children().size() != 0)
		return dynamic_cast<WidgetController*>(Controller::children()[0]);

	return 0;
}	

void WindowController::onInput2D(const Event2D& ev)
{
	WindowModel* model = (WindowModel*) Controller::model();
	
	Pt::Gfx::PointF pos(ev.x(),ev.y());

	if( model->contains(pos))
	{
		const DeviceButton& leftButton = ev.buttons().at(0);

		switch( leftButton.state())
		{
			case DeviceButton::Pressed:
				model->Pinned = true;
				model->PinPosition = pos;
			break;

			case DeviceButton::Released:
				if(model->Pinned.get())
					model->Pinned = false;		
			break;
		}
		
		if(model->Pinned.get())
		{
			Pt::Gfx::PointF delta = pos - model->PinPosition.get();

			model->move(Pt::Gfx::SizeF(delta.x(),delta.y()));
			model->Pinned = true;
			model->PinPosition  =  pos;
		}
		
		model->Changed.send();		
	}	
}

void WindowController::start()
{
	assert(model() != 0);

	const std::vector<InputDevice*>& devices = inputDevices();

	for( size_t i = 0; i < devices.size(); ++i)
	{
		if( Input2DDevice* inputSource = dynamic_cast<Input2DDevice*>( devices[i]))		
			inputSource->Event += Pt::slot(*this, &WindowController::onInput2D);		
	}

	model()->Changed.send();		
}

void WindowController::onModelChanged()
{
	Pt::Hmi::GfxModel* myModel = dynamic_cast<Pt::Hmi::GfxModel*>(model());
/*
	Pt::Gfx::ARgbImage& image = *(myModel->ImagePtr.get());
	
	Pt::Gfx::Size size = myModel->fromUnit(myModel->Size.get());

	image.resize(size.width(), size.height());

	for(size_t i = 0; i < children().size(); ++i)
	{
		Controller* child = children()[i];
		Pt::Hmi::GfxModel* gfx = dynamic_cast<Pt::Hmi::GfxModel*>(child->model());

		if(gfx != 0)
		{
			Pt::Hmi::GfxModel* myGfx = (Pt::Hmi::GfxModel*)model();
			gfx->ImagePtr= myGfx->ImagePtr.get();
		}

		if(child->renderer() != 0)
			child->renderer()->render(child->model());
		
		for(size_t j = 0; j < child->outputDevices().size(); ++i)
		{
			OutputDevice* dev = child->outputDevices()[j];
			dev->output(child->model());
		}
	}

	const std::vector<OutputDevice*>& devices = outputDevices();

	for( size_t i = 0; i < devices.size(); ++i)
		devices[i]->output(model());
*/
}

void WindowController::stop()
{
}

}}
