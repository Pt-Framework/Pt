
#include <Pt/Hmi/ButtonRenderer.h>
#include <Pt/Hmi/ButtonModel.h>
#include <Pt/Hmi/GfxController.h>
#include <Pt/Gfx/Pen.h>
#include <Pt/Gfx/ImagePainter.h>

namespace Pt{
namespace Hmi{

ButtonRenderer::ButtonRenderer()
{
}

ButtonRenderer::~ButtonRenderer()
{
}

void ButtonRenderer::render(Pt::Hmi::Model* m)
{	
	ButtonModel* model = dynamic_cast<ButtonModel*>(m);

	if(!model->Visible.get())
		return;	
	 
	if(!model->Enable.get())
	{
		model->ForeColor.set(Pt::Gfx::ARgbColor(0,100,100,100));
		LabelRenderer::render(m);
		return;
	}			

	model->ForeColor.set(Pt::Gfx::ARgbColor(0,0,0,0));
	model->Invert3DEffect.set(model->ButtonState.get() == DeviceButton::Pressed);

	LabelRenderer::render(m);
	
	if(model->ButtonState.get() == DeviceButton::Pressed)
		return;

	GfxController* ctrl = dynamic_cast<GfxController*>(model->Controller.get());
	
	if( ctrl== 0)
		return;

	Pt::Gfx::ImagePainter localPainter(model->PaintBuffer);

	if(model->Armed.get() || model->Focused.get())
	{
		Pt::Gfx::Size size = model->fromUnit(model->Size.get());
		size.addHeight(-5);
		size.addWidth(-5);

		Pt::Gfx::ARgbColor armedColor(0,160,160,160);
		 
		Pt::Gfx::Pen pen(1, armedColor, Pt::Gfx::Pen::DashStyle);
		
		localPainter.setPen(pen);		
		Pt::Gfx::Rect rect(Pt::Gfx::Point(2,2), size);
		localPainter.drawRect(rect);		
	}		
}


}}
