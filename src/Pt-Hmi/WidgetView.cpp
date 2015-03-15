/* Copyright (C) 2013 Laurentiu-Gheorghe Crisan
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * As a special exception, you may use this file as part of a free
 * software library without restriction. Specifically, if other files
 * instantiate templates or use macros or inline functions from this
 * file, or you compile this file and link it with other files to
 * produce an executable, this file does not by itself cause the
 * resulting executable to be covered by the GNU General Public
 * License. This exception does not however invalidate any other
 * reasons why the executable file might be covered by the GNU Library
 * General Public License.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA*/
#include <Pt/Hmi/WidgetView.h>
#include <Pt/Hmi/WidgetModel.h>
#include <Pt/Gfx/ARgbImage.h>
#include <Pt/Gfx/Brush.h>

namespace Pt{
namespace Hmi{

WidgetView::WidgetView()
{
}

WidgetView::~WidgetView()
{
}

void WidgetView::output(Pt::Hmi::Model* model)
{

}

void WidgetView::render( Pt::Hmi::Model* m )
{		
	Pt::Hmi::WidgetModel* model  = dynamic_cast<Pt::Hmi::WidgetModel*>(m);

	if(!model->Visible.get())
		return;

	if( model->Size.get().width() < 0 ||  model->Size.get().height() < 0)
		return;

	Pt::Gfx::SizeF size = model->Size.get();
	Pt::Gfx::SizeF bufferSize = _paintSurface.size();

	//ToDo: move this check to surface resize.
	if(bufferSize.width() != size.width() ||bufferSize.height() != size.height())
		_paintSurface.resize(size);

	Pt::Gfx::ARgbImage& backImage = model->BackgroundImage.get();
	Pt::Hmi::Painter&	localPainter = _paintSurface.painter();
	Pt::Gfx::RectF		rect(Pt::Gfx::PointF(0,0),size);
	
	localPainter.setFont(model->Font.get());

	if(model->HighLight.get())
	{       
		Pt::Gfx::Brush	brush(model->BackColorHightLight.get());
        
		localPainter.setBrush(brush);
		localPainter.fillRect(rect);
	}
	else
	{
		Pt::Gfx::Brush	brush(model->BackColor.get());
	
		localPainter.setBrush(brush);
    
		localPainter.fillRect(rect);
	}

	if( backImage.width() != 0 && backImage.height() != 0)
	{
		switch(model->BackgroundImageLayout.get())
		{				
			case ImageLayoutType::NoLayout:
			{
				localPainter.drawImage(Pt::Gfx::PointF(0,0), backImage);
			}
			break;
			
			case ImageLayoutType::Tile:
			{
				for( size_t x = 0; x < _paintSurface.size().width();  x += backImage.width())
				{
					for( size_t y = 0; y < _paintSurface.size().height();  y += backImage.height())
						localPainter.drawImage(Pt::Gfx::PointF(x,y), backImage);
				}
			}
			break;

			case ImageLayoutType::Center:
			{
				double x = size.width()/2  - backImage.width()/2;
				double y = size.height()/2  - backImage.height()/2;
				localPainter.drawImage(Pt::Gfx::PointF(x,y), backImage);
			}
			break;
			
			case ImageLayoutType::Strech:
			{
				Pt::Gfx::ARgbImage strech(_paintSurface.size().width(), _paintSurface.size().height() );

				Pt::Gfx::blockScale(backImage.begin(), backImage.width(), backImage.height(), strech.begin(), _paintSurface.size().width(),  _paintSurface.size().height());
				localPainter.drawImage(Pt::Gfx::PointF(0,0), strech);
			}
			break;

			case ImageLayoutType::Zoom:
			{
				Pt::Gfx::ARgbImage strech(_paintSurface.size().width(), _paintSurface.size().height() );
				double factor = _paintSurface.size().width()/(double)backImage.width();

				Pt::Gfx::blockScale(backImage.begin(), backImage.width(), backImage.height(),strech.begin(),  strech.width(), (Pt::size_t)(backImage.height()*factor));
				localPainter.drawImage(Pt::Gfx::PointF(0,0), strech);
			}
			break;
		}
	}
	
}


}}
