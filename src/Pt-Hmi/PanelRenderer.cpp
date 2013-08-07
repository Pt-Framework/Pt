#include <Pt/Hmi/PanelRenderer.h>
#include <Pt/Hmi/PanelModel.h>
#include <Pt/Hmi/GfxController.h>
#include <Pt/Gfx/ImagePainter.h>
#include <Pt/Gfx/Rgb888Color.h>
#include <Pt/Gfx/Rgb888Image.h>
#include <Pt/Gfx/Point.h>
#include <Pt/Hmi/WidgetController.h>

namespace Pt{
namespace Hmi{

PanelRenderer::PanelRenderer()
{
}

PanelRenderer::~PanelRenderer()
{
}

void PanelRenderer::render(Pt::Hmi::Model* m)
{	
	WidgetRenderer::render(m);

	PanelModel* model = dynamic_cast<PanelModel*>(m);

	if(model == 0)
		throw std::logic_error("PanelRenderer: PanleModel expected");

	if(!model->Visible.get())
		return;

	WidgetController* ctrl = dynamic_cast<WidgetController*>(model->Controller.get());
		
	if( ctrl == 0)
		return;
	
	int corner = 0;

	if(model->BorderRoundEdge.get())
		corner = 1;

	int border =  model->fromUnit(model->BorderWidth.get());	
	Pt::Gfx::Size   size = model->fromUnit(model->Size.get());
	Pt::Gfx::SizeF  clientSize(model->Size.get().width() - (model->BorderWidth.get()), model->Size.get().height() - (model->BorderWidth.get()));	
	Pt::Gfx::Size   rectSize = model->fromUnit(clientSize);
	Pt::Gfx::Rect   clientRect(Pt::Gfx::Point(border/2,border/2), rectSize);
	
	Pt::Gfx::ImagePainter localPainter(model->PaintBuffer);
						

						
	switch(model->BorderStyle.get())
	{
		case BorderStyle::None:
		break;
		case BorderStyle::Single:
		{			
			std::vector<Pt::Gfx::Point> points1(5);
			std::vector<Pt::Gfx::Point> points2(5);

			//P0
			points1[0].setX(corner);
			points1[0].setY(clientRect.height());

			//P1
			points1[1].setX(0);
			points1[1].setY(clientRect.height() - corner);

			//P2
			points1[2].setX(0);
			points1[2].setY(corner);

			//P3
			points1[3].setX(corner);
			points1[3].setY(0);

			//P4
			points1[4].setX(clientRect.width() - corner);
			points1[4].setY(0);
			
			//---
			//P0
			points2[0].setX(clientRect.width() - corner);
			points2[0].setY(0);

			//P1
			points2[1].setX(clientRect.width());
			points2[1].setY(corner);

			//P2
			points2[2].setX(clientRect.width());
			points2[2].setY(clientRect.height() - corner);

			//P3
			points2[3].setX(clientRect.width() - corner);
			points2[3].setY(clientRect.height());

			//P4
			points2[4].setX(corner);
			points2[4].setY(clientRect.height());
		
			Pt::Gfx::Pen pen(1, Pt::Gfx::ARgbColor(0,170,170,170));
			localPainter.setPen(pen);
				
			localPainter.drawPolyline(&points1[0], points1.size());								
			localPainter.drawPolyline(&points2[0], points2.size());
		}

		break;
			
		case BorderStyle::Widget:
		{			
			std::vector<Pt::Gfx::Point> points1(5);
			std::vector<Pt::Gfx::Point> points2(5);

			//P0
			points1[0].setX(corner);
			points1[0].setY(clientRect.height());

			//P1
			points1[1].setX(0);
			points1[1].setY(clientRect.height() - corner);

			//P2
			points1[2].setX(0);
			points1[2].setY(corner);

			//P3
			points1[3].setX(corner);
			points1[3].setY(0);

			//P4
			points1[4].setX(clientRect.width() - corner);
			points1[4].setY(0);
			
			//---
			//P0
			points2[0].setX(clientRect.width() - corner);
			points2[0].setY(0);

			//P1
			points2[1].setX(clientRect.width());
			points2[1].setY(corner);

			//P2
			points2[2].setX(clientRect.width());
			points2[2].setY(clientRect.height() - corner);

			//P3
			points2[3].setX(clientRect.width() - corner);
			points2[3].setY(clientRect.height());

			//P4
			points2[4].setX(corner);
			points2[4].setY(clientRect.height());


			if(model->Invert3DEffect.get())
			{
				Pt::Gfx::Pen pen(border, Pt::Gfx::ARgbColor(0,255,255,255));
				localPainter.setPen(pen);
				
				localPainter.drawPolyline(&points2[0], points2.size());
								
				Pt::Gfx::Pen pen2(border, Pt::Gfx::ARgbColor(0,0,0,0));
				localPainter.setPen(pen2);

				localPainter.drawPolyline(&points1[0], points1.size());
			}
			else
			{
				Pt::Gfx::Pen pen(border, Pt::Gfx::ARgbColor(0,0,0,0));
				localPainter.setPen(pen);
				
				localPainter.drawPolyline(&points2[0], points2.size());
								
				Pt::Gfx::Pen pen2(border, Pt::Gfx::ARgbColor(0,255,255,255));
				localPainter.setPen(pen2);

				localPainter.drawPolyline(&points1[0], points1.size());
			}
		}
		break;
		case BorderStyle::Border3D:
		case BorderStyle::Tool:
		{
			std::vector<Pt::Gfx::Point> points1(3);
			std::vector<Pt::Gfx::Point> points2(3);

			points1[0].setX(0);
			points1[0].setY(clientRect.height());

			points1[1].setX(0);
			points1[1].setY(0);
				
			points1[2].setX(clientRect.width());
			points1[2].setY(0);


			points2[0].setX(clientRect.width());
			points2[0].setY(0);

			points2[1].setX(clientRect.width());
			points2[1].setY(clientRect.height());

			points2[2].setX(0);
			points2[2].setY(clientRect.height());

			if(!model->Invert3DEffect.get())
			{
				Pt::Gfx::Pen pen(border, Pt::Gfx::ARgbColor(0,255,255,255));
				localPainter.setPen(pen);
				
				localPainter.drawPolyline(&points1[0], points1.size());
								
				Pt::Gfx::Pen pen2(border, Pt::Gfx::ARgbColor(0,0,0,0));
				localPainter.setPen(pen2);

				localPainter.drawPolyline(&points2[0], points2.size());
			}
			else
			{
				Pt::Gfx::Pen pen(border, Pt::Gfx::ARgbColor(0,255,255,255));
				localPainter.setPen(pen);
				
				localPainter.drawPolyline(&points2[0], points2.size());
								
				Pt::Gfx::Pen pen2(border, Pt::Gfx::ARgbColor(0,0,0,0));
				localPainter.setPen(pen2);

				localPainter.drawPolyline(&points1[0], points1.size());
			}
				
		}
		break;
		case BorderStyle::Sizebale:
		case BorderStyle::ToolSizeable:
		{
			Pt::Gfx::Pen pen1(border, model->ForeColor.get());
			localPainter.setPen(pen1);				
			localPainter.drawRect(clientRect);

			Pt::Gfx::Pen pen2(1, Pt::Gfx::ARgbColor(0,255,255,255));
			localPainter.setPen(pen2);				
			localPainter.drawRect(clientRect);
		}
		break;
			
	}	

}

}}
