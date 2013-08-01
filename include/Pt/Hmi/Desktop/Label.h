#ifndef Pt_Hmi_Desktop_Label_H
#define Pt_Hmi_Desktop_Label_H

#include <Pt/Hmi/Desktop/Widget.h>

namespace Pt{
namespace Hmi{

class LabelController;
class LabelModel;
class LabelRenderer;

namespace Desktop{

class PT_HMI_DESKTOP_API Label : public Widget 
{
public:
	Label();
	virtual ~Label();

private:
	Pt::Hmi::LabelController*	_defController;
	Pt::Hmi::LabelModel*		_defModel;
	Pt::Hmi::LabelRenderer*		_defRenderer;
};
 
}}}
#endif