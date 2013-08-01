#include <Pt/Hmi/ButtonModel.h>

namespace Pt{
namespace Hmi{

ButtonModel::ButtonModel()
:Armed(false)
,ButtonState(DeviceButton::None)
{
	LabelModel::BorderStyle.set(Pt::Hmi::BorderStyle::Widget);
	LabelModel::BorderWidth.set(1);
	LabelModel::Caption.set("Button");	
	LabelModel::AutoSize.set(false);
	LabelModel::TextAlign.set(Pt::Hmi::TextAlign::MidleCenter);
}

ButtonModel::~ButtonModel()
{
}


}}
