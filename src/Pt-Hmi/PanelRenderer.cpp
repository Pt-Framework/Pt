#include <Pt/Hmi/PanelRenderer.h>
#include <Pt/Hmi/PanelModel.h>
#include <Pt/Hmi/GfxController.h>
#include <Pt/Gfx/ImagePainter.h>
#include <Pt/Gfx/Rgb888Color.h>
#include <Pt/Gfx/Rgb888Image.h>
#include <Pt/Gfx/Point.h>

namespace Pt{
namespace Hmi{

PanelRenderer::PanelRenderer()
{
}

PanelRenderer::~PanelRenderer()
{
}

void PanelRenderer::render(Pt::Hmi::Model* model ,Pt::Gfx::Painter* painter)
{
	PanelModel* fmodel = dynamic_cast<PanelModel*>(model);	

	if( fmodel == 0)
		return;

	if(!fmodel->Visible.get())
		return;


	GfxController* ctrl = dynamic_cast<GfxController*>(fmodel->Controller.get());
		
	if( ctrl == 0)
		return;

	
	Pt::Gfx::Size rectSize = fmodel->fromUnit(fmodel->Size.get());
	Pt::Gfx::Point position = fmodel->fromUnit(ctrl->fromClient(fmodel->Position.get(), true));
	Pt::Gfx::Rect rect(position, rectSize);

	Pt::Gfx::Brush	brush(fmodel->BackColor.get());		
	painter->setFont(fmodel->Font.get());
	painter->setBrush(brush);
	painter->fillRect(rect);
						
	switch(fmodel->BorderStyle.get())
	{
		case BorderStyle::None:
		break;
		case BorderStyle::Single:
		{			
			Pt::Gfx::Pen	pen(fmodel->BorderWidth.get(), Pt::Gfx::ARgbColor(0,0,0,0));
			painter->setPen(pen);
			painter->drawRect(rect);
		}

		break;
			
		case BorderStyle::Widget:
		{
			double corner = 1;
			std::vector<Pt::Gfx::Point> points1(5);
			std::vector<Pt::Gfx::Point> points2(5);

			//P0
			points1[0].setX(position.x()+corner);
			points1[0].setY(position.y() + rect.height());

			//P1
			points1[1].setX(position.x());
			points1[1].setY(position.y() + rect.height() - corner);

			//P2
			points1[2].setX(position.x());
			points1[2].setY(position.y()+corner);

			//P3
			points1[3].setX(position.x()+corner);
			points1[3].setY(position.y());

			//P4
			points1[4].setX(position.x()+ rect.width() - corner);
			points1[4].setY(position.y());
			
			//---
			//P0
			points2[0].setX(position.x()+ rect.width() - corner);
			points2[0].setY(position.y());

			//P1
			points2[1].setX(position.x()+ rect.width());
			points2[1].setY(position.y() + corner);

			//P2
			points2[2].setX(position.x()+ rect.width());
			points2[2].setY(position.y() + rect.height() - corner);

			//P3
			points2[3].setX(position.x()+ rect.width() - corner);
			points2[3].setY(position.y() + rect.height());

			//P4
			points2[4].setX(position.x()+corner);
			points2[4].setY(position.y() + rect.height());


			if(!fmodel->Invert3DEffect.get())
			{
				Pt::Gfx::Pen pen(fmodel->BorderWidth.get(), Pt::Gfx::ARgbColor(0,255,255,255));
				painter->setPen(pen);
				
				painter->drawPolyline(&points1[0], points1.size());
								
				Pt::Gfx::Pen pen2(fmodel->BorderWidth.get(), Pt::Gfx::ARgbColor(0,0,0,0));
				painter->setPen(pen2);

				painter->drawPolyline(&points2[0], points2.size());
			}
			else
			{
				Pt::Gfx::Pen pen(fmodel->BorderWidth.get(), Pt::Gfx::ARgbColor(0,255,255,255));
				painter->setPen(pen);
				
				painter->drawPolyline(&points2[0], points2.size());
								
				Pt::Gfx::Pen pen2(fmodel->BorderWidth.get(), Pt::Gfx::ARgbColor(0,0,0,0));
				painter->setPen(pen2);

				painter->drawPolyline(&points1[0], points1.size());
			}
		}
		break;
		case BorderStyle::Border3D:
		case BorderStyle::Tool:
		{
			std::vector<Pt::Gfx::Point> points1(3);
			std::vector<Pt::Gfx::Point> points2(3);

			points1[0].setX(position.x());
			points1[0].setY(position.y() + rect.height());

			points1[1].setX(position.x());
			points1[1].setY(position.y());
				
			points1[2].setX(position.x() + rect.width());
			points1[2].setY(position.y());


			points2[0].setX(position.x() + rect.width());
			points2[0].setY(position.y());

			points2[1].setX(position.x() +  rect.width());
			points2[1].setY(position.y() + rect.height());

			points2[2].setX(position.x());
			points2[2].setY(position.y() + rect.height());

			if(!fmodel->Invert3DEffect.get())
			{
				Pt::Gfx::Pen pen(fmodel->BorderWidth.get(), Pt::Gfx::ARgbColor(0,255,255,255));
				painter->setPen(pen);
				
				painter->drawPolyline(&points1[0], points1.size());
								
				Pt::Gfx::Pen pen2(fmodel->BorderWidth.get(), Pt::Gfx::ARgbColor(0,0,0,0));
				painter->setPen(pen2);

				painter->drawPolyline(&points2[0], points2.size());
			}
			else
			{
				Pt::Gfx::Pen pen(fmodel->BorderWidth.get(), Pt::Gfx::ARgbColor(0,255,255,255));
				painter->setPen(pen);
				
				painter->drawPolyline(&points2[0], points2.size());
								
				Pt::Gfx::Pen pen2(fmodel->BorderWidth.get(), Pt::Gfx::ARgbColor(0,0,0,0));
				painter->setPen(pen2);

				painter->drawPolyline(&points1[0], points1.size());
			}
				
		}
		break;
		case BorderStyle::Sizebale:
		case BorderStyle::ToolSizeable:
		{
			int size = fmodel->fromUnit(fmodel->BorderWidth.get());
			Pt::Gfx::Pen pen1(size, fmodel->ForeColor.get());
			painter->setPen(pen1);				
			painter->drawRect(rect);

			Pt::Gfx::Pen pen2(1, Pt::Gfx::ARgbColor(0,255,255,255));
			painter->setPen(pen2);				
			painter->drawRect(rect);
		}
		break;
			
	}
}

}}
