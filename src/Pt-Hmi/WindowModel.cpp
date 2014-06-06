#include <Pt/Hmi/WindowModel.h>
#include <Pt/Gfx/Color.h>

namespace Pt{
namespace Hmi{

WindowModel::WindowModel()
: DefinePropertyInitMacro(MinimumSize,Pt::Gfx::SizeF(0,0))
, DefinePropertyInitMacro(MaximumSize,Pt::Gfx::SizeF(65535,65535))
, DefinePropertyInitMacro(WindowStartPostion, WindowStartPositionType::Manual)
, DefinePropertyInitMacro(WindowState, WindowStateType::Normal)
, DefinePropertyInitMacro(ShowInTaskbar,true)
, DefinePropertyInitMacro(ShowTitle,true)
, DefinePropertyInitMacro(ShowMinimizeButton,true)
, DefinePropertyInitMacro(ShowMaximizeButton,true)
, DefinePropertyInitMacro(ShowSysMenu,true)	
, DefinePropertyInitMacro(Caption,"")
, DefinePropertyInitMacro(Border,WindowBorderType::Sizeable)
, DefinePropertyInitMacro(Icon, Pt::Gfx::ARgbImage(0,0))
, DefinePropertyInitMacro(Closed,false)
, DefinePropertyInitMacro(CanClose,true)
, DefinePropertyInitMacro(TopMost, false)
, DefinePropertyInitMacro(FocuseMoveKey, "\t")
{
	Visible.set(false);
	Position = Pt::Gfx::PointF(20,20);
	Size = Pt::Gfx::SizeF(200,200);
	Focused.set(true);
}

WindowModel::~WindowModel()
{
}


}}
