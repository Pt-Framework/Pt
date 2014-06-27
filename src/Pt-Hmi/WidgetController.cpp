#include <Pt/Hmi/WidgetController.h>
#include <Pt/Hmi/PointingDevice.h>
#include <Pt/Hmi/WindowController.h>
#include <Pt/Hmi/GfxModel.h>
#include <Pt/Hmi/WidgetModel.h>
#include <Pt/Hmi/WidgetRenderer.h>

namespace Pt{
namespace Hmi{

WidgetController::WidgetController(WidgetModel& model, WidgetRenderer& renderer)
: GfxController(model, renderer)
, _mnemonicWidget(0)
{
}

WidgetController::~WidgetController()
{

}

void WidgetController::onKeyInput(const KeyEvent& ev)
{ 
	WidgetModel& m = widgetModel();
	
	m.KeyStatus = ev;
	
	if(m.UseMnemonic.get() && _mnemonicWidget != 0 && m.Enable.get() && ev.state() == Pt::Hmi::KeyEvent::KeyUp)
	{		
		std::string mnKey = "";

		if(m.KeyStatus.get().alt())
			mnKey = "A//";
			
		mnKey += m.KeyStatus.get().toUTF8String();

		 if(m.getMnemonicKey() == mnKey)
			_mnemonicWidget->onMnemonic();			
	}

	for( size_t i = 0; i < children().size(); ++i)
		children()[i]->notifyKeyInput(ev);
}

void WidgetController::onPointerInput(const PointingEvent& ev)
{
	GfxModel& m = gfxModel();
	
	m.Pointer2DStatus = ev;

	for( size_t i = 0; i < children().size(); ++i)
		children()[i]->notifyPointerInput(ev);
}

void WidgetController::bindMnemonicToWidget(WidgetController* widget)
{
	_mnemonicWidget = widget;
}

void WidgetController::onMnemonic()
{
	if(gfxModel().Focused.get() != true)
		gfxModel().Focused = true;
}

}}
