#ifndef Pt_Hmi_TitleBarController_H
#define Pt_Hmi_TitleBarController_H

#include <Pt/Hmi/WidgetController.h>
#include <Pt/Hmi/Event2D.h>

namespace Pt{
namespace Hmi{

class PT_HMI_API TitleBarController  : public WidgetController
{
public:
	TitleBarController();
	virtual ~TitleBarController();
	
	void start();
	void stop();

protected:
};

}}
#endif
