#include <Pt/Hmi/PanelRenderer.h>
#include <Pt/Hmi/PanelModel.h>
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

	if( fmodel != 0)
	{	
		Pt::Gfx::Size rectSize = fmodel->fromUnit(fmodel->Size.get());
		Pt::Gfx::Rect rect(Pt::Gfx::Point(0,0), rectSize);

		Pt::Gfx::Brush			brush(fmodel->BackColor.get());		

		painter->setBrush(brush);
		painter->fillRect(rect);
						
		switch(fmodel->BorderStyle.get())
		{
			case BorderStyle::None:
			break;
			case BorderStyle::Single:
			{			
				Pt::Gfx::Pen	pen(1, fmodel->ForeColor.get());
				painter->setPen(pen);
				painter->drawRect(rect);
			}

			break;
			case BorderStyle::Border3D:

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
			
			case BorderStyle::Tool:
			
			break;		
		}
	}
}

}}
