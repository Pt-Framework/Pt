#include <Pt/Hmi/LabelModel.h>

namespace Pt{
namespace Hmi{

LabelModel::LabelModel()
: PT_HMI_INIT_PROPERTY_VALUE(AutoSize,true)
{
	Caption.set("Label");
	ForeColor.set(Pt::Gfx::ARgbColor(0,0,0,0));

	registerProperty(AutoSize);
}

LabelModel::~LabelModel()
{
}

}}
