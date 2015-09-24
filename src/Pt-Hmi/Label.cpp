/* Copyright (C) 2015 Marc Boris Duerner 
   Copyright (C) 2015 Laurentiu-Gheorghe Crisan
  
  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.
  
  As a special exception, you may use this file as part of a free
  software library without restriction. Specifically, if other files
  instantiate templates or use macros or inline functions from this
  file, or you compile this file and link it with other files to
  produce an executable, this file does not by itself cause the
  resulting executable to be covered by the GNU General Public
  License. This exception does not however invalidate any other
  reasons why the executable file might be covered by the GNU Library
  General Public License.
  
  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.
  
  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA*/
#include <Pt/Hmi/Label.h>
#include <Pt/Gfx/Point.h>
#include <Pt/Gfx/Pen.h>
#include <Pt/Gfx/FontMetrics.h>

namespace Pt{
namespace Hmi{

Label::Label()
: Panel()
 , PT_HMI_INIT_PROPERTY_VALUE(AutoSize,true)
{
	AutoSize.changed() += Pt::slot(*this, &Label::onAutoSizeChanged);
  Caption.changed() +=  Pt::slot(*this, &Label::onCaptionChanged);
  Caption.set("Label");
  Name.set("Label");
	ForeColor.set(Gfx::Color::fromRgb8(0,0,0,0));
	PanelBorderStyle.set(BorderStyle::NoBorder);
  
}

Label::~Label()
{
}


void Label::recalcNewSize()
{
 if( !AutoSize.get() )
    return;
  
  std::string caption = "";

  if( UseMnemonic.get() )
	  caption = Widget::removeMnemonic(Caption.get()).c_str();
  else
	  caption = Caption.get().c_str();

	PaintSurface surface;
  surface.painter().setFont(Font.get());

  const Gfx::FontMetrics metric = surface.painter().fontMetrics(caption.c_str());

  Size =Gfx::SizeF( metric.width() + Margin.get().left() + Margin.get().right(), metric.height() + Margin.get().top() +  Margin.get().bottom() );  
  invalidate();
}


void Label::onCaptionChanged(const std::string& cap)
{
  recalcNewSize();
}

void Label::onAutoSizeChanged(const bool& a)
{
  recalcNewSize();
}


void Label::onRender(PaintSurface& paintSurface)
{
	Pt::Hmi::Painter& painter = paintSurface.painter();	
 Gfx::SizeF         size = clientSize();
	Gfx::PointF        pos = clientPos();
	Pt::String        caption;
 Gfx::Color         foreColor = Enabled.get() ? ForeColor.get() : DisabledColor.get();
 Gfx::Pen	          pen( 1, foreColor);
    
	if( UseMnemonic.get() )
		caption = Widget::removeMnemonic(Caption.get()).c_str();
	else
		caption = Caption.get().c_str();	  

  painter.setFont(Font.get());

	Gfx::FontMetrics	metric = painter.fontMetrics(caption);
  
  Panel::onRender(paintSurface);
              
	if( !AutoSize.get() )
	{										            	
		switch(TextAlign.get())
		{
      case Hmi::Align::TopLeft:
      {
			  pos +=Gfx::PointF( 0, metric.ascent() );			
      }
      break;

		  case Hmi::Align::TopCenter:
      {
        const double widthHalf		  = size.width()/2;							  				
			  const double textWidthHalf	= metric.width()/2;				  								
			  pos +=Gfx::PointF(widthHalf - textWidthHalf, metric.ascent());	
      }
      break;
		
      case Hmi::Align::TopRight:
      {
			  const double width		  = size.width();				
			  const double textWidth	= metric.width();									
			  pos +=Gfx::PointF(width - textWidth, metric.ascent());	
      }
      break;

		  case Hmi::Align::MidleLeft:
      {			  
        const double heightHalf		  = size.height()/2;				
			  const double textHeightHalf = metric.height()/2;
								
			  pos +=Gfx::PointF(0, (heightHalf - textHeightHalf) + metric.ascent());
      }
      break;

		  case Hmi::Align::MidleCenter:
		  {			
			  const double widthHalf		  = size.width()/2;				
			  const double heightHalf		  = size.height()/2;				
			  const double textWidthHalf	= metric.width()/2;	
			  const double textHeightHalf = metric.height()/2;	
								
			  pos +=Gfx::PointF(widthHalf - textWidthHalf, (heightHalf - textHeightHalf) + metric.ascent());							
		  }
		  break;

		  case Hmi::Align::MidleRight:
      {
			  const double width		  = size.width();				
			  const double textWidth	= metric.width();	
        
        const double heightHalf		  = size.height()/2;				
			  const double textHeightHalf = metric.height()/2;	
								
			  pos +=Gfx::PointF(width - textWidth, (heightHalf - textHeightHalf) + metric.ascent());	
      }
      break;

		  case Hmi::Align::BottomLeft:
      {
        const double height	  = size.height();				
			  const double textHeight = metric.height();	
								
			  pos +=Gfx::PointF(0, (height- textHeight) + metric.ascent());	
      }
      break;

		  case Hmi::Align::BottomCenter:
      {
			  const double widthHalf		  = size.width()/2;				
			  const double textWidthHalf	= metric.width()/2;	

        const double height	  = size.height();				
			  const double textHeight = metric.height();	
								
			  pos +=Gfx::PointF(widthHalf - textWidthHalf, (height- textHeight) + metric.ascent());	
      }
      break;

		  case Hmi::Align::BottomRight:
      {
        const double width		  = size.width();				
			  const double textWidth	= metric.width();	

        const double height	  = size.height();				
			  const double textHeight = metric.height();	
								
			  pos +=Gfx::PointF(width - textWidth, (height- textHeight) + metric.ascent());	
      }
      break;
    }
	}
  else
  {
      pos +=Gfx::PointF( 0, metric.ascent() );
  }

  painter.setPen(pen);
  painter.drawText(pos, caption);

	if( UseMnemonic.get() )
	{			
		int index = Widget::getMnemonicIndex( Caption.get() );
		
		if( index != std::string::npos 
      
      && ((index + 1) < (int) caption.size()) )
		{	
			std::string subString(  caption.begin(), caption.begin() + index );

			Gfx::FontMetrics metric = painter.fontMetrics( Pt::String( subString.c_str() ) );
	
			Gfx::PointF linePos( pos.x() + metric.width() - 1, pos.y()  + 1);

			subString = caption[index];

			metric = painter.fontMetrics( Pt::String( subString.c_str() ) );

			painter.drawLine( linePos ,Gfx::PointF( linePos.x() + metric.width(), linePos.y() ) );
		}
	}
}

}}