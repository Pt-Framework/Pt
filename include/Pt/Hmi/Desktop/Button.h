#ifndef Pt_Hmi_Desktop_Button_H
#define Pt_Hmi_Desktop_Button_H

#include <Pt/Hmi/Desktop/Widget.h>

namespace Pt{
namespace Hmi{

class ButtonController;
class ButtonModel;
class ButtonRenderer;

namespace Desktop{

class PT_HMI_DESKTOP_API Button : public Widget 
{
public:
	Button();
	virtual ~Button();

private:
	Pt::Hmi::ButtonController*	_defController;
	Pt::Hmi::ButtonModel*		_defModel;
	Pt::Hmi::ButtonRenderer*		_defRenderer;
};
 
}}}
#endif