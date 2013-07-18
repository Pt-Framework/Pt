#ifndef Pt_Hmi_Model_Frame_h
#define Pt_Hmi_Model_Frame_h

#include <Pt/Hmi/WidgetModel.h>
#include <Pt/Hmi/Property.h>
#include <Pt/Gfx/Point.h>
#include <Pt/Gfx/Size.h>
#include <Pt/Gfx/ArgbImage.h>

namespace Pt{
namespace Hmi{

namespace BorderStyle
{
	enum Type
	{
		None,
		Single,
		Border3D,
		Sizebale,
		Tool,
		ToolSizeable
	};
}

class PT_HMI_API PanelModel : public WidgetModel
{

public:
	PanelModel();
	virtual ~PanelModel();

	Property<BorderStyle::Type> BorderStyle;	
	Property<double>			BorderWidth;	
	
public:
	Pt::Gfx::SizeF clientSize() const;	
};

}}

#endif
