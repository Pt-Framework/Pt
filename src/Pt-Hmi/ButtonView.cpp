#include <Pt/Hmi/ButtonView.h>
#include <Pt/Hmi/ButtonModel.h>
#include <Pt/Hmi/Widget.h>
#include <Pt/Hmi/Painter.h>
#include <Pt/Gfx/Pen.h>
#include <Pt/Gfx/ImagePainter.h>

namespace Pt{
namespace Hmi{

ButtonView::ButtonView()
{
}

ButtonView::~ButtonView()
{
}

void ButtonView::render(Pt::Hmi::Model* m)
{	
	ButtonModel* model = dynamic_cast<ButtonModel*>(m);

	if(model == 0)
		throw std::logic_error("ButtonRenderer: expect ButtonModel");

	if(!model->Visible.get())
		return;	
	 
	if(!model->Enabled.get())
	{
		model->ForeColor.set(Pt::Gfx::ARgbColor(0,100,100,100));
		LabelView::render(m);
		return;
	}			

	model->ForeColor.set(Pt::Gfx::ARgbColor(0,0,0,0));
	model->HighLight.set(model->ButtonState.get() == DeviceButton::Pressed);

	LabelView::render(m);
	
	if(model->ButtonState.get() == DeviceButton::Pressed)
		return;

	Pt::Hmi::Painter& localPainter = paintSurface().painter();
    Pt::Gfx::SizeF size = model->Size.get();
       
	if(model->Armed.get() || model->Focused.get())
	{
		Pt::Gfx::SizeF size = model->Size.get();
		size.addHeight(-5);
		size.addWidth(-5);

		Pt::Gfx::ARgbColor armedColor(0,160,160,160);
		 
		Pt::Gfx::Pen pen(1, armedColor, Pt::Gfx::Pen::DashStyle);
		
		localPainter.setPen(pen);		
		Pt::Gfx::RectF rect(Pt::Gfx::PointF(2,2), size);
		localPainter.drawRect(rect);		
	}		
}


}}
