#include <Pt/Hmi/PanelModel.h>

namespace Pt{
namespace Hmi{

PanelModel::PanelModel()
: PT_HMI_INIT_PROPERTY_VALUE(BorderStyle,BorderStyleType::Single)
, PT_HMI_INIT_PROPERTY_VALUE(BorderWidth,3)
, PT_HMI_INIT_PROPERTY_VALUE(BorderRoundEdge,false)
{
	AcceptFocus.set(false);
	registerProperty(BorderStyle);
	registerProperty(BorderWidth);
	registerProperty(BorderRoundEdge);
}

PanelModel::~PanelModel()
{
}

Pt::Gfx::SizeF PanelModel::clientSize() const
{
	Pt::Gfx::SizeF cs;

	cs.setWidth(Size.get().width() - BorderWidth.get());
	cs.setHeight(Size.get().height() - BorderWidth.get());
	return cs;
}

}}
