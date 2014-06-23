#ifndef Pt_Hmi_Controller_Widget_H
#define Pt_Hmi_Controller_Widget_H

#include <Pt/Hmi/GfxController.h>
#include <Pt/Hmi/PointingEvent.h>
#include <Pt/Hmi/WidgetModel.h>

namespace Pt{
namespace Hmi{

class GfxModel;
class WidgetRenderer;

class PT_HMI_API WidgetController  : public GfxController
{
public:
	WidgetController(WidgetModel& model, WidgetRenderer& renderer);
	virtual ~WidgetController();		

	const WidgetModel& widgetModel() const 
	{
		return static_cast<const WidgetModel&>( model());
	}

	WidgetModel& widgetModel()
	{
		return static_cast<WidgetModel&>( model());
	}

public:
	virtual void onPointerInput(const PointingEvent& ev);
	virtual void onKeyInput(const KeyEvent& ev);
};

}}
#endif