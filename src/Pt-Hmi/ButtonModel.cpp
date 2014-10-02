#include <Pt/Hmi/ButtonModel.h>

namespace Pt{
namespace Hmi{

ButtonModel::ButtonModel()
: PT_HMI_INIT_PROPERTY_VALUE(ButtonState,Pt::Hmi::DeviceButton::Released)
, PT_HMI_INIT_PROPERTY_VALUE(ActionKey,"")
, PT_HMI_INIT_PROPERTY_VALUE(Armed,false)
, PT_HMI_INIT_PROPERTY_VALUE(ButtonType,Pt::Hmi::ButtonType::Press)
, PT_HMI_INIT_PROPERTY_VALUE(DoublePressTimeInMs,1500)
{
    BackColor.set(Pt::Gfx::ARgbColor(245,245,245));
	BorderStyle.set(Pt::Hmi::BorderStyleType::Widget);
	BorderWidth.set(1);
	Caption.set("Button");	
	AutoSize.set(false);
	TextAlign.set(Pt::Hmi::TextAlignType::MidleCenter);
	AcceptFocus.set(true);
	BorderRoundEdge.set(true);

	registerProperty(ButtonState);
	registerProperty(ActionKey);
	registerProperty(Armed);
	registerProperty(ButtonType);
	registerProperty(DoublePressTimeInMs);
}

ButtonModel::~ButtonModel()
{
}


}}
