#ifndef Pt_Hmi_LabelController_H
#define Pt_Hmi_LabelController_H

#include <Pt/Hmi/PanelController.h>
#include <Pt/Hmi/PointingEvent.h>
#include <Pt/Hmi/LabelModel.h>

namespace Pt{
namespace Hmi{

class LabelRenderer;

class PT_HMI_API LabelController  : public PanelController
{
public:
	LabelController(LabelModel& model, LabelRenderer& renderer);
	virtual ~LabelController();	
	
	const LabelModel& labelModel() const	
	{
		return static_cast<const LabelModel&>(model());
	}

	LabelModel& labelModel()
	{
		return static_cast<LabelModel&>(model());
	}
};

}}

#endif