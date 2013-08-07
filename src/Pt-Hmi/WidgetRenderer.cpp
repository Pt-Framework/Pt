#include <Pt/Hmi/WidgetRenderer.h>
#include <Pt/Hmi/Model.h>
#include <Pt/Hmi/WidgetModel.h>
#include <Pt/Gfx/ImagePainter.h>

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

	Pt::Gfx::Rect			rect(Pt::Gfx::Point(0,0), size);
	Pt::Gfx::ImagePainter	localPainter(wmodel->PaintBuffer);
	Pt::Gfx::Brush			brush(wmodel->BackColor.get());		
	
	localPainter.setBrush(brush);
	localPainter.fillRect(rect);
}

}}
