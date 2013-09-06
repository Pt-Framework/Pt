#include <Pt/Hmi/WindowModel.h>
#include <Pt/Gfx/Color.h>

namespace Pt{
namespace Hmi{

WindowModel::WindowModel()
: DefineProperty(MinimumSize,Pt::Gfx::SizeF(0,0))
, DefineProperty(MaximumSize,Pt::Gfx::SizeF(65535,65535))
, DefineProperty(WindowStartPostion, WindowStartPositionType::Manual)
, DefineProperty(WindowState, WindowStateType::Normal)
, DefineProperty(ShowInTaskbar,true)
, DefineProperty(ShowTitle,true)
, DefineProperty(ShowMinimizeButton,true)
, DefineProperty(ShowMaximizeButton,true)
, DefineProperty(ShowSysMenu,true)	
, DefineProperty(Caption,"")
, DefineProperty(Border,WindowBorderType::Sizeable)
, DefineProperty(WinPos, Pt::Gfx::PointF(0,0))
, DefineProperty(WinSize, Pt::Gfx::SizeF(200,200))
, DefineProperty(Icon, Pt::Gfx::ARgbImage(16,16))
, DefineProperty(Closed,false)
, DefineProperty(CanClose,true)
, DefineProperty(TopMost, false)
, DefineProperty(FocuseMoveKey, "\t")
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
