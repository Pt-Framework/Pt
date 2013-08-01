#include <Pt/Hmi/LabelRenderer.h>
#include <Pt/Hmi/LabelModel.h>
#include <Pt/Gfx/FontMetrics.h>
#include <Pt/Gfx/Pen.h>
#include <Pt/Gfx/Brush.h>
#include <Pt/Hmi/GfxController.h>

namespace Pt{
namespace Hmi{

LabelRenderer::LabelRenderer()
{
}

LabelRenderer::~LabelRenderer()
{
}

void LabelRenderer::render(Pt::Hmi::Model* model ,Pt::Gfx::Painter* painter)
{
	PanelRenderer::render(model, painter);
	
	LabelModel* lmodel = dynamic_cast<LabelModel*>(model);	
	
	if(lmodel == 0)
		return;	
	
	if(!lmodel->Visible.get())
		return;

	GfxController* ctrl = dynamic_cast<GfxController*>(lmodel->Controller.get());
		
	if( ctrl == 0)
		return;

	painter->setFont(lmodel->Font.get());

	if(lmodel->AutoSize.get())
	{
		Pt::Gfx::Pen pen(1,lmodel->ForeColor.get());
		Pt::Gfx::FontMetrics metric = painter->fontMetrics(Pt::String(lmodel->Caption.get()));
		lmodel->Size = Pt::Gfx::SizeF(lmodel->toUnit(Pt::Gfx::Size(metric.width(), metric.height())));
		Pt::Gfx::Point pos = lmodel->fromUnit(ctrl->fromClient(lmodel->Position.get(), true));		

		painter->setPen(pen);

		pos.addY(lmodel->Size.get().height());

		painter->drawText(pos,Pt::String(lmodel->Caption.get()));
	}
	else
	{
		switch(lmodel->TextAlign.get())
		{
			case Pt::Hmi::TextAlign::MidleCenter:
			{
				Pt::Gfx::Pen pen(1,lmodel->ForeColor.get());
				Pt::Gfx::FontMetrics metric = painter->fontMetrics(Pt::String(lmodel->Caption.get()));
				Pt::Gfx::SizeF textSize = Pt::Gfx::SizeF(lmodel->toUnit(Pt::Gfx::Size(metric.width(), metric.height())));
				Pt::Gfx::PointF pos = ctrl->fromClient(lmodel->Position.get(), true);		

				double wWidth2 =  lmodel->Size.get().width()/2.0;				
				double wHeight2 =  lmodel->Size.get().height()/2.0;				
				double tWidth2  = textSize.width()/2.0;	
				double tHeight2  = textSize.height()/2.0;	
				
				pos.addX(wWidth2 - wHeight2);
				pos.addY(wHeight2 - tWidth2);

				
				pos.addY(metric.height());

				painter->setPen(pen);

				painter->drawText(lmodel->fromUnit(pos),Pt::String(lmodel->Caption.get()));		
			}
			break;
		}
	}
}

}}
