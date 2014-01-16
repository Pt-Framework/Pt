#include <Pt/Hmi/WidgetRenderer.h>
#include <Pt/Hmi/Model.h>
#include <Pt/Hmi/WidgetModel.h>
#include <Pt/Gfx/ImagePainter.h>
#include <Pt/Gfx/ImageAlgo.h>

namespace Pt{
namespace Hmi{

WidgetRenderer::WidgetRenderer()
{
}

WidgetRenderer::~WidgetRenderer()
{
}


void WidgetRenderer::render(Pt::Hmi::Model* model)
{
	WidgetModel* wmodel = dynamic_cast<WidgetModel*>(model);
	
	if( wmodel == 0)
		throw std::logic_error("WidgetRenderer: WidgetModel expected");

	if(!wmodel->Visible.get())
		return;

	if( wmodel->Size.get().width() < 0 ||  wmodel->Size.get().height() < 0)
		return;

	Pt::Gfx::SizeF size = wmodel->Size.get();
	Pt::Gfx::SizeF bufferSize = wmodel->PaintBuffer.size();

	if(bufferSize.width() != size.width() ||bufferSize.height() != size.height())
		wmodel->PaintBuffer.resize(size);

	Pt::Gfx::ARgbImage& backImage = wmodel->BackgroundImage.get();
	Pt::Hmi::Painter	localPainter(wmodel->PaintBuffer);
	Pt::Gfx::Rect		rect(Pt::Gfx::Point(0,0), wmodel->fromUnit(size));

	if(wmodel->HighLight.get())
	{       
		Pt::Gfx::Brush	brush(wmodel->BackColorHightLight.get());
        
		localPainter.setBrush(brush);
        
		localPainter.fillRect(rect);
	}
	else
	{
		Pt::Gfx::Brush	brush(wmodel->BackColor.get());
	
		localPainter.setBrush(brush);
    
		localPainter.fillRect(rect);
	}

	if( backImage.width() != 0 && backImage.height() != 0)
	{
		switch(wmodel->BackgroundImageLayout.get())
		{
				
			case ImageLayoutType::NoLayout:
			{
				localPainter.drawImage(Pt::Gfx::Point(0,0), backImage);
			}
			break;
			
			case ImageLayoutType::Tile:
			{
				for( size_t x = 0; x < wmodel->PaintBuffer.size().width();  x += backImage.width())
				{
					for( size_t y = 0; y < wmodel->PaintBuffer.size().height();  y += backImage.height())
					{
							localPainter.drawImage(Pt::Gfx::Point(x,y), backImage);
					}
				}
			}
			break;

			case ImageLayoutType::Center:
			{
				int x = size.width()/2  - backImage.width()/2;
				int y = size.height()/2  - backImage.height()/2;
				localPainter.drawImage(Pt::Gfx::Point(x,y), backImage);
			}
			break;
			
			case ImageLayoutType::Strech:
			{
				Pt::Gfx::ARgbImage strech(wmodel->PaintBuffer.size().width(),wmodel->PaintBuffer.size().height() );

				Pt::Gfx::blockScale(backImage.begin(), backImage.width(), backImage.height(), strech.begin(),  wmodel->PaintBuffer.size().width(),  wmodel->PaintBuffer.size().height());
				localPainter.drawImage(Pt::Gfx::Point(0,0), strech);
			}
			break;

			case ImageLayoutType::Zoom:
			{
				Pt::Gfx::ARgbImage strech(wmodel->PaintBuffer.size().width(),wmodel->PaintBuffer.size().height() );
				double factor = wmodel->PaintBuffer.size().width()/(double)backImage.width();

				Pt::Gfx::blockScale(backImage.begin(), backImage.width(), backImage.height(),strech.begin(),  strech.width(), (Pt::size_t)(backImage.height()*factor));
				localPainter.drawImage(Pt::Gfx::Point(0,0), strech);
			}
			break;
		}
	}

}

}}
