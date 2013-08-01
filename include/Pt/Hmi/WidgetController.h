#ifndef Pt_Hmi_Controller_Widget_H
#define Pt_Hmi_Controller_Widget_H

#include <Pt/Hmi/GfxController.h>
#include <Pt/Hmi/PointingEvent.h>

namespace Pt{
namespace Hmi{

class GfxModel;

class PT_HMI_API WidgetController  : public GfxController
{
public:
	WidgetController();
	virtual ~WidgetController();		

public:
	virtual void onInput2D(const PointingEvent& ev);
};

}}
#endif