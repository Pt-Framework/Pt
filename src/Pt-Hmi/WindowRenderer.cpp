#include <Pt/Hmi/WindowRenderer.h>
#include <Pt/Hmi/WindowModel.h>

namespace Pt{
namespace Hmi{

WindowRenderer::WindowRenderer()
{
}

WindowRenderer::~WindowRenderer()
{
}

void WindowRenderer::render(Pt::Hmi::Model* model)
{
	WindowModel* winModel = dynamic_cast<WindowModel*>(model);	
/*
	if( winModel != 0)
	{	
		Pt::Gfx::ARgbImage& image = *(winModel->ImagePtr.get());
		image.clear();
		//Todo draw the window
	}*/
}

}}

