
#include <Pt/Hmi/ButtonRenderer.h>
#include <Pt/Hmi/ButtonModel.h>
#include <Pt/Hmi/GfxController.h>
#include <Pt/Gfx/Pen.h>

namespace Pt{
namespace Hmi{

ButtonRenderer::ButtonRenderer()
{
}

ButtonRenderer::~ButtonRenderer()
{
}

void ButtonRenderer::render(Pt::Hmi::Model* model ,Pt::Gfx::Painter* painter)
{	
	ButtonModel* bmodel = (ButtonModel*)model;

	if(!bmodel->Visible.get())
		return;

	painter->setFont(bmodel->Font.get());
	if(!bmodel->Enable.get())
	{
		bmodel->ForeColor.set(Pt::Gfx::ARgbColor(0,100,100,100));
		LabelRenderer::render(model,painter);
		return;
	}
	else
	{
		bmodel->ForeColor.set(Pt::Gfx::ARgbColor(0,0,0,0));
		bmodel->Invert3DEffect.set( bmodel->ButtonState.get() == DeviceButton::Pressed);

		LabelRenderer::render(model,painter);		
	}

	if(bmodel->ButtonState.get() == DeviceButton::Pressed)
		return;

	GfxController* ctrl = dynamic_cast<GfxController*>(bmodel->Controller.get());
	
	if( ctrl== 0)
		return;


	if(bmodel->Armed.get() || bmodel->Focused.get())
	{
		Pt::Gfx::Point pos = bmodel->fromUnit(ctrl->fromClient(bmodel->Position.get(), true));
		pos.addX(2);
		pos.addY(2);
		Pt::Gfx::Size size = bmodel->fromUnit(bmodel->Size.get());
		size.addHeight(-4);
		size.addWidth(-4);

		Pt::Gfx::ARgbColor armedColor(0,100,100,100);
		 
		Pt::Gfx::Pen pen(1, armedColor, Pt::Gfx::Pen::DashStyle);
		
		painter->setPen(pen);		
		Pt::Gfx::Rect rect(pos, size);
		painter->drawRect(rect);		
	}

	

	
}


}}
