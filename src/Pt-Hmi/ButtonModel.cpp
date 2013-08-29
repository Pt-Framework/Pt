#include <Pt/Hmi/ButtonModel.h>

namespace Pt{
namespace Hmi{

ButtonModel::ButtonModel()
: ButtonState(me(),Pt::Hmi::DeviceButton::Released)
, ActionKey(me(),"")
, Armed(me(),false)
, ButtonType(me(),Pt::Hmi::ButtonType::Press)
, DoublePressTimeInMs(me(),1500)
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
