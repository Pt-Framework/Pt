#ifndef Pt_Hmi_Desktop_Panel_H
#define Pt_Hmi_Desktop_Panel_H

#include <Pt/Hmi/Desktop/Widget.h>

namespace Pt{
namespace Hmi{

class PanelController;
class PanelModel;
class PanelRenderer;

namespace Desktop{

class PT_HMI_DESKTOP_API Panel : public Widget 
{
public:
	Panel();
	virtual ~Panel();

private:
	Pt::Hmi::PanelController*	_defController;
	Pt::Hmi::PanelModel*		_defModel;
	Pt::Hmi::PanelRenderer*		_defRenderer;
};
 
}}}
#endif