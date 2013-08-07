#include <Pt/Hmi/ButtonModel.h>

namespace Pt{
namespace Hmi{

ButtonModel::ButtonModel()
: ButtonState(Pt::Hmi::DeviceButton::Released)
, ActionKey("")
, Armed(false)
, ButtonType(Pt::Hmi::ButtonType::Press)
{
	BorderStyle.set(Pt::Hmi::BorderStyle::Widget);
	BorderWidth.set(1);
	Caption.set("Button");	
	AutoSize.set(false);
	TextAlign.set(Pt::Hmi::TextAlign::MidleCenter);
	AcceptFocus.set(true);
	BorderRoundEdge.set(true);
}

ButtonModel::~ButtonModel()
{
}


}}
