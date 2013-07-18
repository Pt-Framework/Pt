#ifndef Pt_Hmi_Controller_Widget_H
#define Pt_Hmi_Controller_Widget_H

#include <Pt/Hmi/Controller.h>
#include <Pt/Hmi/Event2D.h>

namespace Pt{
namespace Hmi{

class PT_HMI_API WidgetController  : public Controller
{
public:
	WidgetController();
	virtual ~WidgetController();
	
	Pt::Signal<const Event2D*> Input2D;
};

}}
#endif