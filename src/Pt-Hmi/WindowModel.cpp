#include <Pt/Hmi/WindowModel.h>
#include <Pt/Gfx/Color.h>

namespace Pt{
namespace Hmi{

WindowModel::WindowModel()
: ShowInTaskbar(true)
, ShowTitle(true)
, ShowMinimizeButton(true)
, ShowMaximizeButton(true)
, ShowSysMenu(true)	
, Closed(false)
, CanClose(true)
, MinimumSize(Pt::Gfx::SizeF(0,0))
, MaximumSize(Pt::Gfx::SizeF(65535,65535))
{

	Visible.set(false);
	Position = toUnit(Pt::Gfx::Point(20,20));
	Size = toUnit(Pt::Gfx::Size(200,200));
	Focused.set(true);
}

WindowModel::~WindowModel()
{
}


}}
