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

	Pt::Hmi::Painter&   localPainter = paintSurface().painter();	
	Pt::Gfx::PointF     pos;
	Pt::String          caption;
  Gfx::ARgbColor      foreColor = Enabled.get() ? ForeColor.get() : DisabledColor.get();
  Pt::Gfx::Pen	      pen( 1, foreColor);
  Pt::Gfx::SizeF	    size = paintSurface().size();
  
	if( UseMnemonic.get() )
		caption = Widget::removeMnemonic(Caption.get()).c_str();
	else
		caption = Caption.get().c_str();

	if( AutoSize.get() )
	{		
		//Calculate the current and adjust the size
		localPainter.setFont(Font.get());
		Pt::Gfx::FontMetrics metric = localPainter.fontMetrics(caption);

		Pt::Gfx::SizeF currentSize = Pt::Gfx::SizeF( metric.width() + Margin.get().left() + Margin.get().right() , 
                                                 metric.height() + Margin.get().top() +  Margin.get().bottom() );
    
    Visible.set(false); //No invalidate event.
		Size = currentSize;
    Visible.set(true);

		//Render the panel	
		Panel::onRender();

		pos = Pt::Gfx::PointF(0, metric.ascent());
	}
	else
	{		
		Panel::onRender();
							
		Pt::Gfx::FontMetrics	metric = localPainter.fontMetrics(caption);
    
    localPainter.setFont(Font.get());		

		switch(TextAlign.get())
		{
      case Hmi::Align::TopLeft:
      {
			  pos = Pt::Gfx::PointF( 0, metric.ascent() );			
      }
      break;

		  case Hmi::Align::TopCenter:
      {
        const double widthHalf		  = size.width()/2;							  				
			  const double textWidthHalf	= metric.width()/2;				  								
			  pos = Pt::Gfx::PointF(widthHalf - textWidthHalf, metric.ascent());	
      }
      break;
		
      case Hmi::Align::TopRight:
      {
			  const double width		  = size.width();				
			  const double textWidth	= metric.width();									
			  pos = Pt::Gfx::PointF(width - textWidth, metric.ascent());	
      }
      break;

		  case Hmi::Align::MidleLeft:
      {			  
        const double heightHalf		  = size.height()/2;				
			  const double textHeightHalf = metric.height()/2;
								
			  pos = Pt::Gfx::PointF(0, (heightHalf - textHeightHalf) + metric.ascent());
      }
      break;

		  case Hmi::Align::MidleCenter:
		  {			
			  const double widthHalf		  = size.width()/2;				
			  const double heightHalf		  = size.height()/2;				
			  const double textWidthHalf	= metric.width()/2;	
			  const double textHeightHalf = metric.height()/2;	
								
			  pos = Pt::Gfx::PointF(widthHalf - textWidthHalf, (heightHalf - textHeightHalf) + metric.ascent());							
		  }
		  break;

		  case Hmi::Align::MidleRight:
      {
			  const double width		  = size.width();				
			  const double textWidth	= metric.width();	
        
        const double heightHalf		  = size.height()/2;				
			  const double textHeightHalf = metric.height()/2;	
								
			  pos = Pt::Gfx::PointF(width - textWidth, (heightHalf - textHeightHalf) + metric.ascent());	
      }
      break;

		  case Hmi::Align::BottomLeft:
      {
        const double height	  = size.height();				
			  const double textHeight = metric.height();	
								
			  pos = Pt::Gfx::PointF(0, (height- textHeight) + metric.ascent());	
      }
      break;

		  case Hmi::Align::BottomCenter:
      {
			  const double widthHalf		  = size.width()/2;				
			  const double textWidthHalf	= metric.width()/2;	

        const double height	  = size.height();				
			  const double textHeight = metric.height();	
								
			  pos = Pt::Gfx::PointF(widthHalf - textWidthHalf, (height- textHeight) + metric.ascent());	
      }
      break;

		  case Hmi::Align::BottomRight:
      {
        const double width		  = size.width();				
			  const double textWidth	= metric.width();	

        const double height	  = size.height();				
			  const double textHeight = metric.height();	
								
			  pos = Pt::Gfx::PointF(width - textWidth, (height- textHeight) + metric.ascent());	
      }
      break;
    }
	}

  localPainter.setPen(pen);
  localPainter.drawText(pos, caption);

	if( UseMnemonic.get() )
	{			
		int index = Widget::getMnemonicIndex( Caption.get() );
		
		if( index != std::string::npos  && ((index + 1) < (int) caption.size()) )
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