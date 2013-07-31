#include <Pt/Hmi/WindowModel.h>
#include <Pt/Gfx/Color.h>

namespace Pt{
namespace Hmi{

WindowModel::WindowModel()
{
	Position = toUnit(Pt::Gfx::Point(20,20));
	Size = toUnit(Pt::Gfx::Size(200,200));
}

WindowModel::~WindowModel()
{
}


}}