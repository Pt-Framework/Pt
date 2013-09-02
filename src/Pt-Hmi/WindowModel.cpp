#include <Pt/Hmi/WindowModel.h>
#include <Pt/Gfx/Color.h>

namespace Pt{
namespace Hmi{

WindowModel::WindowModel()
: MinimumSize(me(),Pt::Gfx::SizeF(0,0))
, MaximumSize(me(),Pt::Gfx::SizeF(65535,65535))
, WindowStartPostion(me())
, WindowState(me())
, ShowInTaskbar(me(),true)
, ShowTitle(me(),true)
, ShowMinimizeButton(me(),true)
, ShowMaximizeButton(me(),true)
, ShowSysMenu(me(),true)	
, Caption(me())
, Border(me())
, WinPos(me())
, WinSize(me())
, Icon(me())
, Closed(me(),false)
, CanClose(me(),true)
, TopMost(me(), false)
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
