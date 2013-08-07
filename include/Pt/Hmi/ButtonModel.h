#ifndef Pt_Hmi_ButtonModel_h
#define Pt_Hmi_ButtonModel_h

#include <Pt/Hmi/LabelModel.h>
#include <Pt/Hmi/Property.h>
#include <Pt/Gfx/Point.h>
#include <Pt/Gfx/Size.h>
#include <Pt/Gfx/ArgbImage.h>

namespace Pt{
namespace Hmi{


namespace ButtonType
{
	enum Type
	{
		Press,
		Toggle
	};
}


class PT_HMI_API ButtonModel : public LabelModel
{

public:
	ButtonModel();
	virtual ~ButtonModel();

	Property<DeviceButton::State> ButtonState;	
	Property<std::string>         ActionKey;		
	Property<bool>				  Armed;	
	Property<ButtonType::Type>    ButtonType;	
};

}}

#endif
