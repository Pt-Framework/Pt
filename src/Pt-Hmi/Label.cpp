#include <Pt/Hmi/Label.h>
#include <Pt/Hmi/LabelModel.h>
#include <Pt/Hmi/LabelView.h>

namespace Pt{
namespace Hmi{

Label::Label(LabelModel& model, LabelView& view)
: Panel(model, view)
{
}

Label::~Label()
{
}

}}