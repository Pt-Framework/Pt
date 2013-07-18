#ifndef Pt_Hmi_Model_Window_h
#define Pt_Hmi_Model_Window_h

#include <Pt/Hmi/GfxModel.h>
#include <Pt/Gfx/Point.h>
#include <Pt/Gfx/Size.h>
#include <Pt/Gfx/ARgbImage.h>
#include <Pt/Gfx/Font.h>
#include <Pt/Gfx/Gfx.h>

namespace Pt{
namespace Hmi{

namespace WindowStartPositionType
{
	enum Type
	{
		Manual,
		CenterScreen,
		WindowsDefaultLocation,
		WindowsDefaultBounds,
		CenterParent,
	};
}

namespace WindowStateType
{
	enum Type
	{
		Normal,
		Minimazed,
		Maximazed,
	};
}

class PT_HMI_API WindowModel : public GfxModel
{
public:
	WindowModel();
	virtual ~WindowModel();

	Property<Pt::Gfx::PointF>				PinPosition;
	Property<bool>							Pinned;		
	Property<int>							Cursor;	
	Property<bool>							AllowDrop;
	Property<double>						MinimumSize;
	Property<double>						MaximumSize;
	Property<WindowStartPositionType::Type>		WindowStartPostion;
	Property<WindowStateType::Type>			WindowState;	
	Property<bool>							ShowInTaskbar;
};

}}

#endif