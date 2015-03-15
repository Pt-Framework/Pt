#include <Pt/Hmi/LabelView.h>
#include <Pt/Hmi/LabelModel.h>
#include <Pt/Hmi/Painter.h>
#include <Pt/Hmi/Widget.h>
#include <Pt/Gfx/FontMetrics.h>
#include <Pt/Gfx/ImagePainter.h>
#include <Pt/Gfx/Pen.h>
#include <Pt/Gfx/Brush.h>
#include <Pt/Gfx/Rect.h>

namespace Pt{
namespace Hmi{

LabelView::LabelView()
{
}

LabelView::~LabelView()
{
}

void LabelView::render(Pt::Hmi::Model* m)
{
	LabelModel* model = dynamic_cast<LabelModel*>(m);	
	
	if(model == 0)
		throw std::logic_error("LabelRenderer: LabelModel expected");
	
	if(!model->Visible.get())
		return;	

	Pt::Hmi::Painter& localPainter = paintSurface().painter();	
			
	if(model->AutoSize.get())
	{		
		//Calculate the current and adjust the size
		localPainter.setFont(model->Font.get());
		Pt::Gfx::FontMetrics metric = localPainter.fontMetrics(Pt::String(model->Caption.get().c_str()));
		Pt::Gfx::SizeF currentSize = Pt::Gfx::SizeF(model->toUnit(Pt::Gfx::Size(metric.width(), metric.height())));
		model->Size.set(currentSize);

		//Render the panel	
		PanelView::render(model);

		//Render the label
		Pt::Gfx::PointF	pos(0, currentSize.height()  - metric.ascent() - metric.descent());
		Pt::Gfx::Pen	pen(1, model->ForeColor.get());

		localPainter.setFont(model->Font.get());

		localPainter.setPen(pen);
		localPainter.drawText(pos,Pt::String(model->Caption.get().c_str()));
	}
	else
	{		
		PanelView::render(model);

		switch(model->TextAlign.get())
		{
			case Pt::Hmi::TextAlignType::MidleCenter:
			{
				Pt::Gfx::Pen	pen(1,model->ForeColor.get());
				Pt::Gfx::SizeF	widgetSize =  model->Size.get();
				
				localPainter.setFont(model->Font.get());
				Pt::Gfx::FontMetrics	metric = localPainter.fontMetrics(Pt::String(model->Caption.get().c_str()));
				
				const double widthHalf		= model->Size.get().width()/2;				
				const double heightHalf		= model->Size.get().height()/2;				
				const double textWidthHalf	= metric.width()/2;	
				const double textHeightHalf = metric.height()/2;	
								
				Pt::Gfx::PointF pos(widthHalf - textWidthHalf, (heightHalf - textHeightHalf) +  (metric.height()  - metric.descent()));							
				localPainter.setFont(model->Font.get());
				localPainter.setPen(pen);

				localPainter.drawText(pos,Pt::String(model->Caption.get().c_str()));		
			}
			break;
		}
	}
}

}}
