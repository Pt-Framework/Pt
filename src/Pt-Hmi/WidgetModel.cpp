#include <Pt/Hmi/WidgetModel.h>

namespace Pt{
namespace Hmi{

WidgetModel::WidgetModel()
: DefinePropertyInitMacro(Caption,"")
{
	registerProperty(Caption);
}

WidgetModel::~WidgetModel()
{
}

}}