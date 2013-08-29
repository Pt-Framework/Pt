#include <Pt/Hmi/LabelModel.h>

namespace Pt{
namespace Hmi{

LabelModel::LabelModel()
: AutoSize(me(),true)
{
	Caption.set("Label");
	ForeColor.set(Pt::Gfx::ARgbColor(0,0,0,0));
}

LabelModel::~LabelModel()
{
}

}}
