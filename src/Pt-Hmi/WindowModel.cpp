#include <Pt/Hmi/WindowModel.h>
#include <Pt/Gfx/Color.h>

namespace Pt{
namespace Hmi{

WindowModel::WindowModel()
: PT_HMI_INIT_PROPERTY_VALUE(MinimumSize,Pt::Gfx::SizeF(0,0))
, PT_HMI_INIT_PROPERTY_VALUE(MaximumSize,Pt::Gfx::SizeF(65535,65535))
, PT_HMI_INIT_PROPERTY_VALUE(WindowStartPostion, WindowStartPositionType::Manual)
, PT_HMI_INIT_PROPERTY_VALUE(WindowState, WindowStateType::Normal)
, PT_HMI_INIT_PROPERTY_VALUE(ShowInTaskbar,true)
, PT_HMI_INIT_PROPERTY_VALUE(ShowTitle,true)
, PT_HMI_INIT_PROPERTY_VALUE(ShowMinimizeButton,true)
, PT_HMI_INIT_PROPERTY_VALUE(ShowMaximizeButton,true)
, PT_HMI_INIT_PROPERTY_VALUE(ShowSysMenu,true)	
, PT_HMI_INIT_PROPERTY_VALUE(Caption,"")
, PT_HMI_INIT_PROPERTY_VALUE(Border,WindowBorderType::Sizeable)
, PT_HMI_INIT_PROPERTY_VALUE(Icon, Pt::Gfx::ARgbImage(0,0))
, PT_HMI_INIT_PROPERTY_VALUE(Closed,false)
, PT_HMI_INIT_PROPERTY_VALUE(CanClose,true)
, PT_HMI_INIT_PROPERTY_VALUE(TopMost, false)
, PT_HMI_INIT_PROPERTY_VALUE(FocuseMoveKey, "\t")
{
	Visible.set(false);
	Position.set(Pt::Gfx::PointF(20,20));
	Size.set(Pt::Gfx::SizeF(200,200));
	Focused.set(true);

	registerProperty(MinimumSize);
	registerProperty(MaximumSize);
	registerProperty(WindowStartPostion);
	registerProperty(WindowState);
	registerProperty(ShowInTaskbar);
	registerProperty(ShowTitle);
	registerProperty(ShowMinimizeButton);
	registerProperty(ShowMaximizeButton);
	registerProperty(ShowSysMenu);
	registerProperty(Caption);
	registerProperty(Border);
	registerProperty(Icon);
	registerProperty(Closed);
	registerProperty(CanClose);
	registerProperty(TopMost);
	registerProperty(FocuseMoveKey);
}

WindowModel::~WindowModel()
{
}


}}
