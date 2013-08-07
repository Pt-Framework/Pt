#include <Pt/Hmi/LabelRenderer.h>
#include <Pt/Hmi/LabelModel.h>
#include <Pt/Gfx/FontMetrics.h>
#include <Pt/Gfx/ImagePainter.h>
#include <Pt/Gfx/Pen.h>
#include <Pt/Gfx/Brush.h>
#include <Pt/Gfx/Rect.h>
#include <Pt/Hmi/GfxController.h>

namespace Pt{
namespace Hmi{

LabelRenderer::LabelRenderer()
{
}

LabelRenderer::~LabelRenderer()
{
}

void LabelRenderer::render(Pt::Hmi::Model* m)
{
	PanelRenderer::render(m);

	LabelModel* model = dynamic_cast<LabelModel*>(m);	
	
	if(model == 0)
		return;	
	
	if(!model->Visible.get())
		return;	

	GfxController* controller = dynamic_cast<GfxController*>(m->Controller.get());
			
	if(model->AutoSize.get())
	{
		Pt::Gfx::ImagePainter localPainter(model->PaintBuffer);				

		localPainter.setFont(model->Font.get());
		
		Pt::Gfx::Pen			pen(1,model->ForeColor.get());
		Pt::Gfx::FontMetrics	metric = localPainter.fontMetrics(Pt::String(model->Caption.get()));
		
		model->Size = Pt::Gfx::SizeF(model->toUnit(Pt::Gfx::Size(metric.width(), metric.height())));
		
		Pt::Gfx::Size	size = model->fromUnit(model->Size.get());
		Pt::Gfx::Point	pos(0, size.height() - metric.descent());

		localPainter.setPen(pen);
		localPainter.drawText(pos,Pt::String(model->Caption.get()));
	}
	else
	{		
		Pt::Gfx::ImagePainter localPainter(model->PaintBuffer);

		switch(model->TextAlign.get())
		{
			case Pt::Hmi::TextAlign::MidleCenter:
			{
				Pt::Gfx::Pen			pen(1,model->ForeColor.get());
				Pt::Gfx::FontMetrics	metric = localPainter.fontMetrics(Pt::String(model->Caption.get()));
				Pt::Gfx::SizeF			textSize = Pt::Gfx::SizeF(model->toUnit(Pt::Gfx::Size(metric.width(), metric.height())));
				
				const double widthHalf		= model->Size.get().width()/2.0;				
				const double heightHalf		= model->Size.get().height()/2.0;				
				const double textWidthHalf	= textSize.width()/2.0;	
				const double textHeightHalf	= textSize.height()/2.0;	
				
				Pt::Gfx::PointF pos(widthHalf - textWidthHalf, heightHalf - textHeightHalf);							
				pos.addY(metric.height() - metric.descent());				

				localPainter.setFont(model->Font.get());
				localPainter.setPen(pen);

				localPainter.drawText(model->fromUnit(pos),Pt::String(model->Caption.get()));		
			}
			break;
		}
	}
}

}}
