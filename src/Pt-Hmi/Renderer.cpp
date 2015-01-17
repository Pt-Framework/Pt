#include <Pt/Hmi/Renderer.h>
#include <Pt/Hmi/PaintSurface.h>
#include <Pt/Hmi/WidgetModel.h>
#include <Pt/Hmi/Painter.h>
#include <Pt/Gfx/ImageAlgo.h>
#include <Pt/Gfx/Brush.h>

namespace Pt {

namespace Hmi {

Renderer::Renderer()
{
}

Renderer::~Renderer()
{
}


void Renderer::render(Pt::Hmi::WidgetModel* model)
{
	if(!model->Visible.get())
		return;

	if( model->Size.get().width() < 0 ||  model->Size.get().height() < 0)
		return;

	Pt::Gfx::SizeF size = model->Size.get();
	Pt::Gfx::SizeF bufferSize = model->paintSurface()->size();

	if(bufferSize.width() != size.width() ||bufferSize.height() != size.height())
		model->paintSurface()->resize(size);

	Pt::Gfx::ARgbImage& backImage = model->BackgroundImage.get();
	Pt::Hmi::Painter&	localPainter = model->paintSurface()->painter();
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
				for( size_t x = 0; x < model->paintSurface()->size().width();  x += backImage.width())
				{
					for( size_t y = 0; y < model->paintSurface()->size().height();  y += backImage.height())
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
				Pt::Gfx::ARgbImage strech(model->paintSurface()->size().width(),model->paintSurface()->size().height() );

				Pt::Gfx::blockScale(backImage.begin(), backImage.width(), backImage.height(), strech.begin(),  model->paintSurface()->size().width(),  model->paintSurface()->size().height());
				localPainter.drawImage(Pt::Gfx::PointF(0,0), strech);
			}
			break;

			case ImageLayoutType::Zoom:
			{
				Pt::Gfx::ARgbImage strech(model->paintSurface()->size().width(),model->paintSurface()->size().height() );
				double factor = model->paintSurface()->size().width()/(double)backImage.width();

				Pt::Gfx::blockScale(backImage.begin(), backImage.width(), backImage.height(),strech.begin(),  strech.width(), (Pt::size_t)(backImage.height()*factor));
				localPainter.drawImage(Pt::Gfx::PointF(0,0), strech);
			}
			break;
		}
	}

}

}}
