#include <Pt/Hmi/LabelModel.h>

namespace Pt{
namespace Hmi{

LabelModel::LabelModel()
: DefineProperty(AutoSize,true)
{
	Caption.set("Label");
	ForeColor.set(Pt::Gfx::ARgbColor(0,0,0,0));
}

LabelModel::~LabelModel()
{
}

}}
