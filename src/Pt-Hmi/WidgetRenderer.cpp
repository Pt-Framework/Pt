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

	Pt::Gfx::Size size = wmodel->fromUnit(wmodel->Size.get());

	if(wmodel->PaintBuffer.width() != size.width() ||wmodel->PaintBuffer.height() != size.height())
		wmodel->PaintBuffer.resize(size.width(), size.height());

	Pt::Gfx::ARgbImage& backImage = wmodel->BackgroundImage.get();
	Pt::Gfx::ImagePainter	localPainter(wmodel->PaintBuffer);
	Pt::Gfx::Rect			rect(Pt::Gfx::Point(0,0), size);

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
				for( size_t x = 0; x < wmodel->PaintBuffer.width();  x += backImage.width())
				{
					for( size_t y = 0; y < wmodel->PaintBuffer.height();  y += backImage.height())
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
				Pt::Gfx::blockScale(backImage.begin(), backImage.width(), backImage.height(), wmodel->PaintBuffer.begin(),  wmodel->PaintBuffer.width(),  wmodel->PaintBuffer.height());
			}
			break;

			case ImageLayoutType::Zoom:
			{
				double factor = wmodel->PaintBuffer.width()/(double)backImage.width();

				Pt::Gfx::blockScale(backImage.begin(), backImage.width(), backImage.height(), wmodel->PaintBuffer.begin(),  wmodel->PaintBuffer.width(), (Pt::size_t)(backImage.height()*factor));
			}
			break;
		}
	}

}

}}
