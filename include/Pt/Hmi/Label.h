#ifndef Pt_Hmi_Label_H
#define Pt_Hmi_Label_H

#include <Pt/Hmi/Panel.h>

namespace Pt{
namespace Hmi{

class LabelModel;

class PT_HMI_API Label  : public Panel
{
public:
	Label(LabelModel* model);
	virtual ~Label();	
	
	const LabelModel* labelModel() const	
	{
		return _labelModel;
	}

	LabelModel* labelModel()
	{
		return _labelModel;
	}

protected:
	virtual void onRender();

private:
	LabelModel* _labelModel;
};

}}

#endif