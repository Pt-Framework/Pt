#ifndef Pt_Hmi_Controller_Window_H
#define Pt_Hmi_Controller_Window_H

#include <Pt/Hmi/WidgetController.h>
#include <Pt/Hmi/Event2D.h>
#include <Pt/Hmi/Api.h>

namespace Pt{
namespace Hmi{

class PT_HMI_API WindowController  : public Controller
{
public:
	WindowController();
	virtual ~WindowController();
	
	void start();
	void stop();

	Pt::Signal<const Event2D*> Input2D;
	WidgetController* mainWidget();

private:
	void onInput2D(const Event2D& ev);
	void onModelChanged();		
	
};

}}
#endif