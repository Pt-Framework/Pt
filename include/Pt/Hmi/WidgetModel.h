#ifndef Pt_Hmi_Model_Windget_h
#define Pt_Hmi_Model_Windget_h

#include <Pt/Hmi/GfxModel.h>
#include <Pt/Hmi/Property.h>
#include <Pt/Gfx/Point.h>
#include <Pt/Gfx/Size.h>
#include <Pt/Gfx/ARgbImage.h>

namespace Pt{
namespace Hmi{

class PT_HMI_API WidgetModel : public GfxModel
{
public:
	WidgetModel();
	virtual ~WidgetModel();

public:
	Property<std::string> Caption;
	Property<std::string> Name;		
};

}}

#endif
