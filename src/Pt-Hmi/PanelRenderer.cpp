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

void PanelRenderer::render(Pt::Hmi::Model* model)
{
	PanelModel* fmodel = dynamic_cast<PanelModel*>(model);	

	if( fmodel != 0)
	{	
		Pt::Gfx::Size rectSize = fmodel->fromUnit(fmodel->Size.get());
		Pt::Gfx::Rect rect(Pt::Gfx::Point(0,0), rectSize);

	/*
		Pt::Gfx::ImagePainter	painter(image);
		Pt::Gfx::Brush			brush(fmodel->BackColor.get());		

		painter.setBrush(brush);
		painter.fillRect(rect);
						
		if(fmodel->BorderStyle.get() != BorderStyle::None)
		{
			int size = fmodel->fromUnit(fmodel->BorderWidth.get());

			Pt::Gfx::Pen	pen(size, fmodel->ForeColor.get());
			painter.setPen(pen);
			painter.drawRect(rect);
		}*/
	}
}

}}
