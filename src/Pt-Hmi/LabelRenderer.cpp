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

	GfxController* controller = dynamic_cast<GfxController*>(m->controller());
			
	if(model->AutoSize.get())
	{
		Pt::Hmi::Painter localPainter(model->PaintBuffer);				

		localPainter.setFont(model->Font.get());
		
		Pt::Gfx::Pen			pen(1,model->ForeColor.get());
		Pt::Gfx::FontMetrics	metric = localPainter.fontMetrics(Pt::String(model->Caption.get().c_str()));
		
		model->Size = Pt::Gfx::SizeF(model->toUnit(Pt::Gfx::Size(metric.width(), metric.height())));
		
		Pt::Gfx::Size	size = model->fromUnit(model->Size.get());
		Pt::Gfx::Point	pos(0, size.height() - metric.descent());

		localPainter.setPen(pen);
		localPainter.drawText(pos,Pt::String(model->Caption.get().c_str()));
	}
	else
	{		
		Pt::Hmi::Painter localPainter(model->PaintBuffer);

		switch(model->TextAlign.get())
		{
			case Pt::Hmi::TextAlignType::MidleCenter:
			{
				Pt::Gfx::Pen	pen(1,model->ForeColor.get());
				Pt::Gfx::Size	widgetSize =  model->fromUnit(model->Size.get());
				localPainter.setFont(model->Font.get());
				Pt::Gfx::FontMetrics	metric = localPainter.fontMetrics(Pt::String(model->Caption.get().c_str()));
				
				const int widthHalf		 = model->Size.get().width()/2;				
				const int heightHalf	 = model->Size.get().height()/2;				
				const int textWidthHalf	 = metric.width()/2;	
				const int textHeightHalf = metric.height()/2;	
								

				Pt::Gfx::Point pos(widthHalf - textWidthHalf, (heightHalf - textHeightHalf) +  (metric.height()  - metric.descent()));							
				localPainter.setFont(model->Font.get());
				localPainter.setPen(pen);

				localPainter.drawText(pos,Pt::String(model->Caption.get().c_str()));		
			}
			break;
		}
	}
}

}}
