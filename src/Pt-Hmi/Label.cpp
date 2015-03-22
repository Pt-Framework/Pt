#include <Pt/Hmi/Label.h>
#include <Pt/Gfx/Point.h>
#include <Pt/Gfx/Pen.h>

namespace Pt{
namespace Hmi{

Label::Label()
: Panel()
 , PT_HMI_INIT_PROPERTY_VALUE(AutoSize,true)
{
	Caption.set("Label");
	ForeColor.set(Pt::Gfx::ARgbColor(0,0,0,0));
	BorderStyle.set(BorderStyleType::NoBorder);
}

Label::~Label()
{
}


void Label::onRender()
{
	if(!Visible.get())
		return;		

	Pt::Hmi::Painter& localPainter = paintSurface().painter();	
			
	if(AutoSize.get())
	{		
		//Calculate the current and adjust the size
		localPainter.setFont(Font.get());
		Pt::Gfx::FontMetrics metric = localPainter.fontMetrics(Pt::String(Caption.get().c_str()));
		Pt::Gfx::SizeF currentSize = Pt::Gfx::SizeF(metric.width(), metric.height());
		Size.set(currentSize);

		//Render the panel	
		Panel::onRender();

		//Render the label
		Pt::Gfx::PointF	pos(0, metric.ascent());
		Pt::Gfx::Pen	pen(1, ForeColor.get());

		localPainter.setFont(Font.get());

		localPainter.setPen(pen);
		localPainter.drawText(pos,Pt::String(Caption.get().c_str()));
	}
	else
	{		
		Panel::onRender();

		switch(TextAlign.get())
		{
			case Pt::Hmi::TextAlignType::MidleCenter:
			{
				Pt::Gfx::Pen	pen(1,ForeColor.get());
				Pt::Gfx::SizeF	widgetSize =  Size.get();
				
				localPainter.setFont(Font.get());
				Pt::Gfx::FontMetrics	metric = localPainter.fontMetrics(Pt::String(Caption.get().c_str()));
				
				const double widthHalf		= Size.get().width()/2;				
				const double heightHalf		= Size.get().height()/2;				
				const double textWidthHalf	= metric.width()/2;	
				const double textHeightHalf = metric.height()/2;	
								
				Pt::Gfx::PointF pos(widthHalf - textWidthHalf, (heightHalf - textHeightHalf) + metric.ascent());							
				localPainter.setFont(Font.get());
				localPainter.setPen(pen);

				localPainter.drawText(pos,Pt::String(Caption.get().c_str()));		
			}
			break;
		}
	}
}

}}