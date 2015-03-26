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
  Name.set("Label");
	ForeColor.set(Pt::Gfx::ARgbColor(0,0,0,0));
	PanelBorderStyle.set(BorderStyle::NoBorder);
}

Label::~Label()
{
}


void Label::onRender()
{
	if(!Visible.get())
		return;		

	Pt::Hmi::Painter& localPainter = paintSurface().painter();	
	Pt::Gfx::PointF pos;

	Pt::String caption;

	if( UseMnemonic.get() )
		caption = Widget::removeMnemonic(Caption.get()).c_str();
	else
		caption = Caption.get().c_str();

	if(AutoSize.get())
	{		
		//Calculate the current and adjust the size
		localPainter.setFont(Font.get());
		Pt::Gfx::FontMetrics metric = localPainter.fontMetrics(caption);
		Pt::Gfx::SizeF currentSize = Pt::Gfx::SizeF(metric.width(), metric.height());
		Size.set(currentSize);

		//Render the panel	
		Panel::onRender();

		pos = Pt::Gfx::PointF(0, metric.ascent());
	}
	else
	{		
		Panel::onRender();

		switch(TextAlign.get())
		{
			case Pt::Hmi::Align::MidleCenter:
			{
				Pt::Gfx::Pen	pen(1,ForeColor.get());
				Pt::Gfx::SizeF	widgetSize =  Size.get();
				
				localPainter.setFont(Font.get());
				Pt::Gfx::FontMetrics	metric = localPainter.fontMetrics(caption);
				
				const double widthHalf		= Size.get().width()/2;				
				const double heightHalf		= Size.get().height()/2;				
				const double textWidthHalf	= metric.width()/2;	
				const double textHeightHalf = metric.height()/2;	
								
				pos = Pt::Gfx::PointF(widthHalf - textWidthHalf, (heightHalf - textHeightHalf) + metric.ascent());							
			}
			break;
		}
	}

	Pt::Gfx::Pen	pen(1, ForeColor.get());

	localPainter.setFont(Font.get());
	localPainter.setPen(pen);

	localPainter.drawText(pos, caption);

	if( UseMnemonic.get() )
	{			
		int index = Widget::getMnemonicIndex( Caption.get() );
		
		if( index != std::string::npos  && ((index + 1) < caption.size()) )
		{	
			std::string subString(  caption.begin(), caption.begin() + index );

			Pt::Gfx::FontMetrics metric = localPainter.fontMetrics( Pt::String( subString.c_str() ) );
	
			Gfx::PointF linePos( pos.x() + metric.width() - 1, pos.y()  + 1);

			subString = caption[index];

			metric = localPainter.fontMetrics( Pt::String( subString.c_str() ) );

			localPainter.drawLine( linePos , Pt::Gfx::PointF( linePos.x() + metric.width(), linePos.y() ) );
		}
	}
}

}}