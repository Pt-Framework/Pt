#ifndef Pt_Hmi_Label_H
#define Pt_Hmi_Label_H

#include <Pt/Hmi/Panel.h>
#include <Pt/Hmi/PointingEvent.h>
#include <Pt/Hmi/LabelModel.h>

namespace Pt{
namespace Hmi{

class LabelView;

class PT_HMI_API Label  : public Panel
{
public:
	Label(LabelModel& model, LabelView& view);
	virtual ~Label();	
	
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