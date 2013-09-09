#include <Pt/Hmi/ButtonModel.h>

namespace Pt{
namespace Hmi{

ButtonModel::ButtonModel()
: DefinePropertyInitMacro(ButtonState,Pt::Hmi::DeviceButton::Released)
, DefinePropertyInitMacro(ActionKey,"")
, DefinePropertyInitMacro(Armed,false)
, DefinePropertyInitMacro(ButtonType,Pt::Hmi::ButtonType::Press)
, DefinePropertyInitMacro(DoublePressTimeInMs,1500)
{
    BackColor.set(Pt::Gfx::ARgbColor(245,245,245));
	BorderStyle.set(Pt::Hmi::BorderStyleType::Widget);
	BorderWidth.set(1);
	Caption.set("Button");	
	AutoSize.set(false);
	TextAlign.set(Pt::Hmi::TextAlignType::MidleCenter);
	AcceptFocus.set(true);
	BorderRoundEdge.set(true);
}

ButtonModel::~ButtonModel()
{
}


}}
