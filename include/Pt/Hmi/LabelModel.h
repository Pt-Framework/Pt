#ifndef Pt_Hmi_LabelModel_h
#define Pt_Hmi_LabelModel_h

#include <Pt/Hmi/PanelModel.h>
#include <Pt/Hmi/Property.h>
#include <Pt/Gfx/Point.h>
#include <Pt/Gfx/Size.h>
#include <Pt/Gfx/ArgbImage.h>

namespace Pt{
namespace Hmi{

class PT_HMI_API LabelModel : public PanelModel
{

public:
	LabelModel();
	virtual ~LabelModel();

	Property<bool> AutoSize;		
};

}}

#endif
