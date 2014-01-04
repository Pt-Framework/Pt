#ifndef Pt_Hmi_LabelController_H
#define Pt_Hmi_LabelController_H

#include <Pt/Hmi/PanelController.h>
#include <Pt/Hmi/PointingEvent.h>

namespace Pt{
namespace Hmi{


class PT_HMI_API LabelController  : public PanelController
{
public:
	LabelController();
	virtual ~LabelController();		
};

}}

#endif