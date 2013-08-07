#include <Pt/Hmi/WidgetController.h>
#include <Pt/Hmi/PointingDevice.h>
#include <Pt/Hmi/WindowController.h>
#include <Pt/Hmi/GfxModel.h>

namespace Pt{
namespace Hmi{

WidgetController::WidgetController()
{
}

WidgetController::~WidgetController()
{

}

void WidgetController::onKeyInput(const KeyEvent& ev)
{ 
	GfxModel* m = gfxModel();
	
	m->KeyStatus = ev;
	
	for( size_t i = 0; i < children().size(); ++i)
		children()[i]->notifyKeyInput(ev);
}

void WidgetController::onPointerInput(const PointingEvent& ev)
{
	GfxModel* m = gfxModel();
	
	m->Pointer2DStatus = ev;

	for( size_t i = 0; i < children().size(); ++i)
		children()[i]->notifyPointerInput(ev);
}

}}
